### Build/test container ###
# Define coverage stage
FROM dinner-is-servered:base as coverage

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build_coverage

# Build and run coverage
RUN cmake -DCMAKE_BUILD_TYPE=Coverage ..
RUN make coverage
