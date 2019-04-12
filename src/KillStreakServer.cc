#include <sysexits.h>
#include <string>
#include <chrono>

#include "rpc/server.h"
#include "logger.hpp"
#include "KillStreakServer.hpp"
#include "KillStreakTypes.hpp"

using namespace std;

KillStreakServer::KillStreakServer(INIReader& t_config) : config(t_config), server(nullptr) {
  auto log = logger();

  // pull the address and port from the config file
  string serv_addrport = config.Get("ssd", "server", "");
  if (serv_addrport == "") {
		log->error("Server line not found in config file");
		exit(EX_CONFIG);
	}

  size_t idx = serv_addrport.find(":");
	if (idx == string::npos) {
		log->error("Config line {} is invalid", serv_addrport);
		exit(EX_CONFIG);
	}
	port = strtol(serv_addrport.substr(idx+1).c_str(), nullptr, 0);
	if (port <= 0 || port > 65535) {
		log->error("The port provided is invalid: {}", serv_addrport);
		exit(EX_CONFIG);
	}
  tick_rate = (double)(config.GetInteger("ssd", "tick_rate", -1));
  if (tick_rate == -1) {
    log->error("The tick rate provided is invalid");
    exit(EX_CONFIG);
  }
}

KillStreakServer::~KillStreakServer() {
  for (auto player: players) {
    delete player.client;
  }
  delete server;
}

void KillStreakServer::tick() {
  auto log = logger();
  if (SECONDS_TO_START >= 0) { // super hacky, find a better way later
    for (auto player: players) {
      player.client->call("countdown", SECONDS_TO_START);
    }
    SECONDS_TO_START--;
  } else {
    for (auto player: players) {
      player.client->call("ping");
    }
  }
}

void KillStreakServer::startGame() {
  // broadcast game announcement
  auto log = logger();
  log->info("sending game start to all clients...");
  for (auto player : players) {
    player.client->call("start_announcement", player.name);
  }

  double ns = 1000000000.0 / tick_rate;
  double delta = 0;
  bool running = true;
  // game loop
  auto lastTime = Clock::now();

  log->info("server loop about to run");
  while(running) {
    auto now = Clock::now();
    dsec ds = now - lastTime;
    nanoseconds duration = chrono::duration_cast<nanoseconds>(ds);
    delta += (duration.count() / ns);
    lastTime = now;

    while (delta >= 1) {
      tick();
      delta--;
    }
  }

}

void KillStreakServer::launch() {
  auto log = logger();
  log->info("Launching KillStreakServer...");
  log->info("port: {}", port);
  log->info("tick rate: {}", tick_rate);
  server = new rpc::server(port);

  server->bind("connect", [this](string host, int port, string name) {
    auto client = new rpc::client(host, port);
    Player player;
    player.name = name;
    player.client = client;
    players.push_back(player);
    client->call("connected");
    if (players.size() == 2) {
      startGame();
    }
  });

  // start accepting connections asynchronously
  server->run();
}
