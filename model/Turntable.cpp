#include "Turntable.h"

#include "../agentsystemconnector.h"

static Turntable::Direction toTurntableDirection(Position position) {
  switch (position) {
  case Position::North:
    return Turntable::NorthDir;
  case Position::East:
    return Turntable::EastDir;
  case Position::West:
    return Turntable::WestDir;
  case Position::South:
    return Turntable::SouthDir;
  }
}

void Turntable::classBegin() {}

void Turntable::componentComplete() {  
  if (m_position == Turntable::Position::EastPos) {
    setDirection(toTurntableDirection(agentSystemConnector()->turntableEastDirection()));
    setLoaded(agentSystemConnector()->turntableEastLoaded());

    connect(agentSystemConnector(), &AgentSystemConnector::turntableEastDirectionChanged, [this]() {
      auto ttDir = toTurntableDirection(agentSystemConnector()->turntableEastDirection());
      setDirection(ttDir);
    });

    connect(agentSystemConnector(), &AgentSystemConnector::turntableEastLoadedChanged,
            [this]() { setLoaded(agentSystemConnector()->turntableEastLoaded()); });

  } else if (m_position == Turntable::Position::WestPos) {
    setDirection(toTurntableDirection(agentSystemConnector()->turntableWestDirection()));
    setLoaded(agentSystemConnector()->turntableWestLoaded());

    connect(agentSystemConnector(), &AgentSystemConnector::turntableWestDirectionChanged, [this]() {
      auto ttDir = toTurntableDirection(agentSystemConnector()->turntableWestDirection());
      setDirection(ttDir);
    });

    connect(agentSystemConnector(), &AgentSystemConnector::turntableWestLoadedChanged,
            [this]() { setLoaded(agentSystemConnector()->turntableWestLoaded()); });
  }
  m_isComplete = true;
}
