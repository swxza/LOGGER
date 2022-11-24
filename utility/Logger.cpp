#include "Logger.h"
using namespace xz::utility;

#include <time.h>
#include <string.h>
#include <stdexcept>
#include <iostream>
#include <stdarg.h>
using namespace std;

const char *Logger:: s_level[LEVEL_COUNT] = {
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"
};

//通过指针保存保存
Logger * Logger::m_instance = NULL;

Logger::Logger() : m_level(DEBUG), m_max(0), m_len(0)
{

}

Logger:: ~Logger()
{
    close();
}

Logger * Logger::instance()
{
    if(m_instance == NULL)
    {
        m_instance = new Logger();
    }
    return m_instance;
}

void Logger::open(const string & filename)
{
    m_filename = filename;
    m_fout.open(filename, ios::app);
    if(m_fout.fail())
    {
        throw std::logic_error("open file failed" + filename);
    }
    //获取长度
    m_fout.seekp(0, ios::end);
    m_len = m_fout.tellp();

}

void Logger::close()
{
    m_fout.close();
}

void Logger::log(Level level, const char * file, int line, const char * format, ...)
{
    if(m_level > level)
    {
        return;
    }
    if(m_fout.fail())
    {
        throw std::logic_error("open file failed" + m_filename);
    }

    time_t ticks = time(NULL);   //时间戳
    struct tm * ptm = localtime(&ticks);
    char timestamp[32];   //传到这个数组里
    memset(timestamp, 0, sizeof(timestamp));
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", ptm);

    //做一个格式化
    const char * fmt = "%s %s %s:%d";
    int size = snprintf(NULL, 0, fmt, timestamp, s_level[level], file, line);
    if(size > 0)
    {
        char * buffer = new char[size + 1];
        snprintf(buffer, size+1, fmt, timestamp, s_level[level], file, line);
        buffer[size] = 0;
        cout << buffer << endl;
        m_fout << buffer << " ";
        m_len += size;
        delete buffer;
    }
    
    va_list arg_ptr;
    va_start(arg_ptr, format);
    size = vsnprintf(NULL, 0, format, arg_ptr);
    va_end(arg_ptr);
    if(size > 0)
    {
        char * content = new char[size + 1];
        va_start(arg_ptr, format);
        vsnprintf(content, size+1, format, arg_ptr);
        va_end(arg_ptr);
        cout << content <<endl;
        m_fout << content;
        m_len += size;
        delete content;
    }
    m_fout << "\n";
    m_fout.flush();

    if(m_len >= m_max && m_max >0)
    {
        rotate();
    }
}


void Logger::rotate()
{
    close();    //关闭掉打开的文件
    time_t ticks = time(NULL);    //获取当前的时间戳    通过时间戳做一个备份
    struct tm* ptm = localtime(&ticks);
    char timestamp[32];
    memset(timestamp, 0, sizeof(timestamp));
    strftime(timestamp, sizeof(timestamp), ".%Y_%m_%d_%H_%M_%S", ptm);
    string filename = m_filename + timestamp;
    if(rename(m_filename.c_str(), filename.c_str()) != 0)     //备份到一个新的文件中
    {
        throw std::logic_error("rename log file failed:" + string(strerror(errno)));
    }
    open(m_filename);

}