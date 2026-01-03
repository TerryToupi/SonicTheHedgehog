#pragma once

#include <cstdio>
#include <cstdlib>

#if not defined(NDEBUG)
	#define ASSERT(check, msg)	do { \
									if (!(check)) \
									{ \
										std::fprintf(stderr, "[Engine Assert] FAILED: %s\nMessage: %s\nFile: %s\nLine: %d\n", \
													#check, msg, __FILE__, __LINE__); \
										std::abort(); \
									} \
								} while(false)
#else
	#define ASSERT(check, msg)
#endif