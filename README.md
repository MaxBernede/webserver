A simple HTTP webserver mimicking nginx, written in C++.

## How it works?
- I/O Operations are non-blocking, meaning they go through the Linux system call poll() 

## Features
-[x] CGI
-[] Cookies
-[x] IPs other than localhost/ 127.0.0.1 work
