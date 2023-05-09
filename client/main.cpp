
#include "client.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Client c;

    return a.exec();
}
