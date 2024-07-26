import http from 'k6/http';

export let options = {
    vus: 100,
    iterations: 10000,
};

const BASE_URL = 'http://172.20.0.4:8080/v1/patient';
const AUTH_TOKEN = 'Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjM0NDk3MjAsImlhdCI6MTcyMDg1NzcyMCwiaXNzIjoiUHJvamVjdFZhbGhhbGxhIiwianRpIjoiMTAwMCIsInN1YiI6ImFtcl9uYXNyIn0.PoXaVi0AZ_Ptah1WmeZyKtcpMl4IUbiVSiTFpTbn19c';

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
