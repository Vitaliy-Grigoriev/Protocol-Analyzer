// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#ifndef PROTOCOL_ANALYZER_ARGUMENTS_HANDLERS_HPP
#define PROTOCOL_ANALYZER_ARGUMENTS_HANDLERS_HPP

#include <cstdlib>  // std::exit.
#include <iostream>  // std::cerr.

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
    void ErrorHandler (const ArgumentsParser* parser, std::string_view argumentName, std::string_view argumentValue) noexcept
    {
        if (argumentName.empty() == false)
        {
            // If inputted argument is not found in ArgumentsContainer.
            if (parser->GetArgumentEntryByName(argumentName) == nullptr)
            {
                std::cerr << "Error: Unknown inputted argument name - '" << argumentName << "'." << std::endl;
                std::exit(1);
            }
            // If argument is found and its value undefined.
            else if (argumentValue.empty() == true)
            {
                std::cerr << "Error: Value for '" << argumentName << "' argument is required." << std::endl;
                std::exit(2);
            }
            // If argument and its value are found.
            else
            {
                std::cerr << "Error: Incorrect value '" << argumentValue << "' for '" << argumentName << "' argument." << std::endl;
                std::exit(3);
            }
        }
        // If argument and its value are not found.
        else if (argumentValue.empty() == true)
        {
            for (auto&& it : parser->GetArguments())
            {
                if (it.first.isRequired == true && it.second.empty() == true)
                {
                    std::cerr << "Error: Not found required '" << it.first.argumentName << "' argument on program input." << std::endl;
                    std::exit(4);
                }
            }
        }

        std::cerr << "Error message." << std::flush << std::endl;
        std::exit(1);
    }


    /**
     * @brief Function that handles '-h' program argument.
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
        return false;
    }

}  // namespace settings.


#endif  // PROTOCOL_ANALYZER_ARGUMENTS_HANDLERS_HPP
