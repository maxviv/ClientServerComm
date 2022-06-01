#include <iostream>
#include <comm_layer.h>
#include <map>

enum class CustomMsgTypes : uint32_t
{
	ServerAccept,
	ServerDeny,
	ServerPing,
	ServerMessage,
	Name,
	Age,
};



class AgeInfoServer : public comm::server_interface<CustomMsgTypes>
{
private:
	std::map <std::string, int> name_vs_map;
public:
	AgeInfoServer(uint16_t nPort) : comm::server_interface<CustomMsgTypes>(nPort)
	{
		//Populate DB of Name vs Age
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

		case CustomMsgTypes::Name:
		{
			int per_no;
			msg >> per_no;
			std::string text = "person";
			text += std::to_string(per_no);
			std::cout << "[Clinet ID " << client->GetID() <<  "]" << " has queried the age of " << text << std::endl;
			comm::message<CustomMsgTypes> msg;
			msg.header.id = CustomMsgTypes::Age;
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
	server.Start();

	while (1)
	{
		server.Update(-1, true);
	}



	return 0;
}