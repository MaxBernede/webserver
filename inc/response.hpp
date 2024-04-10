#pragma once


class Response {
	public:
		Response();
		Response(int fd);

		//Getters
		int	get_client_fd();

		//Members functions
		void handle_request();
		std::string parse_buffer(std::string buffer);
		void r_send();
	private:
		std::string file;
		std::string response_text;
		std::string html_file;
		int			client_fd;
};