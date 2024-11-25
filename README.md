# Webserv

**Webserv** is a lightweight HTTP server written in C++, inspired by Nginx. It was developed as part of a project at 42 Lyon.

## Features
- Serve static files.
- Handles HTTP requests with support for basic methods like GET, POST, DELETE, PUT, HEAD.
- Configurable *access log* file.
- CGI support for dynamic content generation.
- Multi-server configuration with independent settings for each server.
- Chunked transfer encoding.
- Directory listing (*autoindex*).
- Custom error pages.


## Getting Started

### Prerequisites
- A C++ compiler supporting the C++98 standard.
- GNU Make.

### Installation
1. Clone the repository:
	```bash 
	git clone git@github.com:Ofacy/webserv
	cd webserv
	```

2. Build the project:
	```bash
	make
	```

3. Run the server with a configuration file:
	```bash
	./webserv <config_file>
	```

## Authors
- [**lcottet**](https://profile.intra.42.fr/users/lcottet)
- [**bwisniew**](https://profile.intra.42.fr/users/bwisniew)
