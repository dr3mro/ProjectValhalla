#!/bin/bash

# Function to generate JSON with incremented IDs
cat <<EOF
{
  "id": $(shuf -i 100000-130057 -n 1),
  "schema": [
    "basic_data",
    "health_data",
    "appointments_data"
  ],
  "username": "amr_nasr",
  "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjIzNDkxMzUsImlhdCI6MTcxOTc1NzEzNSwiaXNzIjoiUHJvamVjdFZhbGhhbGxhIiwic3ViIjoiYW1yX25hc3IifQ.nMye8-rcyOIsbGqF4iNu0eXj-lxJ83puSh7hIdbYvHA"
}
EOF
