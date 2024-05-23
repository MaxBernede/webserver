#!/bin/bash

# Define the base URL of your webserver
BASE_URL="http://localhost:8080"

# Test endpoints
echo "Testing endpoints..."

# Test the root endpoint
curl -o /dev/null -s -w "%{http_code}\n" "$BASE_URL" | grep -q "200"
if [ $? -eq 0 ]; then
    echo "Root endpoint: PASS"
else
    echo "Root endpoint: FAIL"
fi

curl -o /dev/null -s -w "%{http_code}\n" "$BASE_URL/page1.html" | grep -q "200"
if [ $? -eq 0 ]; then
    echo "/page1.html endpoint: PASS"
else
    echo "/page1.html endpoint: FAIL"
fi

# Add more tests as needed

echo "Testing completed."
pkill exe
