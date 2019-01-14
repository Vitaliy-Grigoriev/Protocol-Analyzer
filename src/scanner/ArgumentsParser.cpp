// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#include <Log.hpp>  // MACROS LOG_*.

#include "../../include/scanner/ArgumentsParser.hpp"


namespace analyzer::scanner::settings
{
    // Method that adds new expected argument at the program input and its handler.
    void ArgumentsParser::Add (std::string_view argumentName,
                               std::string_view aliasName,
                               const ArgumentHandler handler,
                               const bool isRequired,
                               const bool isValueExpected) noexcept
    {
        if (argumentName.size() >= ARGUMENT_NAME_LENGTH)
        {
            LOG_FATAL("ArgumentsParser.Add: Wrong length of the program argument name. Valid value is less then ", ARGUMENT_NAME_LENGTH, '.');
            std::terminate();
        }

        if (aliasName.size() >= ARGUMENT_ALIAS_NAME_LENGTH)
        {
            LOG_FATAL("ArgumentsParser.Add: Wrong length of the program argument alias name. Valid value is less then ", ARGUMENT_ALIAS_NAME_LENGTH, '.');
            std::terminate();
        }

        if (isRequired == true) {
            numberOfRequiredArguments++;
        }

        argumentsStorage.emplace_back(ProgramArgumentEntry(isRequired, isValueExpected, argumentName, aliasName, handler), "");
    }


    // Method that returns the pointer to ArgumentEntry structure by specified argument name.
    const ArgumentsParser::ArgumentEntry* ArgumentsParser::GetArgumentEntryByName (std::string_view argumentName) const noexcept
    {        
        ArgumentsContainer::const_iterator it = std::find_if(argumentsStorage.begin(), argumentsStorage.end(),
            [argumentName] (const ArgumentEntry& entry) -> bool
            {
                if (argumentName == entry.first.argumentName || argumentName == entry.first.aliasName) {
                    return true;
                }
                return false;
            });
        return (it != argumentsStorage.end() ? &(*it) : nullptr);
    }


    // Method that parses and handles arguments on program input.
    bool ArgumentsParser::Parse (const char* const* const arguments, const std::size_t count) noexcept
    {
        // If no any arguments on program input but some of them are expected.
        if (count <= 1 && numberOfRequiredArguments > 0)
        {
            if (errorHandler != nullptr) {
                errorHandler(this, "", "");
            }
            return false;
        }

        // Number of required parameters which appeared on input.
        uint16_t numberOfInputtedRequiredArguments = 0;

        // First element - program name, skip it.
        for (std::size_t idx = 1; idx < count; ++idx)
        {
            // Search correct argument entry in ArgumentsContainer.
            const std::string_view arg(arguments[idx]);
            ArgumentsContainer::iterator it = std::find_if(argumentsStorage.begin(), argumentsStorage.end(),
                [arg] (const ArgumentEntry& entry) -> bool
                {
                    if (arg == entry.first.argumentName || arg == entry.first.aliasName) {
                        return true;
                    }
                    return false;
                });

            // If entry found.
            if (it != argumentsStorage.end())
            {
                const ProgramArgumentEntry& entry = it->first;

                if (entry.isRequired == true) {
                    numberOfInputtedRequiredArguments++;
                }

                // If argument's value is expected.
                if (entry.isValueExpected == true)
                {
                    if (idx + 1 < count)
                    {
                        const std::string_view nextParam(arguments[++idx]);
                        // If next parameter is an argument.
                        if (nextParam[0] == '-')
                        {
                            if (errorHandler != nullptr) {
                                errorHandler(this, arg, "");
                            }
                            return false;
                        }

                        // If handler for current argument's value is defined.
                        if (entry.valueHandler != nullptr)
                        {
                            if (entry.valueHandler(nextParam) == true)
                            {
                                it->second = nextParam;
                                continue;
                            }
                        }
                        else  // Handler undefined.
                        {
                            it->second = nextParam;
                            continue;
                        }

                        // If handler function return FALSE value.
                        if (errorHandler != nullptr) {
                            errorHandler(this, arg, nextParam);
                        }
                        return false;
                    }

                    // If value for current argument is required but not appeared on program input.
                    if (errorHandler != nullptr) {
                        errorHandler(this, arg, "");
                    }
                    return false;
                }
                else  // Argument's value do not expected.
                {
                    // If handler for current argument's value is defined.
                    if (entry.valueHandler != nullptr)
                    {
                        if (entry.valueHandler("") == true) {
                            it->second = "true";
                        }
                    }
                    else { it->second = "true"; }
                }
            }
            else  // Entry not found.
            {
                if (errorHandler != nullptr) {
                    errorHandler(this, arg, "");
                }
                return false;
            }
        }

        if (numberOfInputtedRequiredArguments != numberOfRequiredArguments)
        {
            if (errorHandler != nullptr) {
                errorHandler(this, "", "");
            }
            return false;
        }
        return true;
    }

}  // namespace settings.
