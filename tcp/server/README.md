# The Glorious 'Kak by' Chat Application
# 'Kak by' chat -- 'kak by'

## The protocol

### Client to server commands
header:     uint32_t type
            uint32_t username length
            char[]   username
body:       {custom for each type}

type:       0        register to server
body:       {empty}

type:       1        send message
body:       uint32_t receiver username length
            char[]   receiver username
            uint32_t message length
            char[]   message
            uint32_t timestamp            

type:       2        acknowledge message received by client
body:       uint32_t username of the message author length
            char[]   username of the message author
            uint32_t message timestamp
            
type:       3        disconnect
body:       {empty}

### Server to client commands
header:     int      type
body:       {custom for each type}

type:       0        acknowledge registration
body:       {empty}

type:       1        send message
body:       uint32_t sender username length
            char[]   sender username
            uint32_t message length
            char[]   message
            uint32_t timestamp  

type:       2        acknowledge message received by receiver 
body:       uint32_t username of the message receiver length
            char[]   username of the message receiver
            uint32_t message timestamp

type:       3        disconnect
body:       {empty}

type:       619      registration rejected
body:       {empty}
