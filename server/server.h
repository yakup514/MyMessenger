#ifndef SERVER_H
#define SERVER_H
#include "mainwindow.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <unordered_map>
#include <unordered_set>
/*!
 * Класс для работы с матрицами, реализована возможность арифметических
 * операций, сравнения, базовых преобразований матриц. \brief Класс S21Matrix.
 * \author Shelacor \version 1.0 \date Март 2022 года \param rows_ Количество
 * строк в матрице. \param auth_mes Количество столбцов в матрице. \param matrix_
 * Двумерный массив для хранения матрицы в памяти.
 * \param ACCURACY 1e-7  Используемая точность при сравнении и вычислениях.
 *
 */
class Server : public QTcpServer {
    Q_OBJECT
public:
    /**
     * @brief конструктор ксласса
     */
    Server();

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
    void WriteMessageToHistory(QString msg, QString from, QString to);
    uint next_block_size = 0;
    QTcpSocket* socket_;



private slots:
    void incomingConnection(qintptr socket_descriptor);
    void SlotReadyRead();
    void SlotClientDisconected();
    void SlotStartButtonClicked();
};

#endif // SERVER_H
