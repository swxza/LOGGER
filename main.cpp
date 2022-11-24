#include<iostream>
using namespace std;
#include "Logger.h"
using namespace xz::utility;


int main()
{
    
    Logger::instance()->open("./test.log");
    //Logger::instance()->level(Logger::INFO);//只保留INFO以上级别日志内容，以下的级别被丢弃
    Logger::instance()->max(1024);

    debug("HELLO WORLD");
    debug("name is %s, age is %d", "Jack", 18);
    info("info message");
    warn("warn message");
    error("error message");

    return 0;
}