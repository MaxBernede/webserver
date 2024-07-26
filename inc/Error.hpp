#pragma once
#include <string>

# define NOT_FOUND "<html lang=\"en\"> \
	<head> \
		<meta charset=\"UTF-8\"> \
	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> \
		<title>Error Page</title> \
			<style> \
				body { \
					font-family: Arial, sans-serif; \
					background-color: #ff0000; \
					color: #212529; \
					margin: 0; \
					padding: 0; \
					display: flex; \
					justify-content: center; \
					align-items: center; \
					height: 100vh; \
				} \
				.error-container { \
					text-align: center; \
				} \
				h1 { \
					font-size: 36px; \
					margin-bottom: 20px; \
				} \
				p { \
					font-size: 18px; \
				} \
			</style> \
	</head> \
	<body> \
		<div class=\"error-container\"> \
			<h1>Error 404, page not found.</h1> \
			<p>We're sorry, but an error occurred while processing your request.</p> \
			<p>Please try again later.</p> \
			<a href=\"index.html\">Back to Home</a> \
		</div> \
	</body> \
</html>"

# define NOT_ALLOWED "<html lang=\"en\"> \
	<head> \
		<meta charset=\"UTF-8\"> \
	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> \
		<title>Error Page</title> \
			<style> \
				body { \
					font-family: Arial, sans-serif; \
					background-color: #00ff00; \
					color: #212529; \
					margin: 0; \
					padding: 0; \
					display: flex; \
					justify-content: center; \
					align-items: center; \
					height: 100vh; \
				} \
				.error-container { \
					text-align: center; \
				} \
				h1 { \
					font-size: 36px; \
					margin-bottom: 20px; \
				} \
				p { \
					font-size: 18px; \
				} \
			</style> \
	</head> \
	<body> \
		<div class=\"error-container\"> \
			<h1>Error 405, method not allowed.</h1> \
			<p>We're sorry, but an error occurred while processing your request.</p> \
			<p>Please try again later.</p> \
			<a href=\"index.html\">Back to Home</a> \
		</div> \
	</body> \
</html>"

# define NOT_IMPLEMENTED "HTTP/1.1 501 OK\r\nContent-Type: text/html\r\n\r\n \
<html lang=\"en\"> \
	<head> \
		<meta charset=\"UTF-8\"> \
	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> \
		<title>Error Page</title> \
			<style> \
				body { \
					font-family: Arial, sans-serif; \
					background-color: #0000ff; \
					color: #212529; \
					margin: 0; \
					padding: 0; \
					display: flex; \
					justify-content: center; \
					align-items: center; \
					height: 100vh; \
				} \
				.error-container { \
					text-align: center; \
				} \
				h1 { \
					font-size: 36px; \
					margin-bottom: 20px; \
				} \
				p { \
					font-size: 18px; \
				} \
			</style> \
	</head> \
	<body> \
		<div class=\"error-container\"> \
			<h1>Error 501, functionality not implemented.</h1> \
			<p>We're sorry, but an error occurred while processing your request.</p> \
			<p>Please try again later.</p> \
			<a href=\"index.html\">Back to Home</a> \
		</div> \
	</body> \
</html>"

# define START_HTML "<html lang=\"en\"> \
	<head> \
		<meta charset=\"UTF-8\"> \
	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> \
		<title>Error Page</title> \
			<style> \
				body { \
					font-family: Arial, sans-serif; \
					background-color: #ff00ff; \
					color: #212529; \
					margin: 0; \
					padding: 0; \
					display: flex; \
					justify-content: center; \
					align-items: center; \
					height: 100vh; \
				} \
				.error-container { \
					text-align: center; \
				} \
				h1 { \
					font-size: 36px; \
					margin-bottom: 20px; \
				} \
				p { \
					font-size: 18px; \
				} \
			</style> \
	</head> \
	<body> \
		<div class=\"error-container\"> \
			<h1>Error "

#define END_HTML "</h1> \
			<a href=\"index.html\">Back to Home</a> \
		</div> \
	</body> \
</html>"

# define HTML_FORBITTEN "<html lang=\"en\"> \
	<head> \
		<meta charset=\"UTF-8\"> \
	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> \
		<title>Error Page</title> \
			<style> \
				body { \
					font-family: Arial, sans-serif; \
					background-color: #0000ff; \
					color: #212529; \
					margin: 0; \
					padding: 0; \
					display: flex; \
					justify-content: center; \
					align-items: center; \
					height: 100vh; \
				} \
				.error-container { \
					text-align: center; \
				} \
				h1 { \
					font-size: 36px; \
					margin-bottom: 20px; \
				} \
				p { \
					font-size: 18px; \
				} \
			</style> \
	</head> \
	<body> \
		<div class=\"error-container\"> \
			<h1>Error 403, Forbidden</h1> \
			<a href=\"index.html\">Back to Home</a> \
		</div> \
	</body> \
</html>"