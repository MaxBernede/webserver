#pragma once

#define REDIR_START "<html lang=\"en\"> \
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
			<h1>You are being redirected to "

#define REDIR_END "</h1> \
		</div> \
	</body> \
</html>"
