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

- data:
```
{
  old_password: "",
  new_password: ""
}
```

### POST /configure

- data:
```
{
  capacity: 34,
  savezone: 20,
  throughput: 12
}
```

### POST /timers

- data:
```
{
  timers: [
    {
      time: 324,
      offset: 2532,
      amount: 12
    },
    {
      time: 314,
      offset: 3214,
      amount: 2
    }
  ]
}
```
