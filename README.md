# 🚀 PROJECT VALHALLA
![alt text](https://github.com/dr3mro/ProjectValhalla/blob/a4162ecba8c1b9684f540fe0ef75a554b1308bb3/Screenshot.png?raw=true)

 - If you get any issues while testing make sure you clean all the old containers.
 - cd into $PROJECT_DIR/docker and run those commands if you have used it before
```
docker compose down --rmi all --volumes
docker compose down --remove-orphans --volumes
```
- To get the latest version
```
docker compose --profile default up --build
```
or use the dev version
```
docker compose --profile dev up --build
```
### 🐳 Run docker
```
git clone git@github.com:dr3mro/ProjectValhalla.git
cd ProjectValhalla/docker
docker compose --profile default up
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
### 🧮 to create the xxh64sum of the payload use this command
```
jq -Sjc '.payload' patient.json | xxh64sum
```
- DataIntegrity protection is disabled by default and can be enabled anytime to verify payload by setting header value:
- `"Enable-data-integrity-check : true"`
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
  "xxh64sum": "8657efb831ff66d61a6f26106a7acbe3f5a3bbc021b97fe1f1e4f69f21e20ec6"
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
    "status_id": 0,
    "status_message": "success"
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
    "status_id": -1,
    "status_message": "Failed to create a new user, user exists"
}%
```
- the "payload" value is optionally verified on the server against xxh64sum that might be generated and added to the JSON in xxh64sum key.
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
- the username should be in lowercase and/or numbers and never contains spaces or symbols.
- the password should be uppercase and lowercase and characters and symbols.
- A successful login looks like this
```
HTTP/1.1 200 OK
Access-Control-Max-Age: 7200
Access-Control-Allow-Headers: Content-Type, Accept-Encoding, Origin, Access-Control-Request-Method, Authentication
Access-Control-Allow-Methods: GET, POST, OPTIONS
Access-Control-Allow-Credentials: true
Access-Control-Allow-Origin: *
Content-Length: 343
Server: ProjectValhalla
Date: Fri, 12 Jul 2024 19:28:40 GMT
Connection: Keep-Alive

{
    "payload": {
        "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjM0MDQ1MjAsImlhdCI6MTcyMDgxMjUyMCwiaXNzIjoiUHJvamVjdFZhbGhhbGxhIiwic3ViIjoiYW1yX25hc3IifQ.ocAJcFExlNEHSj608C3etgOeeTdBJxhDODaszwPMsV0",
        "user_id": 1000,
        "username": "amr_nasr"
    },
    "status id": 0,
    "status message": "success"
}%

```
- The returned token is valid only for 60 minutes and should be send in header with every subsequent request and after it is expired any request will fail until the user relogin and get a new token.
- a failed login looks like this
```
HTTP/1.1 500 Internal Server Error
Access-Control-Max-Age: 7200
Access-Control-Allow-Headers: Content-Type, Accept-Encoding, Origin, Access-Control-Request-Method, Authentication
Access-Control-Allow-Methods: GET, POST, OPTIONS
Access-Control-Allow-Credentials: true
Access-Control-Allow-Origin: *
Content-Length: 21
Server: ProjectValhalla
Date: Fri, 12 Jul 2024 19:29:51 GMT
Connection: Keep-Alive

Authentication Denied%
```
### example of header Authorization
```
Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjMxNzQ3MjMsImlhdCI6MTcyMDU4MjcyMywiaXNzIjoiUHJvamVjdFZhbGhhbGxhIiwic3ViIjoiYW1yX25hc3IifQ.rI_u7GV9AtaGIawHzPJXEHOm_8wtz_2OKL0_wTAkgGc
```
* Authorization: Bearer ${token}

### 👨🏻‍🦳 Add a patient
```
curl -X POST -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjMxNzQ3MjMsImlhdCI6MTcyMDU4MjcyMywiaXNzIjoiUHJvamVjdFZhbGhhbGxhIiwic3ViIjoiYW1yX25hc3IifQ.rI_u7GV9AtaGIawHzPJXEHOm_8wtz_2OKL0_wTAkgGc" -d @patient.json http://172.20.0.3:8080/v1/patient -i
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
  "xxh64sum": "81eaf148afea6c1c6577e518f6c5e9987a128b0e451670be9f3c171b91205b17"
}
```
- The payload should contain 3 item:
  * "basic_data"
  * "appointments_data"
  * "health_data"
- The basic_data should contains {"id" = 0}.
- Providing empty value '{}' will clear the corresponding field in the database.
- Not providing a key for example "health_data" has no effect as only the provided data is processed.
- The payload might be verified with xxh64sum and the hash can be provided in the JSON as "xxh64sum" value.
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
    "status_id": 0,
    "status_message": "success"
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
    "status_id": -1,
    "status_message": "failed to create a new patient"
}%
```

### 🤓 Get a patient
```
curl -X GET -H "X-Request:ewogICJpZCI6IDEwMDAwNCwKICAic2NoZW1hIjpbCiAgICAiYmFzaWNfZGF0YSIsCiAgICAiaGVhbHRoX2RhdGEiLAogICAgImFwcG9pbnRtZW50c19kYXRhIgogIF0KfQoK" -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjM0MDkxODgsImlhdCI6MTcyMDgxNzE4OCwiaXNzIjoiUHJvamVjdFZhbGhhbGxhIiwianRpIjoiMTAwMCIsInN1YiI6ImFtcl9uYXNyIn0.lKjlzKtTd19DjpO4sBjDs-Z7JuDUha4p8OOzziJcgPo" http://172.20.0.3:8080/v1/patient -i
```
- do a GET request on `/v1/patient` with a header `X-Request` contains a `BASE64` encoded `JSON` with following data
```
{
  "id": 100004,
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
Access-Control-Allow-Credentials: true
Access-Control-Allow-Headers: Content-Type, Accept-Encoding, Origin, Access-Control-Request-Method, Authorization
Access-Control-Allow-Origin: *
Access-Control-Max-Age: 7200
Access-Control-Allow-Methods: GET, POST, DELETE, PATCH, SEARCH, OPTIONS
Content-Length: 788
Server: ProjectValhalla
Date: Sat, 13 Jul 2024 14:37:06 GMT
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
                "id": 100004,
                "lastname": "Doe",
                "occupation": "Engineer",
                "place_of_birth": "New York"
            },
            "health_data": {}
        }
    ],
    "status_id": 0,
    "status_message": "success"
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
    "status_id": -1,
    "status_message": "failed to retrieve patient"
}%
```
- the "schems" is an array of items you want to retrieve
- you can request 1 or more or even 0.
- the 'id' is the patient_id and should exists

### 👍🏻 Update a patient
```
curl -X PATCH -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjMxNzQ3MjMsImlhdCI6MTcyMDU4MjcyMywiaXNzIjoiUHJvamVjdFZhbGhhbGxhIiwic3ViIjoiYW1yX25hc3IifQ.rI_u7GV9AtaGIawHzPJXEHOm_8wtz_2OKL0_wTAkgGc" -d @update_patient.json http://172.20.0.3:8080/v1/patient -i
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
  "xxh64sum": "a57ba46b2f25823cbdbb69a839dd9db9cee7c6fb303302498a7e0516a3431315"
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
    "status_id": 0,
    "status_message": "success"
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
    "status_id": -1,
    "status_message": "failure"
}%
```
- Notice that it returns 200 OK but affected records 0 that means every step was successful except the query to the database.

### ❌ Delete a patient
  ```
curl -X DELETE -H "X-Request: $(cat del_patient.json | base64)" -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjM1NDY3ODgsImlhdCI6MTcyMDk1NDc4OCwiaXNzIjoiUHJvamVjdFZhbGhhbGxhIiwianRpIjoiMTAwMSIsInN1YiI6ImFtcl9uYXNyIn0.iw9Sql6TDnVUmwXEfaFW9tmFqAeHeSZzD-54iReiZNo"  http://172.20.0.3:8080/v1/patient -i
  ```
  - In order to delete a patient do a DELETE request in `/v1/patient` with a header contains JSON under key `X-Request` like this.
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
  "xxh64sum": "eb0ba7887fec5a0e369ae9cb56155ec9b0e13935af64a1af6adae31777e83951"
}
```
- data should be correct as any wrong info will invalidate the request.
- xxh64sum is optional prevent malicious mass deletion of data.
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
    "status_id": 0,
    "status_message": "success"
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
    "status_id": -1,
    "status_message": "failure"
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
    "status_id": -1,
    "status_message": "failed to delete patient"
}%
```


### 🔎  Search
```
curl -X SEARCH -H "X-Request: ewogICAgImtleXdvcmQiIDogIkpvaG4iLAogICAgImxpbWl0IiA6IDUsCiAgICAib2Zmc2V0IiA6IDAsCiAgICAib3JkZXJfYnkiIDogImlkIiwKICAgICJkaXJlY3Rpb24iIDogMCAKfQo=" -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjM0MDkxODgsImlhdCI6MTcyMDgxNzE4OCwiaXNzIjoiUHJvamVjdFZhbGhhbGxhIiwianRpIjoiMTAwMCIsInN1YiI6ImFtcl9uYXNyIn0.lKjlzKtTd19DjpO4sBjDs-Z7JuDUha4p8OOzziJcgPo" http://172.20.0.3:8080/v1/patient -i
```
 - In order to search for a patient do a SEARCH request in `/v1/patient` with a header variable `X-Request` that contains `base64` encoded JSON like this.

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
    "status_id": 0,
    "status_message": "success"
}%
```

- check the `more` key to see if there is any more results
- keep the `offset` to use it with the next request.

🥳🥳🥳🥳🎉🎉🎉🎉

### Test memory leaks
```
ASAN_OPTIONS=detect_leaks=1 ./server
```

### Benchmark
- Machine: MSI B450M, AMD Ryzen 3800X with 32 GB of RAM using Release version built with GCC 14.1 running k6r.js & k6w.js with 1000 virtual users and 100,000 iterations
  ![alt text](https://github.com/dr3mro/ProjectValhalla/blob/4c79a5bc52b3e8a2f489ce8429595a1c63261081/benchmark.png?raw=true)
