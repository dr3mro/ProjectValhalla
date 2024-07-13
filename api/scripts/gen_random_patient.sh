#!/bin/bash

# Function to generate a random string
generate_random_string() {
    length=$1
    LC_ALL=C tr -dc A-Za-z0-9 </dev/urandom | head -c $length ; echo ''
}

# Function to generate a random name
generate_random_name() {
    shuf -n1 /usr/share/dict/propernames | sed 's/\r//g'
}

# Function to generate a random integer between a range
generate_random_int() {
    min=$1
    max=$2
    shuf -i ${min}-${max} -n 1
}

# Function to generate a random email
generate_random_email() {
    local name=$(generate_random_string 8)
    local domain=$(generate_random_string 5)
    echo "${name}@${domain}.com"
}

# Function to generate a random phone number
generate_random_phone() {
    echo "+1"$(generate_random_int 1000000000 9999999999)
}

# Function to generate a random date of birth
generate_random_dob() {
    year=$(generate_random_int 1950 2010)
    month=$(generate_random_int 1 12)
    day=$(generate_random_int 1 28)
    printf "%04d-%02d-%02d" $year $month $day
}

# Generate random data
id=0
firstname=$(generate_random_name)
lastname=$(generate_random_name)
dob=$(generate_random_dob)
gender=$(shuf -e Male Female -n 1)
place_of_birth=$(generate_random_name)
address=$(generate_random_string 10)" St, Anytown, USA"
occupation=$(generate_random_name)
email=$(generate_random_email)
phone=$(generate_random_phone)

# Create JSON
json=$(jq -r -n \
    --argjson id "$id" \
    --arg firstname "$firstname" \
    --arg lastname "$lastname" \
    --arg dob "$dob" \
    --arg gender "$gender" \
    --arg place_of_birth "$place_of_birth" \
    --arg address "$address" \
    --arg occupation "$occupation" \
    --arg email "$email" \
    --arg phone "$phone" \
    --arg username "$username" \
    --arg token "$token" \
    '{
        payload: {
            basic_data: {
                id: $id,
                firstname: $firstname,
                lastname: $lastname,
                date_of_birth: $dob,
                gender: $gender,
                place_of_birth: $place_of_birth,
                address: $address,
                occupation: $occupation,
                contact: [
                    { email: $email },
                    { phone: $phone }
                ]
            },
            health_data: {},
            appointments_data: {}
        },
        sha256sum: ""
    }')

# Calculate SHA-256 checksum of the payload
payload=$(echo "$json" | jq -Scj '.payload')
sha256sum=$(echo -n "$payload" | sha256sum | awk '{print $1}')

# Insert SHA-256 checksum into JSON
json=$(echo "$json" | jq --arg sha256sum "$sha256sum" '.sha256sum = $sha256sum')

# Output JSON to a file
echo "$json"


