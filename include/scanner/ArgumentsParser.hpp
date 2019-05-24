// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#ifndef PROTOCOL_ANALYZER_ARGUMENTS_PARSER_HPP
#define PROTOCOL_ANALYZER_ARGUMENTS_PARSER_HPP

#include <list>  // std::list, std::pair.
#include <cstring>  // std::strncpy.
#include <algorithm>  // std::min, std::find_if.
#include <string_view>  // std::string_view.

#define ARGUMENT_NAME_LENGTH   6
#define ARGUMENT_ALIAS_NAME_LENGTH   16


enum ARGUMENTS_AMOUNT : uint16_t
{
    ARG_ZERO = 0x00,
    ARG_ZERO_OR_SINGLE = 0x01,
    ARG_SINGLE = 0x02,
    ARG_ZERO_OR_MORE = 0x03,
    ARG_SINGLE_OR_MORE = 0x04,
    ARG_DOUBLE = 0x05,
    ARG_TRIPLE = 0x06,
    ARG_MULTIPLE = 0x07
};

enum ERROR_REASONS : uint16_t
{
    ERROR_ARGUMENT_NOT_FOUND = 0x00,
    ERROR_VALUE_NOT_FOUND = 0x01,
    ERROR_VALUE_INCORRECT = 0x02,
    ERROR_REQUIRED_ARGUMENTS_NOT_APPEARED = 0x03,
    ERROR_INCORRECT_NUMBER_OF_INPUTTED_VALUES = 0x04
};


namespace analyzer::scanner::settings
{
    class ArgumentsParser;

    /**
     * @typedef bool (*ArgumentHandler) (std::string_view) noexcept;
     * @brief Functor that validates and handles argument's value.
     *
     * @param [in] argumentValue - Inputted argument's value.
     */
    using ArgumentHandler = bool (*) (std::string_view /*argumentValue*/) noexcept;

    /**
     * @typedef bool (*ArgumentErrorHandler) (std::string_view, std::string_view) noexcept;
     * @brief Functor that handles error in inputted argument.
     *
     * @param [in] parser - Pointer to ArgumentsParser object which calls error handler.
     * @param [in] argumentName - Inputted argument's name.
     * @param [in] argumentValue - Inputted argument's value.
     * @param [in] reason - Reason from ArgumentsParser that describes the place where error was occurred.
     */
    using ArgumentErrorHandler = void (*) (const ArgumentsParser * /*parser*/, std::string_view /*argumentName*/, std::string_view /*argumentValue*/, uint16_t /*reason*/) noexcept;


    /**
     * @struct ProgramArgumentEntry   ArgumentsParser.hpp   "include/scanner/ArgumentsParser.hpp"
     * @brief Structure that describes one argument of program.
     */
    struct ProgramArgumentEntry
    {
        /**
         * @brief Flag that indicates is this argument required on program input.
         */
        bool isRequired = false;
        /**
         * @brief Short name of the argument.
         */
        char argumentName[ARGUMENT_NAME_LENGTH] = { };
        /**
         * @brief Full alias name of the argument.
         */
        char aliasName[ARGUMENT_ALIAS_NAME_LENGTH] = { };
        /**
         * @brief Number of values which expected after argument.
         */
        uint16_t numberOfValues = ARG_ZERO;
        /**
         * @brief Pointer to the function that handles inputted value.
         */
        ArgumentHandler valueHandler = nullptr;


        /**
         * @brief Default constructor of ProgramArgumentEntry structure.
         */
        ProgramArgumentEntry(void) noexcept = default;

        /**
         * @brief Constructor of ProgramArgumentEntry structure.
         *
         * @param [in] required - Flag that indicates is this argument required on program input.
         * @param [in] valueExpected - Flag that indicates is the value expected after this argument.
         * @param [in] name - Short name of the argument.
         * @param [in] alias - Full alias name of the argument.
         * @param [in] handler - Pointer to the function that handles inputted value.
         */
        ProgramArgumentEntry (const bool required,
                              std::string_view name,
                              std::string_view alias,
                              uint16_t valuesAmount,
                              const ArgumentHandler handler) noexcept
            : isRequired(required),
              numberOfValues(valuesAmount),
              valueHandler(handler)
        {
            std::strncpy(argumentName, name.data(), std::min<std::size_t>(name.size(), ARGUMENT_NAME_LENGTH - 1));
            std::strncpy(aliasName, alias.data(), std::min<std::size_t>(alias.size(), ARGUMENT_ALIAS_NAME_LENGTH - 1));
        }
    };


    /**
     * @class ArgumentsParser   ArgumentsParser.hpp   "include/scanner/ArgumentsParser.hpp"
     * @brief This class defined the interface for parsing, check and collect program arguments.
     */
    class ArgumentsParser
    {
    public:
        using ArgumentEntry = std::pair<ProgramArgumentEntry, std::list<std::string>>;
        using ArgumentsContainer = std::list<ArgumentEntry>;

    private:
        /**
         * @brief Container that stores arguments templates for parsing and its values on input.
         */
        ArgumentsContainer argumentsStorage = { };
        /**
         * @brief Pointer to the error handler function.
         */
        ArgumentErrorHandler errorHandler = nullptr;
        /**
        * @brief Variable that containing the number of required parameters.
        */
        uint16_t numberOfRequiredArguments = 0;


        /**
         * @brief Method that returns the pointer to ArgumentEntry structure by specified argument name.
         *
         * @param [in] argumentName - Argument name or alias.
         * @return Pointer to constant ArgumentEntry structure that describes specified argument or nullptr.
         */
        ArgumentEntry* InternalGetArgumentEntryByName (std::string_view /*argumentName*/) noexcept;

    public:
        ArgumentsParser (ArgumentsParser &&) = delete;
        ArgumentsParser (const ArgumentsParser &) = delete;
        ArgumentsParser & operator= (ArgumentsParser &&) = delete;
        ArgumentsParser & operator= (const ArgumentsParser &) = delete;

        /**
         * @brief Default constructor of ArgumentParser class.
         */
        ArgumentsParser(void) = default;

        /**
         * @brief Method that adds new expected argument at the program input and its handler.
         *
         * @param [in] argumentName - Short name of the argument.
         * @param [in] aliasName - Full alias name of the argument. Default: NONE.
         * @param [in] handler - Pointer to the function that handles inputted value. Default: NONE.
         * @param [in] isRequired - Flag that indicates is this argument required on program input. Default: false.
         * @param [in] numberOfValues - Number of values which expected after argument (ARGUMENTS_AMOUNT). Default: ARG_ZERO.
         *
         * @warning ArgumentHandler MUST NOT throw any exceptions.
         */
        void Add (std::string_view /*argumentName*/,
                  std::string_view /*aliasName*/     = "",
                  ArgumentHandler /*handler*/        = nullptr,
                  bool /*isRequired*/                = false,
                  uint16_t /*numberOfValues*/        = ARG_ZERO) noexcept;

        /**
         * @brief Method that returns the pointer to ArgumentEntry structure by specified argument name.
         *
         * @param [in] argumentName - Argument name or alias.
         * @return Pointer to constant ArgumentEntry structure that describes specified argument or nullptr.
         */
        const ArgumentEntry * GetArgumentEntryByName (std::string_view /*argumentName*/) const noexcept;

        /**
         * @brief Method that returns all registered arguments.
         *
         * @return List of all registered arguments.
         */
        inline const ArgumentsContainer & GetArguments(void) const noexcept { return argumentsStorage; }

        /**
         * @brief Method that initialize the pointer to the error handler function.
         *
         * @param [in] handler - Pointer to the error handler function.
         *
         * @note In this handler is needed to analyze each argument where the error maybe occurred.
         * @note The argument name and/or its value will be transferred on input of handler.
         * @note If error handler is initialized then ArgumentsParser::Parse method calls it before it returns FALSE value.
         *
         * @warning ArgumentErrorHandler MUST NOT throw any exceptions.
         */
        inline void AddActionOnError (const ArgumentErrorHandler handler) noexcept { errorHandler = handler; }

        /**
         * @brief Method that parses and handles arguments on program input.
         *
         * @param [in] arguments - Program arguments on input.
         * @param [in] count - Number of arguments on program input.
         * @return TRUE - if all arguments parses correctly and all required arguments appeared on the program input, otherwise - FALSE.
         *
         * @warning All program arguments are required on input (with first item - program name).
         */
        [[nodiscard]]
        bool Parse (const char * const * /*arguments*/, std::size_t /*count*/) noexcept;

        /**
         * @brief Operator that outputs parsed arguments in string format.
         *
         * @param [in,out] stream - Reference of the output stream engine.
         * @param [in] parser - Constant lvalue reference of the ArgumentsParser class.
         * @return Lvalue reference of the inputted STL std::ostream class.
         */
        friend inline std::ostream& operator<< (std::ostream& stream, const ArgumentsParser& parser) noexcept
        {
            for (const ArgumentEntry& entry : parser.argumentsStorage)
            {
                stream << entry.first.argumentName << " (" << entry.first.aliasName << ')' << ':' << std::endl;
                for (const std::string& value : entry.second) {
                    stream << '\t' << value << std::endl;
                }
            }
            stream << std::endl;
            return stream;
        }

        /**
         * @brief Default destructor of ArgumentsParser class.
         */
        ~ArgumentsParser(void) = default;
    };

}  // namespace settings.


#endif  // PROTOCOL_ANALYZER_ARGUMENTS_PARSER_HPP
