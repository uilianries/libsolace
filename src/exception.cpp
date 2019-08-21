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
 *	@file		exception.cpp
 *	@brief		Implementation of exception types
 ******************************************************************************/
#include "solace/exception.hpp"

#include <cstring>  // strerror


using namespace Solace;


static constexpr StringLiteral IOExceptionType{"IOException"};
static constexpr StringLiteral kIllegalArgumentExceptionName{"IllegalArgumentException"};

static constexpr StringLiteral kNoSuchElementMessage{"No such element"};
static constexpr StringLiteral kInvalidStateMessage{"Invalid State"};
static constexpr StringLiteral kIndexOutOfRangeMessage{"Index out of range"};



namespace /*anonymous*/ {

using details::StringWriter;

details::ErrorString formatErrorStringJoin(StringView lhs, StringView rhs, StringView delim = " ") {
	auto const jointSize = lhs.size() + rhs.size() + delim.size();

	return details::StringWriter{narrow_cast<StringWriter::size_type>(jointSize)}
			.append(lhs)
			.append(delim)
			.append(rhs)
			.build();
}

details::ErrorString formatErrono(int errorCode, StringView msgex) {
	auto const errorString = StringView{strerror(errorCode)};
	auto const bufferSize = StringWriter::measure(msgex)
			+ StringWriter::measure("[")
			+ StringWriter::measure(errorCode)
			+ StringWriter::measure("]: ")
			+ StringWriter::measure(errorString);

	return StringWriter{narrow_cast<StringWriter::size_type>(bufferSize)}
			.append(msgex)
			.append("[")
			.appendFormated(errorCode)
			.append("]: ")
			.append(strerror(errorCode))
			.build();
}


details::ErrorString formatErrono(int errorCode) {
	return formatErrono(errorCode, IOExceptionType);
}


details::ErrorString formatIlligalArgName(StringLiteral argName) {
	auto const bufferSize = StringWriter::measure("Illegal argument '")
			+ StringWriter::measure(argName)
			+ StringWriter::measure("'");

	return StringWriter{narrow_cast<StringWriter::size_type>(bufferSize)}
			.append("Illegal argument '")
			.append(argName)
			.append("'")
			.build();
}

details::ErrorString formatIndexOutOfRangeError(StringView messagePrefix, StringLiteral indexName, StringView reason,
									   size_t index, size_t minValue, size_t maxValue) {
	auto const bufferSize = StringWriter::measure(messagePrefix)
			+ StringWriter::measure(" '")
			+ StringWriter::measure(indexName)
			+ StringWriter::measure("'=")
			+ StringWriter::measure(index)
			+ StringWriter::measure(" ")
			+ StringWriter::measure(reason)
			+ StringWriter::measure(" [")
			+ StringWriter::measure(minValue)
			+ StringWriter::measure(", ")
			+ StringWriter::measure(maxValue)
			+ StringWriter::measure(")");


	StringWriter writer{narrow_cast<StringWriter::size_type>(bufferSize)};

	writer.append(messagePrefix);
	if (indexName) {
		writer.append(" '");
		writer.append(indexName);
		writer.append("'=");
	} else {
		writer.append(": ");
	}

	writer.appendFormated(index);
	writer.append(" ");
	writer.append(reason);
	writer.append(" [");
	writer.appendFormated(minValue);
	writer.append(", ");
	writer.appendFormated(maxValue);
	writer.append(")");

	return writer.build();
}


details::ErrorString formatIndexOutOfRangeError(StringView messagePrefix, StringLiteral indexName,
									   size_t index, size_t minValue, size_t maxValue) {
	return formatIndexOutOfRangeError(messagePrefix, indexName, "is out of range", index, minValue, maxValue);
}


details::ErrorString formatOverflowError(StringLiteral indexName, size_t index, size_t minValue, size_t maxValue) {
	return formatIndexOutOfRangeError("Value", indexName, "overflows range", index, minValue, maxValue);
}


}  // namespace anonymous



// cppcheck-suppress passedByValue
Exception::Exception(StringView message) noexcept
	: _message{std::move(message)}
{
    // Nothing else to do here
}


StringView Exception::getMessage() const noexcept {
	return _message.view(); //StringView{_message.c_str(), static_cast<StringView::size_type>(_message.size())};
}


const char* Exception::what() const noexcept {
    return _message.c_str();
}

StringView Exception::toString() const {
	return _message.view();
}




IllegalArgumentException::IllegalArgumentException() noexcept
	: Exception{kIllegalArgumentExceptionName}
{

}


IllegalArgumentException::IllegalArgumentException(StringLiteral argumentName) noexcept
	: Exception{formatIlligalArgName(argumentName)}
{
    // Nothing to do here
}


IndexOutOfRangeException::IndexOutOfRangeException() noexcept
	: Exception{kIndexOutOfRangeMessage}
{
    // No-op
}


IndexOutOfRangeException::IndexOutOfRangeException(size_t index, size_t minValue, size_t maxValue) noexcept
	: Exception{formatIndexOutOfRangeError(kIndexOutOfRangeMessage, "", index, minValue, maxValue)}
{
    // No-op
}


IndexOutOfRangeException::IndexOutOfRangeException(StringLiteral indexName,
												   size_t index, size_t minValue, size_t maxValue) noexcept
	: Exception{formatIndexOutOfRangeError(kIndexOutOfRangeMessage, indexName, index, minValue, maxValue)}
{
    // No-op
}


IndexOutOfRangeException::IndexOutOfRangeException(size_t index, size_t minValue, size_t maxValue,
												   const char* messagePrefix) noexcept
	: Exception{formatIndexOutOfRangeError(messagePrefix, "", index, minValue, maxValue)}
{
    // No-op
}


OverflowException::OverflowException(StringLiteral indexName,
									 size_t index, size_t minValue, size_t maxValue) noexcept
	: Exception{formatOverflowError(indexName, index, minValue, maxValue)}
{
    // Nothing else to do here
}


OverflowException::OverflowException(size_t index, size_t minValue, size_t maxValue) noexcept
	: Exception(formatOverflowError("", index, minValue, maxValue))
{
}


NoSuchElementException::NoSuchElementException() noexcept
	: Exception{kNoSuchElementMessage}
{
    // No-op
}


NoSuchElementException::NoSuchElementException(StringLiteral elementName) noexcept
	: Exception{formatErrorStringJoin(kNoSuchElementMessage, elementName)}
{
    // Nothing else to do here
}


InvalidStateException::InvalidStateException() noexcept
	: Exception{kInvalidStateMessage}
{

}

InvalidStateException::InvalidStateException(const char* tag) noexcept
	: Exception{formatErrorStringJoin(kInvalidStateMessage, tag)}
{

}


IOException::IOException(StringView msg) noexcept
	: Exception{msg}
	, _errorCode{-1}
{
}


IOException::IOException(int errorCode) noexcept
	: Exception{formatErrono(errorCode)}
	, _errorCode{errorCode}
{
}


IOException::IOException(int errorCode, StringView msg) noexcept
	: Exception{formatErrono(errorCode, msg)}
	, _errorCode{errorCode}
{
}


NotOpen::NotOpen() noexcept
	: IOException{"File descriptor not opened"}
{
    // No-op
}

