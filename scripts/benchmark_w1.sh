#!/bin/bash

for ((i=1; i<=1024; i++)); do
    curl --silent -o /dev/null -X POST -H "Content-Type: application/json" -d "$(bash ./gen_random_patient.sh)" http://172.20.0.2:8080/v1/patient &  
done
