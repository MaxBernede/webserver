#!/usr/bin/python3
from os import environ
import sys

# find username from cookies
username = ""
if 'HTTP_COOKIE' in environ and environ['HTTP_COOKIE']:
    # Split the cookies string by ';' to get individual cookies
    cookies = environ['HTTP_COOKIE'].split(';')
    for cookie in cookies:
        key, value = map(str.strip, cookie.split('=', 1))  # Limit split to 1 to handle values with '='
        if key == "username":
            username = value

# prepare the html content
html_content = (
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<body>\n"
    f"<h3>Hello {username}</h3>\n"
    "<p>This is a python script where you can...</p>\n"
    "</body>\n"
    "</html>\n"
    )
print(f"HTTP_COOKIE: {environ.get('HTTP_COOKIE', '')}")
print(html_content)
