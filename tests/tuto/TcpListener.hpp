#pragma once

#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <sstream>

#pragma comment (lib, "ws2_32.lib")

class TcpListener
{
	public:
		
		TcpListener(const std::string ipAddress, int port) :
			m_ipAddress(ipAddress), m_port(port) {}

		// Initialize the listener
		int init();

		// Run the listener
		int run();
	protected:
	private:
		const std::string m_ipAddress;	// IP address server will run on
		int m_port;						// Port # for the web service
		int m_socket;					//Internal FD for the listening socket
}