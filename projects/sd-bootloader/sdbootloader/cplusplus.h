#ifndef _CPLUSPLUS
#define _CPLUSPLUS

#include <stdlib.h> 

inline void * operator new(size_t size) { return malloc(size); }

inline void operator delete(void * ptr) { free(ptr); }

inline void * operator new[](size_t size) 
{ 
    return malloc(size); 
} 

inline void operator delete[](void * ptr) 
{ 
    free(ptr); 
}

__extension__ typedef int __guard __attribute__((mode (__DI__))); 

extern "C" int __cxa_guard_acquire(__guard *); 
extern "C" void __cxa_guard_release (__guard *); 
extern "C" void __cxa_guard_abort (__guard *); 
extern "C" void __cxa_pure_virtual(void); 

#endif
