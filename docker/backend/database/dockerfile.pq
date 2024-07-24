# Dockerfile.pq

FROM postgres:latest

# Copy the initialization script
COPY backend/database/init.sql /docker-entrypoint-initdb.d/
