#ifndef SURFSTORETYPES_HPP
#define SURFSTORETYPES_HPP

#include "rpc/server.h"
#include "rpc/client.h"
#include <string>
#include <chrono>

typedef struct Player {
  rpc::client* client;
  string name;
} Player;

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::nanoseconds nanoseconds;
typedef std::chrono::duration<double> dsec;

#endif // SURFSTORETYPES_HPP
