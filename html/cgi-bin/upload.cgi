#!/usr/bin/python3

import cgi
import os

def main():
	form = cgi.FieldStorage()

	# Print all form fields
	for field in form.keys():
		field_item = form[field]
		if field_item.filename:
			# Field is an uploaded file
			filename = field_item.filename
			file_content = field_item.file.read().decode('utf-8', 'ignore')  # Read file content
			print(f"Uploaded file: {filename}")
			print(f"Content:\n{file_content}")
		else:
			# Regular form field
			value = form.getvalue(field)
			print(f"Field: {field}, Value: {value}")

if __name__ == "__main__":
	main()