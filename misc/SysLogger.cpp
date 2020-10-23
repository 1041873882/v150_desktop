
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#include "mFT.h"
#include "SysCaller.h"
#include "SysLogger.h"

SysLogger sLogger;

void TalkLogger::load(void)
{
	dxml p;

	char s[128];
	sprintf(s, "%s/talk.xml", sLogger.dir());
	p.loadFile(s);

	m_sz = p.getInt("/logger/max", 0);
	if (m_sz > MAX_LOGGER_SZ || m_sz < 0)
		m_sz = 0;
	for(int i=0; i<m_sz; i++) {
		sprintf(s, "/logger/d%d/id", i);
		data[i].id = p.getText(s, "err");
		sprintf(s, "/logger/d%d/type", i);
		data[i].type = p.getInt(s, 0);
		sprintf(s, "/logger/d%d/done", i);
		data[i].done = p.getInt(s, 0);
		sprintf(s, "/logger/d%d/format", i);
		data[i].format = p.getInt(s, 0);
		sprintf(s, "/logger/d%d/timeout", i);
		data[i].timeout = p.getInt(s, 0);
		sprintf(s, "/logger/d%d/ts", i);
		data[i].ts = p.getInt(s, 0);
	}
	m_NA = 0;
}

void TalkLogger::save(void)
{
	char s[128];
	dxml p;
	p.setInt("/logger/max", m_sz);
	for(int i=0; i<m_sz; i++) {
		sprintf(s, "/logger/d%d/id", i);
		p.setText(s, data[i].id.c_str());
		sprintf(s, "/logger/d%d/type", i);
		p.setInt(s, data[i].type);
		sprintf(s, "/logger/d%d/done", i);
		p.setInt(s, data[i].done);
		sprintf(s, "/logger/d%d/format", i);
		p.setInt(s, data[i].format);
		sprintf(s, "/logger/d%d/timeout", i);
		p.setInt(s, data[i].timeout);
		sprintf(s, "/logger/d%d/ts", i);
		p.setInt(s, data[i].ts);
	}
	sprintf(s, "%s/talk.xml", sLogger.dir());
	p.saveFile(s);
	system("sync");
}

void TalkLogger::insert(int type)
{
	if (m_sz >= MAX_LOGGER_SZ)
		m_sz = MAX_LOGGER_SZ-1;
	int n = m_sz;
	while (n > 0) {
		data[n] = data[n-1];
		n--;
	}

	data[0].id = sCaller.m_id;
	data[0].type = type;
	data[0].format = (sCaller.host2id(sCaller.m_id) != NULL) ? 1 : 0;
	data[0].timeout = (type != NA) ? sCaller.ts() : 0;
	data[0].ts = sCaller.m_tv.tv_sec;
	if (data[0].timeout > 12*60*60) //时间错乱
		data[0].timeout = 0;
	m_sz++;
	this->save();

	if (type == OUT) {
		sCaller.logger(3);
	} else if (type == NA) {
		sCaller.logger(1);
		m_NA = 1;
	} else {
		sCaller.logger(0);
	}
}

void TalkLogger::remove(int n)
{
	if (n < m_sz) {
		int m = m_sz-n-1;
		for(int i=0; i<m; i++) {
			data[n+i] = data[n+i+1];
		}
		if (m_sz > 0)
			m_sz--;
		this->save();
	}
}

void JpegLogger::load(void)
{
	dxml p;

	char s[128];
	sprintf(s, "%s/jpeg.xml", sLogger.jpeg_dir());
	p.loadFile(s);

	m_sz = p.getInt("/logger/max", 0);
	if (m_sz > MAX_JPEG_SZ || m_sz < 0)
		m_sz = 0;
	for(int i=0; i<m_sz; i++) {
		sprintf(s, "/logger/d%d/url", i);
		data[i].url = p.getText(s, "err");
		sprintf(s, "/logger/d%d/ts", i);
		data[i].ts = p.getInt(s, 0);
	}

	DIR *dp = opendir(sLogger.jpeg_dir());
	if (dp) {
		while(1) {
			struct dirent *e = readdir(dp);
			if (e == NULL || e->d_name == NULL)
				break;

			if (strstr(e->d_name, ".jpg")) {
				int ok = 0;
				for(int i=0; i<m_sz; i++) {
					if (strstr(data[i].url.c_str(), e->d_name)) {
						ok = 1;
						break;
					}
				}
				if (!ok) { //删除不在记录中的图片
					sprintf(s, "%s/%s", sLogger.jpeg_dir(), e->d_name);
					unlink(s);
				}
			}
		}
		closedir(dp);
	}
}

void JpegLogger::save(void)
{
	char s[128];
	dxml p;
	p.setInt("/logger/max", m_sz);
	for(int i=0; i<m_sz; i++) {
		sprintf(s, "/logger/d%d/url", i);
		p.setText(s, data[i].url.c_str());
		sprintf(s, "/logger/d%d/ts", i);
		p.setInt(s, data[i].ts);
	}
	sprintf(s, "%s/jpeg.xml", sLogger.jpeg_dir());
	p.saveFile(s);
}

void JpegLogger::insert(time_t ts, const char *url)
{
	if (m_sz >= MAX_JPEG_SZ) {
		m_sz = MAX_JPEG_SZ-1;
		unlink(data[m_sz].url.c_str());
	}
	int n = m_sz;
	while (n > 0) {
		data[n] = data[n-1];
		n--;
	}

	data[0].url = url;
	data[0].ts = ts;
	m_sz++;

	this->save();
}

void JpegLogger::remove(time_t ts)
{
	for(int i=0; i<m_sz; i++) {
		if (data[i].ts == ts) {
			unlink(data[i].url.c_str());
			int m = m_sz-i-1;
			for(int j=0; j<m; j++)
				data[i+j] = data[i+j+1];
			if (m_sz > 0)
				m_sz--;
		}
	}
	this->save();
}

const char *JpegLogger::url(time_t ts)
{
	for(int i=0; i<m_sz; i++) {
		if (data[i].ts == ts) {
			return data[i].url.c_str();
		}
	}
	return NULL;
}

void MsgLogger::load(void)
{
	dxml p;

	char s[128];
	sprintf(s, "%s/msg.xml", sLogger.dir());
	p.loadFile(s);

	m_sz = p.getInt("/logger/max", 0);
	if (m_sz > MAX_LOGGER_SZ || m_sz < 0)
		m_sz = 0;
	for(int i=0; i<m_sz; i++) {
		sprintf(s, "/logger/d%d/text", i);
		data[i].text = p.getText(s, "err");
		sprintf(s, "/logger/d%d/done", i);
		data[i].done = p.getInt(s, 0);
		sprintf(s, "/logger/d%d/ts", i);
		data[i].ts = p.getInt(s, 0);
	}
}

void MsgLogger::save(void)
{
	char s[128];
	dxml p;
	p.setInt("/logger/max", m_sz);
	for(int i=0; i<m_sz; i++) {
		sprintf(s, "/logger/d%d/text", i);
		p.setText(s, data[i].text.c_str());
		sprintf(s, "/logger/d%d/done", i);
		p.setInt(s, data[i].done);
		sprintf(s, "/logger/d%d/ts", i);
		p.setInt(s, data[i].ts);
	}
	sprintf(s, "%s/msg.xml", sLogger.dir());
	p.saveFile(s);
	system("sync");
}

void MsgLogger::insert(const char *text, time_t ts)
{
	if (text == NULL)
		return;

	if (m_sz >= MAX_LOGGER_SZ)
		m_sz = MAX_LOGGER_SZ-1;
	int n = m_sz;
	while (n > 0) {
		data[n] = data[n-1];
		n--;
	}

	if (ft.is_u8(text, strlen(text)))
		data[0].text = text;
	else {
		char s[4*1024];
		ft.gbk_u8(text, s, sizeof(s));
		data[0].text = s;
	}
	data[0].done = 0;
	data[0].ts = ts;
	m_sz++;
	m_have = 1;

	this->save();
}

void MsgLogger::remove(int n)
{
	if (n < m_sz) {
		int m = m_sz-n-1;
		for(int i=0; i<m; i++)
			data[n+i] = data[n+i+1];
		if (m_sz > 0)
			m_sz--;
		this->save();
	}
}

void MsgLogger::done(int n)
{
	if (n < m_sz) {
		data[n].done = 1;
		this->save();
	}
}

SysLogger::SysLogger()
{
	struct stat st;
	if (stat(this->dir(), &st))
		mkdir(this->dir(), 0777);
	if (stat(this->jpeg_dir(), &st))
		mkdir(this->jpeg_dir(), 0777);
}

SysLogger::~SysLogger()
{
}

const char *SysLogger::dir(void)
{
	return "/dnake/data/logger";
}

const char *SysLogger::jpeg_dir(void)
{
	if (sys.flash() != 0)
		return "/dnake/data/logger/jpeg";
	return "/var/jpeg";
}

void SysLogger::load(void)
{
	talk.load();
	jpeg.load();
	msg.load();
}
