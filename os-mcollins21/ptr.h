#if defined(__SIZEOF_POINTER__) && __SIZEOF_POINTER__ == 4
typedef unsigned long uintptr_t;
#elif defined(__SIZEOF_POINTER__) && __SIZEOF_POINTER__ == 8
typedef unsigned long long uintptr_t;
#else
#error "unsupported pointer size"
#endif
