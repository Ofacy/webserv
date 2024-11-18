FROM ubuntu:latest

WORKDIR /app

RUN apt-get update && apt-get install -y \
	php-cgi build-essential valgrind

COPY . .

RUN make

EXPOSE 8080
EXPOSE 8082
EXPOSE 8083

CMD ["/app/webserv"]