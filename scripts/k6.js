import { check } from 'k6';
import http from 'k6/http';

export let options = {
  vus: 100,
  iterations: 10000,
};

const BASE_URL = 'http://172.20.0.4:8080/v1/patient';
const AUTH_TOKEN = 'Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjM0NDk3MjAsImlhdCI6MTcyMDg1NzcyMCwiaXNzIjoiUHJvamVjdFZhbGhhbGxhIiwianRpIjoiMTAwMCIsInN1YiI6ImFtcl9uYXNyIn0.PoXaVi0AZ_Ptah1WmeZyKtcpMl4IUbiVSiTFpTbn19c';
const X_REQUEST = 'ewogICJpZCI6IDE0MDAwNCwKICAic2NoZW1hIjpbCiAgICAiYmFzaWNfZGF0YSIsCiAgICAiaGVhbHRoX2RhdGEiLAogICAgImFwcG9pbnRtZW50c19kYXRhIgogIF0KfQoK';

export default function() {
  const getParams = {
    headers: {
      'Authorization': AUTH_TOKEN,
      'Content-Type': 'application/x-www-form-urlencoded',
      'X-Request': X_REQUEST,
    },
  };

  const postParams = {
    headers: {
      'Authorization': AUTH_TOKEN,
      'Content-Type': 'application/x-www-form-urlencoded',
    },
  };

  // Perform GET request
  let getRes = http.get(BASE_URL, getParams);

  check(getRes, {
    'GET request status is 200': (r) => r.status === 200,
  });

  if (getRes.status !== 200) {
    console.error(`GET request failed with status ${getRes.status}`);
  }

  // Perform POST request
  const data = __ENV.PATIENT_DATA;
  let postRes = http.post(BASE_URL, data, postParams);

  check(postRes, {
    'POST request status is 200': (r) => r.status === 200,
  });

  if (postRes.status !== 200) {
    console.error(`POST request failed with status ${postRes.status}`);
  }
}

