#pragma once

#include <exception>
#include <string>
#include "Webserver.hpp"

class HTTPError : public std::exception
{
private:
	ErrorCode _errorCode; // Store the error number

public:
	// Constructor accepting message and error number
	HTTPError(ErrorCode errorCode) : _errorCode(errorCode) {}

	// Override what() function to include error number
	const char *what() const noexcept override {
		// Format the error message to include both the original message and the error number
		static std::string error_message = httpStatus[int(_errorCode)];
		error_message += " - error " + std::to_string(_errorCode);
		return error_message.c_str();
	}

	// Getter for the error number
	ErrorCode getErrorCode() const noexcept {
		return (_errorCode);
	}
};