#pragma once

enum class Position { North, West, South, East, NorthWest, SouthWest, SouthEast, NorthEast };

class AgentSystemConnector;

extern void run_agent_system(int argc, char *argv[], AgentSystemConnector *connector);
