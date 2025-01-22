#ifndef _LOG_H_
#define _LOG_H_

#include <iostream>

#define OPEN_LOG

#if defined(OPEN_LOG)
    #define LOG_INFO(...)   std::cout<<"[INFO]: ";  Log(__VA_ARGS__);
    #define LOG_DEBUG(...)  std::clog<<"[DEBUG]: "; Log(__VA_ARGS__);
    #define LOG_ERROR(...)  std::cerr<<"[ERROR]: "; Log(__VA_ARGS__);

    template<typename... Args>
    void Log(Args... args)
    {
        ((std::cout<<args), ...) << std::endl;
    }

#else
    #define LOG_INFO(...)   ;
    #define LOG_DEBUG(...)  ;
    #define LOG_ERROR(...)  ;

#endif

#endif //! _LOG_H_
