#include <iostream>
#include <comm_layer.h>
#include <map>

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
* AgeInfoServer is a simple server which contains a database of person's ID vs Age.
* If the server gets a Age Query from a client, it replies back with a message to client.
* The message body contains the person ID and age of the person.
* If the person ID doesnt exist in Server's data base, it replies by sending age as -1 in message body
* The AgeInfo server waits in a busy loop looking for requests from clients.Once it gets any request it processes the client request
* If the message queue of the server is empty it waits till there are some messages in its queue.
*/
class AgeInfoServer : public comm::server_interface<CustomMsgTypes>
{
private:
	//This map contains info about the age given a person's ID
	std::map <std::string, int> name_vs_map;
public:
	AgeInfoServer(uint16_t nPort) : comm::server_interface<CustomMsgTypes>(nPort)
	{
		//Populate DB of personID vs Age
		for (int i = 1; i <= 10; i++) {
			std::string name = "person";
			name += std::to_string(i);
			name_vs_map[name] = 10*i;
		}
	
	}

protected:
	virtual bool OnClientConnect(std::shared_ptr<comm::connection<CustomMsgTypes>> client)
	{
		comm::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::ServerAccept;
		client->Send(msg);
		return true;
	}

	// Called when a client appears to have disconnected
	virtual void OnClientDisconnect(std::shared_ptr<comm::connection<CustomMsgTypes>> client)
	{
		std::cout << "Removing client [" << client->GetID() << "]\n";
	}

	// Called when a message arrives
	virtual void OnMessage(std::shared_ptr<comm::connection<CustomMsgTypes>> client, comm::message<CustomMsgTypes>& msg)
	{
		switch (msg.header.id)
		{
		case CustomMsgTypes::ServerPing:
		{
			std::cout << "[" << client->GetID() << "]: Server Ping\n";

			break;

		}

		case CustomMsgTypes::AgeQuery:
		{
			int per_no;
			msg >> per_no;
			std::string text = "person";
			text += std::to_string(per_no);
			std::cout << "[Client ID " << client->GetID() <<  "]" << " has queried the age of " << text << std::endl;
			comm::message<CustomMsgTypes> msg;
			msg.header.id = CustomMsgTypes::AgeReply;
			if (name_vs_map.find(text) == name_vs_map.end()) {
				std::cout << " No age info for " << text << std::endl;
				msg << per_no;
				msg << -1;
			}
			else {
				msg << per_no;
				msg << name_vs_map[text];
				
			}
			client->Send(msg);
			break;

		}


		}
	}
};

int main()
{
	AgeInfoServer server(60000);
	server.start();

	while (1)
	{
		server.processClientRequests(-1, true);
	}



	return 0;
}