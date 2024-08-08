#!/usr/bin/env python3

import cgi, os, sys
from os import environ
import cgitb; cgitb.enable()

message = ""

def handle_post():
	form = cgi.FieldStorage()
	if 'file' in form:
		file_item = form['file']
		if file_item.filename:
			file_name = os.path.basename(file_item.filename) #get the filename
			upload_dir = os.path.join(os.getcwd(), 'html/upload')
			file_path = os.path.join(upload_dir, file_name)
			# Check if the upload directory exists
			if os.path.exists(file_path):
				message = "Upload failed: a file with the same name already exists on the server."
				status_code = "409 Conflict"
			else:
				if not os.path.exists(upload_dir): #if the uploaddir doesnt exist creat it
					os.makedirs(upload_dir)
				# Write the file to the specified directory
				with open(file_path, 'wb') as fd:
					while True:
						chunk = file_item.file.read(1024)  # Read in chunks of 1024 bytes
						if not chunk:
							break  # EOF reached
						fd.write(chunk)
				message = f"Uploaded '{file_name}'"
				status_code = "201 Created"
		else:
			message = "No file was specified in the request"
			status_code = " 400 Bad Request"
	else:
		message = "No file was specified in the request"
		status_code = "400 Bad Request"

	html_content = (
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<body>\n"
		f"<h3>{message}</h3>\n"
		"<p><a href='../index.html'>Go to Homepage</a></p>\n"
		"</body>\n"
		"</html>\n"
	)
	html_headers = (
		"HTTP/1.1 " + status_code + "\r\n"
		"Content-Type: text/html\r\n"
		"Connection: close\r\n"
		f"Content-Length: {len(html_content)}\r\n\r"
	)
	print(html_headers)
	print(html_content)

def main():
	handle_post()

if __name__ == "__main__":
	main()