#include <iostream>
#include <comm_layer.h>
#include <chrono>
#include <thread>
using namespace std::this_thread;
using namespace std::chrono; 

enum class CustomMsgTypes : uint32_t
{
	ServerAccept,
	ServerDeny,
	ServerPing,
	ServerMessage,
	Name,
	Age,
};



class CustomClient : public comm::client_interface<CustomMsgTypes>
{
public:
	void pingServer()
	{
		comm::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::ServerPing;

		std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();

		msg << timeNow;
		Send(msg);
	}

	void queryAge(int personNum)
	{
		comm::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::Name;
		msg << personNum;
		Send(msg);

	}


};

int main()
{
	CustomClient c;
	c.Connect("127.0.0.1", 60000);


	bool bQuit = false;
	while (!bQuit)
	{
		
		
		//c.pingServer();
		sleep_for(seconds(5));
		if (c.IsConnected())
		{
			
			if (!c.Incoming().empty())
			{
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

				case CustomMsgTypes::ServerMessage:
				{
					// Server has responded to a ping request	
					uint32_t clientID;
					msg >> clientID;
					std::cout << "Hello from [" << clientID << "]\n";
				}
				break;

				case CustomMsgTypes::Age:
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