#!/bin/bash

# example CGI in bash

# html_content:
date=$(date)
html_content="<!DOCTYPE html>\
			<html><body>\
			<h2>$date</h2>
			</body></html>"

# body with html content
echo -n -e "$html_content\r\n"

