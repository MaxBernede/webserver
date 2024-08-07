#include "CGI.hpp"

CGI::CGI(Request* request, int clientFd) : _request(request), _clientFd(clientFd)
{
	makeEnvArr();
	makeEnvCStr();
	if (pipe(_responsePipe) < 0)
		throw (Exception("Pipe failed", 1));
	if (pipe(_uploadPipe) < 0)
		throw(Exception("Pipe failed", 1));
}

CGI::~CGI(void)
{
	delete[] _cgiEnvCStr;
}

void CGI::run()
{
	_pid = fork(); //forking to create new process
	if (_pid == 0) //if child process
	{
		// redirect I/O
		close(_uploadPipe[1]); // close write-side of upload pipe
		dup2(_uploadPipe[0], STDIN_FILENO); // read from upload pipe 
		close(_responsePipe[0]); // close read-end of response pipe
		dup2(_responsePipe[1], STDOUT_FILENO); // write to response pipe
		std::string cgiFilePath = _request->getConfig().getRoot() + _request->getFileName();
		char* argv[2] = { (char*)cgiFilePath.c_str(), NULL };
		execve(cgiFilePath.c_str(), argv, _cgiEnvCStr);
		// if execve fails
		delete[] _cgiEnvCStr;
		Logger::log("Running CGI script failed (execve), path: " + cgiFilePath, LogLevel::ERROR);
		exit(1); // exit child process with 1, upon failure
	}
	else //parent (main) process
	{
		_forkTime = std::chrono::high_resolution_clock::now();
		close(_responsePipe[1]); // close write-end of the response pipe (send)
	}
}

bool 	CGI::waitCgiChild()
{
	int exitCode;
	int status = waitpid(_pid, &exitCode, WNOHANG);
	if (status == -1)
		throw(Exception("Error while waiting for cgi with pid " + std::to_string(_pid), 1));
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

void CGI::makeEnvArr()
{
	std::vector<std::string> envArr{
		"CONTENT_LENGTH=" + _request->getValues("Content-Length"),
		"CONTENT_TYPE=" + _request->getValues("Content-Type"),
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
	_cgiEnvArr = envArr;
}

void CGI::makeEnvCStr()
{
	char** env = new char* [_cgiEnvArr.size() + 1];
	for (size_t i = 0; i < _cgiEnvArr.size(); ++i)
		env[i] = (char*)this->_cgiEnvArr[i].c_str(); // setting strings, but not allocated so _cgiEnvArr must remain
	env[_cgiEnvArr.size()] = NULL;
	_cgiEnvCStr = env;
}

void	CGI::killChild()
{
	if (_pid > 0)
	{
		if (kill(_pid, SIGKILL) < 0)
			Logger::log("failed to kill child", ERROR);
		waitpid(_pid, nullptr, 0); // Wait for the child process to terminate
	}
}

bool CGI::isTimeOut()
{
	auto _end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> _timePassed = _end - _forkTime;
	if (_timePassed.count() > 10)
		return (true);
	return (false);
}

void	CGI::closeUploadPipe()
{
	close(_uploadPipe[1]); // close write-end of the upload pipe
	close(_uploadPipe[0]); // close read-end of the upload pipe
}

int	CGI::getReadFd()
{
	return (_responsePipe[0]); // read-side of response pipe
}

int CGI::getWriteFd()
{
	return (_uploadPipe[1]); // write-side of upload pipe
}


int CGI::getClientFd()
{
	return (_clientFd);
}

Request* CGI::getRequest()
{
	return (_request);
}

