#include <QApplication>
#include "protodemo.h"

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  app.setQuitOnLastWindowClosed(true);
  ProtoDemo demo;
  demo.show();
  return app.exec();
}
