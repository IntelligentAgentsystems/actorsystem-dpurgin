#pragma once

#include <string>

enum class Position { North, West, South, East, NorthWest, SouthWest, SouthEast, NorthEast };
enum class Color { Cyan, Magenta, Yellow, Black };

class AgentSystemConnector;

extern void run_agent_system(int argc, char *argv[], AgentSystemConnector *connector);
extern std::string to_string(Position position);
