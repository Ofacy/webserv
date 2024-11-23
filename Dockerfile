FROM debian:latest

WORKDIR /app

RUN apt update && apt install -y \
	php-cgi g++ make valgrind

COPY . .

RUN make

EXPOSE 8080
EXPOSE 8082
EXPOSE 8083

CMD ["/app/webserv"]