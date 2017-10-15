#include "cbProf.hpp"


int
main (int argc, char* argv[])
{
    cbProf::prof prof("memcpy: ", true);
    char     dst[1024];
    char     src[1024];
    int      i;

   for ( i = 0; i < 1000; ++i ) {
       prof.begin();
       memcpy(dst, src, sizeof(dst));
       prof.end();
   }
   prof.makeHist();
   std::cout << prof.str() << "\n";

   exit(0);
   return 0;                    // make gcc happy
}
