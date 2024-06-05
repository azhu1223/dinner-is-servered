#!/bin/bash

config_folder="../configs"
config_name="/crud_config"
expected_output_folder="../integration_test_expected_outputs"
expected_output_name="/successful_LIST_output"

# Parse the server_path argument
if [ $# -ne 1 ]; then
    echo "Usage: $0 server_path"
    exit 1
fi

server_path=$1

# Create data directory if it doesn't exist
if [ ! -d "../data" ]; then
    mkdir -p "../data"
fi

# Start the server process
$server_path "$config_folder$config_name" --disable-file-logging &
server_pid=$!

# Function to check if the server is ready
check_server_ready() {
    for i in {1..10}; do
        if curl -s localhost:80 > /dev/null; then
            return 0
        fi
        sleep 1
    done
    return 1
}

# Wait for the server to be ready
if ! check_server_ready; then
    echo "Server did not become ready in time."
    kill $server_pid
    exit 1
fi

# Send three HTTP requests to the server
for i in {1..3}; do
    curl -isd "test" localhost:80/api/test
done

# Send HTTP request to the server via curl
curl_output=$(curl -is localhost:80/api/test > temp)

# curl -is localhost:80/api/test > "$expected_output_folder$expected_output_name"


# Compare the output of the curl to the expected output using diff
echo "Difference"
echo diff  "$expected_output_folder$expected_output_name" "temp"

# Capture the return code of diff
diff_return_code=$?

echo "Diff return code: $diff_return_code"


# Kill the server process
kill $server_pid

# Remove the data directory
rm -rf ../data
rm text

# Check the return code of diff (0 means the output from curl is the expected output)
if [ $diff_return_code -eq 0 ]; then
    exit 0
else
    exit 1
fi
