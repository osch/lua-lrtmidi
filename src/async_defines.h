#ifndef LRTMIDI_ASYNC_DEFINES_H
#define LRTMIDI_ASYNC_DEFINES_H

/* -------------------------------------------------------------------------------------------- */

#if    defined(LRTMIDI_ASYNC_USE_WIN32) \
    && (   defined(LRTMIDI_ASYNC_USE_STDATOMIC) \
        || defined(LRTMIDI_ASYNC_USE_GNU))
  #error "LRTMIDI_ASYNC: Invalid compile flag combination"
#endif
#if    defined(LRTMIDI_ASYNC_USE_STDATOMIC) \
    && (   defined(LRTMIDI_ASYNC_USE_WIN32) \
        || defined(LRTMIDI_ASYNC_USE_GNU))
  #error "LRTMIDI_ASYNC: Invalid compile flag combination"
#endif
#if    defined(LRTMIDI_ASYNC_USE_GNU) \
    && (   defined(LRTMIDI_ASYNC_USE_WIN32) \
        || defined(LRTMIDI_ASYNC_USE_STDATOMIC))
  #error "LRTMIDI_ASYNC: Invalid compile flag combination"
#endif
 
/* -------------------------------------------------------------------------------------------- */

#if    !defined(LRTMIDI_ASYNC_USE_WIN32) \
    && !defined(LRTMIDI_ASYNC_USE_STDATOMIC) \
    && !defined(LRTMIDI_ASYNC_USE_GNU)

    #if defined(WIN32) || defined(_WIN32)
        #define LRTMIDI_ASYNC_USE_WIN32
    #elif defined(__GNUC__)
        #define LRTMIDI_ASYNC_USE_GNU
    #elif __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_ATOMICS__)
        #define LRTMIDI_ASYNC_USE_STDATOMIC
    #else
        #error "LRTMIDI_ASYNC: unknown platform"
    #endif
#endif

/* -------------------------------------------------------------------------------------------- */

#if defined(__unix__) || defined(__unix) || (defined (__APPLE__) && defined (__MACH__))
    #include <unistd.h>
#endif

#if    !defined(LRTMIDI_ASYNC_USE_WINTHREAD) \
    && !defined(LRTMIDI_ASYNC_USE_PTHREAD) \
    && !defined(LRTMIDI_ASYNC_USE_STDTHREAD)
    
    #ifdef LRTMIDI_ASYNC_USE_WIN32
        #define LRTMIDI_ASYNC_USE_WINTHREAD
    #elif _XOPEN_VERSION >= 600
        #define LRTMIDI_ASYNC_USE_PTHREAD
    #elif __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_THREADS__)
        #define LRTMIDI_ASYNC_USE_STDTHREAD
    #else
        #define LRTMIDI_ASYNC_USE_PTHREAD
    #endif
#endif

/* -------------------------------------------------------------------------------------------- */

#if defined(LRTMIDI_ASYNC_USE_PTHREAD)
    #ifndef _XOPEN_SOURCE
        #define _XOPEN_SOURCE 600 /* must be defined before any other include */
    #endif
    #include <errno.h>
    #include <sys/time.h>
    #include <pthread.h>
#endif
#if defined(LRTMIDI_ASYNC_USE_WIN32) || defined(LRTMIDI_ASYNC_USE_WINTHREAD)
    #include <windows.h>
#endif
#if defined(LRTMIDI_ASYNC_USE_STDATOMIC)
    #include <stdint.h>
    #include <stdatomic.h>
#endif
#if defined(LRTMIDI_ASYNC_USE_STDTHREAD)
    #include <sys/time.h>
    #include <threads.h>
#endif

/* -------------------------------------------------------------------------------------------- */

#if __STDC_VERSION__ >= 199901L
    #include <stdbool.h>
#else
    #if !defined(__GNUC__) || defined(__STRICT_ANSI__)
        #define inline
    #endif 
    #define bool int
    #define true 1
    #define false 0
#endif

/* -------------------------------------------------------------------------------------------- */


#endif /* LRTMIDI_ASYNC_DEFINES_H */
