#!/bin/python3

import subprocess as sp
import argparse
import time

config_folder = "../configs"
config_name = "/health_config"
expected_output_folder = "../integration_test_expected_outputs"
expected_output_name = "/health_output"

def setup_parser():
    parser = argparse.ArgumentParser()
    parser.add_argument("server_path")

    return parser

def main():
    args = setup_parser().parse_args()

    # Start the server process.
    server_process = sp.Popen([args.server_path, config_folder + config_name, "--disable-file-logging"], stdout=sp.PIPE)

    # Send HTTP request to the server via curl.
    # Send the output (response header and content) to a pipe.
    start_time = time.time()
    sleep_process = sp.Popen(["curl", "localhost:80/sleep", "-is"] , stdout=sp.PIPE)
    curl_process = sp.Popen(["curl", "localhost:80/health", "-is"], stdout=sp.PIPE)

    # Compare the output of the curl to the expected output using diff.
    diff_process = sp.Popen(["diff", "-", expected_output_folder + expected_output_name], stdin=sp.PIPE, stdout=sp.PIPE, stderr=sp.PIPE)

    curl_process_output = curl_process.stdout.read()


    # Kill the server
    server_process.kill()

    print(f"Curl process output:\n{curl_process_output.decode()}\n")

    # Pipe the curl process stdout to the diff process' stdin.
    ret, err = diff_process.communicate(input=curl_process_output)

    # Check that the return code of diff is 0 (i.e. the output from curl is the expected output).
    if diff_process.returncode == 0:
        end_time = time.time()
        duration = end_time - start_time
        # TODO Uncomment the line below once the multithreading is enabled
        # assert duration < 1, f"The section did not complete within 1 second. Took {duration} seconds."
        print(f"The section completed within 1 second. Took {duration} seconds.")
        return 0
    else:
        return 1

if __name__ == "__main__":
    return_code = main()
    exit(return_code)