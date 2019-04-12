#ifndef SURFSTORECLIENT_HPP
#define SURFSTORECLIENT_HPP

#include <string>
#include <list>

#include "inih/INIReader.h"
#include "rpc/client.h"
#include "rpc/server.h"

#include "logger.hpp"
#include "KillStreakTypes.hpp"

using namespace std;

class KillStreakClient {
public:
    KillStreakClient(INIReader& t_config);
    ~KillStreakClient();

	void startGame();

protected:

  INIReader& config;

	rpc::client* killStreakServer; // super confusing
  rpc::server* thisClient;
  string clientHost;
  int clientPort;
};

#endif // SURFSTORECLIENT_HPP
