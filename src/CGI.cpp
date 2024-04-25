#include "CGI.hpp"


CGI::CGI(std::string req) : _request(req), _cgiEnv(makeEnv(req))
{
	pipe(_receivePipe);
	pipe(_sendPipe);
}

CGI::~CGI(void)
{
	delete [] _cgiEnv;
}

void CGI::runCgi()
{
	int pid = fork(); //forking to create new process
	if (pid == 0) //if child process
	{
		// redirect I/O
		close(_sendPipe[0]); // close read-end of response
		close(_receivePipe[1]); // close write-end of receiving pipe
		dup2(_sendPipe[1], STDOUT_FILENO); // write to response pipe
		dup2(_receivePipe[0], STDIN_FILENO); // read from receiving pipe

		std::string cgiFilePath = "var/www";
		std::string cgiFilename = cgiFilePath + "python.cgi";

		execve(cgiFilename.c_str(), NULL, _cgiEnv);
		// if execve fails
		std::cout << "Running CGI script failed (execve), path: " << cgiFilename << std::endl;
		exit(1); // exit child process with 1, upon failure
	}
	else //parent (main) process
	{
		close(_sendPipe[1]); // close write-end of the response pipe (send)
		close(_receivePipe[0]); //_receivePipe[0] -> will be given to pollFd to write to the CGI! FOR NOW CLOSED
	}
}

char **CGI::makeEnv(std::string req)
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
		"REQUEST_METHOD=",
		"SCRIPT_NAME=",
		"SCRIPT_FILENAME=",
		"SERVER_NAME=",
		"SERVER_PORT=",
		"SERVER_PROTOCOL=HTTP/1.1", // fixed
		"SERVER_SOFTWARE=WebServServer/1.0", // fixed
		"HTTP_COOKIE=",
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