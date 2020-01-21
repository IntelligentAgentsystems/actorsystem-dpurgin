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
    connect(agentSystemConnector(), &AgentSystemConnector::turntableEastDirectionChanged, [this]() {
      auto ttDir = toTurntableDirection(agentSystemConnector()->turntableEastDirection());
      setDirection(ttDir);
    });
  } else if (m_position == Turntable::Position::WestPos) {
    connect(agentSystemConnector(), &AgentSystemConnector::turntableWestDirectionChanged, [this]() {
      auto ttDir = toTurntableDirection(agentSystemConnector()->turntableWestDirection());
      setDirection(ttDir);
    });
  }
  m_isComplete = true;
}

void Turntable::setDirection(Turntable::Direction direction) {
  if (m_direction != direction) {
    m_direction = direction;
    Q_EMIT directionChanged();
  }
}
