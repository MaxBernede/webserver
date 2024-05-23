#!/bin/bash

GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m'

if nc -z localhost 8080; then
    BASE_URL="http://localhost:8080"
else
    BASE_URL="http://localhost:8090"
fi

# Test endpoints
echo "Testing endpoints..."

test_endpoint() {
    local endpoint="$1"
    local expected_status="$2"
    local BASE_URL="$3"

    # Send a request to the endpoint and check the HTTP status code
    response_code=$(curl -o /dev/null -s -w "%{http_code}\n" "$BASE_URL$endpoint")

    # Check if the response code matches the expected status
    if [ "$response_code" -eq "$expected_status" ]; then
        echo -e "$endpoint endpoint: ${GREEN}PASS${NC}"
    else
        echo -e "$endpoint endpoint: ${RED}FAIL${NC}"
    fi
}

test_endpoint "/" 200 "$BASE_URL"  # Testing root endpoint
test_endpoint "/page1.html" 200 "$BASE_URL"

# Add more tests as needed

echo "Testing completed."
pkill exe
