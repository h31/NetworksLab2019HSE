# Запросы 
### Добавить пользователя 
POST /user\
input: 
```json
{
  "login": "alol",
  "password": "123"
}
```
output: 
```json
{
  "status": "OK",
  "description": "description"
}
```
    	

### Выдать все кошельки для всех пользователей
GET /users\
output: - массив логинов
```json
["user1", "user2", "user3"] 
```

### Перевод от одного пользователя к другому
POST /send\
input: 
```json
{
  "fromLogin":"loginFrom",
  "fromPassword":"passwordTo",
  "toLogin":"loginTo",
  "count":20
}
```

output: 
```json
{
  "status": "OK",
  "description": "description"
}
```


### Запросить перевод
POST /send
input: 
```json
{
  "fromLogin":"loginFrom",
  "fromPassword":"passwordTo",
  "toLogin":"loginTo",
  "count":20
}
```

output:
 ```json
 {
  "status": "OK",
  "description": "",
  "entityId": "p626u7copf2tha8rc3yz"
}
```

### Реакция на запрос перевода перевода 
POST /accept
input 
```json
{
  "login":"login",
  "password":"123",
  "key":"qe2bmngpw41niaa6junt",
  "actionType":"ACCEPT"
}
```
output: 
```json
{
  "status": "OK",
  "description": ""
}
```

### Проверить состояние кошелька
POST /wallet
input: 
```json
{
  "login": "alol",
  "password": "123"
}
```
output:
```json
{
  "login": "olya2",
  "count": 88
}
```

### Просмотреть все запросы 
POST /request
input: 
```json
{
  "login": "alol",
  "password": "123"
}
```
output:
```json
[
  {
    "from": "fromLogin",
    "key": "ulrrp3ebqfq0nsrjdaz1",
    "count": 30
  },
  {
    "from": "user2",
    "key": "5sqwz36u0gtf0v91mxfx", 
    "count": 20
  }
]
```







