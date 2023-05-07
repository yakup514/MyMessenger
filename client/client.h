#ifndef CLIENT_H
#define CLIENT_H
#include <QTcpSocket>

class Client {
public:
    Client();

    QTcpSocket *socket_;
    QByteArray data_;
public slots:
    void SlotConnectButtonClicked();
};

#endif // CLIENT_H
