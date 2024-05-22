#!/bin/python3

import subprocess as sp
import argparse
import shutil
import os

config_folder = "../configs"
config_name = "/crud_config"
expected_output_folder = "../integration_test_expected_outputs"
expected_output_name = "/successful_PUT_output"
target_output_file = "../data/test/1"

def setup_parser():
    parser = argparse.ArgumentParser()
    parser.add_argument("server_path")

    return parser

def main():
    args = setup_parser().parse_args()

    # Start the server process.
    server_process = sp.Popen([args.server_path, config_folder + config_name, "--disable-file-logging"], stdout=sp.PIPE)

    os.system("curl -isd test localhost:80/api/test")
    os.system("curl -isX PUT -d asdf localhost:80/api/test/1")

    diff_process = sp.Popen(["diff", expected_output_folder + expected_output_name, target_output_file], stdout=sp.PIPE, stderr=sp.PIPE)

    # Kill the server
    server_process.kill()

    # Pipe the curl process stdout to the diff process' stdin.
    ret, err = diff_process.communicate()

    # Check that the return code of diff is 0 (i.e. the output from curl is the expected output).
    if diff_process.returncode == 0:
        return 0
    else:
        return 1

if __name__ == "__main__":
    return_code = main()
    shutil.rmtree("../data")
    exit(return_code)