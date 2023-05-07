#include "client.h"

Client::Client() {
    socket_ = new QTcpSocket;
}


void Client::SlotConnectButtonClicked() {
    socket_->connectToHost("127.0.0.1", 2323);
}
