# rocs
RObot Control System, a linux based robotic control system for Bananas.

# 1. Project goal
Aim of this project is to implement a linux based control framework for robots. It uses several components which exchange queued messages via socket connections.  
Each component can send messages to the server without knowing if there is a receiver for it.
Messages consist of a message id and a tail. Maybe there will be a time to live in future.
The server is able to determine receivers for a message by applying a message filter on the message id. This filter is given by each component. 

This enables the framework to be easily extended by starting new components.

# 2. Project structure
##   1. rocsmq-server
This is basically a simple chat server which enables the components to communicate. When a client connects, it will send its name and message filter data.
Any message will be relayed to all clients (actually, even the sender)
  
##   2. rocsmq-clients 
A rocsmq-client connects to the server and sends it's login data. 
Additionally, it creates a Thread which listens for messages and queues them for the main loop.

# 3. ROCSMQ(ROCS-Message Queue) Clients 
##   1. rocsmq-client-example
This is an example client which can be used as base for your own clients.  
~TODO: there is some sorting needed to enable some priority management.
~TODO: there is a priority mechanism needed to override other messages..

##   2. rocsmq-canbus
This is a (still not entirely working) client which is able to relay messages to the can bus

##   3. rocsmq-i2cbus
This is a (still not entirely working) client which is able to relay messages to the i²c bus
 
