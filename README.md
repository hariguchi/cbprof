# Code Block Profiler

## C Implementation

```C
#include "cbProf.h"

cbProf; /* or struct cbProf */
```

## C++ Implementation

```C++
#include "cbProf.hpp"

cbProf::prof;
```

## Introduction

This directory contains C and C++ implementation of a code
block profiler. It measures execution time of a code block and
makes a histogram like as follows:

```
memcpy:  1000 calls, 73.70 us, 0.07 us/call, min: 0 ns, max: 0.5 us
memcpy:      0ns -  100ns: 82.80%  828
memcpy:    100ns -  200ns:  0.00%  0
memcpy:    200ns -  300ns:  0.00%  0
memcpy:    300ns -  400ns:  0.00%  0
memcpy:    400ns -  500ns: 12.30%  123
memcpy:    500ns -  600ns:  4.90%  49
memcpy:    600ns -  700ns:  0.00%  0
memcpy:    700ns -  800ns:  0.00%  0
memcpy:    800ns -  900ns:  0.00%  0
memcpy:    900ns - 1000ns:  0.00%  0
memcpy:      1us - 10us:    0.00%  0
memcpy:     10us - 100us:   0.00%  0
memcpy:    100us - 1000us:  0.00%  0
memcpy:           >1ms:      ---   0
```

## Requirements

* *local_types.h* must be stored in the include path as well as
  *cbProf.h* and *cbProf.hpp*.


## cbProf

Code block profiler, a C implementation.

### include files

```C
#include "cbProf.h"
```

### Examples

```C
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
```

## cbProf::prof

Code block profiler, a C++ implementation.

### include files

```C++
#include "cbProf.hpp"
```

### Examples

```C++
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
```

