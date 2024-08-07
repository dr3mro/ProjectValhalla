import {check, sleep} from 'k6';
import http from 'k6/http';

export let options = {
  stages: [
    {duration: '1m', target: 50},   // ramp up to 50 VUs over 1 minute
    {duration: '2m', target: 50},   // stay at 50 VUs for 2 minutes
    {duration: '1m', target: 100},  // ramp up to 100 VUs over 1 minute
    {duration: '2m', target: 100},  // stay at 100 VUs for 2 minutes
    {duration: '1m', target: 150},  // ramp up to 150 VUs over 1 minute
    {duration: '2m', target: 150},  // stay at 150 VUs for 2 minutes
    {duration: '2m', target: 0},    // ramp down to 0 VUs over 2 minutes
  ],
};

const BASE_URL = 'http://172.20.0.10:80/v1/patient';
const AUTH_TOKEN =
    'Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3MjUyMDM2NjYsImdyb3VwIjoicHJvdmlkZXJzIiwiaWF0IjoxNzIyNjExNjY2LCJpc3MiOiJ2YWxoYWxsYSIsImp0aSI6IjEwMDEiLCJsbG9kdCI6IjIwMjQtMDgtMDEgMjM6MTU6MjUrMDAiLCJzdWIiOiJwcm92aWRlcjEifQ.5rXYoxgghv8LTJfmAFVItMt671aah-ek2THlq9iPWfA';
const X_REQUEST =
    'ewogICJpZCI6IDEwMDAwMSwKICAic2NoZW1hIjpbCiAgICAiYmFzaWNfZGF0YSIsCiAgICAiaGVhbHRoX2RhdGEiLAogICAgImFwcG9pbnRtZW50c19kYXRhIgogIF0KfQoK';

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
