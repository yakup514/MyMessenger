#include "client.h"
#include "ui_authwindow.h"
#include "ui_mainwindow.h"
#include <QRegExp>
#include <QMessageBox>
#include <QFile>

Client::Client() {

    mw = new MainWindow;
    socket_ = new QTcpSocket;
    mw->statusBar()->showMessage("disconnected");
    mw->ui->disconnect_btn->setEnabled(false);
    mw->show();
    aw = new AuthWindow;
    aw->setModal(true);
    connect(mw->ui->connect_btn, SIGNAL(clicked()), this, SLOT(SlotConnnectButtonClicked()));
    connect(mw->ui->disconnect_btn, SIGNAL(clicked()), this, SLOT(SlotDisconnectButtonClicked()));
    connect(mw->ui->send_btn, SIGNAL(clicked()), this, SLOT(SlotSendButtonClicked()));
    connect(socket_, SIGNAL(readyRead()), this, SLOT(SlotReadyRead()));
    connect(socket_, SIGNAL(connected()), this, SLOT(SlotSocketConnected()));
    connect(socket_, SIGNAL(disconnected()), this, SLOT(SlotSocketDisconnected()));
    connect(aw->ui->auth_button, SIGNAL(clicked()), this, SLOT(SlotAuthButtonClicked()));
    connect(aw->ui->reg_button, SIGNAL(clicked()), this, SLOT(SlotRegButtonClicked()));
    connect(mw->ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(SlotBuddyChoosed(QListWidgetItem*)));
    mw->ui->send_btn->setEnabled(false);
}


void Client::SlotConnnectButtonClicked() {

    socket_->connectToHost(mw->ui->ip_le->text(), mw->ui->port_le->text().toInt());
    mw->statusBar()->showMessage("connecting...");
    if (socket_->waitForConnected(1000)){
        mw->statusBar()->showMessage("connected!");
        mw->ui->connect_btn->setEnabled(false);
        mw->ui->disconnect_btn->setEnabled(true);
    } else {
        mw->ui->connect_btn->setEnabled(true);
        mw->ui->disconnect_btn->setEnabled(false);
        mw->statusBar()->showMessage("could not connect to server");
    }
}

void Client::SlotDisconnectButtonClicked() {
    socket_->disconnectFromHost();

}

void Client::SlotSendButtonClicked() {
   SendToClient(mw->ui->mess_le->text());
   WriteMessageToHistory(mw->ui->mess_le->text(), buddy_name_, true);
   mw->ui->mess_le->clear();
   SetHistory();
}

void Client::SlotSocketConnected() {
    qDebug() << "connected";

    aw->show();
}

void Client::SlotReadyRead() {
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
                if (mess_type == Client::auth_ok) {
                    is_auth = true;
                    QString tmp;
                    in >> tmp;
                    my_name_ = tmp;
                    aw->close();

                } else if (mess_type == Client::auth_not) {
                    QString tmp;
                    in >> tmp;
                    QMessageBox msg_box;
                    msg_box.setText("ошибка авторизации");
                    msg_box.exec();
                }else if (mess_type == Client::send_clients) {
                    GetClientsList(in);
                   // next_block_size = 0;
                } else if (mess_type == Client::reg_ok) {
                    QString tmp;
                    in >> tmp;
                    QMessageBox msg_box;
                    msg_box.setText("вы успешно зарегистрировались, нажмите на кнопку авторизация");
                    msg_box.exec();
                } else if (mess_type == Client::reg_not) {
                    QString tmp;
                    in >> tmp;
                    QMessageBox msg_box;
                    msg_box.setText("имя клиента занято");
                    msg_box.exec();
                } else if (mess_type == Client::send_msg) {
                    QString from, msg;
                    in >> from >> msg;
                    WriteMessageToHistory(msg, from, false);
                    SetHistory();
                    names_to_items_[from]->setBackground(Qt::blue);
                }
//                QString str;
//                in >> str;
//                ui->textBrowser->append(str);
                next_block_size = 0;
            } else {
                mw->ui->textBrowser->append("read error");
            }
        }
    }
}

void Client::SendToServer(QString str, quint8 mess_type){
    data_.clear();
    QDataStream out(&data_, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    out << quint16(0) << mess_type << my_name_<< str;
    out.device()->seek(0);
    out <<quint16(data_.size() - sizeof(quint16));
    socket_->write(data_);
}

void Client::SendToClient(QString msg) {
    data_.clear();
    QDataStream out(&data_, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    out << quint16(0) << Client::send_msg << my_name_<< buddy_name_<< msg;
    out.device()->seek(0);
    out <<quint16(data_.size() - sizeof(quint16));
    socket_->write(data_);
}

void Client::GetClientsList(QDataStream& in) {
    mw->ui->listWidget->clear();
    quint32 size;
    in >> size;
    int ind = 0;
    for (quint32 i = 0; i < size; ++i) {
        QString name;
        bool is_online;
        in >> name >> is_online;
        if (name != my_name_) {
        mw->ui->listWidget->addItem(name);
        names_to_items_[name] = mw->ui->listWidget->item(ind);
        is_online ? mw->ui->listWidget->item(ind++)->setForeground(Qt::green) :
                    mw->ui->listWidget->item(ind++)->setForeground(Qt::red);
        }
    }
}

bool Client::CheckName(const QString &name) {
    QRegExp rx("^[a-zA-Z0-9]+$");
    return rx.exactMatch(name) && name.size() > 3;
}

bool Client::CheckPass(const QString &pass) {
    return pass.size() > 3;
}

int Client::WriteMessageToHistory(QString msg, QString name, bool is_my_msg) {
    int result = Client::file_err;
    QFile file(name);
    if(file.open(QIODevice::Append| QIODevice::Text)) {
        QTextStream write_stream(&file);
        if (is_my_msg) {
            write_stream << my_name_ << ":" << msg<<"\n";
        } else {
            write_stream << name << ":" << msg<<"\n";
        }
        file.close();
        result = Client::msg_ok;
    }
    return result;
}

void Client::SetHistory() {
    QFile file(buddy_name_);
    qDebug() << "set " << buddy_name_;
    mw->ui->textBrowser->clear();
    if(file.open(QIODevice::ReadOnly| QIODevice::Text)) {
        QTextStream read_stream(&file);
        QString temp;
        while(1) {
        read_stream >> temp;
        if (temp.isNull()) {
            break;
        }
        mw->ui->textBrowser->append(temp);
        }
        file.close();
    }
}

void Client::SlotAuthButtonClicked() {
    if (aw->ui->name_le->text().size() != 0) {
        my_name_ = aw->ui->name_le->text();
        SendToServer(aw->ui->pass_le->text(), Client::auth_mes);
    }
}

void Client::SlotRegButtonClicked() {
    if (CheckName(aw->ui->name_le->text()) &&
        CheckPass(aw->ui->name_le->text())) {
        my_name_ = aw->ui->name_le->text();
        SendToServer(aw->ui->pass_le->text(), Client::reg_mes);
    } else {
        QMessageBox msg_box;
        msg_box.setText("некорректные данные:\n-имя должно содеражать латинские "
                        "буквы и цифры и быть длиннее 3-х символов\n-пароль "
                        "должен быть длиннее 3-х символов");
        msg_box.exec();
    }
}

void Client::SlotBuddyChoosed(QListWidgetItem* item) {
    buddy_name_ = item->text();
    SetHistory();
    mw->ui->send_btn->setEnabled(true);
    item->setBackground(Qt::white);
}

void Client::SlotSocketDisconnected() {
    QMessageBox msg_box;
    msg_box.setText("отключение от сервера, для переподключения нажмите connect");
    msg_box.exec();
    mw->ui->listWidget->clear();
    mw->ui->textBrowser->clear();
    mw->statusBar()->showMessage("disconnected");
    mw->ui->connect_btn->setEnabled(true);
    mw->ui->disconnect_btn->setEnabled(false);
    is_auth = false;
}
