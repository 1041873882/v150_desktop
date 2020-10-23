
#include "sys.h"
#include "MediaPlayer.h"

static void *sys_media_thread(void *)
{
	player.process();
	return NULL;
}

MediaPlayer player;

MediaPlayer::MediaPlayer()
{
	m_ctx = NULL;
	m_convert = NULL;
	m_stop = 0;
	m_cycle = 0;

	sem_init(&m_wait, 0, 0);
	pthread_mutex_init(&m_mutex, NULL);
}

MediaPlayer::~MediaPlayer()
{
}

int MediaPlayer::open(const char *url)
{
	AutoMutex mutex(&m_mutex);

	if (m_ctx != NULL || url == NULL)
		return -1;

	AVInputFormat *ifmt = NULL;
	const char *fmt = NULL;

	for (int i=strlen(url)-1; i>=0; i--) {
		if (url[i] == '.') {
			fmt = url+i;
			break;
		}
	}
	if (fmt == NULL)
		return -1;

	if (!(ifmt = av_find_input_format(fmt+1))) {
		LOGE("MediaPlayer::open av_find_input_format error!\n");
		return -1;
	}
	int r = av_open_input_file(&m_ctx, url, ifmt, 0, NULL);
	if (r < 0) {
		LOGE("MediaPlayer::open av_open_input_file %s error[%d]!\n", url, r);
		return -1;
	}
	r = av_find_stream_info(m_ctx);
	if (r < 0) {
		LOGE("MediaPlayer::open av_find_stream_info %s error!\n", url);
		return -1;
	}
	AVCodecContext *ctx = NULL;
	for(int i = 0; i < (int)m_ctx->nb_streams; i++) {
		ctx = m_ctx->streams[i]->codec;
		if (ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
			AVCodec *c = avcodec_find_decoder(ctx->codec_id);
			if (!c || avcodec_open(ctx, c) < 0) {
				fprintf (stderr, "MediaPlayer::open avcodec_open error!\n");
				continue;
			}
			if (ctx->sample_fmt != SAMPLE_FMT_S16)
				m_convert = av_audio_convert_alloc(SAMPLE_FMT_S16, ctx->channels, ctx->sample_fmt, ctx->channels, NULL, 0);

			m_dev.set(ctx->sample_rate, ctx->channels, 1);
			break;
		}
	}
	if (ctx == NULL)
		return -1;
	if (m_dev.open())
		return -1;

	return 0;
}

void MediaPlayer::close(void)
{
	m_dev.close();

	AutoMutex mutex(&m_mutex);
	if (m_ctx) {
		if (m_convert) {
			av_audio_convert_free(m_convert);
			m_convert = NULL;
		}
		for(int i=0; i<(int)m_ctx->nb_streams; i++) {
			AVStream *s = m_ctx->streams[i];
			avcodec_close(s->codec);
		}
		av_close_input_file(m_ctx);
		m_ctx = NULL;
	}
}

static int mThread = 0;
void MediaPlayer::start(const char *url, int cycle)
{
	if (url == NULL || m_cycle || m_list.used() >= 3*sizeof(player_list_t))
		return;

	if (!mThread) {
		mThread = 1;
		pthread_t pid;
		if (pthread_create(&pid, NULL, sys_media_thread, NULL) != 0)
			perror("pthread_create sys_media_thread\n");
	}

	player_list_t list;
	strcpy(list.url, url);
	list.cycle = cycle;
	m_list.put((uint8_t *)&list, sizeof(list));
	sem_post(&m_wait);
}

void MediaPlayer::stop(void)
{
	m_list.flush();
	m_cycle = 0;
	m_stop = 1;
}

int MediaPlayer::decode(void)
{
	AutoMutex mutex(&m_mutex);
	int sz = -1;

	if (m_ctx) {
		AVPacket pkt;
		short d[AVCODEC_MAX_AUDIO_FRAME_SIZE];
		int r = av_read_frame(m_ctx, &pkt);
		if (r < 0)
			return -1;
		AVCodecContext *ctx = m_ctx->streams[pkt.stream_index]->codec;
		if (ctx && ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
			sz = AVCODEC_MAX_AUDIO_FRAME_SIZE;
			if (m_convert) {
				short d2[AVCODEC_MAX_AUDIO_FRAME_SIZE];
				avcodec_decode_audio3(ctx, d2, &sz, &pkt);
				if (sz > 0) {
					int osize = av_get_bits_per_sample_format(SAMPLE_FMT_S16)/8;
					int isize = av_get_bits_per_sample_format(ctx->sample_fmt)/8;
					int istride[6] = {isize};
					int ostride[6] = {osize};
					const void *ibuf[6] = {d2};
					void *obuf[6] = {d};
					int len = sz/istride[0];
					if (av_audio_convert(m_convert, obuf, ostride, ibuf, istride, len)<0) {
						LOGE("MediaPlayer::decode av_audio_convert() failed.\n");
					} else
						sz = len*osize;
				}
			} else {
				avcodec_decode_audio3(ctx, d, &sz, &pkt);
			}
		}
		av_free_packet(&pkt);
		if (sz > 0) {
			int v = sys.volume.music();
			if (v) {
				for(int i=0; i<(sz/2); i++)
					d[i] >>= v;
			}
			m_dev.write(d, sz);
		}
	}
	return sz;
}

void MediaPlayer::playing(player_list_t *list)
{
	if (list->cycle)
		m_list.put((uint8_t *)list, sizeof(player_list_t));
	m_cycle = list->cycle;

	int r = this->open(list->url);
	if (r) {
		this->close();
		m_cycle = 0;
		return;
	}

	m_dev.volume(0x32);
	while (!m_stop) {
		struct timeval tv;
		fd_set wfds;
		FD_ZERO(&wfds);
		FD_SET(m_dev.fd(), &wfds);
		tv.tv_sec = 0;
		tv.tv_usec = 40*1000;
		select(m_dev.fd()+1, NULL, &wfds, NULL, &tv);
		if (FD_ISSET(m_dev.fd(), &wfds)) {
			int r = this->decode();
			if (r <= 0)
				break;
		}
	}
	this->close();
	m_cycle = 0;
}

void MediaPlayer::process(void)
{
	av_register_all();

	while (1) {
		m_stop = 0;
		player_list_t list;
		int r = m_list.get((uint8_t *)&list, sizeof(list));
		if (r > 0) {
			this->playing(&list);
		} else {
			sem_wait(&m_wait);
		}
	}
}
