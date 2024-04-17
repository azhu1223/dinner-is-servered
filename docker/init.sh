#!/bin/bash

#Build base image
docker build -f docker/base.Dockerfile -t dinner-is-servered:base .

# #Build main image
docker build -f docker/Dockerfile -t dinner-is-servered .

# #Run container
docker run --rm -dp 127.0.0.1:80:80 --name my_run dinner-is-servered:latest