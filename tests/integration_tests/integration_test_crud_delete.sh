#!/bin/bash

config_folder="../configs"
config_name="/crud_config"
expected_output_folder="../integration_test_expected_outputs"
expected_output_name="/successful_DELETE_output"

# Parse the server_path argument
if [ $# -ne 1 ]; then
    echo "Usage: $0 server_path"
    exit 1
fi

server_path=$1

# Create data directory
mkdir -p ../data

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

# Send HTTP requests to the server via curl (GET and DELETE)
curl -isd "test" localhost:80/api/test
http DELETE localhost:80/api/test/1 > temp.txt

# Compare the temp file with the expected output using diff
diff temp.txt "$expected_output_folder$expected_output_name"

# Capture the return code of diff
diff_return_code=$?

# Kill the server process
kill $server_pid

# Clean up
rm -rf ../data
rm temp.txt

# Check the return code of diff (0 means the temp file matches the expected output and the data directory is deleted)
if [ $diff_return_code -eq 0 ] && [ ! -d "../data/test/1" ]; then
    exit 0
else
    exit 1
fi
