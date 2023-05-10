#ifndef CLIENT_H
#define CLIENT_H
#include "auth_window.h"
#include "mainwindow.h"
#include <QTcpSocket>
#include <unordered_map>

class Client : public QObject{
    Q_OBJECT
public:
    static const quint8 msg_ok = 0;
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
public:
    Client();
private slots:
    void SlotConnnectButtonClicked();
    void SlotDisconnectButtonClicked();
    void SlotSendButtonClicked();
    void SlotSocketConnected();
    void SlotAuthButtonClicked();
    void SlotRegButtonClicked();
    void SlotBuddyChoosed(QListWidgetItem*);
    void SlotSocketDisconnected();

public slots:
    void SlotReadyRead();
private:
    AuthWindow* aw;
    MainWindow* mw;
    QTcpSocket *socket_;
    QByteArray data_;
    void SendToServer(QString str, quint8 mess_type);
    void SendToClient(QString msg);
    uint next_block_size = 0;
    QString my_name_;
    QString buddy_name_;
    bool is_auth = false;
    void GetClientsList(QDataStream& in);
    bool CheckName(const QString& name);
    bool CheckPass(const QString& pass);
    void WriteMessageToHistory(QString msg, QString name, bool is_my_msg);
    void SetHistory();
    std::unordered_map<QString, QListWidgetItem*> names_to_items_;
    std::unordered_map<QString, bool> clients_list_;
};

#endif // CLIENT_H
