#include "agentsystemconnector.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutex>
#include <QMutexLocker>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class AgentSystemConnector::Impl {
  friend class AgentSystemConnector;

  QMutex lock;

  struct Turntable {
    Position direction{Position::North};
    bool loaded{false};
  } east, west;

  struct Plotter {
    char color{'C'};
    bool loaded{false};
  } ne, nw, se, sw;

  PrintCallback printCallback;
};

AgentSystemConnector::AgentSystemConnector(QObject *parent) : QObject{parent}, d{new Impl} {}

AgentSystemConnector::~AgentSystemConnector() = default;

Position AgentSystemConnector::turntableEastDirection() const {
  QMutexLocker locker{&d->lock};
  return d->east.direction;
}

void AgentSystemConnector::setTurntableEastDirection(Position direction) {
  QMutexLocker locker{&d->lock};
  if (d->east.direction != direction) {
    d->east.direction = direction;
    locker.unlock();
    Q_EMIT turntableEastDirectionChanged();
  }
}

bool AgentSystemConnector::turntableEastLoaded() const {
  QMutexLocker locker{&d->lock};
  return d->east.loaded;
}

void AgentSystemConnector::setTurntableEastLoaded(bool loaded) {
  QMutexLocker locker{&d->lock};
  if (d->east.loaded != loaded) {
    d->east.loaded = loaded;
    locker.unlock();
    Q_EMIT turntableEastLoadedChanged();
  }
}

Position AgentSystemConnector::turntableWestDirection() const {
  QMutexLocker locker{&d->lock};
  return d->west.direction;
}

void AgentSystemConnector::setTurntableWestDirection(Position direction) {
  QMutexLocker locker{&d->lock};
  if (d->west.direction != direction) {
    d->west.direction = direction;
    locker.unlock();
    Q_EMIT turntableWestDirectionChanged();
  }
}

bool AgentSystemConnector::turntableWestLoaded() const {
  QMutexLocker locker{&d->lock};
  return d->west.loaded;
}

void AgentSystemConnector::setTurntableWestLoaded(bool loaded) {
  QMutexLocker locker{&d->lock};
  if (d->west.loaded != loaded) {
    d->west.loaded = loaded;
    locker.unlock();
    Q_EMIT turntableWestLoadedChanged();
  }
}

char AgentSystemConnector::plotterNorthEastColor() const {
  QMutexLocker locker{&d->lock};
  return d->ne.color;
}

void AgentSystemConnector::setPlotterNorthEastColor(char color) {
  QMutexLocker locker{&d->lock};
  if (d->ne.color != color) {
    d->ne.color = color;
    locker.unlock();
    Q_EMIT plotterNorthEastColorChanged();
  }
}

char AgentSystemConnector::plotterNorthWestColor() const {
  QMutexLocker locker{&d->lock};
  return d->nw.color;
}

void AgentSystemConnector::setPlotterNorthWestColor(char color) {
  QMutexLocker locker{&d->lock};
  if (d->nw.color != color) {
    d->nw.color = color;
    locker.unlock();
    Q_EMIT plotterNorthWestColorChanged();
  }
}

char AgentSystemConnector::plotterSouthEastColor() const {
  QMutexLocker locker{&d->lock};
  return d->se.color;
}

void AgentSystemConnector::setPlotterSouthEastColor(char color) {
  QMutexLocker locker{&d->lock};
  if (d->se.color != color) {
    d->se.color = color;
    locker.unlock();
    Q_EMIT plotterSouthEastColorChanged();
  }
}

char AgentSystemConnector::plotterSouthWestColor() const {
  QMutexLocker locker{&d->lock};
  return d->sw.color;
}

void AgentSystemConnector::setPlotterSouthWestColor(char color) {
  QMutexLocker locker{&d->lock};
  if (d->sw.color != color) {
    d->sw.color = color;
    locker.unlock();
    Q_EMIT plotterSouthWestColorChanged();
  }
}

bool AgentSystemConnector::plotterNorthEastLoaded() const {
  QMutexLocker locker{&d->lock};
  return d->ne.loaded;
}

void AgentSystemConnector::setPlotterNorthEastLoaded(bool loaded) {
  QMutexLocker locker{&d->lock};
  if (d->ne.loaded != loaded) {
    d->ne.loaded = loaded;
    locker.unlock();
    Q_EMIT plotterNorthEastLoadedChanged();
  }
}

bool AgentSystemConnector::plotterNorthWestLoaded() const {
  QMutexLocker locker{&d->lock};
  return d->nw.loaded;
}

void AgentSystemConnector::setPlotterNorthWestLoaded(bool loaded) {
  QMutexLocker locker{&d->lock};
  if (d->nw.loaded != loaded) {
    d->nw.loaded = loaded;
    locker.unlock();
    Q_EMIT plotterNorthWestLoadedChanged();
  }
}

bool AgentSystemConnector::plotterSouthEastLoaded() const {
  QMutexLocker locker{&d->lock};
  return d->se.loaded;
}

void AgentSystemConnector::setPlotterSouthEastLoaded(bool loaded) {
  QMutexLocker locker{&d->lock};
  if (d->se.loaded != loaded) {
    d->se.loaded = loaded;
    locker.unlock();
    Q_EMIT plotterSouthEastLoadedChanged();
  }
}

bool AgentSystemConnector::plotterSouthWestLoaded() const {
  QMutexLocker locker{&d->lock};
  return d->sw.loaded;
}

void AgentSystemConnector::setPlotterSouthWestLoaded(bool loaded) {
  QMutexLocker locker{&d->lock};
  if (d->sw.loaded != loaded) {
    d->sw.loaded = loaded;
    locker.unlock();
    Q_EMIT plotterSouthWestLoadedChanged();
  }
}

void AgentSystemConnector::print(const QString &sequence) {
  if (d->printCallback) {
    qDebug() << "Sending to bridge: print" << sequence;
    d->printCallback(sequence.toUpper().toStdString());
  }
}

void AgentSystemConnector::installPrintCallback(PrintCallback callback) { d->printCallback = callback; }

AgentSystemConnector *agentSystemConnector() {
  static AgentSystemConnector instance;
  return &instance;
}
