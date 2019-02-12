# Зависимости
- Boost 1.058.00

# Протокол игры "Рулетка"

## Формат сообщений

    +--------------------+--------------------+-------------+
    | int message type   | int body length    | char[] body | 
    +--------------------+--------------------+-------------+

## Клиент

- Запрос на добавление игрока

      type: 100
      body: "Name" 

- Запрос на вход крупье по паролю

      type: 200
      body: "Password"

- Запрос на добавление ставки 

      type: 300
      body: "N" | "even" | "odd"
    
- Запрос на начало розыгрыша
  
      type: 400
      
- Запрос на конец розыгрыша
  
      type: 500
      
- Получить список всех ставок
    
      type 700
    
## Сервер

- Отчёт о получении сообщения от неавторизованного игрока или крупье

      type: 1
      body: "Optional message"

- Отчёт о получении некорректного сообщения

      type: 2
      body: "Optional message"

- Отчёт о добавлении игрока

      type: 101
      body: "Optional message"

- Отчёт об отказе в добавлении игрока

      type: 102
      body: "Optional message, ex: name is taken"

- Отчёт об авторизации крупье

      type: 201
      body: "Optional message"

- Отчёт об ошибке ввода пароля крупье

      type: 202
      body: "Optional message, ex: wrong passord"

- Отчёт об отказе входа крупье

      type: 203
      body: "Optional message, ex: croupier already authorised"

- Отчёт о принятии ставки

      type: 301
      body: "Optional message"

- Отчёт о повторной ставке

      type: 302
      body: "Optional message, ex: you already made your bet"
  
- Отчёт о несвоевременной ставке

      type: 303
      body: "Optional message, ex: game in progress"

- Отчёт о начале розыгрыша

      type: 401
      body: "Optional message"

- Отчёт об отклонении начала розыгрыша

      type: 402
      body: "Optional message, ex: game in progress"

 - Отчёт о завершении розыгрыша

        type: 501
        body: "Optional message"
  
- Отчёт об отклонении конца розыгрыша

        type: 502
        body: "Optional message, ex: game is already fineshed"
  
- Оповещение с результатами розыгрыша

      type: 600
      body: "N GAIN"
      
- Список ставок всех игроков

      type: 800
      body: "NAME1 BET1 NAME2 BET2 ..."
