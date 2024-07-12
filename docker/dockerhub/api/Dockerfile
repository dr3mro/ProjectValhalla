# Use a specific version of Ubuntu for consistency
FROM ubuntu:latest


# update and install some packages
RUN apt-get update && apt-get install -y wget libpq5

# Create API server directory and cd into it
RUN mkdir /home/ubuntu/ProjectValhalla && cd /home/ubuntu/ProjectValhalla

# Set working directory
WORKDIR /home/ubuntu/ProjectValhalla

# Download latest release from github
RUN wget https://github.com/dr3mro/ProjectValhalla/releases/download/0.1/server-x86_64.tar.gz

# extract and delete the archive
RUN tar -xvf server-x86_64.tar.gz && rm server-x86_64.tar.gz

# make it executable
RUN chmod +x server

# run it as an entry point
ENTRYPOINT ["/home/ubuntu/ProjectValhalla/server"]
