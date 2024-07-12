# 🚀 PROJECT VALHALLA

 - Before testing make sure you clean all the old containers.
 - cd into $PROJECT_DIR/docker and run those commands if you have used it before
```
docker-compose down --rmi all --volumes
docker-compose down --remove-orphans --volumes
```

### 🐳 Run docker
```
git clone git@github.com:dr3mro/ProjectValhalla.git
cd ProjectValhalla/docker
docker-compose --profile default up
```

### 🧪 Verify the server is running

```
curl http://172.20.0.3:8080/v1/hello
```
- if you get this reply
```
{
"Message" : "Welcome to ASGARD."
}%
```
that means the server is up and running

### 🚦 Resource not found
- if for any reason the server did not find the requested end point it will display this message

```
{
"Message" : "NOT FOUND, Are you lost?"
}%
```
### 🧮 to create the sha256sum of the payload use this command
```
jq -Sjc '.payload' patient.json | sha256sum
```

### 📦 Test GZIP
```
curl -X GET -H "Accept-Encoding:gzip" -d "$(bash ./gen_random_get_patient.sh)" http://172.20.0.3:8080/v1/patient --compressed --include  --output -
```


### 🎉 Create a new User
```
curl -X POST -H "Content-Type: application/json" -d @user.json http://172.20.0.3:8080/v1/user -i
```
- do a POST request on `/v1/user` with a body contains a JSON with following data
```
{
  "payload": {
    "fullname": "Amr Nasr",
    "username": "amr_nasr",
    "password": "123Mm@p0",
    "role": 0,
    "user_data": {
      "contact": {
        "phone": "+201299999999",
        "email": "amr@mail.com"
      },
      "address": {
        "city": "Damietta",
        "street": "portsaid street"
      },
      "dob": "1990-10-10",
      "gender": "male",
      "married": true,
      "job": {
        "position": "Doctor",
        "speciality": "Cardiology"
      }
    }
  },
  "sha256sum": "8657efb831ff66d61a6f26106a7acbe3f5a3bbc021b97fe1f1e4f69f21e20ec6"
}
```
- a sucessful registeration will yield this result in json format.
```
HTTP/1.1 200 OK
Access-Control-Allow-Credentials: true
Access-Control-Allow-Headers: Content-Type, Authentication, Accept-Encoding
Access-Control-Allow-Origin: *
Access-Control-Max-Age: 240
Access-Control-Allow-Methods: GET, POST
Content-Length: 132
Server: ProjectValhalla
Date: Wed, 10 Jul 2024 00:31:45 GMT
Connection: Keep-Alive

{
    "payload": [
        {
            "affected rows": 1
        }
    ],
    "status id": 0,
    "status message": "success"
}%

```
- a failed regieration , for example user exists yields this results.
```
HTTP/1.1 400 Bad Request
Access-Control-Allow-Credentials: true
Access-Control-Allow-Headers: Content-Type, Authentication, Accept-Encoding
Access-Control-Allow-Origin: *
Access-Control-Max-Age: 240
Access-Control-Allow-Methods: GET, POST
Content-Length: 129
Server: ProjectValhalla
Date: Wed, 10 Jul 2024 00:31:53 GMT
Connection: Keep-Alive

{
    "payload": "User already exists",
    "status id": -1,
    "status message": "Failed to create a new user, user exists"
}%
```
- the "payload" value is verified on the server against SHA256SUM that should be generated and added to the JSON in sha256sum key.
- in the "payload" the username should only composed of lower case characters and numbers and not start with number and does not contains white spaces.
- the password should contains upper and lower case characters, symbols, numbers and at lest 8 characters long.
- the email should be in a valid format user@domain.ext
- the role value is for now '0' as the role implementation is WIP.
- The REQUIRED data are (username, email, role, password);

### 🚪 Login
```
curl -X GET -H "Authentication:ewogICAgInVzZXJuYW1lIiA6ICJhbXJfbmFzciIsCiAgICAicGFzc3dvcmQiIDogIjEyM01tQHAwIgp9" http://172.20.0.3:8080/v1/user -i
```
- do a GET request on `/v1/user` with a header contains a BASE64 encoded JSON with following data under key "Authentication"
```
{
    "username" : "amr_nasr",
    "password" : "123Mm@p0"
}
```
- the username should be in lowecase and/or numbers and never contains spaces or symbols.
- the password should be uppercase and lowercase and characters and symbols.
- A successful login looks like this
```
HTTP/1.1 200 OK
Access-Control-Max-Age: 240
Access-Control-Allow-Headers: Content-Type, Authentication, Accept-Encoding
Access-Control-Allow-Methods: GET, POST
Access-Control-Allow-Credentials: true
Access-Control-Allow-Origin: *
Content-Length: 343
Server: ProjectValhalla
Date: Wed, 10 Jul 2024 00:36:12 GMT
Connection: Keep-Alive

{
    "payload": {
        "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjMxNjM3NzIsImlhdCI6MTcyMDU3MTc3MiwiaXNzIjoiUHJvamVjdFZhbGhhbGxhIiwic3ViIjoiYW1yX25hc3IifQ.sxwWc_CrMJ_GkYbXT9AAGyKNfD-vpUdvkzYUw5VpXtI",
        "user_id": 1002,
        "username": "amr_nasr"
    },
    "status id": 0,
    "status message": "success"
}%
```
- The returned token is valid only for 60 minutes and should be send in header with every subsequent request and after it is expired any request will fail until the user relogin and get a new token.
- a failed login looks like this
```
HTTP/1.1 400 Bad Request
Access-Control-Max-Age: 240
Access-Control-Allow-Headers: Content-Type, Authentication, Accept-Encoding
Access-Control-Allow-Methods: GET, POST
Access-Control-Allow-Credentials: true
Access-Control-Allow-Origin: *
Content-Length: 126
Server: ProjectValhalla
Date: Wed, 10 Jul 2024 00:35:15 GMT
Connection: Keep-Alive

{
    "payload": "User 'amr_nasr' not found or wrong password",
    "status id": -1,
    "status message": "Login Failure"
}%
```
### example of header Authorization
```
Authorization: Bearer amr_nasr:eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjMxNzQ3MjMsImlhdCI6MTcyMDU4MjcyMywiaXNzIjoiUHJvamVjdFZhbGhhbGxhIiwic3ViIjoiYW1yX25hc3IifQ.rI_u7GV9AtaGIawHzPJXEHOm_8wtz_2OKL0_wTAkgGc
```
* Authorization: Bearer
* username:token

### 👨🏻‍🦳 Add a patient
```
curl -X POST -H "Authorization: Bearer amr_nasr:eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjMxNzQ3MjMsImlhdCI6MTcyMDU4MjcyMywiaXNzIjoiUHJvamVjdFZhbGhhbGxhIiwic3ViIjoiYW1yX25hc3IifQ.rI_u7GV9AtaGIawHzPJXEHOm_8wtz_2OKL0_wTAkgGc" -d @patient.json http://172.20.0.3:8080/v1/patient -i
```
- In order to add a new user do a POST request in `/v1/patient` with a body contains JSON like this.
```
{
  "payload": {
    "basic_data": {
      "id": 0,
      "firstname": "John",
      "lastname": "Doe",
      "date_of_birth": "1990-01-01",
      "gender": "Male",
      "place_of_birth": "New York",
      "address": "123 Main St, Anytown, USA",
      "occupation": "Engineer",
      "contact": [
        {
          "email": "john.doe@example.com"
        },
        {
          "phone": "+1987654321"
        }
      ]
    },
    "health_data": {
    },
    "appointments_data": {
    }
  },
  "sha256sum": "81eaf148afea6c1c6577e518f6c5e9987a128b0e451670be9f3c171b91205b17"
}
```
- The payload should contain 3 item:
  * "basic_data"
  * "appointments_data"
  * "health_data"
- The basic_data should contains {"id" = 0}.
- Providing empty value '{}' will clear the corresponding field in the database.
- Not providing a key for example "health_data" has no effect as only the provided data is processed.
- The payload should be verified with sha256sum and the hash should be provided in the JSON as "sha256sum" value.
- A successful request will return the user_id and it looks like this.
```
HTTP/1.1 200 OK
Content-Length: 869
Server: ProjectValhalla
Date: Sat, 29 Jun 2024 21:49:37 GMT
Connection: Keep-Alive

{
    "payload": [
        {
            "affected rows": 1
        },
        {
            "appointments_data": {},
            "basic_data": {
                "address": "123 Main St, Anytown, USA",
                "contact": [
                    {
                        "email": "john.doe@example.com"
                    },
                    {
                        "phone": "+1987654321"
                    }
                ],
                "date_of_birth": "1990-01-01",
                "firstname": "John",
                "gender": "Male",
                "id": 100034,
                "lastname": "Doe",
                "occupation": "Engineer",
                "place_of_birth": "New York"
            },
            "health_data": {},
            "id": 100034
        }
    ],
    "status id": 0,
    "status message": "success"
}%

```
- A failed request for example if the token becomes expired would look like this.
```
HTTP/1.1 400 Bad Request
Content-Length: 133
Server: ProjectValhalla
Date: Sat, 29 Jun 2024 13:49:52 GMT
Connection: Keep-Alive

{
    "payload": "authentication token invalidated",
    "status id": -1,
    "status message": "failed to create a new patient"
}%
```

### 🤓 Get a patient
```
curl -X GET -H "Authorization: Bearer amr_nasr:eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjMxNzQ3MjMsImlhdCI6MTcyMDU4MjcyMywiaXNzIjoiUHJvamVjdFZhbGhhbGxhIiwic3ViIjoiYW1yX25hc3IifQ.rI_u7GV9AtaGIawHzPJXEHOm_8wtz_2OKL0_wTAkgGc" -d @get_patient.json http://172.20.0.3:8080/v1/patient -i
```
- do a GET request on `/v1/patient` with a body contains a JSON with following data
```
{
  "id": 100015,
  "schema":[
    "basic_data",
    "health_data",
    "appointments_data"
  ]
}
```
- A successful request looks like this:
```
HTTP/1.1 200 OK
Content-Length: 784
Server: ProjectValhalla
Date: Sat, 29 Jun 2024 13:53:05 GMT
Connection: Keep-Alive

{
    "payload": [
        {
            "appointments_data": {},
            "basic_data": {
                "address": "123 Main St, Anytown, USA",
                "contact": [
                    {
                        "email": "john.doe@example.com"
                    },
                    {
                        "phone": "+1987654321"
                    }
                ],
                "date_of_birth": "1990-01-01",
                "firstname": "John",
                "gender": "Male",
                "id": 0,
                "lastname": "Doe",
                "occupation": "Engineer",
                "place_of_birth": "New York"
            },
            "health_data": {}
        }
    ],
    "status id": 0,
    "status message": "success"
}%
```
- A failed request due to for example expired access token looks something like this:
```
HTTP/1.1 400 Bad Request
Content-Length: 117
Server: ProjectValhalla
Date: Sat, 29 Jun 2024 13:52:36 GMT
Connection: Keep-Alive

{
    "payload": "token is invalidated",
    "status id": -1,
    "status message": "failed to retrieve patient"
}%
```
- the "schems" is an array of items you want to retrieve
- you can request 1 or more or even 0 but I did not test this case yet.
- the 'id' is the patient_id and should exists

### 👍🏻 Update a patient
```
curl -X PATCH -H "Authorization: Bearer amr_nasr:eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjMxNzQ3MjMsImlhdCI6MTcyMDU4MjcyMywiaXNzIjoiUHJvamVjdFZhbGhhbGxhIiwic3ViIjoiYW1yX25hc3IifQ.rI_u7GV9AtaGIawHzPJXEHOm_8wtz_2OKL0_wTAkgGc" -d @update_patient.json http://172.20.0.3:8080/v1/patient -i
```
- In order to update a patient do a PATCH request in `/v1/patient` with a body contains JSON like this.
```
{
  "payload": {
    "basic_data": {
      "id": 100001,
      "firstname": "Mark",
      "lastname": "Johnson",
      "date_of_birth": "1990-01-01",
      "gender": "Male",
      "place_of_birth": "Philadilphia",
      "address": "123 Main St, Anytown, USA",
      "occupation": "Nurse",
      "contact": [
        {
          "email": "mark@example.com"
        },
        {
          "phone": "+1987654321"
        }
      ]
    },
    "health_data": {
    },
    "appointments_data": {
    }
  },
  "sha256sum": "a57ba46b2f25823cbdbb69a839dd9db9cee7c6fb303302498a7e0516a3431315"
}
```
- a successful delete reply with a json like this
```
HTTP/1.1 200 OK
Content-Length: 133
Server: ProjectValhalla
Date: Sun, 30 Jun 2024 19:47:04 GMT
Connection: Keep-Alive

{
    "payload": [
        {
            "affected rows": 1
        }
    ],
    "status id": 0,
    "status message": "success"
}%
```
- a failed request might be for example due to patient not found would look like this
```
HTTP/1.1 200 OK
Content-Length: 134
Server: ProjectValhalla
Date: Sun, 30 Jun 2024 19:52:33 GMT
Connection: Keep-Alive

{
    "payload": [
        {
            "affected rows": 0
        }
    ],
    "status id": -1,
    "status message": "failure"
}%
```
- Notice that it returns 200 OK but affected records 0 that means every step was successful except the query to the database.

### ❌ Delete a patient
  ```
  curl -X DELETE -H "Authorization: Bearer amr_nasr:eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjMxNzQ3MjMsImlhdCI6MTcyMDU4MjcyMywiaXNzIjoiUHJvamVjdFZhbGhhbGxhIiwic3ViIjoiYW1yX25hc3IifQ.rI_u7GV9AtaGIawHzPJXEHOm_8wtz_2OKL0_wTAkgGc" -d @del_patient.json http://172.20.0.3:8080/v1/patient -i
  ```
  - In order to delete a patient do a DELETE request in `/v1/patient` with a body contains JSON like this.
  ```
  {
  "payload": {
    "basic_data": {
      "id": 100032,
      "firstname": "John",
      "lastname": "Doe",
      "date_of_birth": "1990-01-01",
      "gender": "Male"
    }
  },
  "sha256sum": "eb0ba7887fec5a0e369ae9cb56155ec9b0e13935af64a1af6adae31777e83951"
}
```
- data should be correct as any wrong info will invalidate the request.
- sha256sum is needed to prevent malicious mass deletion of data.
- a successful delete reply with a json like this
```
HTTP/1.1 200 OK
Content-Length: 133
Server: ProjectValhalla
Date: Sun, 30 Jun 2024 12:33:40 GMT
Connection: Keep-Alive

{
    "payload": [
        {
            "affected rows": 1
        }
    ],
    "status id": 0,
    "status message": "success"
}%
```
- a failed delete request will reply with
```
HTTP/1.1 200 OK
Content-Length: 134
Server: ProjectValhalla
Date: Sun, 30 Jun 2024 12:34:23 GMT
Connection: Keep-Alive

{
    "payload": [
        {
            "affected rows": 0
        }
    ],
    "status id": -1,
    "status message": "failure"
}%
```
- notice that the http return code is `200` indicating it did reach the database but found no identical entry. it the data cannot get to the database due any other error the return code would be `400` like for example token invalidation or expiration.

```
HTTP/1.1 400 Bad Request
Content-Length: 134
Server: ProjectValhalla
Date: Sun, 30 Jun 2024 12:29:44 GMT
Connection: Keep-Alive

{
    "payload": "authentication token invalid or expired",
    "status id": -1,
    "status message": "failed to delete patient"
}%
```


### 🔎  Search
```
curl -X SEARCH -H "Authorization: Bearer amr_nasr:eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjMxNzQ3MjMsImlhdCI6MTcyMDU4MjcyMywiaXNzIjoiUHJvamVjdFZhbGhhbGxhIiwic3ViIjoiYW1yX25hc3IifQ.rI_u7GV9AtaGIawHzPJXEHOm_8wtz_2OKL0_wTAkgGc" -d @search.json http://172.20.0.3:8080/v1/patient -i
```
 - In order to search for a patient do a SEARCH request in `/v1/patient` with a body contains JSON like this.

```
{
    "keyword" : "John",
    "limit" : 5,
    "offset" : 0,
    "order_by" : "id",
    "direction" : 0
}
```
 - explanation:

    * keyword: the text to search for in basic data
    * limit: maximum number of returned results
    * offset : start with 0 and keep it to send it in the next request to get the next patch
    * order_by : name of json key to sort by
    * direction : ascending = 0 , descending = 1

- a reply would look like this
```
HTTP/1.1 200 OK
Content-Length: 2034
Server: ProjectValhalla
Date: Mon, 01 Jul 2024 13:23:47 GMT
Connection: Keep-Alive

{
    "more": true,
    "offset": 3,
    "payload": [
        {
            "basic_data": {
                "address": "123 Main St, Anytown, USA",
                "contact": [
                    {
                        "email": "john.doe@example.com"
                    },
                    {
                        "phone": "+1987654321"
                    }
                ],
                "date_of_birth": "1990-01-01",
                "firstname": "John",
                "gender": "Male",
                "id": 100009,
                "lastname": "Doe",
                "occupation": "Engineer",
                "place_of_birth": "New York"
            }
        },
        {
            "basic_data": {
                "address": "123 Main St, Anytown, USA",
                "contact": [
                    {
                        "email": "john.doe@example.com"
                    },
                    {
                        "phone": "+1987654321"
                    }
                ],
                "date_of_birth": "1990-01-01",
                "firstname": "John",
                "gender": "Male",
                "id": 100008,
                "lastname": "Doe",
                "occupation": "Engineer",
                "place_of_birth": "New York"
            }
        },
        {
            "basic_data": {
                "address": "123 Main St, Anytown, USA",
                "contact": [
                    {
                        "email": "john.doe@example.com"
                    },
                    {
                        "phone": "+1987654321"
                    }
                ],
                "date_of_birth": "1990-01-01",
                "firstname": "John",
                "gender": "Male",
                "id": 100007,
                "lastname": "Doe",
                "occupation": "Engineer",
                "place_of_birth": "New York"
            }
        }
    ],
    "status id": 0,
    "status message": "success"
}%
```

- check the `more` key to see if there is any more results
- keep the `offset` to use it with the next request.

🥳🥳🥳🥳🎉🎉🎉🎉

