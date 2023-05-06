#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <unordered_map>
#include <unordered_set>

class Server : public QTcpServer {
    Q_OBJECT
public:
    Server();
    QTcpSocket* socket_;

private:
    std::unordered_map<QString, QTcpSocket*> names_to_sockets_;
    std::unordered_set<QTcpSocket*> sockets_;
    QByteArray data_;
    void SendToClient(QString client_name);
public slots:
    void incomingConnection(qintptr socket_descriptor);
    void SlotReadyRead();
};

#endif // SERVER_H
