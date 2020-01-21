#pragma once

#include <QObject>

class Laptop : public QObject {
  Q_OBJECT

public:
  Q_INVOKABLE void print(const QString &sequence);
};
