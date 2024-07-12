# Use the official Ubuntu image as the base image
FROM ubuntu:latest

# Install necessary packages and clean up
RUN apt-get update && apt-get upgrade -y && apt-get install -y \
    git \
    cmake \
    ninja-build \
    g++ \
    python3 \
    libpq-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /usr/src/

# Clone the repository
RUN git clone https://github.com/dr3mro/ProjectValhalla.git /usr/src/ProjectValhalla/

# Make the entrypoint script executable
RUN chmod +x /usr/src/ProjectValhalla/docker/entrypoint.sh

# Define the volume for persistent data
VOLUME ["/usr/src"]

# Expose port 8080
EXPOSE 8080

# Use the entrypoint script as the default command
ENTRYPOINT ["/usr/src/ProjectValhalla/docker/entrypoint.sh"]
