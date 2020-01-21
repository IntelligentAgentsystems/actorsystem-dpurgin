#include "agentsystem.h"

#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QThread>
#include <QTimer>

#include "agentsystemconnector.h"
#include "model/Laptop.h"
#include "model/Plotter.h"
#include "model/Turntable.h"

int main(int argc, char *argv[]) {  
  QGuiApplication app{argc, argv};

  QQmlApplicationEngine engine;
  engine.addImportPath(":/qml");

  std::thread t{run_agent_system, argc, argv, agentSystemConnector()};

  qmlRegisterType<Turntable>("AGS.Model", 1, 0, "Turntable");
  qmlRegisterType<Plotter>("AGS.Model", 1, 0, "Plotter");
  qmlRegisterType<Laptop>("AGS.Model", 1, 0, "Laptop");

  engine.load(":/qml/Main.qml");

  return app.exec();
}
