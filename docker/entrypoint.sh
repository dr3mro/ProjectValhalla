#!/bin/bash
set -e

cd /usr/src/ProjectValhalla/

git pull

# Build the project
cmake -S . -Bbuild -GNinja
cmake --build build -j

# Execute the server
exec ./server
