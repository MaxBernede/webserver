#pragma once

#include <string>
#include <vector>
#include <list>
#include "OurClasses.hpp"
#include "Utils.hpp"

class Config{
	protected:
		// which methods are allowed
		std::vector<bool>		_methods;
		// http redirect definitions
		std::list<s_redirect>	_redirect;
		// (dis)allow directory listing
		bool					_autoIndex;
		// "home page", or default page to respond with when directory is requested
		std::string				_index;
		// whether common gateway interface is allowed
		bool					_cgi;
		// in case of POST or DELETE request:
		// directory in which to POST of from which to DELETE
		std::string				_path;
		std::string				_basePath;
	public:
		Config();
		~Config();
		Config(const Config &obj);
		Config &operator=(const Config &obj);

		void	setMethod(int method, bool value);
		void	setRedirect(s_redirect redir);
		void	setRedirect(std::list<s_redirect> redir);
		void	setAutoIndex(bool autoIndex);
		void	setIndex(std::string index);
		void	setCGI(bool CGI);
		void	setPath(std::string value);

		std::vector<bool>		getMethods() const;
		bool					getMethod(int i) const;
		std::list<s_redirect>	getRedirect() const;
		bool					getAutoIndex() const;
		std::string				getIndex() const;
		bool					getCGI() const;
		std::string				getPath() const;
};