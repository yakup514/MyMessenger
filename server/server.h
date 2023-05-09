#ifndef SERVER_H
#define SERVER_H
#include "mainwindow.h"
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
    static const quint8 auth_mes = 1;
    static const quint8 auth_ok = 10;
    static const quint8 auth_not = 11;
    static const quint8 reg_mes = 2;
    static const quint8 reg_ok = 20;
    static const quint8 reg_not = 21;
    static const quint8 send_clients = 3;
    static const quint8 send_his = 4;
    static const quint8 file_err = 5;
    static const quint8 client_alreday_exist_err = 6;
    static const quint8 send_msg = 7;
    MainWindow* mw;
    std::unordered_map<QString, QTcpSocket*> names_to_sockets_;
    std::unordered_map<QTcpSocket*,QString> sockets_to_names_;
    std::unordered_map<QString, bool> clients_list_;
    bool is_auth = false;
    QByteArray data_;
    void MakeClientList();
    void SetClientList();
    int RegNewUser(QString name, QString pass);
    bool AuthNewUser(QString name, QString pass);
    void AutorizationRequest(QDataStream& in, QTcpSocket* socket);
    void RegistrationRequest(QDataStream& in, QTcpSocket* socket);
    void SendClientsList(QTcpSocket* socket);
    void SendToClient(QTcpSocket* socket, QString str, quint8 mess_type);
    void SendMsgToClient(QTcpSocket *socket, QString from, QString msg);


private slots:
    void incomingConnection(qintptr socket_descriptor);
    void SlotReadyRead();
    void SlotClientDisconected();
};

#endif // SERVER_H
