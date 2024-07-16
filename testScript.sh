# Colors for pass/fail
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Base URL for the server
BASE_URL="http://localhost:8080"

# Function to test GET endpoints
test_get_endpoint() {
	local endpoint="$1"
	local expected_status="$2"
	local BASE_URL="$3"

	# Send a GET request to the endpoint and check the HTTP status code
	response_code=$(curl -o /dev/null -s -w "%{http_code}\n" "$BASE_URL$endpoint")

	# Check if the response code matches the expected status
	if [ "$response_code" -eq "$expected_status" ]; then
		echo -e "$endpoint endpoint: ${GREEN}PASS${NC}"
	else
		echo -e "$endpoint endpoint: ${RED}FAIL${NC}"
	fi
}

# Function to test POST endpoints
test_post_endpoint() {
	local endpoint="$1"
	local expected_status="$2"
	local BASE_URL="$3"
	local data="$4"
	local content_type="$5"

	# Send a POST request to the endpoint and check the HTTP status code
	response_code=$(curl -o /dev/null -s -w "%{http_code}\n" -X POST -d "$data" -H "Content-Type: $content_type" "$BASE_URL$endpoint")

	# Check if the response code matches the expected status
	if [ "$response_code" -eq "$expected_status" ]; then
		echo -e "$endpoint endpoint: ${GREEN}PASS${NC}"
	else
		echo -e "$endpoint endpoint: ${RED}FAIL${NC}"
	fi
}

# Test GET endpoints
echo "Testing GET endpoints..."
test_get_endpoint "/" 200 "$BASE_URL"
test_get_endpoint "/page1.html" 200 "$BASE_URL"
test_get_endpoint "/complete_error" 404 "$BASE_URL"  # Assuming this should be 404

# Test POST endpoints
echo "Testing POST endpoints..."
test_post_endpoint "/post_endpoint" 200 "$BASE_URL" "param1=value1&param2=value2" "application/x-www-form-urlencoded"
test_post_endpoint "/upload" 201 "$BASE_URL" "@/path/to/your/file" "multipart/form-data"
