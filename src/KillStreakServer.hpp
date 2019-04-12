#ifndef KILLSTREAKSERVER_HPP
#define KILLSTREAKSERVER_HPP

#include "inih/INIReader.h"
#include "logger.hpp"
#include <vector>
#include "rpc/server.h"
#include "KillStreakTypes.hpp"

using namespace std;

class KillStreakServer {
  public:
    KillStreakServer(INIReader& t_config);
    ~KillStreakServer();
    void launch();
    void tick();
    void startGame();
    const int NUM_WORKER_THREADS = 4;
    int SECONDS_TO_START = 10;

  protected:
    INIReader& config;
    int port;
    double tick_rate;
    vector<Player> players;
    rpc::server* server;
};

#endif // KILLSTREAKSERVER_HPP
