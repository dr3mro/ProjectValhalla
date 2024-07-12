# Use the official Ubuntu image as the base image
FROM ubuntu:latest

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

RUN git clone https://github.com/dr3mro/ProjectValhalla.git /usr/src/ProjectValhalla/

RUN chmod +x /usr/src/ProjectValhalla/docker/entrypoint.sh

# Use the entrypoint script as the default command
ENTRYPOINT ["/usr/src/ProjectValhalla/docker/entrypoint.sh"]

# Expose port 8080
EXPOSE 8080
# Copy local files to the container (if needed)
# COPY . /app

# Specify a default command to run when the container starts (if needed)
# CMD ["bash"]

