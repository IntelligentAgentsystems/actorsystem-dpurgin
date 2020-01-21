#pragma once

#include <functional>

#include <QObject>
#include <QScopedPointer>

#include "agentsystem.h"

class AgentSystemConnector : public QObject {
  Q_OBJECT

public:
  using PrintCallback = std::function<void(std::string)>;

public:
  explicit AgentSystemConnector(QObject *parent = nullptr);
  ~AgentSystemConnector() override;

  Position turntableEastDirection() const;
  void setTurntableEastDirection(Position direction);

  Position turntableWestDirection() const;
  void setTurntableWestDirection(Position direction);

  char plotterNorthEastColor() const;
  void setPlotterNorthEastColor(char color);

  char plotterNorthWestColor() const;
  void setPlotterNorthWestColor(char color);

  char plotterSouthEastColor() const;
  void setPlotterSouthEastColor(char color);

  char plotterSouthWestColor() const;
  void setPlotterSouthWestColor(char color);

  void print(const QString &sequence);

  void installPrintCallback(PrintCallback callback);

Q_SIGNALS:
  void turntableEastDirectionChanged();
  void turntableWestDirectionChanged();
  void plotterNorthEastColorChanged();
  void plotterNorthWestColorChanged();
  void plotterSouthEastColorChanged();
  void plotterSouthWestColorChanged();

private:
  struct Impl;
  QScopedPointer<Impl> d;
};

extern AgentSystemConnector *agentSystemConnector();
