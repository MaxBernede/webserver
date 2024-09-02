#include "CGI.hpp"

CGI::CGI(Request* request, int clientFd) : _request(request), _clientFd(clientFd)
{
	_filePath = _request->getConfig().getRoot() + _request->getFileName();
	if (!isExecutable()) // No execution rights for the file
	{
		Logger::log("No execution rights on CGI file", LogLevel::ERROR);
		throw(HTTPError(INTERNAL_SRV_ERR));
	}
	makeEnvArr();
	makeEnvCStr();
	if (pipe(_responsePipe) < 0 || pipe(_uploadPipe) < 0)
	{
		Logger::log("Pipe failed", LogLevel::ERROR);
		throw(HTTPError(INTERNAL_SRV_ERR));
	}
}

CGI::~CGI()
{
	delete[] _cgiEnvCStr;
}

bool CGI::isExecutable()
{
	struct stat fileStat;
	
 	// Retrieve file status
    if (stat(_filePath.c_str(), &fileStat) != 0) {
        std::cerr << "Error retrieving file status." << std::endl;
        return false;
    }
	// Check executable permission for the owner
    return (fileStat.st_mode & S_IXUSR) != 0;
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
		char* argv[2] = { (char*)_filePath.c_str(), NULL };
		execve(_filePath.c_str(), argv, _cgiEnvCStr);
		// if execve fails
		delete[] _cgiEnvCStr;
		std::cerr << "Running CGI script failed (execve), path: " << _filePath << std::endl;
		exit(1); // exit child process with 1, upon failure
	}
	else //parent (main) process
	{
		close(_responsePipe[1]); // close write-end of the response pipe (send)
	}
}

bool CGI::waitCgiChild()
{
	int exitCode;
	int status = waitpid(_pid, &exitCode, WNOHANG);
	if (status == -1)
	{
		throw(Exception("Error while waiting for cgi with pid " + std::to_string(_pid), 1));
	}
	else if (status == 0) // cgi not done
	{
		return false;
	}
	else
	{
		if (WIFEXITED(exitCode)) // Child exited normally
		{
			if (WEXITSTATUS(exitCode) != 0) // Non-zero exit status
			{
				throw(Exception("Error: CGI script with pid " + std::to_string(_pid) + " exited with status " + std::to_string(WEXITSTATUS(exitCode)), 1));
			}
		}
		else if (WIFSIGNALED(exitCode)) // Child terminated by a signal
		{

			Logger::log("Error: CGI script with pid " + std::to_string(_pid) + " was killed by signal " + std::to_string(WTERMSIG(exitCode)), LogLevel::ERROR);
			throw(HTTPError(INTERNAL_SRV_ERR));
		}
		Logger::log("Cgi child process finished", LogLevel::INFO);
		return true;
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

