/*
AYMO - Accelerated YaMaha Operator
Copyright (c) 2023-2024 Andrea Zoppi.

This file is part of AYMO.

AYMO is free software: you can redistribute it and/or modify it under the
terms of the GNU Lesser General Public License as published by the Free
Software Foundation, either version 2.1 of the License, or (at your option)
any later version.

AYMO is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with AYMO. If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef _include_aymo_cc_h
#define _include_aymo_cc_h

// Use "aymo_cc.h" as the common file including "aymo_config.h" (if required)
#ifdef AYMO_HAVE_CONFIG_H  // command line / build system
    #include "aymo_config.h"
#endif


// Usual macros to generate strings
#ifndef AYMO_STRINGIFY
    #define AYMO_STRINGIFY(token)   #token
    #define AYMO_STRINGIFY2(token)  AYMO_STRINGIFY(token)
#endif


// Unused variable
#define AYMO_UNUSED_VAR(x)  ((void)(x))


// Common C++ name mangling wrappers.
#ifndef AYMO_CXX_EXTERN_C_BEGIN
    #ifdef __cplusplus
        #define AYMO_CXX_EXTERN_C_BEGIN     extern "C" {
        #define AYMO_CXX_EXTERN_C_END       }  // extern "C"
    #else
        #define AYMO_CXX_EXTERN_C_BEGIN     // ignore
        #define AYMO_CXX_EXTERN_C_END       // ignore
    #endif
#endif


// #pragma pack(push/pop) equivalents.
// Originally by MSVC, also supported by modern GCC/Clang.
#ifndef AYMO_PRAGMA_POP
    #if (defined(_MSC_VER) || defined(__GNUC__) || defined(__clang__))
        #define AYMO_PRAGMA_PACK_PUSH_N(n)      _Pragma(AYMO_STRINGIFY(pack(push, n)))
        #define AYMO_PRAGMA_PACK_PUSH_1         AYMO_PRAGMA_PACK_PUSH_N(1)
        #define AYMO_PRAGMA_PACK_PUSH_DEFAULT   _Pragma("pack(push)")  _Pragma("pack()")
        #define AYMO_PRAGMA_PACK_POP            _Pragma("pack(pop)")
    #else
        // Use default packing (i.e. compiler forced to 1-byte packing)
        #ifndef _include_aymo_cc_h_AYMO_PRAGMA_POP
            #define _include_aymo_cc_h_AYMO_PRAGMA_POP
            #warning "Unsupported packing directives. Please set 1-byte packing to your compiler."
        #endif
        #define AYMO_PRAGMA_PACK_PUSH_N(n)      // keep default
        #define AYMO_PRAGMA_PACK_PUSH_1         // keep default
        #define AYMO_PRAGMA_PACK_PUSH_DEFAULT   // keep default
        #define AYMO_PRAGMA_PACK_POP            // keep default
    #endif
#endif


// Aligns to some bytes.
// To be put AFTER the typename.
#ifndef AYMO_ALIGN
    #if defined(_MSC_VER)
        #define AYMO_ALIGN(n)   __declspec(align(n))
    #elif (defined(__GNUC__) || defined(__clang__))
        #define AYMO_ALIGN(n)   __attribute__((aligned(n)))
    #else
        #define AYMO_ALIGN(n)   // default
    #endif
#endif


// Sets bit-fields order as little-endian.
#ifndef AYMO_PRAGMA_SCALAR_STORAGE_ORDER_DEFAULT
    #if defined(__GNUC__) && defined(AYMO_CC_ID_GCC)
        #define AYMO_PRAGMA_SCALAR_STORAGE_ORDER_LITTLE_ENDIAN  _Pragma("scalar_storage_order little-endian")
        #define AYMO_PRAGMA_SCALAR_STORAGE_ORDER_DEFAULT        _Pragma("scalar_storage_order default")
    #else
        #define AYMO_PRAGMA_SCALAR_STORAGE_ORDER_LITTLE_ENDIAN  // default
        #define AYMO_PRAGMA_SCALAR_STORAGE_ORDER_DEFAULT        // default
    #endif
#endif


// Mark the symbol as public for shared objects (aka DLL).
#ifndef AYMO_PUBLIC
    #if (defined(AYMO_CC_HOST_WINDOWS)        )//FIXME: || defined(AYMO_CC_HOST_CYGWIN))
        // Using MSVC attribute, also supported by modern GCC/Clang.
        #ifdef AYMO_BUILD
            #define AYMO_PUBLIC     extern __declspec(dllexport)
        #else
            #define AYMO_PUBLIC     extern __declspec(dllimport)
        #endif
        #define AYMO_PRIVATE        // ignore

    #elif (defined(__GNUC__) || defined(__clang__))
        // Using GCC-specific attribute
        #ifdef AYMO_BUILD
            #define AYMO_PUBLIC     extern __attribute__((visibility("default")))
        #else
            #define AYMO_PUBLIC     // ignore
        #endif
        #define AYMO_PRIVATE        extern __attribute__((visibility("hidden")))

    #else
        #ifndef _include_aymo_cc_h_AYMO_PUBLIC
            #define _include_aymo_cc_h_AYMO_PUBLIC
            #warning "Cannot assume a proper way to declare shared object functions."
        #endif
        #define AYMO_PUBLIC         extern  // ignore
        #define AYMO_PRIVATE        extern  // ignore
    #endif
#endif


// Wrap the condition expression with this
#ifndef AYMO_LIKELY
    #if (defined(__GNUC__) || defined(__clang__))
        #define AYMO_LIKELY(x)      (__builtin_expect(!!(x), 1))
        #define AYMO_UNLIKELY(x)    (__builtin_expect(!!(x), 0))
    #else
        #define AYMO_LIKELY(x)      (x)  // ignore
        #define AYMO_UNLIKELY(x)    (x)  // ignore
    #endif
#endif


// Usual macro to get 1D array size
#ifndef AYMO_VECTOR_LENGTH
    #define AYMO_VECTOR_LENGTH(name)    (sizeof(name) / sizeof((name)[0]))
#endif


// Cheap alternative to memset()
// No care for performance; made just to avoid a library call
static inline void aymo_memset(void* data, int value, unsigned long size)
{
    char* ptr = (char*)data;
    const char* end = (char*)data + size;
    while (ptr != end) {
        *(ptr++) = value;
    }
}


// Cheap alternative to memcpy()
// No care for performance; made just to avoid a library call
static inline void aymo_memcpy(void* dst, void* src, unsigned long size)
{
    char* dstp = (char*)dst;
    const char* srcp = (const char*)src;
    const char* end = (const char*)src + size;
    while (srcp != end) {
        *(dstp++) = *(srcp++);
    }
}


// Cheap alternative to strcmp()
// No care for performance; made just to avoid a library call
static inline int aymo_strcmp(const char* a, const char* b)
{
    if (a && b) {
        do {
            if (*a != *b) {
                return (*a - *b);
            }
        } while (*(a++) && *(b++));
        return 0;
    }
    return -0x8000;
}


#endif  // _include_aymo_cc_h
