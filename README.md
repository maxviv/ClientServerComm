# ClientServerComm
Simple Project to demonstrate communication between Client and Server using pub/sub model implemented on top of boost::asio

Requirements
•	We need to build a client server system 
•	The client and server will run in separate machines
•	The client and server can use any pub/sub models implementing DDS to communicate
•	The server has a database of Person Info containing Person ID and corresponding Age
•	The client can send request to the Server asking for the age of a particular person ID
•	The server should then reply the client with the age and ID of the person
•	Good to have features
o	There may be multiple clients connecting with the same server
o	The server should be scalable and highly responsive

Demo :
![image](https://user-images.githubusercontent.com/2766749/171420403-b863d692-db28-494c-a13f-d0fbaab2a278.png)
