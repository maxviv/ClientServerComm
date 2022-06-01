#include <iostream>
#include <chrono>
#include <thread>

#include <comm_layer.h>

using namespace std::this_thread;
using namespace std::chrono; 

enum class CustomMsgTypes : uint32_t
{
	ServerAccept,
	ServerDeny,
	ServerPing,
	ServerMessage,
	AgeQuery,
	AgeReply,
};

/*
* This is a simple client which tries to connect to the server. The server has a database of person info containting attributes such as age
* The client asks for input the Person ID for whih you want to query age .
* The client then passes this message to server.
* The server replies by sending a message back to the client which contains the ID and age of the person.
* If the person ID is not there in server's database, the server sends a message which contains age value as -1.
* The client is in a busy loop polling for the messages from the server  in its queue and asking for the query.
* Client can be quit if we input -1 as the ID of the person
*/
class CustomClient : public comm::client_interface<CustomMsgTypes>
{
public:
	void pingServer()
	{
		comm::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::ServerPing;

		std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();

		msg << timeNow;
		send(msg);
	}

	void queryAge(int personNum)
	{
		comm::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::AgeQuery;
		msg << personNum;
		send(msg);

	}


};

int main()
{
	CustomClient c;
	c.connect("127.0.0.1", 60000);


	bool bQuit = false;
	while (!bQuit)
	{
		sleep_for(seconds(5));
		if (c.isConnected())
		{
			
			if (!c.Incoming().empty())
			{
				//pop the message in the client's queue
				auto msg = c.Incoming().pop_front().msg;

				switch (msg.header.id)
				{
				case CustomMsgTypes::ServerAccept:
				{
					// Server has responded to a ping request				
					std::cout << "<< Server Accepted Connection\n";
				}
				break;


				case CustomMsgTypes::ServerPing:
				{
					// Server has responded to a ping request
					std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
					std::chrono::system_clock::time_point timeThen;
					msg >> timeThen;
					std::cout << "Ping: " << std::chrono::duration<double>(timeNow - timeThen).count() << "\n";
				}
				break;

				case CustomMsgTypes::AgeReply:
				{
					// Server has responded to a ping request	
					int per_no, age;
					msg >> age >> per_no;
					if (age == -1) {
						std::cout << "<< Server Replied. Age info not found for person" << per_no << "\n";
					}
					else {
						std::cout << "<< Server Replied. Age of person" << per_no <<  " is " << age << "\n";
					}
				}
				break;
				}
			}
			std::cout << ">> Query the Age of a person. Enter -1 to end" << std::endl;
			int personNum;
			std::cin >> personNum;
			if (personNum == -1) {
				std::cout << "Quitting";
				bQuit = true;
			}
			c.queryAge(personNum);
		}
		else
		{
			std::cout << "<< Server Down\n";
			bQuit = true;
		}

	}

	return 0;
}