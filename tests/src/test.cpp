#include <iostream>
#include <sstream>
#include <string>
#include <vector>


std::pair<std::string, std::string> create_pair(const std::string &line, size_t pos){
	std::string key = line.substr(0, pos);
	std::string value = line.substr(pos + 1); // Skip the delimiter
	if (!value.empty() && value[0] == ' ')
		value = value.substr(1); // Remove leading space if present
	return std::make_pair(key, value);
}

std::vector<std::pair<std::string, std::string>> parse_response(const std::string& headers) {
	std::vector<std::pair<std::string, std::string>> headerVec;

	std::istringstream iss(headers);
	std::string line;

	std::getline(iss, line);
	size_t pos = line.find(' ');
	if (pos != std::string::npos)
		headerVec.emplace_back(create_pair(line, pos));
	while (std::getline(iss, line)) {
		size_t pos = line.find(':');
		if (pos != std::string::npos)
			headerVec.emplace_back(create_pair(line, pos));
	}

	return headerVec;
}

int main() {
	std::string text = "GET / HTTP/1.1\n"
						  "Host: localhost:8081\n"
						  "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:125.0) Gecko/20100101 Firefox/125.0\n"
						  "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\n"
						  "Accept-Language: en-US,en;q=0.5\n"
						  "Accept-Encoding: gzip, deflate, br\n"
						  "Connection: keep-alive\n"
						  "Upgrade-Insecure-Requests: 1\n"
						  "Sec-Fetch-Dest: document\n"
						  "Sec-Fetch-Mode: navigate\n"
						  "Sec-Fetch-Site: none\n"
						  "Sec-Fetch-User: ?1\n";

	std::vector<std::pair<std::string, std::string>> headerVec = parse_response(text);

	// Print parsed headers
	for (const auto& pair : headerVec) {
		std::cout << pair.first << ": " << pair.second << std::endl;
	}

	return 0;
}
