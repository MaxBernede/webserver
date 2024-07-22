#include "CGI.hpp"
#include <cstring>

CGI::CGI(Request *request, int clientFd) : _request(request), _clientFd(clientFd), _cgiEnv(makeEnv())
{
	pipe(_sendPipe);
}

CGI::~CGI(void)
{
	delete [] _cgiEnv;
	delete _request;
}

void CGI::runCgi()
{
	int pid = fork(); //forking to create new process
	if (pid == 0) //if child process
	{
		for (int i = 0; i < ENV_SIZE; i++)
		{
			std::cout << "env[i]: " << _cgiEnv[i] << std::endl;
		}
		// redirect I/O
		close(_sendPipe[0]); // close read-end of response
		dup2(_sendPipe[1], STDOUT_FILENO); // write to response pipe
		std::string cgiFilePath = "html";
		std::string cgiFilename = cgiFilePath + _request->getFileName();
		char *argv[2] = {(char *)cgiFilename.c_str(), NULL};
		execve(cgiFilename.c_str(), argv, _cgiEnv);
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

char **CGI::makeEnv()
{
		std::array<std::string, ENV_SIZE> envArr{
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

		char **env = new char*[ENV_SIZE + 1];
		for (int i = 0; i < ENV_SIZE; i++)
		{
			env[i] = new char[envArr[i].length() + 1];
			env[i] = (char *)envArr[i].c_str();
		}
		env[ENV_SIZE] = NULL;
		return (env);
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
