#ifndef __VIOLET_LOG_H__
#define __VIOLET_LOG_H__

#include <stdio.h>
#include <time.h>

#define __LOG_COLOR_RED 		"1;31"
#define __LOG_COLOR_GREEN 		"0;32"
#define __LOG_COLOR_BLUE 		"1;34"
#define __LOG_COLOR_YELLOW      "0;33"
#define __LOG_COLOR_WHITE 		"0;37"

#ifdef _LOG_NO_COLOR
	#define __LOG_COLOR(COLOR, context, fmt, args...) printf("[@%p]: "#fmt" \n", context, ##args)
#else
	#define __LOG_COLOR(COLOR, context, fmt, args...)  								\
	do {																			\ 	
		time_t timep;																\		
		time(&timep); 																\			
		struct tm result; 															\
		gmtime_r(&timep, &result);													\
      	printf("\033[%sm[%02d:%02d:%02d][@%s]: "#fmt"\033[m\n", COLOR, result.tm_hour, result.tm_min, result.tm_sec, context, ##args);  \
	}while(0)
		
#endif

#define _LOG_ALL

#ifdef _LOG_ALL
	#ifndef _LOG_VERBOSE
		#define _LOG_VERBOSE
	#endif
	#ifndef _LOG_WARNING
		#define _LOG_WARNING
	#endif
	#ifndef _LOG_ERROR
		#define _LOG_ERROR
	#endif
	#ifndef _LOG_INFO
		#define _LOG_INFO
	#endif
	#ifndef _LOG_DEBUG
		#define _LOG_DEBUG
	#endif
#endif

/*DISABLE COLOR WIN32*/
#if defined _WIN32 && !(defined _LOG_COLOR)
	#define _LOG_NO_COLOR
#endif

#if defined _LOG_VERBOSE && !(defined _LOG_NO_COLOR)
	#define VERBOSE(CONTEXT, FORMAT, args...) __LOG_COLOR(__LOG_COLOR_WHITE, CONTEXT, FORMAT, ##args)
#else
	#define VERBOSE(CONTEXT, FORMAT, args...)
#endif

#if defined _LOG_WARNING && !(defined _LOG_NO_COLOR)
	#define WARNING(CONTEXT, FORMAT, args...) __LOG_COLOR(__LOG_COLOR_YELLOW, CONTEXT, FORMAT, ##args)
#else
	#define WARNING(CONTEXT, FORMAT, args...)
#endif

#if defined _LOG_INFO && !(defined _LOG_NO_COLOR)
	#define INFO(CONTEXT, FORMAT, args...) __LOG_COLOR(__LOG_COLOR_GREEN, CONTEXT, FORMAT, ##args)
#else
	#define INFO(CONTEXT, FORMAT, args...)
#endif

#if defined _LOG_ERROR && !(defined _LOG_NO_COLOR)
	#define ERROR(CONTEXT, FORMAT, args...) __LOG_COLOR(__LOG_COLOR_RED, CONTEXT, FORMAT, ##args)
#else
	#define ERROR(CONTEXT, FORMAT, args...)
#endif

#if defined _LOG_DEBUG && !(defined _LOG_NO_COLOR)
	#define VDEBUG(CONTEXT, FORMAT, args...) __LOG_COLOR(__LOG_COLOR_BLUE, CONTEXT, FORMAT, ##args)
#else
	#define VDEBUG(CONTEXT, FORMAT, args...)
#endif


#endif /*__VIOLET_LOG_H__*/

