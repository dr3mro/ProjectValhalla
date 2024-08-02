import http from 'k6/http';

export let options = {
    vus: 100,
    iterations: 10000,
};

const BASE_URL = 'http://172.20.0.10:80/v1/patient';
const AUTH_TOKEN = 'Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjUyMDY2NTgsImdyb3VwIjoicHJvdmlkZXJzIiwiaWF0IjoxNzIyNjE0NjU4LCJpc3MiOiJ2YWxoYWxsYSIsImp0aSI6IjEwMDEiLCJsbG9kdCI6IjIwMjQtMDgtMDEgMjM6MTU6MjUrMDAiLCJzdWIiOiJwcm92aWRlcjEifQ.kCxEAlz-8xhuZ9CkBFOwiNcDXM1r0Lbfb_srcsjGCUw';

export default function () {
    const data = __ENV.PATIENT_DATA;

    const params = {
        headers: {
            'Authorization': AUTH_TOKEN,
            'Content-Type': 'application/x-www-form-urlencoded'
        },
    };

    let res = http.post(BASE_URL, data, params);
    
    if (res.status !== 200) {
        console.error(`Request failed with status ${res.status}`);
    }

}
