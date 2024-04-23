#!/bin/python3

import subprocess as sp
import argparse

config_folder = "../configs"
config_name = "/empty_config"

def setup_parser():
    parser = argparse.ArgumentParser()
    parser.add_argument("server_path")

    return parser

def main():
    args = setup_parser().parse_args()

    # Start the server process.
    server_process = sp.Popen([args.server_path, config_folder + config_name], stdout=sp.PIPE)

    # Added so that the server_process return code won't be None for some reason???
    server_process.wait()

    # Check that the return code of server is 1 (i.e. the server failed to start).
    if server_process.returncode == 1:
        return 0
    else:
        return 1

if __name__ == "__main__":
    exit(main())

