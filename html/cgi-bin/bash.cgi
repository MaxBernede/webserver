#!/bin/bash

# example CGI in bash

# html_content:
date=$(date)
html_content="<!DOCTYPE html><html><body><h2>Current time: $date </h2><a href='/'>go home</a></body></html>"
content_length=${#html_content}

# body with html content
echo -n -e "$html_content\r\n"

