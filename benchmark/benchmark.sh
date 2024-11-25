#! /bin/bash

THREADS=$(nproc --all)
REQUESTS=8000

echo "RUNNING BENCHMARK"
echo "THREADS: $THREADS"
echo "REQUESTS: $REQUESTS"
echo "NGINX RESULT"
siege -c $THREADS -r $REQUESTS -b -f ./tester_benchmark.nginx.url

echo "WEBSERVER RESULT"
siege -c $THREADS -r $REQUESTS -b -f ./tester_benchmark.url
