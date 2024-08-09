#include "Request.hpp"

// Setters

void Request::setErrorCode(ErrorCode err)
{
	_errorCode = err;
}

void Request::setFileName(std::string newName) {
	_file = newName;
}

//getmethod or replace by the config
void Request::setFile() {
	_file = getMethod(1);
	_file.erase(0, 1);
	if (_file == "")
		_file = _config.getIndex();
}