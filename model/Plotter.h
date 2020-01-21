#pragma once

#include <QObject>
#include <QQmlParserStatus>

class Plotter : public QObject, public QQmlParserStatus {
  Q_OBJECT
  Q_INTERFACES(QQmlParserStatus)

public:
  enum Position { NorthEastPos, NorthWestPos, SouthEastPos, SouthWestPos };
  enum Color { Cyan, Magenta, Yellow, Black };
  Q_ENUM(Position)
  Q_ENUM(Color)

  Q_PROPERTY(Plotter::Position position READ position WRITE setPosition NOTIFY positionChanged)
  Q_PROPERTY(Plotter::Color color READ color WRITE setColor NOTIFY colorChanged)

public:
  Plotter::Position position() const { return m_position; }
  Plotter::Color color() const { return m_color; }

  void setPosition(Plotter::Position position) {
    if (m_position != position) {
      m_position = position;
      Q_EMIT positionChanged();
    }
  }

  void setColor(Plotter::Color color) {
    if (m_color != color) {
      m_color = color;
      Q_EMIT colorChanged();
    }
  }

  void classBegin() override;
  void componentComplete() override;

Q_SIGNALS:
  void positionChanged();
  void colorChanged();

private:
  bool m_isComplete{false};
  Plotter::Position m_position;
  Plotter::Color m_color;
};
