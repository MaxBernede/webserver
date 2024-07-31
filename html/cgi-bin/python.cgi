#!/usr/bin/python3
from os import environ
import sys

def parse_body():
	# Read all input from stdin
	body = sys.stdin.read()
	return (body)


def generate_html_body():
	# Get the body content from stdin
	body_content = parse_body()

	# Create the HTML body string
	# html_body = f"<p>{body_content}</p>"
	return html_body

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
html_content_start = (
	"<!DOCTYPE html>\n"
	"<html>\n"
	"<body>\n"
	f"<h3>Hello {username}</h3>\n"
	"<p>This is what you posted:</p>\n"
	)



html_content_end = (
	"</body>\n"
	"</html>\n"
)

print(html_content_start)
print(generate_html_body())
print(html_content_end)
