# 🚀 PROJECT VALHALLA
![alt text](https://github.com/dr3mro/ProjectValhalla/blob/a4162ecba8c1b9684f540fe0ef75a554b1308bb3/Screenshot.png?raw=true)

### 🐳 Run docker
```
git clone git@github.com:dr3mro/ProjectValhalla.git
cd ProjectValhalla/docker/
docker compose --profile run up --build
```
if you want to try the dev version
```
docker compose --profile dev up --build
```
### 🧪 Verify the server is running

```
curl http://172.20.0.10:80/v1/hello
```
- if you get this reply
```
{
"Message" : "Welcome to ASGARD."
}%
```
that means the server is up and running

### 🧮 to create the xxh64sum of the payload use this command
```
jq -Sjc '.payload' api/patient/create_patient.json | xxh64sum
```
- DataIntegrity protection is disabled by default and can be enabled anytime to verify payload by setting header value:
- `"Enable-data-integrity-check : true"`
### 📦 Test GZIP
```
curl -X GET -H "Accept-Encoding:gzip" -H "X-Request: $(cat api/patient/read_patient.json | base64 -w0)"  -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjUxODIwNDAsImdyb3VwIjoidXNlcnMiLCJpYXQiOjE3MjI1OTAwNDAsImlzcyI6InZhbGhhbGxhIiwianRpIjoiMTAxMSIsImxsb2R0IjoiMjAyNC0wOC0wMSAyMzoxMzoxNiswMCIsInN1YiI6InVzZXIxIn0.fNF0YvJ1zcrxFzMbZ1Mf2DS4BXq9cV0lDhNatSMbxnA"  http://172.20.0.10:80/v1/patient --compressed --include  --output -
```

### 🎉 Create a new User
```
curl -X POST -H "Content-Type: application/json" -d @api/users/create_user.json http://172.20.0.10:80/v1/user
```
- do a `POST` request on `/v1/user` with a `body` contains a `JSON` with data from `api/users/create_user.json`

- the "payload" value is optionally verified on the server against xxh64sum that might be generated and added to the JSON in xxh64sum key.
- in the "payload" the username should only composed of lower case characters and numbers and not start with number and does not contains white spaces.
- the password should contains upper and lower case characters, symbols, numbers and at lest 8 characters long.
- the email should be in a valid format user@domain.ext
- the role value is for now '0' as the role implementation is WIP.
- The REQUIRED data are (username, email, role, password);

### 🚪 Login
```
curl -X POST -H "Authentication: $(cat api/users/login_user.json | base64 -w0)" http://172.20.0.10:80/v1/ulogin
```
- do a `POST` request on `/v1/ulogin` with a `header` contains a `BASE64` encoded JSON with data from `api/users/login_user.json` in header key `Authentication`
- the username should be in lowercase and/or numbers and never contains spaces or symbols.
- the password should be uppercase and lowercase and characters and symbols.


### example of header Authorization
```
Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjMxNzQ3MjMsImlhdCI6MTcyMDU4MjcyMywiaXNzIjoiUHJvamVjdFZhbGhhbGxhIiwic3ViIjoiYW1yX25hc3IifQ.rI_u7GV9AtaGIawHzPJXEHOm_8wtz_2OKL0_wTAkgGc
```

### Logout a User
- To logout just send the last valid token to `/v1/ulogout` under header key `Deauthentication`
```
curl -X POST -H "Deauthentication: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjUyMTAxMTcsImdyb3VwIjoidXNlcnMiLCJpYXQiOjE3MjI2MTgxMTcsImlzcyI6InZhbGhhbGxhIiwianRpIjoiMTAxNiIsImxsb2R0IjoiMjAyNC0wOC0wMiAxNjo1ODozNSswMCIsInN1YiI6InVzZXIifQ.faC_aadCWNqY67_tTYGM2DZuRPTy12Rma_6bwdITrN0"  http://172.20.0.10:80/v1/ulogout
```
### 🤓 Get a User
```
curl -X GET -H "X-Request: $(cat api/users/read_user.json | base64 -w0)" -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjUyMTcyNDksImdyb3VwIjoidXNlcnMiLCJpYXQiOjE3MjI2MjUyNDksImlzcyI6InZhbGhhbGxhIiwianRpIjoiMTAxNyIsImxsb2R0IjoiMjAyNC0wOC0wMiAxOTowMDo0MCswMCIsInN1YiI6InVzZXIifQ.1Wz9jFiBhzfe6svyB8dQZeMwfgxKZzaCjsQnQkUn1L0" http://172.20.0.10:80/v1/user
```
- do a `GET` request on `/v1/user` with a header `X-Request` contains a `BASE64` encoded `JSON` with data from `api/users/read_user.json`

### 👍🏻 Update a user
```
curl -X PUT -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjUyMTc1MDgsImdyb3VwIjoidXNlcnMiLCJpYXQiOjE3MjI2MjU1MDgsImlzcyI6InZhbGhhbGxhIiwianRpIjoiMTAxNyIsImxsb2R0IjoiMjAyNC0wOC0wMiAxOTowMDo0MCswMCIsInN1YiI6InVzZXIifQ.AXQzdnCRqIEC96LrSOH2NNMwQrALlOhH2xxWqESF_fA" -d @api/users/update_user.json http://172.20.0.10:80/v1/user -i
```
- In order to update a patient do a `PUT` request in `/v1/user` with a `body` contains `JSON` like `api/users/update_user.json`

### Delete a User
- To delete a user just send `DELETE` http request to `/v1/user` with
```
curl -X DELETE -H "X-Request: $(cat api/users/delete_user.json | base64 -w0)" -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjUyMTExNTQsImdyb3VwIjoidXNlcnMiLCJpYXQiOjE3MjI2MTkxNTQsImlzcyI6InZhbGhhbGxhIiwianRpIjoiMTAxNiIsImxsb2R0IjoiMjAyNC0wOC0wMiAxNzoxNDo1MyswMCIsInN1YiI6InVzZXIifQ.UCKJeblAPzcERZcsVL1cFXPIJwsdDRCVdvY8jBlP7Pw"  http://172.20.0.10:80/v1/user
```

### 🎉 Create a new Provider
```
curl -X POST -H "Content-Type: application/json" -d @api/providers/create_provider.json http://172.20.0.10:80/v1/provider
```
- do a `POST` request on `/v1/provider` with a `body` contains a `JSON` with data from `api/providers/create_provider.json`

- the "payload" value is optionally verified on the server against xxh64sum that might be generated and added to the JSON in xxh64sum key.
- in the "payload" the username should only composed of lower case characters and numbers and not start with number and does not contains white spaces.
- the password should contains upper and lower case characters, symbols, numbers and at lest 8 characters long.
- the email should be in a valid format user@domain.ext
- the role value is for now '0' as the role implementation is WIP.
- The REQUIRED data are (username, email, role, password);

### 🚪 Login
```
curl -X POST -H "Authentication: $(cat api/providers/login_provider.json | base64 -w0)" http://172.20.0.10:80/v1/plogin
```
- do a `POST` request on `/v1/plogin` with a `header` contains a `BASE64` encoded `JSON` with data from `api/providers/login_provider.json` in header key `Authentication`
- the username should be in lowercase and/or numbers and never contains spaces or symbols.
- the password should be uppercase and lowercase and characters and symbols.

### Logout a Provider
- To logout just send the last valid token to `/v1/plogout` under header key `Deauthentication`
```
curl -X POST -H "Deauthentication: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjUyMTAxMTcsImdyb3VwIjoidXNlcnMiLCJpYXQiOjE3MjI2MTgxMTcsImlzcyI6InZhbGhhbGxhIiwianRpIjoiMTAxNiIsImxsb2R0IjoiMjAyNC0wOC0wMiAxNjo1ODozNSswMCIsInN1YiI6InVzZXIifQ.faC_aadCWNqY67_tTYGM2DZuRPTy12Rma_6bwdITrN0"  http://172.20.0.10:80/v1/plogout
```
### 🤓 Get a Provider
```
curl -X GET -H "X-Request: $(cat api/Providers/read_provider.json | base64 -w0)" -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjUyMTcyNDksImdyb3VwIjoidXNlcnMiLCJpYXQiOjE3MjI2MjUyNDksImlzcyI6InZhbGhhbGxhIiwianRpIjoiMTAxNyIsImxsb2R0IjoiMjAyNC0wOC0wMiAxOTowMDo0MCswMCIsInN1YiI6InVzZXIifQ.1Wz9jFiBhzfe6svyB8dQZeMwfgxKZzaCjsQnQkUn1L0" http://172.20.0.10:80/v1/provider
```
- do a `GET` request on `/v1/provider` with a header `X-Request` contains a `BASE64` encoded `JSON` data from `api/Providers/read_provider.json`

### 👍🏻 Update a provider
```
curl -X PUT -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjUyMTc1MDgsImdyb3VwIjoidXNlcnMiLCJpYXQiOjE3MjI2MjU1MDgsImlzcyI6InZhbGhhbGxhIiwianRpIjoiMTAxNyIsImxsb2R0IjoiMjAyNC0wOC0wMiAxOTowMDo0MCswMCIsInN1YiI6InVzZXIifQ.AXQzdnCRqIEC96LrSOH2NNMwQrALlOhH2xxWqESF_fA" -d @api/providers/update_provider.json http://172.20.0.10:80/v1/provider -i
```
- In order to update a patient do a `PUT` request in `/v1/provider` with a `body` contains `JSON` like `api/providers/update_provider.json `


### Delete a Provider
- To delete a user just send `DELETE` http request to `/v1/provider` with `base64` encoded `X-Request` header of the file in `api/providers/delete_provider.json`
-
```
curl -X DELETE -H "X-Request: $(cat api/providers/delete_provider.json | base64 -w0)" -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjUyMTExNTQsImdyb3VwIjoidXNlcnMiLCJpYXQiOjE3MjI2MTkxNTQsImlzcyI6InZhbGhhbGxhIiwianRpIjoiMTAxNiIsImxsb2R0IjoiMjAyNC0wOC0wMiAxNzoxNDo1MyswMCIsInN1YiI6InVzZXIifQ.UCKJeblAPzcERZcsVL1cFXPIJwsdDRCVdvY8jBlP7Pw"  http://172.20.0.10:80/v1/provider
```

### 👨🏻‍🦳 Add a patient
```
curl -X POST -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjUyMTg2NzAsImdyb3VwIjoidXNlcnMiLCJpYXQiOjE3MjI2MjY2NzAsImlzcyI6InZhbGhhbGxhIiwianRpIjoiMTAxNyIsImxsb2R0IjoiMjAyNC0wOC0wMiAxOTowMDo0MCswMCIsInN1YiI6InVzZXIifQ.VkWRvZ-CI97vRq0hYy4Ibb-EX4FbT_MSuE-A_550Lvg" -d @api/patient/create_patient.json http://172.20.0.10:80/v1/patient
```
- In order to add a new patient do a `POST` request in `/v1/patient` with a `body` contains JSON like `api/patient/create_patient.json`.

- The payload should contain 3 item:
  * "basic_data"
  * "appointments_data"
  * "health_data"
- The basic_data should contains {"id" = 0}.
- Providing empty value '{}' will clear the corresponding field in the database.
- Not providing a key for example "health_data" has no effect as only the provided data is processed.
- The payload might be verified with xxh64sum and the hash can be provided in the JSON as "xxh64sum" value.

### 🤓 Get a patient
```
curl -X GET -H "X-Request: $(cat api/patient/read_patient.json | base64 -w0)" -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjUyMTg2NzAsImdyb3VwIjoidXNlcnMiLCJpYXQiOjE3MjI2MjY2NzAsImlzcyI6InZhbGhhbGxhIiwianRpIjoiMTAxNyIsImxsb2R0IjoiMjAyNC0wOC0wMiAxOTowMDo0MCswMCIsInN1YiI6InVzZXIifQ.VkWRvZ-CI97vRq0hYy4Ibb-EX4FbT_MSuE-A_550Lvg" http://172.20.0.10:80/v1/patient -i
```
- do a `GET` request on `/v1/patient` with a header `X-Request` contains a `BASE64` encoded `JSON` with following data

- the "schems" is an array of items you want to retrieve
- you can request 1 or more or even 0.
- the 'id' is the patient_id and should exists

### 👍🏻 Update a patient
```
curl -X PUT -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjUyMTg2NzAsImdyb3VwIjoidXNlcnMiLCJpYXQiOjE3MjI2MjY2NzAsImlzcyI6InZhbGhhbGxhIiwianRpIjoiMTAxNyIsImxsb2R0IjoiMjAyNC0wOC0wMiAxOTowMDo0MCswMCIsInN1YiI6InVzZXIifQ.VkWRvZ-CI97vRq0hYy4Ibb-EX4FbT_MSuE-A_550Lvg" -d @api/patient/update_patient.json http://172.20.0.10:80/v1/patient
```
- In order to update a patient do a `PUT` request in `/v1/patient` with a body contains JSON like `api/patient/update_patient.json`

### ❌ Delete a patient
```
curl -X DELETE -H "X-Request: $(cat api/patient/delete_patient.json | base64 -w0)" -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjUyMTg2NzAsImdyb3VwIjoidXNlcnMiLCJpYXQiOjE3MjI2MjY2NzAsImlzcyI6InZhbGhhbGxhIiwianRpIjoiMTAxNyIsImxsb2R0IjoiMjAyNC0wOC0wMiAxOTowMDo0MCswMCIsInN1YiI6InVzZXIifQ.VkWRvZ-CI97vRq0hYy4Ibb-EX4FbT_MSuE-A_550Lvg"  http://172.20.0.10:80/v1/patient
```
- In order to delete a patient do a `DELETE` request in `/v1/patient` with a header contains JSON under key `X-Request` like this.

### 🔎  Search
```
curl -X SEARCH -H "X-Request: ewogICAgImtleXdvcmQiIDogIkpvaG4iLAogICAgImxpbWl0IiA6IDUsCiAgICAib2Zmc2V0IiA6IDAsCiAgICAib3JkZXJfYnkiIDogImlkIiwKICAgICJkaXJlY3Rpb24iIDogMCAKfQo=" -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjM0MDkxODgsImlhdCI6MTcyMDgxNzE4OCwiaXNzIjoiUHJvamVjdFZhbGhhbGxhIiwianRpIjoiMTAwMCIsInN1YiI6ImFtcl9uYXNyIn0.lKjlzKtTd19DjpO4sBjDs-Z7JuDUha4p8OOzziJcgPo" http://172.20.0.10:80/v1/patient -i
```
 - In order to search for a patient do a SEARCH request in `/v1/patient` or `/v1/user` or `/v1/provider` with a header variable `X-Request` that contains `base64` encoded JSON like this.

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
### Services

all services are in `v1/service`
available services:
- clinics
- pharmacies
- laboratories
- radiologycenters

you can for example use `v1/service/clinics` to use clinic services and so on.

* `POST` request to `v1/service/clinics` with a `body` contains `JSON` like `api/services/create.json`
* `GET` request to `v1/service/clinics` with a `header` contains `X-Request` with `base64` encoded `JSON` like `api/services/read.json`
* `PUT` request to `v1/service/clinics` with a `body` contains `JSON` like `api/services/update.json`
* `DELETE` request to `v1/service/clinics` with a `header` contains `X-Request` with `base64` encoded `JSON` like `api/services/delete.json`
* `SEARCH` request to `v1/service/clinics` with a `header` contains `X-Request` with `base64` encoded `JSON` like `api/services/search.json`

you can for example use `v1/service/pharmacies` to use pharmacies services and so on.

* `POST` request to `v1/service/pharmacies` with a `body` contains `JSON` like `api/services/create.json`
* `GET` request to `v1/service/pharmacies` with a `header` contains `X-Request` with `base64` encoded `JSON` like `api/services/read.json`
* `PUT` request to `v1/service/pharmacies` with a `body` contains `JSON` like `api/services/update.json`
* `DELETE` request to `v1/service/pharmacies` with a `header` contains `X-Request` with `base64` encoded `JSON` like `api/services/delete.json`
* `SEARCH` request to `v1/service/pharmacies` with a `header` contains `X-Request` with `base64` encoded `JSON` like `api/services/search.json`

you can for example use `v1/service/laboratories` to use laboratories services and so on.

* `POST` request to `v1/service/laboratories` with a `body` contains `JSON` like `api/services/create.json`
* `GET` request to `v1/service/laboratories` with a `header` contains `X-Request` with `base64` encoded `JSON` like `api/services/read.json`
* `PUT` request to `v1/service/laboratories` with a `body` contains `JSON` like `api/services/update.json`
* `DELETE` request to `v1/service/laboratories` with a `header` contains `X-Request` with `base64` encoded `JSON` like `api/services/delete.json`
* `SEARCH` request to `v1/service/laboratories` with a `header` contains `X-Request` with `base64` encoded `JSON` like `api/services/search.json`

you can for example use `v1/service/radiologycenters` to use radiologycenters services and so on.

* `POST` request to `v1/service/radiologycenters` with a `body` contains `JSON` like `api/services/create.json`
* `GET` request to `v1/service/radiologycenters` with a `header` contains `X-Request` with `base64` encoded `JSON` like `api/services/read.json`
* `PUT` request to `v1/service/radiologycenters` with a `body` contains `JSON` like `api/services/update.json`
* `DELETE` request to `v1/service/radiologycenters` with a `header` contains `X-Request` with `base64` encoded `JSON` like `api/services/delete.json`
* `SEARCH` request to `v1/service/radiologycenters` with a `header` contains `X-Request` with `base64` encoded `JSON` like `api/services/search.json`

- do the same for other services.

### Benchmark
- Machine: MSI B450M, AMD Ryzen 3800X with 32 GB of RAM using Release version built with GCC 14.1 running k6r.js & k6w.js with 1000 virtual users and 100,000 iterations
  ![alt text](https://github.com/dr3mro/ProjectValhalla/blob/4c79a5bc52b3e8a2f489ce8429595a1c63261081/benchmark.png?raw=true)
