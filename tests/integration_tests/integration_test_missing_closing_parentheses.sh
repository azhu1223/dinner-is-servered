#!/bin/bash

config_folder="../configs"
config_name="/missing_closing_parentheses"

# Parse the server_path argument
if [ $# -ne 1 ]; then
    echo "Usage: $0 server_path"
    exit 1
fi

server_path=$1

# Start the server process
$server_path "$config_folder$config_name" --disable-file-logging &
server_pid=$!

# Wait for the server process to complete
wait $server_pid
server_return_code=$?

# Check that the return code of the server is 1 (i.e., the server failed to start)
if [ $server_return_code -eq 1 ]; then
    exit 0
else
    exit 1
fi
