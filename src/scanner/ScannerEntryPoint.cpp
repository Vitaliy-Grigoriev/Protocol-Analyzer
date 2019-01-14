// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#include <Log.hpp>  // MACROS LOG_*.

#include "../../include/scanner/ArgumentsHandlers.hpp"  // settings::ArgumentsParser, settings::HelpHandler.

using namespace analyzer::scanner;
using namespace analyzer::framework;


int32_t main (const int32_t size, const char* const* const data)
{
    log::Logger::Instance().SetLogLevel(log::LEVEL::TRACE);
    log::Logger::Instance().SwitchLoggingEngine();

    settings::ArgumentsParser arguments;
    arguments.Add("-h", "--help", settings::HelpHandler, false, false);
    arguments.Add("-c", "--config", settings::ProgramConfigHandler, true, true);
    arguments.Add("-p", "--protocol", nullptr, true, true);

    arguments.AddActionOnError(settings::ErrorHandler);

    if (arguments.Parse(data, static_cast<std::size_t>(size)) == false)
    {
        LOG_FATAL("main: Error occurred while parsing program arguments.");
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}

