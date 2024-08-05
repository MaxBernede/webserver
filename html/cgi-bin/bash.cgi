#!/bin/bash

# example CGI in bash

# html_content:
date=$(date)



html_content="<!DOCTYPE html>\
			<head><style>\
			body{font-family: monospace;}\
			h2{align-content: centre;}
			</style></head>
			<html><body>\
			<h2>$date</h2>
			</body></html>"

headers="HTTP/1.1 200 OK \r\nConnection: close \r\n"

# body with html content
# echo -n -e "HTTP/1.1 200 OK\r\n"
# echo -n -e "Connection: close\r\n"
echo -n -e "$headers"
echo -n -e "\r\n"
echo -n -e "$html_content\r\n"

