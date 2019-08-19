#ifndef _COMMON_H_
#define _COMMON_H_
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#define Logi(fmt, ...) {time_t timep; time(&timep); struct tm *p = localtime(&timep); printf("%4d-%02d-%02d %02d:%02d:%02d [%u][I] ", 1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p-> tm_hour, p->tm_min, p->tm_sec, pthread_self());printf(fmt, ##__VA_ARGS__);printf("\n");fflush(stdout);}
#define Logd(fmt, ...) {time_t timep; time(&timep); struct tm *p = localtime(&timep); printf("%4d-%02d-%02d %02d:%02d:%02d [%u][D] ", 1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p-> tm_hour, p->tm_min, p->tm_sec, pthread_self());printf(fmt, ##__VA_ARGS__);printf("\n");fflush(stdout);}
#define Logw(fmt, ...) {time_t timep; time(&timep); struct tm *p = localtime(&timep); printf("%4d-%02d-%02d %02d:%02d:%02d [%u][W] ", 1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p-> tm_hour, p->tm_min, p->tm_sec, pthread_self());printf(fmt, ##__VA_ARGS__);printf("\n");fflush(stdout);}
#define Loge(fmt, ...) {time_t timep; time(&timep); struct tm *p = localtime(&timep); printf("%4d-%02d-%02d %02d:%02d:%02d [%u][E] ", 1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p-> tm_hour, p->tm_min, p->tm_sec, pthread_self());printf(fmt, ##__VA_ARGS__);printf("\n");fflush(stdout);}

#endif