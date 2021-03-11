# API

### GET /wifi/scan

```
  [
  {
    ssid: "test",
    rssi: 34,
  },
  {
    ssid: "test2",
    rssi: 35,
  }
  ]
```

### POST /wifi/connect

- data: ssid, password

### GET /active

```
{
  active: true,
}
```

### POST /active

- data: active

### POST /auth

- data: password

### POST /password

- store new password hash in nvs

- data: password
