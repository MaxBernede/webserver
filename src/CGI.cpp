#include "CGI.hpp"
#include <cstring>
#include <vector>

CGI::CGI(Request *request, int clientFd) : _request(request), _clientFd(clientFd), _cgiEnvArr(makeEnvArr()), _cgiEnvCStr(makeEnvCStr())
{
	pipe(_sendPipe);
}

CGI::~CGI(void)
{
	delete[] _cgiEnvCStr;
}

void CGI::runCgi()
{
	for (int i = 0; i < _cgiEnvArr.size(); i++)
	{
		std::cout << "cgi[i]: " << _cgiEnvCStr[i] << std::endl;
	}
	_pid = fork(); //forking to create new process
	if (_pid == 0) //if child process
	{
		// redirect I/O
		close(_sendPipe[0]); // close read-end of response
		dup2(_sendPipe[1], STDOUT_FILENO); // write to response pipe
		std::string cgiFilePath = "html"; // TODO change this, hard-coded monster
		std::string cgiFilename = cgiFilePath + _request->getFileName();
		char *argv[2] = {(char *)cgiFilename.c_str(), NULL};
		execve(cgiFilename.c_str(), argv, _cgiEnvCStr);
		// if execve fails
		std::cout << "Running CGI script failed (execve), path: " << cgiFilename << std::endl;
		std::cout << "Errno: " << std::strerror(errno) << std::endl;
		exit(1); // exit child process with 1, upon failure
	}
	else //parent (main) process
	{
		close(_sendPipe[1]); // close write-end of the response pipe (send)
	}
}

bool 	CGI::waitCgiChild()
{
	int exitCode;
	int status = waitpid(_pid, &exitCode, WNOHANG);
	if (status == -1)
		throw(Exception("Error while waiting for cgi with pid " + std::to_string(_pid) , 1));
	else if (status == 0) // cgi not done
		return (false);
	else
	{
		if (exitCode != 0)
			throw(Exception("Error while running cgi with pid " + std::to_string(_pid), 1));
		Logger::log("Cgi child process finished", LogLevel::INFO);
		return (true);
	}

}

std::vector<std::string>	CGI::makeEnvArr()
{
	std::vector<std::string> envArr {
		"CONTENT_LENGTH=",
		"CONTENT_TYPE=multipart/form-data; boundary=",
		"GATEWAY_INTERFACE=CGI/1.1", // fixed
		"PATH_INFO=",
		"PATH_TRANSLATED=",
		"QUERY_STRING=",
		"REMOTE_ADDR=",
		"REMOTE_HOST=",
		"REMOTE_IDENT=",
		"REMOTE_USER=",
		"REQUEST_METHOD=" + _request->getMethod(METHOD),
		"SCRIPT_NAME=" + _request->getFileName(),
		"SCRIPT_FILENAME=" + _request->getFileName(),
		"SERVER_NAME=" + _request->getServerName(),
		"SERVER_PORT=" + std::to_string(_request->getRequestPort()),
		"SERVER_PROTOCOL=HTTP/1.1", // fixed
		"SERVER_SOFTWARE=WebServServer/1.0", // fixed
		"HTTP_COOKIE=" + _request->getValues("Cookie"),
	};
	return (envArr);
}

char **CGI::makeEnvCStr()
{
	char **env = new char*[_cgiEnvArr.size() + 1];
	for (size_t i = 0; i < _cgiEnvArr.size(); ++i)
	{
	//	env[i] = new char[envArr[i].length() + 1];
		//std::strcpy(env[i], envArr[i].c_str());
		env[i] = (char *)this->_cgiEnvArr[i].c_str();
	}
	env[_cgiEnvArr.size()] = NULL;
	return env;
}

int CGI::getReadFd()
{
	return (_sendPipe[0]);
}

int CGI::getClientFd()
{
	return (_clientFd);
}

Request *CGI::getRequest()
{
	return (_request);
}

