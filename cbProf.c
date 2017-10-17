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
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

#include "cbProf.h"

void
cbProfInit (cbProf* p)
{
    memset(p, 0, sizeof(*p));
    p->min.tv_sec = 1000;       /* 1000 seconds. Large enough. */
}


void
cbProfSetBanner (cbProf* p, char* s)
{
    memcpy(p->banner, s, sizeof(p->banner));
}


void
cbProfRun (cbProf* p)
{
    p->running = 1;
}


void
cbProfStop (cbProf* p)
{
    p->running = 0;
}


int
cbProfBegin (cbProf* p)
{
    int rc;


    if ( !p->running ) {
        return -1;
    }
    rc = clock_gettime(CLOCK_REALTIME, &p->t1);
    return rc;
}


int
cbProfEnd (cbProf* p)
{
    struct timespec t2;
    int rc;

    if ( !p->running ) {
        return -1;
    }
    rc = clock_gettime(CLOCK_REALTIME, &t2);
    if ( rc < 0 ) {
        return rc;
    }
    if (tmsIsLarge(&p->t1, &t2) > 0) {
        /*
         * start time is older than end time.
         */
        ++p->hist[37];
        return 0;
    }        

    tmsSubtract(&t2, &p->t1);   /* t2 -= p->t1 */
    if ( (t2.tv_sec > 0) || (t2.tv_nsec >= 1000*1000) ) {
        /*
         * >= 1ms. Omit from profiling, but keep the record
         */
        ++p->hist[37];
        return 0;
    }
    /*
     * Update the histogram
     */
    ++p->nCalls;                /* number of calls */
    tmsAdd(&p->sum, &t2);       /* p->sum += t2 */
    if (t2.tv_nsec < 1000) {
        ++p->hist[t2.tv_nsec/100];
    } else if (t2.tv_nsec < 10*1000) {
        ++p->hist[9 + (t2.tv_nsec/1000)];
    } else if (t2.tv_nsec < 100*1000) {
        ++p->hist[18 + (t2.tv_nsec/10000)];
    } else if (t2.tv_nsec < 1000*1000) {
        ++p->hist[27 + (t2.tv_nsec/100000)];
    }
    if ( tmsIsLarge(&t2, &p->max) > 0 ) {
        p->max.tv_sec = t2.tv_sec;
        p->max.tv_nsec = t2.tv_nsec;
    }
    if ( tmsIsLarge(&t2, &p->min) < 0 ) {
        p->min.tv_sec = t2.tv_sec;
        p->min.tv_nsec = t2.tv_nsec;
    }
    return 0;
}


static int
cbProfMakeHistEnt (cbProf* p, char* s, int begin, int end,
                     int tm, int d, int len, char* fmt1, char* fmt2)
{
    double r;
    double nCalls = (double)p->nCalls;
    u32    sum;
    int    i;
    int    n;
    int    olen = len;

    sum = 0;
    for ( i = begin; i < end; ++i ) {
        sum += p->hist[i];
    }
    /*
     * If the sum of p->hist[begin..end] is < 1%,
     * output only the summary. Otherwise output the details.
     */
    r = ((double)sum)/nCalls;
    if (r <= 0.01) {
        n = snprintf(s, len, "%s", p->banner);
        s   += n;
        len -= n;
        n = snprintf(s, len, fmt1, r*100.0, sum);
        len -= n;
    } else {
        for (i = begin; i < end; ++i, tm += d) {
            n = snprintf(s, len, "%s", p->banner);
            s   += n;
            len -= n;
            n = snprintf(s, len, fmt2, tm, tm+d, 
                         (((double)p->hist[i])*100.0)/nCalls, p->hist[i]);
            s   += n;
            len -= n;
            if ( len <= 0 ) {
                break;
            }
        }
    }
    return olen - len;
}


int
cbProfMakeHist (cbProf* p)
{
    int   n;
    int   len = sizeof(p->msg);
    char* s   = p->msg;

    n = snprintf(s, len, "%s %d calls, %.2f us, "
                 "%.2f us/call, min: %"PRIu64" ns, max: %"PRIu64" ns\n",
                 p->banner, p->nCalls, tmsUsecs(&p->sum),
                 tmsUsecs(&p->sum)/(double)p->nCalls,
                 tmsNanoSecs(&p->min), tmsNanoSecs(&p->max));
    s   += n;
    len -= n;
    if ( len <= 0 ) {
        return -1;
    }

    /*
     * 0ns - 1000ns
     */
    n = cbProfMakeHistEnt(p, s, 0, 10, 0, 100, len,
                            "     0ns - 1000ns: %5.2f%%  %d\n",
                            "  %4dns - %4dns: %5.2f%%  %d\n");
    s   += n;
    len -= n;
    if ( len <= 0 ) {
        return -1;
    }

    /*
     * 1us - 10us
     */
    n = cbProfMakeHistEnt(p, s, 10, 19, 1, 1, len,
                            "     1us - 10us:   %5.2f%%  %d\n",
                            "  %4dus - %2dus:   %5.2f%%  %d\n");
    s   += n;
    len -= n;
    if ( len <= 0 ) {
        return -1;
    }

    /*
     * 10 - 100us
     */
    n = cbProfMakeHistEnt(p, s, 19, 28, 10, 10, len,
                            "    10us - 100us:  %5.2f%%  %d\n",
                            "  %4dus - %3dus:  %5.2f%%  %d\n");
    s   += n;
    len -= n;
    if ( len <= 0 ) {
        return -1;
    }

    /*
     * 100us - 1000us
     */
    n = cbProfMakeHistEnt(p, s, 28, 37, 100, 100, len,
                            "   100us - 1000us: %5.2f%%  %d\n",
                            "  %4dus - %4dus: %5.2f%%  %d\n");

    s   += n;
    len -= n;
    if ( len <= 0 ) {
        return -1;
    }

    n = snprintf(s, len, "%s          >1ms:      ---   %d\n",
                 p->banner, p->hist[37]);
    len -= n;
    return (len > 0) ? 0 : -1;
}


void
tmsAdd (struct timespec* t1, struct timespec* t2)
{
    t1->tv_sec += t2->tv_sec;
    t1->tv_nsec += t2->tv_nsec;
    if ( t1->tv_nsec >= 1000*1000*1000 ) {
        ++t1->tv_sec;
        t1->tv_nsec -= 1000*1000*1000;
    }
    assert(t1->tv_nsec < 1000*1000*1000);
}


void
tmsSubtract (struct timespec* t1, struct timespec* t2)
{
    if ( t1->tv_nsec < t2->tv_nsec ) {
        --t1->tv_sec;
        t1->tv_nsec += 1000*1000*1000;
        t1->tv_nsec -= t2->tv_nsec;

        assert(t1->tv_nsec < 1000*1000*1000);
    } else {
        t1->tv_nsec -= t2->tv_nsec;
    }
    t1->tv_sec -= t2->tv_sec;
}

void
tmsDiv (struct timespec* t1, struct timespec* t2)
{
    u64 ns1;
    u64 ns2;

    ns1 = (t1->tv_sec * (1000*1000*1000)) + t1->tv_nsec;
    ns2 = (t2->tv_sec * (1000*1000*1000)) + t2->tv_nsec;
    t1->tv_sec  = (ns1/ns2) / (1000*1000*1000);
    t1->tv_nsec = ns1 - (ns2 * t1->tv_sec);
}

int
tmsIsLarge (struct timespec* t1, struct timespec* t2)
{
    if ( t1->tv_sec > t2->tv_sec ) {
        return 1;               /* t1 > t2 */
    }
    if ( t1->tv_sec < t2->tv_sec ) {
        return -1;               /* t1 < t2 */
    }
    /*
     * t1->tv_sec == t2->tv_sec
     */
    if ( t1->tv_nsec > t2->tv_nsec ) {
        return 1;               /* t1 > t2 */
    }
    if ( t1->tv_nsec < t2->tv_nsec ) {
        return -1;              /* t1 < t2 */
    }
    return 0;                   /* t1 == t2 */
}
