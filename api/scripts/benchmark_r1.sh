#!/bin/bash

for ((i=1; i<=2566; i++)); do
    curl -X GET -H "Content-Type: application/json" -d "$(bash ./gen_random_get_patient.sh)" http://172.20.0.2:8080/v1/patient &
done
