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
    quint16 next_block_size = 0;
private:
    static const quint8 auth_ok = 10;
    static const quint8 auth_not = 11;
    std::unordered_map<QString, QTcpSocket*> names_to_sockets_;
    std::unordered_set<QTcpSocket*> sockets_;
    QStringList clients_list_;
    QByteArray data_;
    void SendToClient(QString client_name);
    int RegNewUser(QString name, QString pass);
    bool AuthNewUser(QString name, QString pass);
public slots:
    void incomingConnection(qintptr socket_descriptor);
    void SlotReadyRead();
};

#endif // SERVER_H
