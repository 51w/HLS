#ifndef _COMMON_H_
#define _COMMON_H_
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include <iostream>
#include <sstream>


typedef unsigned char		uchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned long		ulong;
typedef unsigned long long	ullong;


#define Logi(fmt, ...) {time_t timep; time(&timep); struct tm *p = localtime(&timep); printf("%4d-%02d-%02d %02d:%02d:%02d [%u][I] ", 1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p-> tm_hour, p->tm_min, p->tm_sec, pthread_self());printf(fmt, ##__VA_ARGS__);printf("\n");fflush(stdout);}
#define Logd(fmt, ...) {time_t timep; time(&timep); struct tm *p = localtime(&timep); printf("%4d-%02d-%02d %02d:%02d:%02d [%u][D] ", 1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p-> tm_hour, p->tm_min, p->tm_sec, pthread_self());printf(fmt, ##__VA_ARGS__);printf("\n");fflush(stdout);}
#define Logw(fmt, ...) {time_t timep; time(&timep); struct tm *p = localtime(&timep); printf("%4d-%02d-%02d %02d:%02d:%02d [%u][W] ", 1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p-> tm_hour, p->tm_min, p->tm_sec, pthread_self());printf(fmt, ##__VA_ARGS__);printf("\n");fflush(stdout);}
#define Loge(fmt, ...) {time_t timep; time(&timep); struct tm *p = localtime(&timep); printf("%4d-%02d-%02d %02d:%02d:%02d [%u][E] ", 1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p-> tm_hour, p->tm_min, p->tm_sec, pthread_self());printf(fmt, ##__VA_ARGS__);printf("\n");fflush(stdout);}


class DateLogger 
{
public:
    DateLogger() 
    {
    #if defined(_MSC_VER)
        _tzset();
    #endif
    }

    const char* HumanDate() 
    {
    #if defined(_MSC_VER)
        _strtime_s(buffer_, sizeof(buffer_));
    #else
        time_t time_value = time(NULL);
        struct tm *pnow;
        #if !defined(_WIN32)
        struct tm now;
        pnow = localtime_r(&time_value, &now);
        #else
        pnow = localtime(&time_value);  // NOLINT(*)
        #endif
        snprintf(buffer_, sizeof(buffer_), "%02d:%02d:%02d",
        pnow->tm_hour, pnow->tm_min, pnow->tm_sec);
    #endif
    return buffer_;
    }

private:
    char buffer_[9];
};


#endif