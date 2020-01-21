#include "Plotter.h"

#include "../agentsystemconnector.h"

#include <QDebug>

static Plotter::Color toPlotterColor(char ch) {
  switch (::tolower(ch)) {
  case 'c':
    return Plotter::Color::Cyan;
  case 'm':
    return Plotter::Color::Magenta;
  case 'y':
    return Plotter::Color::Yellow;
  case 'k':
    return Plotter::Color::Black;
  }

  return Plotter::Color::Black;
}

void Plotter::classBegin() {}

void Plotter::componentComplete() {
  if (m_position == NorthEastPos) {
    setColor(toPlotterColor(agentSystemConnector()->plotterNorthEastColor()));

    connect(agentSystemConnector(), &AgentSystemConnector::plotterNorthEastColorChanged, [this]() {
      auto clr = toPlotterColor(agentSystemConnector()->plotterNorthEastColor());
      setColor(clr);
    });
  } else if (m_position == NorthWestPos) {
    setColor(toPlotterColor(agentSystemConnector()->plotterNorthWestColor()));

    connect(agentSystemConnector(), &AgentSystemConnector::plotterNorthWestColorChanged, [this]() {
      auto clr = toPlotterColor(agentSystemConnector()->plotterNorthWestColor());
      setColor(clr);
    });

  } else if (m_position == SouthEastPos) {
    setColor(toPlotterColor(agentSystemConnector()->plotterSouthEastColor()));

    connect(agentSystemConnector(), &AgentSystemConnector::plotterSouthEastColorChanged, [this]() {
      auto clr = toPlotterColor(agentSystemConnector()->plotterSouthEastColor());
      setColor(clr);
    });

  } else if (m_position == SouthWestPos) {
    setColor(toPlotterColor(agentSystemConnector()->plotterSouthWestColor()));

    connect(agentSystemConnector(), &AgentSystemConnector::plotterSouthWestColorChanged, [this]() {
      auto clr = toPlotterColor(agentSystemConnector()->plotterSouthWestColor());
      setColor(clr);
    });
  }
}
