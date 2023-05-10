
#include <QApplication>

#include "client.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  Client c;

  return a.exec();
}
