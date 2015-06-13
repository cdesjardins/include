
#ifndef UNREF_PARAM

#ifndef WIN32
#define UNREF_PARAM(expr) do { (void)(expr); } while (0)
#else
#define UNREF_PARAM(expr) do { (void)(expr); } while (0,0)
#endif

#endif
