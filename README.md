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
    
## Сервер

- Отчёт об успешной операции

      type: 0
      body: "Optional message"

- Отчёт о получении сообщения от неавторизованного игрока или крупье

      type: 1
      body: "Optional message"

- Отчёт о получении некорректного сообщения

      type: 2
      body: "Optional message"

- Отчёт об отказе в добавлении игрока

      type: 101
      body: "Optional message, ex: name is taken"

- Отчёт об ошибке ввода пароля крупье

      type: 201
      body: "Optional message, ex: wrong passord"

- Отчёт об отказе входа крупье

      type: 202
      body: "Optional message, ex: croupier already authorised"

- Отчёт о повторной ставке

      type: 301
      body: "Optional message, ex: you already made your bet"
  
- Отчёт о несвоевременной ставке

      type: 302
      body: "Optional message, ex: game in progress"

- Отчёт об отклонении начала розыгрыша

      type: 401
      body: "Optional message, ex: game in progress"
      
 - Отчёт об отклонении конца розыгрыша

      type: 501
      body: "Optional message, ex: game is already fineshed"
  
- Оповещение о завершении розыгрыша

      type: 600
      body: "N GAIN"
