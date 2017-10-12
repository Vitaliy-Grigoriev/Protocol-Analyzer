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

    std::cout << std::boolalpha << analyzer::log::Logging::Instance().SwitchLoggingEngine() << std::endl;

    LOG_TRACE("Test TRACE ", 6, '.');
    LOG_INFO("Test INFO ", 7, '.');
    LOG_WARNING("Test WARNING ", 8, '.');
    LOG_ERROR("Test ERROR ", 9, '.');
    LOG_FATAL("Test FATAL ", 10, '.');

    std::cout << std::boolalpha << analyzer::log::Logging::Instance().ChangeLogFileName("../log/program_1.log") << std::endl;
    std::cout << std::boolalpha << analyzer::log::Logging::Instance().SwitchLoggingEngine() << std::endl;

    LOG_TRACE("Test TRACE ", 11, '.');
    LOG_INFO("Test INFO ", 12, '.');
    LOG_WARNING("Test WARNING ", 13, '.');
    LOG_ERROR("Test ERROR ", 14, '.');
    LOG_FATAL("Test FATAL ", 15, '.');

    std::cout << std::boolalpha << analyzer::log::Logging::Instance().SwitchLoggingEngine() << std::endl;

    LOG_TRACE("Test TRACE ", 16, '.');
    LOG_INFO("Test INFO ", 17, '.');
    LOG_WARNING("Test WARNING ", 18, '.');
    LOG_ERROR("Test ERROR ", 19, '.');
    LOG_FATAL("Test FATAL ", 10, '.');

    return EXIT_SUCCESS;
}


