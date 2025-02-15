#ifndef __LOGGING_H__
#define __LOGGING_H__

#include <stdio.h>

#define LOG_PRINT(_fmt, ...) (printf("[%s:%d]: " _fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))

#if DEBUG==1
	#define INFO(...) (printf(__VA_ARGS__))
	#define LOG(_fmt, ...) LOG_PRINT(_fmt, ##__VA_ARGS__)
	#define LOG_WARN(_fmt, ...) LOG_PRINT(_fmt, ##__VA_ARGS__)
	#define LOG_TRACE(_fmt, ...) LOG_PRINT(_fmt, ##__VA_ARGS__)
#else
	#define INFO(...) (printf(__VA_ARGS__))
	#define LOG(...)
	#define LOG_WARN(_fmt, ...) LOG_PRINT(_fmt, ##__VA_ARGS__)
	#define LOG_TRACE(...)
#endif

#endif // __LOGGING_H__