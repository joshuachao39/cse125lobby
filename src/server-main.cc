#include <iostream>
#include <stdlib.h>
#include <sysexits.h>

#include "inih/INIReader.h"
#include "rpc/server.h"
#include "logger.hpp"
#include "KillStreakServer.hpp"

using namespace std;

int main(int argc, char** argv) {
  initLogging();
  auto log = logger();

  // handle command line args
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " [config file]" << endl;
    return EX_USAGE;
  }

  // read in config file
  INIReader config(argv[1]);

  if (config.ParseError() < 0) {
		cerr << "Error parsing config file " << argv[1] << endl;
		return EX_CONFIG;
	}

  // server loop here
  if (config.GetBoolean("ssd", "enabled", true)) {
    log->info("Killstreak server enabled");
    KillStreakServer* killServer = new KillStreakServer(config);
    killServer->launch();
  } else {
    log->error("Killstreak server disabled");
  }
  return 0;
}
