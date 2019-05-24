// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#ifndef PROTOCOL_ANALYZER_ARGUMENTS_HANDLERS_HPP
#define PROTOCOL_ANALYZER_ARGUMENTS_HANDLERS_HPP

#include <cstdlib>  // std::exit.
#include <iostream>  // std::cerr.

#include <Log.hpp>  // MACROS LOG_*.

#include "../../include/scanner/ArgumentsParser.hpp"  // std::string_view, settings::ArgumentsParser.


namespace analyzer::scanner::settings
{
    /**
     * @brief Function that handles error on incorrect program input.
     *
     * @param [in] argumentName - Inputted argument's name.
     * @param [in] argumentValue - Inputted argument's value.
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

        log::Logger::Instance().SetLogLevel(log::LEVEL::TRACE);

        return true;
    }

}  // namespace settings.


#endif  // PROTOCOL_ANALYZER_ARGUMENTS_HANDLERS_HPP
