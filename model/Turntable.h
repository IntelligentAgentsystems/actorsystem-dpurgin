#pragma once

#include <QObject>
#include <QQmlParserStatus>

class Turntable : public QObject, public QQmlParserStatus {
  Q_OBJECT
  Q_INTERFACES(QQmlParserStatus)

public:
  enum Position { WestPos, EastPos };
  enum Direction { NorthDir, SouthDir, WestDir, EastDir };
  Q_ENUM(Position)
  Q_ENUM(Direction)

  Q_PROPERTY(Turntable::Position position READ position WRITE setPosition NOTIFY positionChanged)
  Q_PROPERTY(Turntable::Direction direction READ direction NOTIFY directionChanged)
  Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged)

public:
  Turntable::Position position() const { return m_position; }
  Turntable::Direction direction() const { return m_direction; }
  bool loaded() const { return m_loaded; }

  void setPosition(Turntable::Position position) {
    if (m_position != position && !m_isComplete) {
      m_position = position;
      Q_EMIT positionChanged();
    }
  }

  void classBegin() override;
  void componentComplete() override;

Q_SIGNALS:
  void positionChanged();
  void directionChanged();
  void loadedChanged();

private:
  void setDirection(Turntable::Direction direction) {
    if (m_direction != direction) {
      m_direction = direction;
      Q_EMIT directionChanged();
    }
  }
  void setLoaded(bool loaded) {
    if (m_loaded != loaded) {
      m_loaded = loaded;
      Q_EMIT loadedChanged();
    }
  }

private:
  Turntable::Position m_position{Turntable::EastPos};
  Turntable::Direction m_direction{Turntable::NorthDir};
  bool m_loaded{false};
  bool m_isComplete{false};
};
