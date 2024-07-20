#!/bin/bash

git rebase

# Build the project
cmake -S . -Bbuild -GNinja
cmake --build build -j

# Execute the server
exec build/server
