#pragma once

#include <functional>

#include <QObject>
#include <QScopedPointer>

#include "agentsystem.h"
#include "model/Plotter.h"
#include "model/Turntable.h"

class AgentSystemConnector : public QObject {
  Q_OBJECT

public:
  using PrintCallback = std::function<void(std::string)>;

public:
  explicit AgentSystemConnector(QObject *parent = nullptr);
  ~AgentSystemConnector() override;

  Position turntableEastDirection() const;
  void setTurntableEastDirection(Position direction);

  bool turntableEastLoaded() const;
  void setTurntableEastLoaded(bool loaded);

  Position turntableWestDirection() const;
  void setTurntableWestDirection(Position direction);

  bool turntableWestLoaded() const;
  void setTurntableWestLoaded(bool loaded);

  char plotterNorthEastColor() const;
  void setPlotterNorthEastColor(char color);

  bool plotterNorthEastLoaded() const;
  void setPlotterNorthEastLoaded(bool loaded);

  char plotterNorthWestColor() const;
  void setPlotterNorthWestColor(char color);

  bool plotterNorthWestLoaded() const;
  void setPlotterNorthWestLoaded(bool loaded);

  char plotterSouthEastColor() const;
  void setPlotterSouthEastColor(char color);

  bool plotterSouthEastLoaded() const;
  void setPlotterSouthEastLoaded(bool loaded);

  char plotterSouthWestColor() const;
  void setPlotterSouthWestColor(char color);

  bool plotterSouthWestLoaded() const;
  void setPlotterSouthWestLoaded(bool loaded);

  void print(const QString &sequence);

  void installPrintCallback(PrintCallback callback);  

Q_SIGNALS:
  void turntableEastDirectionChanged();
  void turntableWestDirectionChanged();

  void turntableEastLoadedChanged();
  void turntableWestLoadedChanged();

  void plotterNorthEastColorChanged();
  void plotterNorthWestColorChanged();
  void plotterSouthEastColorChanged();
  void plotterSouthWestColorChanged();

  void plotterNorthWestLoadedChanged();
  void plotterNorthEastLoadedChanged();
  void plotterSouthWestLoadedChanged();
  void plotterSouthEastLoadedChanged();

  void printed(char color);
  void leftToPrint(const QString &sequence);

private:
  struct Impl;
  QScopedPointer<Impl> d;
};

extern AgentSystemConnector *agentSystemConnector();
