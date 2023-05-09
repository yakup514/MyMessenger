#include "server.h"
#include <QFile>
#include <QTextStream>
#include "ui_mainwindow.h"

Server::Server() {
    mw = new MainWindow;
    mw->show();
    MakeClientList();
    SetClientList();

    for (auto& p : clients_list_) {
        qDebug() << p.first << ' ' << p.second;
    }
    if (listen(QHostAddress::Any, 1234)) {
        qDebug() << "start";
    } else {
        qDebug() << "error";
    }


}

void Server::MakeClientList() {
    QFile file("reg_inf");
    if(file.open(QIODevice::ReadOnly| QIODevice::Text)) {
        QTextStream read_stream(&file);
        QString temp;
        while(1) {
            read_stream >> temp;
            if (temp.isNull()) {
                break;
            } else {
                clients_list_[temp];
            }
            read_stream >> temp;
        }
        file.close();
    }
}

void Server::SetClientList() {
    mw->ui->listWidget->clear();
    int ind = 0;
    for (auto &p : clients_list_) {
        mw->ui->listWidget->addItem(p.first);
        p.second ? mw->ui->listWidget->item(ind++)->setForeground(Qt::green)
                  : mw->ui->listWidget->item(ind++)->setForeground(Qt::red);
    }
}



int Server::RegNewUser(QString name, QString pass) {
    QFile file("reg_inf");
    int result = Server::file_err;
    if (!file.exists()) {
        if(file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream write_stream(&file);
            write_stream << name << " " << pass << "\n";
            result = Server::reg_ok;
            file.close();
        }
    } else {
        if(file.open(QIODevice::ReadOnly| QIODevice::Text)) {
            QTextStream read_stream(&file);
            QString temp;
            while(1) {
            read_stream >> temp;
            if (temp.isNull()) {
                result = Server::reg_ok;
                break;
            } else if (temp == name) {
                result = Server::client_alreday_exist_err;
                break;
            }
            read_stream >> temp;
            }
            file.close();
            if (result == Server::reg_ok) {
                if(file.open(QIODevice::Append | QIODevice::Text)) {
                    QTextStream write_stream(&file);
                    write_stream << name << " " << pass<<"\n";
                    result = Server::reg_ok;
                    file.close();
                } else {
                    result = Server::file_err;
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
        file.close();
    }
    return result;
}

void Server::AutorizationRequest(QDataStream &in, QTcpSocket* socket) {
    QString name, pass;
    in >> name >> pass;
    int res = AuthNewUser(name, pass);
    if (res && !clients_list_[name]) {
        SendToClient(socket, name, Server::auth_ok);
        names_to_sockets_[name] = socket;
        clients_list_[name] = true;
        sockets_to_names_[socket] = name;
        for (auto& p : names_to_sockets_) {
            SendClientsList(p.second);
        }
        SetClientList();
    } else {
        SendToClient(socket, " ", Server::auth_not);
    }
    qDebug() << res;
}

void Server::RegistrationRequest(QDataStream &in, QTcpSocket* socket) {
    QString name, pass;
    in >> name >> pass;
    int res = RegNewUser(name, pass);
    if (res == Server::reg_ok) {
        clients_list_[name] = false;
        SetClientList();
        SendToClient(socket, " ", Server::reg_ok);
    } else {
        SendToClient(socket, " ", Server::reg_not);
    }
    qDebug() << res;
}

void Server::SendClientsList(QTcpSocket* socket) {
    data_.clear();
    QDataStream out(&data_, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    out << quint16(0) << Server::send_clients << (quint32)clients_list_.size();
    for (auto& p : clients_list_) {
        out << p.first << p.second;
    }
    out.device()->seek(0);
    out <<quint16(data_.size() - sizeof(quint16));
    socket->write(data_);
}

void Server::SendToClient(QTcpSocket *socket, QString str, quint8 mess_type){
    data_.clear();
    QDataStream out(&data_, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    out << quint16(0) << mess_type << str;
    out.device()->seek(0);
    out <<quint16(data_.size() - sizeof(quint16));
    socket->write(data_);
}

void Server::SendMsgToClient(QTcpSocket *socket, QString from, QString msg) {
    data_.clear();
    QDataStream out(&data_, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    out << quint16(0) << Server::send_msg << from<< msg;
    out.device()->seek(0);
    out <<quint16(data_.size() - sizeof(quint16));
    socket->write(data_);
}



void Server::incomingConnection(qintptr socket_descriptor) {
    socket_ = new QTcpSocket;
    socket_->setSocketDescriptor(socket_descriptor);
    connect(socket_, SIGNAL( readyRead()), this, SLOT(SlotReadyRead()));
    connect(socket_, SIGNAL(disconnected()), this, SLOT(SlotClientDisconected()));
    connect(socket_, SIGNAL(disconnected()), socket_, SLOT(deleteLater()));

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
                if (mess_type == Server::auth_mes) {
                    AutorizationRequest(in, socket_);
                } else if (mess_type == Server::reg_mes) {
                    RegistrationRequest(in, socket_);
                } else if (mess_type == Server::send_msg) {
                    QString from, to, msg;
                    in >> from >> to >> msg;
                    if (names_to_sockets_.at(to)) {
                        SendMsgToClient(names_to_sockets_[to], from, msg);
                    }
                }
                next_block_size = 0;
            } else {

            }
        }
    }
}

void Server::SlotClientDisconected() {
    auto it = sockets_to_names_.find((QTcpSocket*)sender());
    if ( it !=sockets_to_names_.end()) {
        names_to_sockets_.erase(it->second);
        clients_list_[it->second] = false;
        for (auto& p : names_to_sockets_) {
            SendClientsList(p.second);
        }
        SetClientList();
    }
}

