#ifndef _COMMON_COMMON_H_
#define _COMMON_COMMON_H_
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <stdio.h>

typedef unsigned char		uchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned long		ulong;
typedef unsigned long long	ullong;

#define LOGI(fmt, ...)  \
{  \
time_t timep;  \
time(&timep);  \
struct tm *p = localtime(&timep);  \
fprintf(stderr, "[%4d-%02d-%02d %02d:%02d:%02d] INFO ", \
1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p-> tm_hour, p->tm_min, p->tm_sec \
);  \
fprintf(stderr, fmt, ##__VA_ARGS__);  \
fprintf(stderr, "\n");  \
}   \


#define LOG_FATAL     LogMessageFatal(__FILE__, __LINE__)
#define LOG_INFO      LogMessage(__FILE__, __LINE__, 0)
#define LOG_ERROR     LogMessage(__FILE__, __LINE__, 1)
#define LOG_WARNING   LogMessage(__FILE__, __LINE__, 2)
#define LOG(Lv)       LOG_##Lv.stream()


#define CHECK(x)     \
if (!(x))            \
LogMessageFatal(__FILE__, __LINE__).stream() << "Check failed: " #x << ' '


class DateLogger 
{
public:
    DateLogger() {}

    const char* HumanDate() 
    {
        time_t time_value = time(NULL);
        struct tm *pnow;
        pnow = localtime(&time_value);

        snprintf(_buffer, sizeof(_buffer), "%4d-%02d-%02d %02d:%02d:%02d",
        1900+pnow->tm_year, 1+pnow->tm_mon, pnow->tm_mday, pnow-> tm_hour, pnow->tm_min, pnow->tm_sec);
        
        return _buffer;
    }

private:
    char _buffer[20];
};

class LogMessage 
{
public:
	LogMessage(const char* file, int line, int lv)
      : _stream(std::cerr)
	{
        std::string info;
        if(lv == 0) info = "INFO ";
        if(lv == 1) info = "WARNING ";
        if(lv == 2) info = "ERROR ";
	    _stream << "[" << _date.HumanDate() << "] " << info << file << ":" << line << ": ";
	}
	~LogMessage() 
    {
        _stream << '\n'; 
    }

	std::ostream& stream() 
    {
        return _stream;
    }	

private:
    std::ostream& _stream;
    DateLogger _date;

	LogMessage(const LogMessage&);
	void operator=(const LogMessage&);
};


struct Error : public std::runtime_error 
{
	explicit Error(const std::string &s) 
      : std::runtime_error(s) 
	{}
};

class LogMessageFatal 
{
public:
	LogMessageFatal(const char* file, int line)
    {
	    _stream << "[" << _date.HumanDate() << "] " << file << ":" << line << ": ";
	}
	//~LogMessageFatal() noexcept(false) //C++11 >> no catch
    ~LogMessageFatal()  //try catch
    {
		LOG(ERROR) << _stream.str();
		throw Error(_stream.str());
	}

	std::ostringstream &stream()
    {
        return _stream;
    }

private:
    std::ostringstream _stream;
    DateLogger _date;
	
	LogMessageFatal(const LogMessageFatal&);
	void operator=(const LogMessageFatal&);
};


#endif