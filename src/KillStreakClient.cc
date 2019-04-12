#include <sysexits.h>
#include <string>
#include <iostream>

#include "rpc/server.h"
#include "rpc/client.h"
#include "logger.hpp"
#include "KillStreakTypes.hpp"
#include "KillStreakClient.hpp"

using namespace std;

KillStreakClient::KillStreakClient(INIReader& t_config)
    : config(t_config), killStreakServer(nullptr), thisClient(nullptr)
{
    auto log = logger();

	// pull the address and port for the server
	string serverconf = config.Get("ssd", "server", "");
	if (serverconf == "") {
		log->error("The server was not found in the config file");
		exit(EX_CONFIG);
	}
	size_t idx = serverconf.find(":");
	if (idx == string::npos) {
		log->error("Config line {} is invalid", serverconf);
		exit(EX_CONFIG);
	}
	string serverHost = serverconf.substr(0,idx);
	int serverPort = strtol(serverconf.substr(idx+1).c_str(), nullptr, 0);
	if (serverPort <= 0 || serverPort > 65535) {
		log->error("The port provided is invalid: {}", serverconf);
		exit(EX_CONFIG);
	}

  string clientconf = config.Get("ss", "client", "");
  if (clientconf == "") {
    log->error("The client was not found in the config file");
    exit(EX_CONFIG);
  }

  idx = clientconf.find(":");
  if (idx == string::npos) {
    log->error("Config line {} is invalid", clientconf);
    exit(EX_CONFIG);
  }

  clientHost = clientconf.substr(0, idx);
  clientPort = strtol(clientconf.substr(idx+1).c_str(), nullptr, 0);
  if (clientPort <= 0 || clientPort > 65535) {
    log->error("The port provided is invalid: {}", clientconf);
    exit(EX_CONFIG);
  }

  log->info("Welcome to KillStreak! You've just launched the game client!");

	killStreakServer = new rpc::client(serverHost, serverPort);
  thisClient = new rpc::server(clientPort);

  thisClient->bind("connected", []() {
    auto log = logger();
    log->info("Successfully connected to the KillStreak game server. Waiting for 3 other players...");
  });

  thisClient->bind("start_announcement", [](string name) {
    auto log = logger();
    log->info("Hello {}! We've found 3 other players; starting game momentarily...", name);
  });

  thisClient->bind("countdown", [](int countdown) {
    auto log = logger();
    log->info("Starting game in {}...", countdown);
  });

  thisClient->bind("ping", []() {
    auto log = logger();
    log->info("Game is playing!!");
  });

  thisClient->async_run(1);
}

KillStreakClient::~KillStreakClient() {
  if (thisClient) {
    delete thisClient;
    thisClient = nullptr;
  }
  if (killStreakServer) {
    delete killStreakServer;
    killStreakServer = nullptr;
  }
}

void KillStreakClient::startGame() {
  auto log = logger();
  log->info("Alright, what's your name this game? (max 6 characters)");
  bool goodName = false;
  string name;
  while (!goodName) {
    getline(cin, name);
    if (name.size() <= 6) {
      goodName = true;
    }
  }
  killStreakServer->call("connect", clientHost, clientPort, name);
  while (true) {
    // do nothing
  }
}
