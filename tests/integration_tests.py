#!/bin/python3

import subprocess as sp

if __name__ == "__main__":
    # Start the server process.
    server_process = sp.Popen(["../build/bin/server", "example_config"], stdout=sp.PIPE)

    # Send HTTP request to the server via curl.
    # Send the output (response header and content) to a pipe.
    curl_process = sp.Popen(["curl", "localhost:80", "-is"], stdout=sp.PIPE)

    # Kill the server
    server_process.kill()

    # Compare the output of the curl to the expected output using diff.
    diff_process = sp.Popen(["diff", "-", "example_config_expected_output"], stdin=sp.PIPE, stdout=sp.PIPE, stderr=sp.PIPE)

    curl_process_output = curl_process.stdout.read()

    # Pipe the curl process stdout to the diff process' stdin.
    ret, err = diff_process.communicate(input=curl_process_output)

    # Check that the return code of diff is 0 (i.e. the output from curl is the expected output).
    if diff_process.returncode == 0:
        exit(0)
    else:
        exit(1)

