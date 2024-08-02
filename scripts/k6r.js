import {check} from 'k6';
import http from 'k6/http';

export let options = {
  vus: 100,
  iterations: 10000,
};

const BASE_URL = 'http://172.20.0.10:80/v1/patient';
const AUTH_TOKEN =
    'Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjUxODIwNDAsImdyb3VwIjoidXNlcnMiLCJpYXQiOjE3MjI1OTAwNDAsImlzcyI6InZhbGhhbGxhIiwianRpIjoiMTAxMSIsImxsb2R0IjoiMjAyNC0wOC0wMSAyMzoxMzoxNiswMCIsInN1YiI6InVzZXIxIn0.fNF0YvJ1zcrxFzMbZ1Mf2DS4BXq9cV0lDhNatSMbxnA';
const X_REQUEST =
    'ewogICJpZCI6IDEwMDAwMSwKICAic2NoZW1hIjpbCiAgICAiYmFzaWNfZGF0YSIsCiAgICAiaGVhbHRoX2RhdGEiLAogICAgImFwcG9pbnRtZW50c19kYXRhIgogIF0KfQoK';

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
