#ifndef __CBPROF_H__
#define __CBPROF_H__

/**
 * Copyright (c) 2017 Yoichi Hariguchi
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the
 * Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so,
 * subject to the following conditions: 
 *
 * The above copyright notice and this permission notice shall
 * be included in all copies or substantial portions of the
 * Software. 
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
 * OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
 *
 *   cbProf: Code Block Profiler
 *
 *   Example:
 *
 *   cbProf prof;
 *
 *   cbProfInit(&prof);
 *   cbProfSetBanner(&prof, "Example: ");
 *   cbProfRun(&prof);
 *
 *   for ( i = 0; i < 1000; ++i ) {
 *       cbProfBegin(&prof);
 *       functionToBeProfiled();
 *       cbProfEnd(&prof);
 *   }
 *   cbProfMakeHist(&prof);
 *   printf("%s\n", prof.msg);
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>
#include "local_types.h"


typedef struct cbProf cbProf;
struct cbProf {
    u32 nCalls;
    int running;                /* 1: running, 0: not nunning */
    struct timespec t1;
    struct timespec min;
    struct timespec max;
    struct timespec sum;
    /*
     * histogram:
     *  hist[0..9]:   0ns   - 1000ns (100ns granularity)
     *  hist[10..18]: 1us   - 10us   (1us granularity)
     *  hist[19..27]: 10us  - 100us  (10us granularity)
     *  hist[28..36]: 100us - 1000us (100us granularity)
     *  hist[37]: >1ms
     */
    u32  hist[38];
    char banner[128];
    char msg[2048];
};

void   cbProfInit(cbProf* p);
void   cbProfSetBanner(cbProf* p, char* s);
void   cbProfRun(cbProf* p);
void   cbProfStop(cbProf* p);
int    cbProfBegin(cbProf* p);
int    cbProfEnd(cbProf* p);
void   cbProfLog(cbProf* p);
int    cbProfMakeHist(cbProf* p);

int    tmsIsLarge(struct timespec* t1, struct timespec* t2);
void   tmsAdd(struct timespec* t1, struct timespec* t2);
void   tmsSubtract(struct timespec* t1, struct timespec* t2);


/*
 * Inline functions
 */
static inline double
tmsSecs (struct timespec* t)
{
    return ((double)t->tv_sec +
            (((double)t->tv_nsec) / (1000.0*1000.0*1000.0)));
}

static inline double
tmsMsecs (struct timespec* t)
{
    return ((((double)t->tv_sec) * 1000.0) +
            (((double)t->tv_nsec) / (1000.0*1000.0)));
}

static inline double
tmsUsecs (struct timespec* t)
{
    return ((((double)t->tv_sec) * (1000.0*1000.0)) +
            (((double)t->tv_nsec) / 1000.0));
}

static inline double
tmsNsecs (struct timespec* t)
{
    return ((((double)t->tv_sec) * (1000.0*1000.0*1000.0)) +
            (double)t->tv_nsec);
}

static inline u64
tmsNanoSecs (struct timespec* t)
{
    return ((((u64)t->tv_sec) * (1000*1000*1000)) + ((u64)t->tv_nsec));
}


#ifdef __cplusplus
}
#endif
#endif /* __CBPROF_H__ */
