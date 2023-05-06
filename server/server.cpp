#include "server.h"


Server::Server() {
    if (listen(QHostAddress::Any, 2323)) {
        qDebug() << "start";
    } else {
        qDebug() << "error";
    }
}

void Server::incomingConnection(qintptr socket_descriptor) {
    socket_ = new QTcpSocket;
    socket_->setSocketDescriptor(socket_descriptor);
    connect(socket_, SIGNAL(readyRead()), this, SLOT(SlotReadyRead()));
    sockets_.insert(socket_);
    qDebug() << "client connected" << socket_descriptor;
}

void Server::SlotReadyRead() {

}
