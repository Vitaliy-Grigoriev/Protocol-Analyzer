// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>

#include "../include/analyzer/Log.hpp"


int main(void)
{
    LOG_TRACE("Test TRACE ", 1, '.');
    LOG_INFO("Test INFO ", 2, '.');
    LOG_WARNING("Test WARNING ", 3, '.');
    LOG_ERROR("Test ERROR ", 4, '.');
    LOG_FATAL("Test FATAL ", 5, '.');

    std::cout << std::boolalpha << analyzer::log::Logger::Instance().SwitchLoggingEngine() << std::endl;
    std::cout << std::boolalpha << analyzer::log::Logger::Instance().SetLogFileRecordsLimit(3) << std::endl;

    LOG_TRACE("Test TRACE ", 6, '.');
    LOG_INFO("Test INFO ", 7, '.');
    LOG_WARNING("Test WARNING ", 8, '.');
    //std::cout << std::boolalpha << analyzer::log::Logger::Instance().SwitchLoggingEngine() << std::endl;
    LOG_ERROR("Test ERROR ", 9, '.');
    LOG_FATAL("Test FATAL ", 10, '.');

    std::cout << std::boolalpha << analyzer::log::Logger::Instance().ChangeLogFileName("../log/program_2.log") << std::endl;
    std::cout << std::boolalpha << analyzer::log::Logger::Instance().SwitchLoggingEngine() << std::endl;

    LOG_TRACE("Test TRACE ", 11, '.');
    LOG_INFO("Test INFO ", 12, '.');
    LOG_WARNING("Test WARNING ", 13, '.');
    LOG_ERROR("Test ERROR ", 14, '.');
    LOG_FATAL("Test FATAL ", 15, '.');

    std::cout << std::boolalpha << analyzer::log::Logger::Instance().SwitchLoggingEngine() << std::endl;

    LOG_TRACE("Test TRACE ", 16, '.');
    LOG_INFO("Test INFO ", 17, '.');
    LOG_WARNING("Test WARNING ", 18, '.');
    LOG_ERROR("Test ERROR ", 19, '.');
    LOG_FATAL("Test FATAL ", 20, '.');

    std::cout << std::boolalpha << analyzer::log::Logger::Instance().SwitchLoggingEngine() << std::endl;
    std::cout << std::boolalpha << analyzer::log::Logger::Instance().SetLogFileRecordsLimit(4) << std::endl;
    std::cout << std::boolalpha << analyzer::log::Logger::Instance().ChangeLogFileName("../log/program") << std::endl;

    LOG_TRACE("Test TRACE ", 21, '.');
    LOG_INFO("Test INFO ", 22, '.');
    LOG_WARNING("Test WARNING ", 23, '.');
    LOG_ERROR("Test ERROR ", 24, '.');
    LOG_FATAL("Test FATAL ", 25, '.');

    std::cout << std::boolalpha << analyzer::log::Logger::Instance().ChangeLogFileName("../log/program") << std::endl;

    LOG_TRACE("Test TRACE ", 26, '.');
    LOG_INFO("Test INFO ", 27, '.');
    LOG_WARNING("Test WARNING ", 28, '.');
    LOG_ERROR("Test ERROR ", 29, '.');
    LOG_FATAL("Test FATAL ", 30, '.');

    std::cout << std::boolalpha << analyzer::log::Logger::Instance().SwitchLoggingEngine() << std::endl;
    std::cout << std::boolalpha << analyzer::log::Logger::Instance().SetLogFileRecordsLimit(7) << std::endl;
    std::cout << std::boolalpha << analyzer::log::Logger::Instance().ChangeLogFileName("../log/program_1.log") << std::endl;

    LOG_TRACE("Test TRACE ", 31, '.');
    LOG_INFO("Test INFO ", 32, '.');
    LOG_WARNING("Test WARNING ", 33, '.');
    LOG_ERROR("Test ERROR ", 34, '.');
    LOG_FATAL("Test FATAL ", 35, '.');

    std::cout << std::boolalpha << analyzer::log::Logger::Instance().SwitchLoggingEngine() << std::endl;

    LOG_TRACE("Test TRACE ", 36, '.');
    LOG_INFO("Test INFO ", 37, '.');
    LOG_WARNING("Test WARNING ", 38, '.');
    LOG_ERROR("Test ERROR ", 39, '.');
    LOG_FATAL("Test FATAL ", 40, '.');

    return EXIT_SUCCESS;
}


