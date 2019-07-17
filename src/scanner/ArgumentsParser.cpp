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
    // Method that returns the pointer to ArgumentEntry structure by specified argument name.
    ArgumentsParser::ArgumentEntry* ArgumentsParser::InternalGetArgumentEntryByName (std::string_view argumentName) noexcept
    {
        ArgumentsContainer::iterator it = std::find_if(argumentsStorage.begin(), argumentsStorage.end(),
            [argumentName] (const ArgumentEntry& entry) -> bool
            {
                if (argumentName == entry.first.argumentName || argumentName == entry.first.aliasName) {
                    return true;
                }
                return false;
            });
        return (it != argumentsStorage.end() ? &(*it) : nullptr);
    }

    // Method that adds new expected argument at the program input and its handler.
    void ArgumentsParser::Add (std::string_view argumentName,
                               std::string_view aliasName,                               
                               const ArgumentHandler handler,
                               const bool isRequired,
                               uint16_t numberOfValues) noexcept
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

        argumentsStorage.emplace_back(ProgramArgumentEntry(isRequired, argumentName, aliasName, numberOfValues, handler), std::list<std::string>());
    }


    // Method that returns the pointer to ArgumentEntry structure by specified argument name.
    const ArgumentsParser::ArgumentEntry* ArgumentsParser::GetArgumentEntryByName (std::string_view argumentName) const noexcept
    {
        ArgumentsContainer::const_iterator it = std::find_if(argumentsStorage.cbegin(), argumentsStorage.cend(),
            [argumentName] (const ArgumentEntry& entry) -> bool
            {
                if (argumentName == entry.first.argumentName || argumentName == entry.first.aliasName) {
                    return true;
                }
                return false;
            });
        return (it != argumentsStorage.cend() ? &(*it) : nullptr);
    }


    // Method that parses and handles arguments on program input.
    bool ArgumentsParser::Parse (const char* const* const arguments, const std::size_t count) noexcept
    {
        bool flagSearchNextValue = false;
        ArgumentEntry* previousArg = nullptr;

        // If no any arguments on program input but some of them are expected.
        if (count <= 1 && numberOfRequiredArguments > 0)
        {
            if (errorHandler != nullptr) {
                errorHandler(this, "", "", ERROR_REQUIRED_ARGUMENTS_NOT_APPEARED);
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
            ArgumentEntry* const it = InternalGetArgumentEntryByName(arg);

            // If several arguments are expected on program input.
            if (flagSearchNextValue == true && it == nullptr && previousArg != nullptr)
            {
                const ProgramArgumentEntry& entry = previousArg->first;

                // If handler for current argument's value is defined.
                if (entry.valueHandler != nullptr)
                {
                    if (entry.valueHandler(arg) == true)
                    {
                        previousArg->second.emplace_back(arg);
                        if (entry.numberOfValues == ARG_DOUBLE && previousArg->second.size() == 2) {
                            flagSearchNextValue = false;
                        }
                        else if (entry.numberOfValues == ARG_TRIPLE && previousArg->second.size() == 3) {
                            flagSearchNextValue = false;
                        }
                        continue;
                    }

                    // If handler function return FALSE value.
                    if (errorHandler != nullptr) {
                        errorHandler(this, entry.argumentName, arg, ERROR_VALUE_INCORRECT);
                    }
                    return false;
                }
                else
                {
                    previousArg->second.emplace_back(arg);
                    if (entry.numberOfValues == ARG_DOUBLE && previousArg->second.size() == 2) {
                        flagSearchNextValue = false;
                    }
                    else if (entry.numberOfValues == ARG_TRIPLE && previousArg->second.size() == 3) {
                        flagSearchNextValue = false;
                    }
                }
            }
            // If entry found.
            else if (it != nullptr)
            {
                // Check the correctness of multi-values arguments.
                if (flagSearchNextValue == true && previousArg != nullptr)
                {
                    const ProgramArgumentEntry& entry = previousArg->first;
                    if ((entry.numberOfValues == ARG_DOUBLE && previousArg->second.size() != 2) ||
                        (entry.numberOfValues == ARG_TRIPLE && previousArg->second.size() != 3) ||
                        (entry.numberOfValues == ARG_MULTIPLE && previousArg->second.size() < 2))
                    {
                        if (errorHandler != nullptr) {
                            errorHandler(this, entry.argumentName, "", ERROR_INCORRECT_NUMBER_OF_INPUTTED_VALUES);
                        }
                        return false;
                    }
                    flagSearchNextValue = false;
                }

                const ProgramArgumentEntry& entry = it->first;
                if (entry.isRequired == true) {
                    numberOfInputtedRequiredArguments++;
                }

                // If argument's value may be expected.
                if (entry.numberOfValues >= ARG_ZERO_OR_SINGLE)
                {
                    if (idx + 1 < count)
                    {
                        const std::string_view nextParam(arguments[++idx]);
                        // If next inputted parameter is similar as a program argument.
                        if (nextParam[0] == '-')
                        {
                            const ArgumentEntry* const itt = InternalGetArgumentEntryByName(nextParam);

                            // If next inputted parameter is a program argument.
                            if (itt != nullptr)
                            {
                                if (entry.numberOfValues != ARG_ZERO_OR_SINGLE && entry.numberOfValues != ARG_ZERO_OR_MORE)
                                {
                                    // If value for current argument is required but not appeared on program input.
                                    if (errorHandler != nullptr) {
                                        errorHandler(this, arg, "", ERROR_VALUE_NOT_FOUND);
                                    }
                                    return false;
                                }

                                // Argument's value does not inputted and value may be not expected.
                                if (entry.valueHandler != nullptr)
                                {
                                    if (entry.valueHandler("") == true) {
                                        it->second.emplace_back("true");
                                    }
                                    // Value handler failed.
                                    else if (errorHandler != nullptr) {
                                        errorHandler(this, arg, "", ERROR_VALUE_INCORRECT);
                                    }
                                }
                                else { it->second.emplace_back("true"); }

                                if (entry.numberOfValues >= ARG_ZERO_OR_MORE) {
                                    flagSearchNextValue = true;
                                    previousArg = it;
                                }
                                --idx;
                                continue;
                            }
                        }

                        // If next inputted parameter is argument's value.
                        if (entry.valueHandler != nullptr)
                        {
                            if (entry.valueHandler(nextParam) == true)
                            {
                                it->second.emplace_back(nextParam);
                                if (entry.numberOfValues >= ARG_ZERO_OR_MORE) {
                                    flagSearchNextValue = true;
                                    previousArg = it;
                                }
                                continue;
                            }

                            // If handler function return FALSE value.
                            if (errorHandler != nullptr) {
                                errorHandler(this, arg, nextParam, ERROR_VALUE_INCORRECT);
                            }
                            return false;
                        }

                        it->second.emplace_back(nextParam);
                        if (entry.numberOfValues >= ARG_ZERO_OR_MORE) {
                            flagSearchNextValue = true;
                            previousArg = it;
                        }
                        continue;
                    }
                    else  // If value for current argument is required but not appeared on program input.
                    {
                        if (entry.numberOfValues != ARG_ZERO_OR_SINGLE && entry.numberOfValues != ARG_ZERO_OR_MORE)
                        {
                            // If value for current argument is required but not appeared on program input.
                            if (errorHandler != nullptr) {
                                errorHandler(this, arg, "", ERROR_VALUE_NOT_FOUND);
                            }
                            return false;
                        }

                        // Argument's value does not inputted and value may be not expected.
                        if (entry.valueHandler != nullptr)
                        {
                            if (entry.valueHandler("") == true) {
                                it->second.emplace_back("true");
                            }
                            // Value handler failed.
                            else if (errorHandler != nullptr) {
                                errorHandler(this, arg, "", ERROR_VALUE_INCORRECT);
                            }
                        }
                        else { it->second.emplace_back("true"); }

                        if (entry.numberOfValues >= ARG_ZERO_OR_MORE) {
                            flagSearchNextValue = true;
                            previousArg = it;
                        }
                        continue;
                    }
                }
                else  // Argument's value does not expected.
                {
                    // If handler for current argument's value is defined.
                    if (entry.valueHandler != nullptr)
                    {
                        if (entry.valueHandler("") == true) {
                            it->second.emplace_back("true");
                        }
                        // Value handler failed.
                        else if (errorHandler != nullptr) {
                            errorHandler(this, arg, "", ERROR_VALUE_INCORRECT);
                        }
                    }
                    else { it->second.emplace_back("true"); }
                }
            }
            else  // Entry not found.
            {
                if (errorHandler != nullptr) {
                    errorHandler(this, arg, "", ERROR_ARGUMENT_NOT_FOUND);
                }
                return false;
            }
        }

        if (numberOfInputtedRequiredArguments != numberOfRequiredArguments)
        {
            if (errorHandler != nullptr) {
                errorHandler(this, "", "", ERROR_REQUIRED_ARGUMENTS_NOT_APPEARED);
            }
            return false;
        }
        return true;
    }

}  // namespace settings.
