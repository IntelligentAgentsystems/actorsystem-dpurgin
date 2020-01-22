#pragma once

#include <QObject>

class Laptop : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString printed READ printed NOTIFY printedChanged)
  Q_PROPERTY(QString leftToPrint READ leftToPrint NOTIFY leftToPrintChanged)

public:
  explicit Laptop(QObject *parent = nullptr);

  Q_INVOKABLE void print(const QString &sequence);

  QString printed() const { return m_printed; }
  QString leftToPrint() const { return m_leftToPrint; }

Q_SIGNALS:
  void printedChanged();
  void leftToPrintChanged();

private:
  QString m_printed;
  QString m_leftToPrint;
};
