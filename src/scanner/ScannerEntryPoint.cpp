// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#include "../../include/scanner/ArgumentsHandlers.hpp"  // settings::ArgumentsParser, settings::HelpHandler.

using namespace analyzer::scanner;
using namespace analyzer::framework;


int32_t main (const int32_t size, const char* const* const data)
{
    log::Logger::Instance().SwitchLoggingEngine();

    settings::ArgumentsParser arguments;
    arguments.Add("-h", "--help", settings::HelpHandler, false);
    arguments.Add("-c", "--config", settings::ProgramConfigHandler, true, ARG_SINGLE);
    arguments.Add("-p", "--protocol", nullptr, true, ARG_SINGLE);
    arguments.Add("-v", "--verbose", settings::LoggingHandler, false);
    arguments.Add("-o", "--output", nullptr, false, ARG_SINGLE);
    arguments.Add("-V", "--volume", nullptr, false, ARG_SINGLE);
    arguments.Add("-t1", "--test1", nullptr, false, ARG_MULTIPLE);
    arguments.Add("-t2", "--test2", nullptr, false, ARG_SINGLE);
    arguments.Add("-t3", "--test3", nullptr, false, ARG_ZERO_OR_MORE);
    arguments.Add("-t4", "--test4", nullptr, false, ARG_ZERO);
    arguments.Add("-t5", "--test5", nullptr, false, ARG_DOUBLE);

    arguments.AddActionOnError(settings::DefaultErrorHandler);

    if (arguments.Parse(data, static_cast<std::size_t>(size)) == false)
    {
        LOG_FATAL("main: Error occurred while parsing program arguments.");
        return EXIT_FAILURE;
    }

    std::cout << arguments << std::endl;

    return EXIT_SUCCESS;
}

