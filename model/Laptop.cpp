#include "Laptop.h"

#include "../agentsystemconnector.h"

Laptop::Laptop(QObject *parent) : QObject{parent} {
  connect(agentSystemConnector(), &AgentSystemConnector::printed, [this](char color) {
    m_printed += color;
    Q_EMIT printedChanged();
  });

  connect(agentSystemConnector(), &AgentSystemConnector::leftToPrint, [this](const QString &leftToPrint) {
    if (m_leftToPrint != leftToPrint) {
      m_leftToPrint = leftToPrint;
      Q_EMIT leftToPrintChanged();
    }
  });
}

void Laptop::print(const QString &sequence) {
  m_printed.clear();
  m_leftToPrint.clear();

  Q_EMIT printedChanged();
  Q_EMIT leftToPrintChanged();

  agentSystemConnector()->print(sequence);
}
