### Build/test container ###
# Define builder stage
FROM dinner-is-servered:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

# Build and test
RUN cmake ..
RUN make
RUN ctest --output-on_failure


### Deploy container ###
# Define deploy stage
FROM ubuntu:jammy as deploy

COPY --from=builder /usr/src/project/build/bin/server ./bin
COPY --from=builder /usr/src/project/confs/sample_conf .
COPY --from=builder /usr/src/project/resources ./resources


EXPOSE 80

ENTRYPOINT ["./bin/server"]

CMD ["sample_conf"]
