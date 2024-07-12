#!/bin/bash
set -e

cd /usr/src/ProjectValhalla/

git pull

# Build the project
cmake -S . -GNinja
cmake --build . -j

# Execute the server
exec ./server
