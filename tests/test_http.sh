#!/bin/bash

./webserv tests/ci_test.conf &
SERVER_PID=$!

trap "kill $SERVER_PID 2>/dev/null" EXIT

sleep 1

#GET TEST
echo "Testing GET..."

TEST1=$(curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/)
if [ "$TEST1" = "200" ]; then
	echo "GET / PASS"
else
	echo "GET / FAILED (got $TEST1)"
	exit 1
fi

TEST2=$(curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/doesnotexist)
if [ "$TEST2" = "404" ]; then
	echo "GET /doesnotexist PASS"
else
	echo "GET /doesnotexist FAILED (got $TEST2)"
	exit 1
fi

TEST3=$(curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/noperm)
if [ "$TEST3" = "403" ]; then
	echo "GET /noperm PASS"
else
	echo "GET /noperm FAILED (got $TEST3)"
	exit 1
fi

TEST4=$(curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/$(python3 -c "print('a' * 10000)"))
if [ "$TEST4" = "414" ]; then
	echo "GET /URI too long PASS"
else
	echo "GET /URI too long FAILED (got $TEST4)"
	exit 1
fi

echo "Testing POST..."

rm -f srcs/www/upload/test.txt

TEST5=$(curl -s -X POST -d "hello mario" http://localhost:8080/upload/test.txt -o /dev/null -w "%{http_code}")
if [ "$TEST5" = "201" ]; then
	echo "POST /upload/test.txt PASS"
else
	echo "POST /upload/test.txt FAILED (got $TEST5)"
	exit 1
fi

TEST6=$(curl -s -X POST -d "hello mario" http://localhost:8080/upload/test.txt -o /dev/null -w "%{http_code}")
if [ "$TEST6" = "409" ]; then
	echo "POST /upload/test.txt (conflict) PASS"
else
	echo "POST /upload/test.txt (conflict) FAILED (got $TEST6)"
	exit 1
fi

TEST7=$(curl -s -X POST -d "$(python3 -c "print('a'*100000)")" http://localhost:8080/upload/big.txt -o /dev/null -w "%{http_code}")
if [ "$TEST7" = "413" ]; then
	echo "POST content too large PASS"
	echo "check maybe it passed becasue of the config"
else
	echo "POST content too large FAILED (got $TEST7)"
	exit 1
fi

echo "Testing DELETE..."

TEST8=$(curl -s -X DELETE http://localhost:8080/upload/test.txt -o /dev/null -w "%{http_code}")
if [ "$TEST8" = "405" ]; then
	echo "DELETE /upload/test.txt PASS"
else
	echo "DELETE /upload/test.txt FAILED (got $TEST7)"
	exit 1
fi

echo "All tests PASSED!"