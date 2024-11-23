FROM debian:latest

WORKDIR /app

RUN apt update && apt install -y \
	php-cgi g++ make valgrind

COPY . .

RUN make

EXPOSE 8080

CMD ["/app/webserv", "/app/benchmark/benchmark.conf"]