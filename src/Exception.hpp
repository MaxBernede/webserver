#pragma once

#include <exception>
#include <string>
#include <cstring>

class Exception : public std::exception
{
private:
	int _error_number; // Store the error number
	std::string _msg;

public:
	// Constructor accepting message and error number
	Exception(const std::string& message, int error_number) : _msg(message), _error_number(error_number) {}
	~Exception( void ) {};

	// Override what() function to include error number
	const char* what() const noexcept override {
		// Format the error message to include both the original message and the error number
		static std::string error_message;
		error_message = _msg;
		error_message += " - error " + std::to_string(_error_number) + ": " + std::strerror(_error_number) + ")";
		return error_message.c_str();
	}

	// Getter for the error number
	int error_number() const noexcept {
		return _error_number;
	}
};
