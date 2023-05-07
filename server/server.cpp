#include "server.h"
#include <QFile>
#include <QTextStream>

Server::Server() {
    if (listen(QHostAddress::Any, 1234)) {
        qDebug() << "start";
    } else {
        qDebug() << "error";
    }

    qDebug() << isListening();
}

int Server::RegNewUser(QString name, QString pass) {
    QFile file("reg_inf");
    int result = -1;
    if (!file.exists()) {
        if(file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream write_stream(&file);
            write_stream << name << " " << pass << "\n";
            result = 0;
            file.close();
        }
    } else {
        if(file.open(QIODevice::ReadOnly| QIODevice::Text)) {
            QTextStream read_stream(&file);
            QString temp;
            while(1) {
            read_stream >> temp;
            if (temp.isNull()) {
                result = 0;
                break;
            } else if (temp == name) {
                result = 1;
                break;
            }
            read_stream >> temp;
            }
            file.close();
            if (result == 0) {
                if(file.open(QIODevice::Append | QIODevice::Text)) {
                    QTextStream write_stream(&file);
                    write_stream << name << " " << pass<<"\n";
                    result = 0;
                    file.close();
                } else {
                    result = -1;
                }
            }
        }
    }
    return result;
}

bool Server::AuthNewUser(QString name, QString pass) {
    QFile file("reg_inf");
    bool result = false;
    if (file.exists()) {
        if(file.open(QIODevice::ReadOnly| QIODevice::Text)) {
            QTextStream read_stream(&file);
            QString temp_name, temp_pass;
            while(1) {
            read_stream >> temp_name >> temp_pass;
            if (temp_name.isNull() || temp_pass.isNull()) {
                break;
            } else if (temp_name == name && temp_pass == pass) {
                result = true;
                break;
            }
            }
        }
    }
    return result;
}

void Server::incomingConnection(qintptr socket_descriptor) {
    socket_ = new QTcpSocket;
    socket_->setSocketDescriptor(socket_descriptor);
    connect(socket_, SIGNAL( readyRead()), this, SLOT(SlotReadyRead()));
    sockets_.insert(socket_);
    qDebug() << "client connected" << socket_descriptor;
}

void Server::SlotReadyRead() {
    socket_ = (QTcpSocket*) sender();
    QDataStream in(socket_);
    in.setVersion(QDataStream::Qt_6_4);
    if (in.status() == QDataStream::Ok) {
        while(1) {
            if (next_block_size == 0) {
                if (socket_->bytesAvailable() < 2) {
                    break;
                }
                in >> next_block_size;
                if (socket_->bytesAvailable() < next_block_size) {
                    break;
                }
                quint8 mess_type;
                in >>mess_type;
                if (mess_type == 1) {
                    QString name, pass;
                    in >> name >> pass;
                    int res = AuthNewUser(name, pass);
                    if (res) {
                        names_to_sockets_[name] = socket_;
                        clients_list_.append(name);
                    }
                    qDebug() << res;
                } else if (mess_type == 2) {
                    QString name, pass;
                    in >> name >> pass;
                    int res = RegNewUser(name, pass);
                    if (res == 1) {
                        names_to_sockets_[name] = socket_;
                        clients_list_.append(name);
                    }
                    qDebug() << res;
                }
                //qDebug() <<from << str;
                next_block_size = 0;
            } else {

            }
        }
    }
}

