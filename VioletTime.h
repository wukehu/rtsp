/*

  This file is a part of JRTPLIB
  Copyright (c) 1999-2007 Jori Liesenborgs

  Contact: jori.liesenborgs@gmail.com

  This library was developed at the "Expertisecentrum Digitale Media"
  (http://www.edm.uhasselt.be), a research center of the Hasselt University
  (http://www.uhasselt.be). The library is based upon work done for 
  my thesis at the School for Knowledge Technology (Belgium/The Netherlands).

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included
  in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
  IN THE SOFTWARE.

*/

/**
 * \file VIOLETTimeutilities.h
 */

#ifndef VIOLET_TIME_H_

#define VIOLET_TIME_H_


typedef unsigned int uint32_t;
#include <string>

#ifndef WIN32
	#include <sys/time.h>
	#include <time.h>
#else
		#ifndef _WIN32_WCE
        	#include <sys/timeb.h>
        #endif // _WIN32_WINCE
	#include <winsock2.h>	
	#include <ws2tcpip.h>
	#include <sys/types.h>
#endif // WIN32

#define RTP_NTPTIMEOFFSET									2208988800UL


typedef struct{
	uint32_t year;
	uint32_t month;
	uint32_t day;
	uint32_t weekDay;
	uint32_t hour;
	uint32_t minute;
	uint32_t second;
}STTime;


/**
 * This is a simple wrapper for the most significant word (MSW) and least 
 * significant word (LSW) of an NTP timestamp.
 */
class VIOLETNTPTime
{
public:
	/** This constructor creates and instance with MSW \c m and LSW \c l. */
	VIOLETNTPTime(uint32_t m,uint32_t l)							{ msw = m ; lsw = l; }

	/** Returns the most significant word. */
	uint32_t GetMSW() const								{ return msw; }

	/** Returns the least significant word. */
	uint32_t GetLSW() const								{ return lsw; }
private:
	uint32_t msw,lsw;
};

/** This class is used to specify wallclock time, delay intervals etc.
 *  This class is used to specify wallclock time, delay intervals etc. 
 *  It stores a number of seconds and a number of microseconds.
 */
class VIOLETTime
{
public:
	/** Returns an VIOLETTime instance representing the current wallclock time. 
	 *  Returns an VIOLETTime instance representing the current wallclock time. This is expressed 
	 *  as a number of seconds since 00:00:00 UTC, January 1, 1970.
	 */
	static VIOLETTime CurrentTime(void);

	static void Current(STTime* time);

	static std::string CurrentTimeString(const char* format);

	static std::string ConvertToString(tm* p, const char* format);

	static unsigned long long ConvertToINT64(const char* str);
	
	static unsigned long long ConvertToINT64(const char* str, const char* format);

	static unsigned long RealSeconds();
	
	/** This function waits the amount of time specified in \c delay. */
	static void Wait(const VIOLETTime &delay);
		
	/** Creates an VIOLETTime instance representing \c t, which is expressed in units of seconds. */
	VIOLETTime(double t);

	/** Creates an instance that corresponds to \c ntptime. 
	 *  Creates an instance that corresponds to \c ntptime.  If
	 *  the conversion cannot be made, both the seconds and the
	 *  microseconds are set to zero.
	 */
	VIOLETTime(VIOLETNTPTime ntptime);

	/** Creates an instance corresponding to \c seconds and \c microseconds. */
	VIOLETTime(uint32_t seconds,uint32_t microseconds)					{ sec = seconds; microsec = microseconds; }

	/** Returns the number of seconds stored in this instance. */
	uint32_t GetSeconds() const										{ return sec; }

	/** Returns the number of microseconds stored in this instance. */
	uint32_t GetMicroSeconds() const								{ return microsec; }

	/** Returns the time stored in this instance, expressed in units of seconds. */
	double GetDouble() const 										{ return (((double)sec)+(((double)microsec)/1000000.0)); }

	/** Returns the NTP time corresponding to the time stored in this instance. */
	VIOLETNTPTime GetNTPTime() const;

	VIOLETTime &operator-=(const VIOLETTime &t);
	VIOLETTime &operator+=(const VIOLETTime &t);
	bool operator<(const VIOLETTime &t) const;
	bool operator>(const VIOLETTime &t) const;
	bool operator<=(const VIOLETTime &t) const;
	bool operator>=(const VIOLETTime &t) const;
private:
#if (defined(WIN32) || defined(_WIN32_WCE))
	static inline unsigned __int64 CalculateMicroseconds(unsigned __int64 performancecount,unsigned __int64 performancefrequency);
#endif // WIN32 || _WIN32_WCE

	uint32_t sec,microsec;
};

inline VIOLETTime::VIOLETTime(double t)
{
	sec = (uint32_t)t;

	double t2 = t-((double)sec);
	t2 *= 1000000.0;
	microsec = (uint32_t)t2;
}

inline VIOLETTime::VIOLETTime(VIOLETNTPTime ntptime)
{
	if (ntptime.GetMSW() < RTP_NTPTIMEOFFSET)
	{
		sec = 0;
		microsec = 0;
	}
	else
	{
		sec = ntptime.GetMSW() - RTP_NTPTIMEOFFSET;
		
		double x = (double)ntptime.GetLSW();
		x /= (65536.0*65536.0);
		x *= 1000000.0;
		microsec = (uint32_t)x;
	}
}

#if (defined(WIN32) || defined(_WIN32_WCE))

inline unsigned __int64 VIOLETTime::CalculateMicroseconds(unsigned __int64 performancecount,unsigned __int64 performancefrequency)
{
	unsigned __int64 f = performancefrequency;
	unsigned __int64 a = performancecount;
	unsigned __int64 b = a/f;
	unsigned __int64 c = a%f; // a = b*f+c => (a*1000000)/f = b*1000000+(c*1000000)/f

	return b*1000000ui64+(c*1000000ui64)/f;
}

inline VIOLETTime VIOLETTime::CurrentTime(void)
{
	static int inited = 0;
	static unsigned __int64 microseconds, initmicroseconds;
	static LARGE_INTEGER performancefrequency;

	unsigned __int64 emulate_microseconds, microdiff;
	SYSTEMTIME systemtime;
	FILETIME filetime;

	LARGE_INTEGER performancecount;

	QueryPerformanceCounter(&performancecount);
    
	if(!inited){
		inited = 1;
		QueryPerformanceFrequency(&performancefrequency);
		GetSystemTime(&systemtime);
		SystemTimeToFileTime(&systemtime,&filetime);
		microseconds = ( ((unsigned __int64)(filetime.dwHighDateTime) << 32) + (unsigned __int64)(filetime.dwLowDateTime) ) / 10ui64;
		microseconds-= 11644473600000000ui64; // EPOCH
		initmicroseconds = CalculateMicroseconds(performancecount.QuadPart, performancefrequency.QuadPart);
	}
    
	emulate_microseconds = CalculateMicroseconds(performancecount.QuadPart, performancefrequency.QuadPart);

	microdiff = emulate_microseconds - initmicroseconds;

	return VIOLETTime((uint32_t)((microseconds + microdiff) / 1000000ui64),((uint32_t)((microseconds + microdiff) % 1000000ui64)));
}

inline void VIOLETTime::Wait(const VIOLETTime &delay)
{
	DWORD t;

	t = ((DWORD)delay.GetSeconds())*1000+(((DWORD)delay.GetMicroSeconds())/1000);
	Sleep(t);
}

class VIOLETTimeInitializer
{
public:
	VIOLETTimeInitializer();
	void Dummy() { dummy++; }
private:
	int dummy;
};

extern VIOLETTimeInitializer timeinit;

#else // unix style

inline VIOLETTime VIOLETTime::CurrentTime()
{
	struct timeval tv;
	
	gettimeofday(&tv,0);
	return VIOLETTime((uint32_t)tv.tv_sec,(uint32_t)tv.tv_usec);
}

inline void VIOLETTime::Wait(const VIOLETTime &delay)
{
	struct timespec req,rem;

	req.tv_sec = (time_t)delay.sec;
	req.tv_nsec = ((long)delay.microsec)*1000;
	nanosleep(&req,&rem);
}

#endif // WIN32

inline void VIOLETTime::Current(STTime* stime)
{
	time_t timep;
	time(&timep);
	struct tm * p = localtime(&timep); 
	stime->year = p->tm_year;
	stime->month = p->tm_mon,
	stime->day = p->tm_mday;
	stime->weekDay = p->tm_wday;
	stime->hour = p->tm_hour;
	stime->minute = p->tm_min;
	stime->second = p->tm_sec;
}

#include <stdio.h>
#ifdef WIN32
	#define  snprintf sprintf_s
#endif

#define FORMAT_YYYY_MM_DD_HH_MM_SS  "YYYY-MM-DD HH:MM::SS"
#define FORMAT_YYYYMMDDHHMMSS		"YYYYMMDDHHMMSS"
inline std::string VIOLETTime::CurrentTimeString(const char* format)
{
	time_t timep;
	time(&timep);
	struct tm * p = localtime(&timep); 
	char szTime[64];
	char* fmt = NULL;
	if (strcmp(format, FORMAT_YYYY_MM_DD_HH_MM_SS) == 0)
		fmt = "%04d-%02d-%02d %02d:%02d:%02d";
	else if (strcmp(format, FORMAT_YYYYMMDDHHMMSS) == 0)
		fmt = "%04d%02d%02d%02d%02d%02d";
	else 
		fmt = "%04d-%02d-%02d %02d:%02d:%02d";
	
	snprintf(szTime, sizeof(szTime),
		     fmt, p->tm_year+1900, p->tm_mon+1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
	return szTime;
}

inline std::string VIOLETTime::ConvertToString(tm* p, const char* format)
{
	char szTime[64];
	snprintf(szTime, sizeof(szTime),
		    "%04d-%02d-%02d %02d:%02d:%02d",
		    p->tm_year+1900, p->tm_mon+1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
	return szTime;
}

inline unsigned long long VIOLETTime::ConvertToINT64(const char* str)
{
	struct tm tm_time;
	strptime(str, "%Y-%m-%d %H:%M:%S", &tm_time);
	unsigned long long dwTime = (unsigned long long)mktime(&tm_time) * 1000000;
	return dwTime;
}

inline unsigned long long VIOLETTime::ConvertToINT64(const char* str, const char* format)
{
	struct tm tm_time;
	int ret = sscanf(str, "%04d-%02d-%02d-%02d:%02d:%02d.dat", 
		   &tm_time.tm_year, &tm_time.tm_mon, &tm_time.tm_mday, &tm_time.tm_hour, &tm_time.tm_min, &tm_time.tm_sec);

	if (ret != 6)
		return 0;
	
	tm_time.tm_year -= 1900;
	tm_time.tm_mon  -= 1;
	unsigned long long dwTime = (unsigned long long)mktime(&tm_time)*1000000;
	return dwTime;
}

inline unsigned long VIOLETTime::RealSeconds()
{
	int ret = 0;
	struct timespec t;
	ret = clock_gettime(CLOCK_REALTIME, &t);
	if(ret != 0) {
		return 0;
	}
	return t.tv_sec;
}

inline VIOLETTime &VIOLETTime::operator-=(const VIOLETTime &t)
{ 
	sec -= t.sec; 
	if (t.microsec > microsec)
	{
		sec--;
		microsec += 1000000;
	}
	microsec -= t.microsec;
	return *this;
}

inline VIOLETTime &VIOLETTime::operator+=(const VIOLETTime &t)
{ 
	sec += t.sec; 
	microsec += t.microsec;
	if (microsec >= 1000000)
	{
		sec++;
		microsec -= 1000000;
	}
	return *this;
}

inline VIOLETNTPTime VIOLETTime::GetNTPTime() const
{
	uint32_t msw = sec+RTP_NTPTIMEOFFSET;
	uint32_t lsw;
	double x;
	
      	x = microsec/1000000.0;
	x *= (65536.0*65536.0);
	lsw = (uint32_t)x;

	return VIOLETNTPTime(msw,lsw);
}

inline bool VIOLETTime::operator<(const VIOLETTime &t) const
{
	if (sec < t.sec)
		return true;
	if (sec > t.sec)
		return false;
	if (microsec < t.microsec)
		return true;
	return false;
}

inline bool VIOLETTime::operator>(const VIOLETTime &t) const
{
	if (sec > t.sec)
		return true;
	if (sec < t.sec)
		return false;
	if (microsec > t.microsec)
		return true;
	return false;
}

inline bool VIOLETTime::operator<=(const VIOLETTime &t) const
{
	if (sec < t.sec)
		return true;
	if (sec > t.sec)
		return false;
	if (microsec <= t.microsec)
		return true;
	return false;
}

inline bool VIOLETTime::operator>=(const VIOLETTime &t) const
{
	if (sec > t.sec)
		return true;
	if (sec < t.sec)
		return false;
	if (microsec >= t.microsec)
		return true;
	return false;
}
#endif // VIOLETTimeUTILITIES_H

