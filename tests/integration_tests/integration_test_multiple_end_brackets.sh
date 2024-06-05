#!/bin/bash

config_folder="../configs"
config_name="/multiple_end_brackets"
expected_output_folder="../integration_test_expected_outputs"
expected_output_name="/404_not_found"

# Parse the server_path argument
if [ $# -ne 1 ]; then
    echo "Usage: $0 server_path"
    exit 1
fi

server_path=$1

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

# Send HTTP request to the server via curl and capture the output
curl_output=$(curl localhost:80 -is)

# Kill the server process
kill $server_pid

# Print the curl process output
echo -e "Curl process output:\n$curl_output\n"

# Compare the output of the curl to the expected output using diff
echo "$curl_output" | diff - "$expected_output_folder$expected_output_name"

# Check the return code of diff (0 means the output from curl matches the expected output)
if [ $? -eq 0 ]; then
    exit 0
else
    exit 1
fi
