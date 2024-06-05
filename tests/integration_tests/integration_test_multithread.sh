#!/bin/bash

config_folder="../configs"
config_name="/health_config"
expected_output_folder="../integration_test_expected_outputs"
expected_output_name="/health_output"

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

# Send HTTP request to the server via curl
start_time=$(date +%s.%N)
curl -is "localhost:80/sleep" > /dev/null &
curl_output=$(curl -is "localhost:80/health")

# Compare the output of the curl to the expected output using diff
echo "$curl_output" | diff - "$expected_output_folder$expected_output_name"

# Capture the return code of diff
diff_return_code=$?

# Kill the server process
kill $server_pid

# Check the return code of diff (0 means the output from curl is the expected output)
if [ $diff_return_code -eq 0 ]; then
    duration=$(echo "$end_time - $start_time" | bc)
    if (( $(echo "$duration < 1" | bc -l) )); then
        echo "The section completed within 1 second. Took $duration seconds."
        exit 0
    else
        echo "The section did not complete within 1 second. Took $duration seconds."
        exit 1
    fi
else
    exit 1
fi
