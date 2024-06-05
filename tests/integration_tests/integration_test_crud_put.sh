#!/bin/bash

config_folder="../configs"
config_name="/crud_config"
expected_output_folder="../integration_test_expected_outputs"
expected_output_name="/successful_PUT_output"
target_output_file="../data/test/1"

# Parse the server_path argument
if [ $# -ne 1 ]; then
    echo "Usage: $0 server_path"
    exit 1
fi

server_path=$1

# Start the server process
$server_path "$config_folder$config_name" --disable-file-logging &
server_pid=$!

# Send HTTP requests to the server via curl (POST and PUT)
curl -isd "test" localhost:80/api/test
curl -isX PUT -d "asdf" localhost:80/api/test/1

# Compare the target output file with the expected output using diff
diff "$expected_output_folder$expected_output_name" "$target_output_file"

# Capture the return code of diff
diff_return_code=$?

# Kill the server process
kill $server_pid

# Clean up
rm -rf ../data

# Check the return code of diff (0 means the target output file matches the expected output)
if [ $diff_return_code -eq 0 ]; then
    exit 0
else
    exit 1
fi
