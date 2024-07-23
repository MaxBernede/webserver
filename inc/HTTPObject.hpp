#include "webserver.hpp"

class HTTPObject {
	private:
		int			_port;
		int			_clientFd;
		Server 		_config;
		Request		*_request;
		Response	*_response;
		CGI			_cgi;
		bool		_isCgi;
	
	public:
		HTTPObject(int clientFd);
		~HTTPObject(void);
		
		std::string redirectResponse(void);
		void		sendResponse(void);
		void		setConfig();

		bool		isCgi(void);
		int			getClientFd(void); 
};
