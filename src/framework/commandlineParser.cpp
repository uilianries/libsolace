/*
*  Copyright 2016 Ivan Ryabov
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*/
/*******************************************************************************
 * @file: framework/commandlineParser.cpp
 *
 *  Created by soultaker on 18/08/16.
*******************************************************************************/

#include "solace/framework/commandlineParser.hpp"
#include "solace/framework/commandlineUtils.hpp"
#include "solace/parseUtils.hpp"

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <cstring>
#include <cstdlib>

#include <iomanip>
#include <iostream>
#include <sstream>  // std::stringstream

#include <utility>


using namespace Solace;
using namespace Solace::Framework;


const char CommandlineParser::DefaultPrefix = '-';
const char CommandlineParser::DefaultValueSeparator = '=';



template <typename... Args>
Optional<Error>
formatOptionalError(const char* fmt, Args&&... values) {
    return Optional<Error>::of(fmt::format(fmt, std::forward<Args>(values)...));
}



template<typename T>
Optional<Error>
parseIntArgument(T* dest, const StringView& value, const CommandlineParser::Context&) {
    auto val = tryParse<T>(value);

    if (val) {
        *dest = static_cast<T>(val.unwrap());
        return None();
    } else {
        // TODO(abbyssoul): Result::getError() must return Optional<Error>
        return Optional<Error>::of(val.moveError());
    }
}



Optional<Error>
parseBoolean(bool* dest, StringView value) {
    auto val = tryParse<bool>(value);

    if (val) {
        *dest = val.unwrap();
        return None();
    } else {
        // TODO(abbyssoul): Result::getError() must return Optional<Error>
        return Optional<Error>::of(val.moveError());
    }
}


CommandlineParser::Option::Option(std::initializer_list<StringLiteral> names, StringLiteral desc, StringView* dest) :
    Option(names, desc, OptionArgument::Required,
           [dest](const Optional<StringView>& value, const Context&) -> Optional<Error> {
        *dest = value.get();

        return None();
    })
{
}


CommandlineParser::Option::Option(std::initializer_list<StringLiteral> names, StringLiteral desc, int8* dest):
    Option(names, desc, OptionArgument::Required,
           [dest](const Optional<StringView>& value, const Context& context) {
                return parseIntArgument(dest, value.get(), context);
            })
{
}

CommandlineParser::Option::Option(std::initializer_list<StringLiteral> names, StringLiteral desc, uint8* dest):
    Option(names, desc, OptionArgument::Required,
           [dest](const Optional<StringView>& value, const Context& context) {
               return parseIntArgument(dest, value.get(), context);
           })
{
}


CommandlineParser::Option::Option(std::initializer_list<StringLiteral> names, StringLiteral desc, int16* dest):
    Option(names, desc, OptionArgument::Required,
           [dest](const Optional<StringView>& value, const Context& context) {
               return parseIntArgument(dest, value.get(), context);
           })
{
}

CommandlineParser::Option::Option(std::initializer_list<StringLiteral> names, StringLiteral desc, uint16* dest):
    Option(names, desc, OptionArgument::Required,
           [dest](const Optional<StringView>& value, const Context& context) {
               return parseIntArgument(dest, value.get(), context);
           })
{
}


CommandlineParser::Option::Option(std::initializer_list<StringLiteral> names, StringLiteral desc, int32* dest):
    Option(names, desc, OptionArgument::Required,
           [dest](const Optional<StringView>& value, const Context& context) {
               return parseIntArgument(dest, value.get(), context);
           })
{
}

CommandlineParser::Option::Option(std::initializer_list<StringLiteral> names, StringLiteral desc, uint32* dest):
    Option(names, desc, OptionArgument::Required,
           [dest](const Optional<StringView>& value, const Context& context) {
               return parseIntArgument(dest, value.get(), context);
           })
{
}


CommandlineParser::Option::Option(std::initializer_list<StringLiteral> names, StringLiteral desc, int64* dest):
    Option(names, desc, OptionArgument::Required,
           [dest](const Optional<StringView>& value, const Context& context) {
               return parseIntArgument(dest, value.get(), context);
           })
{
}

CommandlineParser::Option::Option(std::initializer_list<StringLiteral> names, StringLiteral desc, uint64* dest):
    Option(names, desc, OptionArgument::Required,
           [dest](const Optional<StringView>& value, const Context& context) {
               return parseIntArgument(dest, value.get(), context);
           })
{
}

CommandlineParser::Option::Option(std::initializer_list<StringLiteral> names, StringLiteral desc, float32* dest) :
    Option(names, desc, OptionArgument::Required,
           [dest](const Optional<StringView>& value, const Context& context) -> Optional<Error> {
        char* pEnd = nullptr;
        auto val = strtof(value.get().c_str(), &pEnd);

        if (!pEnd || pEnd == value.get().data()) {  // No conversion has been done
            return formatOptionalError("Option '{}' is not float32 value: '{}'", context.name, value.get());
        }

        *dest = static_cast<float32>(val);

        return None();
    })
{
}

CommandlineParser::Option::Option(std::initializer_list<StringLiteral> names, StringLiteral desc, float64* dest) :
    Option(names, desc, OptionArgument::Required,
           [dest](const Optional<StringView>& value, const Context& context) -> Optional<Error> {
        char* pEnd = nullptr;
        auto val = strtod(value.get().c_str(), &pEnd);

        if (!pEnd || pEnd == value.get().data()) {  // No conversion has been done
            return formatOptionalError("Option '{}' is not float64 value: '{}'", context.name, value.get());
        }

        *dest = static_cast<float64>(val);

        return None();
    })
{
}


CommandlineParser::Option::Option(std::initializer_list<StringLiteral> names, StringLiteral desc, bool* dest) :
     Option(names, desc, OptionArgument::Optional,
            [dest](const Optional<StringView>& value, const Context&) -> Optional<Error> {
         if (value.isSome()) {
             return parseBoolean(dest, value.get());
         } else {
             *dest = true;

             return None();
         }
    })
{
}



CommandlineParser::Argument::Argument(StringLiteral name, StringLiteral description, int8* dest):
    Argument(name, description,
             [dest](const StringView& value, const Context& context) { return parseIntArgument(dest, value, context); })
{
}

CommandlineParser::Argument::Argument(StringLiteral name, StringLiteral description, uint8* dest):
    Argument(name, description,
             [dest](const StringView& value, const Context& context) { return parseIntArgument(dest, value, context); })
{
}

CommandlineParser::Argument::Argument(StringLiteral name, StringLiteral description, int16* dest):
    Argument(name, description,
             [dest](const StringView& value, const Context& context) { return parseIntArgument(dest, value, context); })

{
}

CommandlineParser::Argument::Argument(StringLiteral name, StringLiteral description, uint16* dest):
    Argument(name, description,
             [dest](const StringView& value, const Context& context) { return parseIntArgument(dest, value, context); })

{
}

CommandlineParser::Argument::Argument(StringLiteral name, StringLiteral description, int32* dest):
    Argument(name, description,
             [dest](const StringView& value, const Context& context) { return parseIntArgument(dest, value, context); })

{
}

CommandlineParser::Argument::Argument(StringLiteral name, StringLiteral description, uint32* dest):
    Argument(name, description,
             [dest](const StringView& value, const Context& context) { return parseIntArgument(dest, value, context); })

{
}

CommandlineParser::Argument::Argument(StringLiteral name, StringLiteral description, int64* dest):
    Argument(name, description,
             [dest](const StringView& value, const Context& context) { return parseIntArgument(dest, value, context); })

{
}

CommandlineParser::Argument::Argument(StringLiteral name, StringLiteral description, uint64* dest):
    Argument(name, description,
             [dest](const StringView& value, const Context& context) { return parseIntArgument(dest, value, context); })

{
}

CommandlineParser::Argument::Argument(StringLiteral name, StringLiteral description, float32* dest):
    Argument(name, description,
             [dest](const StringView& value, const Context& context) {
        char* pEnd = nullptr;
        *dest = strtof(value.c_str(), &pEnd);

        return (!pEnd || pEnd == value.data())
                ? formatOptionalError("Argument '{}' is not float32 value: '{}'", context.name, value)
                : None();
    })
{
}


CommandlineParser::Argument::Argument(StringLiteral name, StringLiteral description, float64* dest):
    Argument(name, description,
             [dest](const StringView& value, const Context& context) {
        char* pEnd = nullptr;
        *dest = strtod(value.c_str(), &pEnd);

        return (!pEnd || pEnd == value.data())
                ? formatOptionalError("Argument '{}' is not float64 value: '{}'", context.name, value)
                : None();
    })
{
}


CommandlineParser::Argument::Argument(StringLiteral name, StringLiteral description, bool* dest) :
    Argument(name, description, [dest](const StringView& value, const Context&) { return parseBoolean(dest, value); })
{
}


CommandlineParser::Argument::Argument(StringLiteral name, StringLiteral description, StringView* dest) :
    Argument(name, description, [dest](const StringView& value, const Context&) { *dest = value; return None(); })
{
}


Result<void, Error>
idleAction() {
    return Ok();
}


CommandlineParser::CommandlineParser(StringView appDescription) :
    _prefix(DefaultPrefix),
    _valueSeparator(DefaultValueSeparator),
    _defaultAction(std::move(appDescription), idleAction)
{
}


CommandlineParser::CommandlineParser(StringView appDescription,
                                     const std::initializer_list<Option>& options) :
    _prefix(DefaultPrefix),
    _valueSeparator(DefaultValueSeparator),
    _defaultAction(std::move(appDescription), idleAction, options)
{
}


bool CommandlineParser::Option::isMatch(StringView name) const noexcept {
    for (const auto& optName : _names) {
        if (optName == name) {
            return true;
        }
    }

    return false;
}


Optional<Error>
CommandlineParser::Option::match(const Optional<StringView>& value, const Context& cntx) const {
    return _callback(value, cntx);
}



Optional<Error>
CommandlineParser::Argument::match(const StringView& value, const Context& cntx) const {
    return _callback(value, cntx);
}



template<typename... Args>
Result<std::function<Result<void, Error> ()>, Error>
fail(const char* msg, Args&&...args) {
    return Err(Error((fmt::format(msg, std::forward<Args>(args)...))));
}

template<typename... Args>
Result<uint, Error>
failUint(const char* msg, Args&&... args) {
    return Err(Error(fmt::format(msg, std::forward<Args>(args)...)));
}


std::pair<StringView, Optional<StringView>>
parseOption(StringView arg, char prefix, char valueSeparator) {

    const StringView::size_type startIndex = (arg.substring(1).startsWith(prefix)) ? 2 : 1;
    if (startIndex >= arg.length()) {
        return std::make_pair(StringView(), None());
    }

    StringView::size_type endIndex = startIndex;

    while ((endIndex < arg.length()) && arg[endIndex] != valueSeparator) {
        ++endIndex;
    }

    if (endIndex < arg.length())
        return std::make_pair(arg.substring(startIndex, endIndex - startIndex),
                        Optional<StringView>::of(arg.substring(endIndex + 1)));
    else
        return std::make_pair(arg.substring(startIndex), None());
}


Result<uint, Error>
parseOptions(const CommandlineParser::Context& cntx,
             const Array<CommandlineParser::Option>& options,
             char prefix, char separator) {
    uint firstPositionalArgument = cntx.offset;

    // Parse array of strings until we error out or there is no more flags:
    for (uint i = firstPositionalArgument; i < cntx.argc; ++i, ++firstPositionalArgument) {
        if (!cntx.argv[i]) {
            return failUint("Invalid number of arguments!");
        }

        const StringView arg {cntx.argv[i]};

        // Check if the arg string starts with a prefix char
        if (!arg.startsWith(prefix)) {  // Nope, not a flag, stop processing
            break;
        }

        auto res = parseOption(arg, prefix, separator);
        const auto argName = std::get<0>(res);
        Optional<StringView> argValue { std::move(res.second) };

        if (argValue.isNone()) {
            if (i + 1 < cntx.argc) {  // Check that there are more arguments in the argv, thus we can expect a value
                StringView nextArg{cntx.argv[i + 1]};

                if (!nextArg.startsWith(prefix)) {
                    argValue = std::move(nextArg);

                    // Adjust current index in the array
                    ++i;
                    ++firstPositionalArgument;
                }
            }
        }

        uint numberMatched = 0;

        const CommandlineParser::Context optCntx {cntx.argc, cntx.argv, i, argName, cntx.parser};
        for (auto& option : options) {
            if (option.isMatch(argName)) {
                if (argValue.isNone() &&
                    option.getArgumentExpectations() == CommandlineParser::OptionArgument::Required) {
                    // Argument is required but none was given, error out!
                    return failUint("Option '{}' expects a value, none were given", optCntx.name);
                }

                numberMatched += 1;

                auto r = option.match(argValue, optCntx);
                if (r.isSome()) {
                    return Err(r.get());
                }
            }
        }

        if (numberMatched < 1) {
//            return failUint("Unexpected option '{}'", optCntx.argv[i]);
            return failUint("Unexpected option '{}'", argName);
        }
    }

    return Ok(firstPositionalArgument);
}


Result<CommandlineParser::ParseResult, Error>
parseCommand(const CommandlineParser::Command& cmd, const CommandlineParser::Context& cntx) {

    auto optionsParsingResult = parseOptions(cntx,
                                             cmd.options(),
                                             cntx.parser.optionPrefix(),
                                             cntx.parser.valueSeparator());
    if (!optionsParsingResult) {
        return Err(optionsParsingResult.moveError());
    }

    const uint positionalArgument = optionsParsingResult.unwrap();

    // Positional arguments processing
    if (positionalArgument < cntx.argc) {

        if (!cmd.commands().empty()) {
            const StringView subcmdName{cntx.argv[positionalArgument]};
            const auto cmdIt = cmd.commands().find(subcmdName);
            if (cmdIt == cmd.commands().end()) {
                return fail("Command '{}' not supported", subcmdName);
            }

            const CommandlineParser::Context subcomandCntx{ cntx.argc,
                        cntx.argv,
                        positionalArgument + 1,
                        subcmdName,
                        cntx.parser};

            return parseCommand(cmdIt->second, subcomandCntx);
        } else if (!cmd.arguments().empty()) {
            const CommandlineParser::Context subcomandCntx{cntx.argc,
                        cntx.argv,
                        positionalArgument,
                        StringView(),
                        cntx.parser};

            // TODO(abbyssoul): Parse arguments
//            parseArguments(subcomandCntx, cmd.arguments());

            return Ok<CommandlineParser::ParseResult>(cmd.callback());
        } else {
            return fail("Unexpected arguments given");
        }

    } else {

        return (cmd.arguments().empty() && cmd.commands().empty())
            ? Ok<CommandlineParser::ParseResult>(cmd.callback())
            : fail("Not enough arguments");
    }
}

Result<CommandlineParser::ParseResult, Error>
CommandlineParser::parse(int argc, const char *argv[]) const {
    if (argc < 0) {
        return fail("Number of arguments can not be negative");
    }

    if (argc < 1) {
        return (_defaultAction.arguments().empty() && _defaultAction.commands().empty())
                ? Ok<CommandlineParser::ParseResult>(_defaultAction.callback())
                : fail("Not enough arguments");
    }

    const CommandlineParser::Context cntx {static_cast<uint>(argc), argv, 1, argv[0], *this};

    return parseCommand(_defaultAction, cntx);
}


CommandlineParser::Option
CommandlineParser::printVersion(StringView appName, const Version& appVersion) {
    return {
        {"v", "version"},
        "Print version",
        CommandlineParser::OptionArgument::NotRequired,
        [name = std::move(appName), version = std::move(appVersion)]
                (const Optional<StringView>&, const Context&) -> Optional<Error> {
            VersionPrinter printer(name, version);

            printer(std::cout);

            return None();
        }
    };
}


CommandlineParser::Option
CommandlineParser::CommandlineParser::printHelp() {
    return {
        {"h", "help"},
        "Print help",
        CommandlineParser::OptionArgument::NotRequired,
        [](const Optional<StringView>& value, const Context& cntx) -> Optional<Error> {
            HelpFormatter printer(std::cout, cntx.parser.optionPrefix());

            if (value.isNone()) {
                printer(cntx.argv[0],
                        cntx.parser.description(),
                        cntx.parser.options(),
                        cntx.parser.commands());
            } else {

                const auto& cmdIt = cntx.parser.commands().find(value.get());
                if (cmdIt != cntx.parser.commands().end()) {
                    printer(cmdIt->first,
                            cmdIt->second.description(),
                            cmdIt->second.options(),
                            cmdIt->second.commands());
                } else {
                    return Optional<Error>::of({"Unknown command"});
                }
            }

            return None();
        }
    };
}
