#!/bin/python3

import subprocess as sp
import argparse

config_folder = "../configs"
config_name = "/static_file_serving_location"
request_folder = "/static"
request_file = "/dsjghasgdjgkh.txt"
expected_output_folder = "../integration_test_expected_outputs"
expected_output_name = "/404_not_found"

def setup_parser():
    parser = argparse.ArgumentParser()
    parser.add_argument("server_path")

    return parser

def main():
    args = setup_parser().parse_args()

    # Start the server process.
    server_process = sp.Popen([args.server_path, config_folder + config_name, "--disable-file-logging"], stdout=sp.PIPE)

    request_path = request_folder + request_file

    print(request_path)

    # Send HTTP request to the server via curl.
    # Send the output (response header and content) to a pipe.
    curl_process = sp.Popen(["curl", "localhost:80" + request_path, "-is"], stdout=sp.PIPE)

    # Compare the output of the curl to the expected output using diff.
    diff_process = sp.Popen(["diff", "-", expected_output_folder + expected_output_name], stdin=sp.PIPE, stdout=sp.PIPE, stderr=sp.PIPE)

    curl_process_output = curl_process.stdout.read()

    print(curl_process_output.decode())

    # Kill the server
    server_process.kill()

    # Pipe the curl process stdout to the diff process' stdin.
    ret, err = diff_process.communicate(input=curl_process_output)

    # Check that the return code of diff is 0 (i.e. the output from curl is the expected output).
    if diff_process.returncode == 0:
        return 0
    else:
        return 1

if __name__ == "__main__":
    return_code = main()
    exit(return_code)
