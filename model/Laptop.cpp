#include "Laptop.h"

#include "../agentsystemconnector.h"

void Laptop::print(const QString &sequence) { agentSystemConnector()->print(sequence); }
