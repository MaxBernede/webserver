#pragma once

#include <exception>
#include <string>
#include "Webserver.hpp"

class HTTPError : public std::exception
{
	private:
		ErrorCode _errorCode; // Store the error number
		std::string _errorMessage;
	
	public:
		// Constructor accepting message and error number
		HTTPError(ErrorCode errorCode) : _errorCode(errorCode) {
			_errorMessage = httpStatus[static_cast<int>(_errorCode)];
			_errorMessage += " - error " + std::to_string(static_cast<int>(_errorCode));
		}
	
		// Override what() function to include error number
		const char *what() const noexcept override {
			return _errorMessage.c_str();
		}
	
		// Getter for the error number
		ErrorCode getErrorCode() const noexcept {
			return _errorCode;
		}
};