// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#ifndef PROTOCOL_ANALYZER_ARGUMENTS_HANDLERS_HPP
#define PROTOCOL_ANALYZER_ARGUMENTS_HANDLERS_HPP

#include <cstdlib>  // std::exit.
#include <iostream>  // std::cerr.

#include <Log.hpp>  // MACROS LOG_*.
#include <Common.hpp>  // text::isNumber.

#include "../../include/scanner/ArgumentsParser.hpp"  // std::string_view, settings::ArgumentsParser.


namespace analyzer::scanner::settings
{
    /**
     * @brief Function that handles error on incorrect program input.
     *
     * @param [in] parser - Pointer to ArgumentsParser object which calls error handler.
     * @param [in] argumentName - Inputted argument's name.
     * @param [in] argumentValue - Inputted argument's value.
     * @param [in] reason - Reason of occurred error (ERROR_REASONS).
     * @return Exit with error code from the program with help message.
     */
    [[noreturn]]
    void DefaultErrorHandler (const ArgumentsParser* parser, std::string_view argumentName, std::string_view argumentValue, const uint16_t reason) noexcept
    {
        switch (reason)
        {
            case ERROR_ARGUMENT_NOT_FOUND:
                std::cerr << "Error: Unknown program argument '" << argumentName << "'." << std::endl;
                std::exit(1);
            case ERROR_VALUE_NOT_FOUND:
                std::cerr << "Error: Value for '" << argumentName << "' argument is required." << std::endl;
                std::exit(2);
            case ERROR_VALUE_INCORRECT:
                std::cerr << "Error: Incorrect value '" << argumentValue << "' for '" << argumentName << "' argument." << std::endl;
                std::exit(3);
            case ERROR_REQUIRED_ARGUMENTS_NOT_APPEARED:
                for (auto&& it : parser->GetArguments())
                {
                    if (it.first.isRequired == true && it.second.empty() == true) {
                        std::cerr << "Error: Not found required argument '" << it.first.argumentName << "' on program input." << std::endl;
                    }
                }
                std::exit(4);
            case ERROR_INCORRECT_NUMBER_OF_INPUTTED_VALUES:
                std::cerr << "Error: Incorrect number of values for argument '" << argumentName << "'." << std::endl;
                std::exit(5);
            default:
                std::cerr << "Unknown Error." << std::flush << std::endl;
                std::exit(1);
        }
    }


    /**
     * @brief Function that handles '-h' ('--help') program argument.
     *
     * @param [in] argumentValue - Inputted argument's value.
     * @return Normally exit from the program with help message.
     */
    bool HelpHandler ([[maybe_unused]] std::string_view /*argumentValue*/) noexcept
    {
        std::cerr << "Help message." << std::flush << std::endl;
        std::exit(0);
    }


    /**
     * @brief Function that handles '-c' ('--config') program argument.
     *
     * @param [in] argumentValue - Inputted argument's value.
     * @return TRUE - if program config parses correctly and valid, otherwise - FALSE.
     */
    bool ProgramConfigHandler (std::string_view argumentValue) noexcept
    {
        return true;
    }

    /**
     * @brief Function that handles '-v' ('--verbose') program argument.
     *
     * @param [in] argumentValue - Inputted argument's value.
     * @return TRUE - if program verbose level parses correctly and valid, otherwise - FALSE.
     */
    bool LoggingHandler (std::string_view argumentValue) noexcept
    {
        using namespace analyzer::framework;

        if (argumentValue.empty() == true)
        {
            log::Logger::Instance().SetLogLevel(log::LEVEL::INFORMATION);
            LOG_INFO("ArgumentsParser.LoggingHandler: Logger changed to INFORMATION level.");
            return true;
        }

        if (argumentValue.size() != 1) { return false; }
        if (common::text::isNumber(argumentValue[0]) == false) { return false; }

        switch (static_cast<uint16_t>(argumentValue[0]) - 0x30)
        {
            case log::LEVEL::FATAL:
                LOG_MAJOR("ArgumentsParser.LoggingHandler: Logger changed to FATAL level.");
                log::Logger::Instance().SetLogLevel(log::LEVEL::FATAL);
                break;
            case log::LEVEL::ERROR:
                LOG_MAJOR("ArgumentsParser.LoggingHandler: Logger changed to ERROR level.");
                log::Logger::Instance().SetLogLevel(log::LEVEL::ERROR);
                break;
            case log::LEVEL::WARNING:
                LOG_MAJOR("ArgumentsParser.LoggingHandler: Logger changed to WARNING level.");
                log::Logger::Instance().SetLogLevel(log::LEVEL::WARNING);
                break;
            case log::LEVEL::MAJOR:
                LOG_MAJOR("ArgumentsParser.LoggingHandler: Logger changed to MAJOR level.");
                log::Logger::Instance().SetLogLevel(log::LEVEL::MAJOR);
                break;
            case log::LEVEL::INFORMATION:
                LOG_MAJOR("ArgumentsParser.LoggingHandler: Logger changed to INFORMATION level.");
                log::Logger::Instance().SetLogLevel(log::LEVEL::INFORMATION);
                break;
            case log::LEVEL::TRACE:
                LOG_MAJOR("ArgumentsParser.LoggingHandler: Logger changed to TRACE level.");
                log::Logger::Instance().SetLogLevel(log::LEVEL::TRACE);
                break;
            default:
                LOG_MAJOR("ArgumentsParser.LoggingHandler: Logger changed to MAJOR level by default.");
                log::Logger::Instance().SetLogLevel(log::LEVEL::MAJOR);
        }
        return true;
    }

}  // namespace settings.


#endif  // PROTOCOL_ANALYZER_ARGUMENTS_HANDLERS_HPP
