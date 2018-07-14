// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#include <iostream>

#include "../include/framework/Log.hpp"


int32_t main (int32_t size, char** data)
{
    // File 'program_volume1.log' --- 5
    LOG_TRACE("Test TRACE ", 1, '.');
    LOG_INFO("Test INFO ", 2, '.');
    LOG_WARNING("Test WARNING ", 3, '.');
    LOG_ERROR("est ERROR ", 4, '.');
    LOG_FATAL("Test FATAL ", 5, '.');

    std::cerr << std::boolalpha << analyzer::log::Logger::Instance().SwitchLoggingEngine() << std::endl;
    std::cerr << std::boolalpha << analyzer::log::Logger::Instance().SetLogFileRecordsLimit(3) << std::endl;
    // File 'program_volume2.log' --- 0

    // Monitor
    LOG_TRACE("Test TRACE ", 6, '.');
    LOG_INFO("Test INFO ", 7, '.');
    LOG_WARNING("Test WARNING ", 8, '.');
    LOG_ERROR("Test ERROR ", 9, '.');
    LOG_FATAL("Test FATAL ", 10, '.');

    std::cerr << std::boolalpha << analyzer::log::Logger::Instance().ChangeLogFileName("../log/program_2.log") << std::endl;
    // File 'program_2_volume1.log' --- 0
    std::cerr << std::boolalpha << analyzer::log::Logger::Instance().SwitchLoggingEngine() << std::endl;

    // Monitor
    LOG_TRACE("Test TRACE ", 11, '.');
    LOG_INFO("Test INFO ", 12, '.');
    LOG_WARNING("Test WARNING ", 13, '.');
    LOG_ERROR("Test ERROR ", 14, '.');
    LOG_FATAL("Test FATAL ", 15, '.');

    std::cerr << std::boolalpha << analyzer::log::Logger::Instance().SwitchLoggingEngine() << std::endl;

    // File 'program_2_volume1.log' --- 3
    LOG_TRACE("Test TRACE ", 16, '.');
    LOG_INFO("Test INFO ", 17, '.');
    LOG_WARNING("Test WARNING ", 18, '.');
    // File 'program_2_volume2.log' --- 2
    LOG_ERROR("Test ERROR ", 19, '.');
    LOG_FATAL("Test FATAL ", 20, '.');

    std::cerr << std::boolalpha << analyzer::log::Logger::Instance().SetLogFileRecordsLimit(4) << std::endl;

    // File 'program_2_volume2.log' --- 4
    LOG_TRACE("Test TRACE ", 21, '.');
    LOG_INFO("Test INFO ", 22, '.');
    // File 'program_2_volume3.log' --- 3
    LOG_WARNING("Test WARNING ", 23, '.');
    LOG_ERROR("Test ERROR ", 24, '.');
    LOG_FATAL("Test FATAL ", 25, '.');

    std::cerr << std::boolalpha << analyzer::log::Logger::Instance().ChangeLogFileName("../log/program_1") << std::endl;

    // File 'program_1_volume1.log' --- 4
    LOG_TRACE("Test TRACE ", 26, '.');
    LOG_INFO("Test INFO ", 27, '.');
    LOG_WARNING("Test WARNING ", 28, '.');
    LOG_ERROR("Test ERROR ", 29, '.');
    // File 'program_1_volume2.log' --- 1
    LOG_FATAL("Test FATAL ", 30, '.');

    std::cerr << std::boolalpha << analyzer::log::Logger::Instance().SwitchLoggingEngine() << std::endl;
    std::cerr << std::boolalpha << analyzer::log::Logger::Instance().SetLogFileRecordsLimit(7) << std::endl;
    std::cerr << std::boolalpha << analyzer::log::Logger::Instance().ChangeLogFileName("../log/program_1.log") << std::endl;

    // File 'program_1_volume1.log' --- 7
    LOG_TRACE("Test TRACE ", 31, '.');
    LOG_INFO("Test INFO ", 32, '.');
    // File 'program_1_volume2.log' --- 5
    LOG_WARNING("Test WARNING ", 33, '.');
    LOG_ERROR("Test ERROR ", 34, '.');
    LOG_FATAL("Test FATAL ", 35, '.');

    std::cerr << std::boolalpha << analyzer::log::Logger::Instance().SwitchLoggingEngine() << std::endl;

    // Monitor
    LOG_TRACE("Test TRACE ", 36, '.');
    std::cerr << std::boolalpha << analyzer::log::Logger::Instance().SwitchLoggingEngine() << std::endl;
    // File 'program_1_volume2.log' --- 7
    LOG_INFO("Test INFO ", 37, '.');
    LOG_WARNING("Test WARNING ", 38, '.');
    // File 'program_1_volume3.log' --- 2
    LOG_ERROR("Test ERROR ", 39, '.');
    LOG_FATAL("Test FATAL ", 40, '.');

    std::cerr << std::boolalpha << analyzer::log::Logger::Instance().SetLogFileRecordsLimit(3) << std::endl;
    std::cerr << std::boolalpha << analyzer::log::Logger::Instance().ChangeLogFileName("../log/program_2.log") << std::endl;

    // File 'program_2_volume4.log' --- 3
    LOG_TRACE("Test TRACE ", 41, '.');
    LOG_INFO("Test INFO ", 42, '.');
    LOG_WARNING("Test WARNING ", 43, '.');
    // File 'program_2_volume5.log' --- 2
    LOG_ERROR("Test ERROR ", 44, '.');
    LOG_FATAL("Test FATAL ", 45, '.');

    std::cerr << std::boolalpha << analyzer::log::Logger::Instance().SwitchLoggingEngine() << std::endl;

    // Monitor
    LOG_TRACE("Test TRACE ", 46, '.');
    LOG_INFO("Test INFO ", 47, '.');

    std::cerr << std::boolalpha << analyzer::log::Logger::Instance().SetLogFileRecordsLimit(5) << std::endl;
    std::cerr << std::boolalpha << analyzer::log::Logger::Instance().ChangeLogFileName("../log/program_2.log") << std::endl;

    // File 'program_2_volume1.log' --- 5
    LOG_TRACE("Test TRACE ", 48, '.');
    // File 'program_2_volume3.log' --- 5
    LOG_INFO("Test INFO ", 49, '.');
    // File 'program_2_volume4.log' --- 5
    LOG_WARNING("Test WARNING ", 50, '.');
    // File 'program_2_volume5.log' --- 5
    LOG_ERROR("Test ERROR ", 51, '.');
    LOG_FATAL("Test FATAL ", 52, '.');

    std::cerr << std::boolalpha << analyzer::log::Logger::Instance().SwitchLoggingEngine() << std::endl;
    std::cerr << std::boolalpha << analyzer::log::Logger::Instance().SetLogFileRecordsLimit(4) << std::endl;
    std::cerr << std::boolalpha << analyzer::log::Logger::Instance().ChangeLogFileName("../log/program_1.log") << std::endl;

    // File 'program_1_volume3.log' --- 4
    LOG_TRACE("Test TRACE ", 53, '.');
    // File 'program_1_volume4.log' --- 4
    LOG_INFO("Test INFO ", 54, '.');
    LOG_WARNING("Test WARNING ", 55, '.');
    LOG_ERROR("Test ERROR ", 56, '.');
    LOG_FATAL("Test FATAL ", 57, '.');
    // File 'program_1_volume5.log' --- 0

    return EXIT_SUCCESS;
}


