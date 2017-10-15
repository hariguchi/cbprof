#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cbProf.h"


int
main (int argc, char* argv[])
{
    cbProf prof;
    char     dst[1024];
    char     src[1024];
    int      i;

    cbProfInit(&prof);
    cbProfSetBanner(&prof, "memcpy: ");
    cbProfRun(&prof);

   for ( i = 0; i < 1000; ++i ) {
       cbProfBegin(&prof);
       memcpy(dst, src, sizeof(dst));
       cbProfEnd(&prof);
   }
   cbProfMakeHist(&prof);
   printf("%s\n", prof.msg);
   exit(0);
   return 0;                    /* make gcc happy */
}
