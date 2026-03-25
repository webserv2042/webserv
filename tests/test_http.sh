#!/bin/bash

./webserv tests/server.conf &
SERVER_PID=$!

trap "kill $SERVER_PID 2>/dev/null" EXIT

sleep 1

#GET TEST
echo "Testing Get..."

TEST1=$(curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/)
if [ "$TEST1" = "200" ]; then
	echo "GET / PASS"
else
	echo "GET / FAILED (got $TEST1)"
	kill $SERVER_PID
	exit 1
fi

TEST2=$(curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/doesnotexist)
if [ "$TEST2" = "404" ]; then
	echo "GET /doesnotexist PASS"
else
	echo "GET /doesnotexist FAILED (got $TEST2)"
	kill $SERVER_PID
	exit 1
fi

TEST3=$(curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/noperm)
if [ "$TEST3" = "403" ]; then
	echo "GET /noperm PASS"
else
	echo "GET /noperm FAILED (got $TEST3)"
	kill $SERVER_PID
	exit 1
fi


kill $SERVER_PID
