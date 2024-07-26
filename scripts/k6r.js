import {check} from 'k6';
import http from 'k6/http';

export let options = {
  vus: 100,
  iterations: 10000,
};

const BASE_URL = 'http://172.20.0.4:8080/v1/patient';
const AUTH_TOKEN =
    'Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjM0NDk3MjAsImlhdCI6MTcyMDg1NzcyMCwiaXNzIjoiUHJvamVjdFZhbGhhbGxhIiwianRpIjoiMTAwMCIsInN1YiI6ImFtcl9uYXNyIn0.PoXaVi0AZ_Ptah1WmeZyKtcpMl4IUbiVSiTFpTbn19c';
const X_REQUEST =
    'ewogICJpZCI6IDE0MDAwNCwKICAic2NoZW1hIjpbCiAgICAiYmFzaWNfZGF0YSIsCiAgICAiaGVhbHRoX2RhdGEiLAogICAgImFwcG9pbnRtZW50c19kYXRhIgogIF0KfQoK';

export default function() {
  const params = {
    headers: {
      'Authorization': AUTH_TOKEN,
      'Content-Type': 'application/x-www-form-urlencoded',
      'X-Request': X_REQUEST,
    },
  };

  let res = http.get(BASE_URL, params);

  check(res, {
    'is status 200': (r) => r.status === 200,
  });

  if (res.status !== 200) {
    console.error(`Request failed with status ${res.status}`);
  }
}
