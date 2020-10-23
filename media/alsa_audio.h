/*
** Copyright 2010, The Android Open-Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#ifndef _ALSAAUDIO_H_
#define _ALSAAUDIO_H_

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>

#include <linux/ioctl.h>

#define __force
#define __bitwise
#define __user
#include "asound.h"

static inline int param_is_mask(int p)
{
	return (p >= SNDRV_PCM_HW_PARAM_FIRST_MASK) && (p <= SNDRV_PCM_HW_PARAM_LAST_MASK);
}

static inline int param_is_interval(int p)
{
	return (p >= SNDRV_PCM_HW_PARAM_FIRST_INTERVAL) && (p <= SNDRV_PCM_HW_PARAM_LAST_INTERVAL);
}

static inline struct snd_interval *param_to_interval(struct snd_pcm_hw_params *p, int n)
{
	return &(p->intervals[n - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL]);
}

static inline struct snd_mask *param_to_mask(struct snd_pcm_hw_params *p, int n)
{
	return &(p->masks[n - SNDRV_PCM_HW_PARAM_FIRST_MASK]);
}

static inline void param_set_mask(struct snd_pcm_hw_params *p, int n, unsigned bit)
{
	if (bit >= SNDRV_MASK_MAX)
		return;
	if (param_is_mask(n)) {
		struct snd_mask *m = param_to_mask(p, n);
		m->bits[0] = 0;
		m->bits[1] = 0;
		m->bits[bit >> 5] |= (1 << (bit & 31));
	}
}

static inline void param_set_min(struct snd_pcm_hw_params *p, int n, unsigned val)
{
	if (param_is_interval(n)) {
		struct snd_interval *i = param_to_interval(p, n);
		i->min = val;
	}
}

static inline void param_set_max(struct snd_pcm_hw_params *p, int n, unsigned val)
{
	if (param_is_interval(n)) {
		struct snd_interval *i = param_to_interval(p, n);
		i->max = val;
	}
}

static inline void param_set_int(struct snd_pcm_hw_params *p, int n, unsigned val)
{
	if (param_is_interval(n)) {
		struct snd_interval *i = param_to_interval(p, n);
		i->min = val;
		i->max = val;
		i->integer = 1;
	}
}

static inline void param_init(struct snd_pcm_hw_params *p)
{
	int n;
	memset(p, 0, sizeof(*p));
	for (n = SNDRV_PCM_HW_PARAM_FIRST_MASK; n <= SNDRV_PCM_HW_PARAM_LAST_MASK; n++) {
		struct snd_mask *m = param_to_mask(p, n);
		m->bits[0] = ~0;
		m->bits[1] = ~0;
	}
	for (n = SNDRV_PCM_HW_PARAM_FIRST_INTERVAL; n <= SNDRV_PCM_HW_PARAM_LAST_INTERVAL; n++) {
		struct snd_interval *i = param_to_interval(p, n);
		i->min = 0;
		i->max = ~0;
	}
}

#endif
