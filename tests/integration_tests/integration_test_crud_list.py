#!/bin/python3

import subprocess as sp
import argparse
import shutil
import os
import time

config_folder = "../configs"
config_name = "/crud_config"
expected_output_folder = "../integration_test_expected_outputs"
expected_output_name = "/successful_LIST_output"

def setup_parser():
    parser = argparse.ArgumentParser()
    parser.add_argument("server_path")

    return parser

def main():
    args = setup_parser().parse_args()
    if (not os.path.exists("../data")):
        os.mkdir("../data")

    # Start the server process.
    server_process = sp.Popen([args.server_path, config_folder + config_name, "--disable-file-logging"], stdout=sp.PIPE)

    for i in range(3):
        os.system("curl -isd test localhost:80/api/test")   

    curl_process = sp.Popen(["curl", "-is", "localhost:80/api/test"], stdout=sp.PIPE)

    diff_process = sp.Popen(["diff", "-", expected_output_folder + expected_output_name], stdin=sp.PIPE, stdout=sp.PIPE, stderr=sp.PIPE)

    curl_process_output = curl_process.stdout.read()

    # Kill the server
    server_process.kill()

    print(f"Curl process output:\n{curl_process_output.decode()}\n")

    # Pipe the curl process stdout to the diff process' stdin.
    ret, err = diff_process.communicate(input=curl_process_output)

    # Check that the return code of diff is 0 (i.e. the output from curl is the expected output).
    if diff_process.returncode == 0:
        return 0
    else:
        return 1

if __name__ == "__main__":
    return_code = main()
    if (os.path.exists("../data")):
        shutil.rmtree("../data")
    exit(return_code)
