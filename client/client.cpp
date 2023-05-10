#include "client.h"

#include <QCryptographicHash>
#include <QFile>
#include <QMessageBox>
#include <QRegExp>

#include "ui_authwindow.h"
#include "ui_mainwindow.h"

Client::Client() {
  mw = new MainWindow;
  socket_ = new QTcpSocket;
  mw->statusBar()->showMessage("disconnected");
  mw->ui->disconnect_btn->setEnabled(false);
  mw->show();
  aw = new AuthWindow;
  aw->setModal(true);
  connect(mw->ui->connect_btn, SIGNAL(clicked()), this,
          SLOT(SlotConnnectButtonClicked()));
  connect(mw->ui->disconnect_btn, SIGNAL(clicked()), this,
          SLOT(SlotDisconnectButtonClicked()));
  connect(mw->ui->send_btn, SIGNAL(clicked()), this,
          SLOT(SlotSendButtonClicked()));
  connect(socket_, SIGNAL(readyRead()), this, SLOT(SlotReadyRead()));
  connect(socket_, SIGNAL(connected()), this, SLOT(SlotSocketConnected()));
  connect(socket_, SIGNAL(disconnected()), this,
          SLOT(SlotSocketDisconnected()));
  connect(aw->ui->auth_button, SIGNAL(clicked()), this,
          SLOT(SlotAuthButtonClicked()));
  connect(aw->ui->reg_button, SIGNAL(clicked()), this,
          SLOT(SlotRegButtonClicked()));
  connect(mw->ui->listWidget, SIGNAL(itemClicked(QListWidgetItem *)), this,
          SLOT(SlotBuddyChoosed(QListWidgetItem *)));
  mw->ui->send_btn->setEnabled(false);
}

void Client::SlotConnnectButtonClicked() {
  socket_->connectToHost(mw->ui->ip_le->text(),
                         mw->ui->port_le->text().toInt());
  mw->statusBar()->showMessage("connecting...");
  if (socket_->waitForConnected(1000)) {
    mw->statusBar()->showMessage("connected!");
    mw->ui->connect_btn->setEnabled(false);
    mw->ui->disconnect_btn->setEnabled(true);
  } else {
    mw->ui->connect_btn->setEnabled(true);
    mw->ui->disconnect_btn->setEnabled(false);
    mw->statusBar()->showMessage("could not connect to server");
  }
}

void Client::SlotDisconnectButtonClicked() { socket_->disconnectFromHost(); }

void Client::SlotSendButtonClicked() {
  SendToClient(mw->ui->mess_le->text());
  WriteMessageToHistory(mw->ui->mess_le->text(), buddy_name_, true);
  mw->ui->mess_le->clear();
  SetHistory();
}

void Client::SlotSocketConnected() { aw->show(); }

void Client::SlotReadyRead() {
  QDataStream in(socket_);
  in.setVersion(QDataStream::Qt_6_4);
  if (in.status() == QDataStream::Ok) {
    while (1) {
      if (next_block_size == 0) {
        if (socket_->bytesAvailable() < 2) {
          break;
        }
        in >> next_block_size;
        if (socket_->bytesAvailable() < next_block_size) {
          break;
        }
        quint8 mess_type;
        in >> mess_type;
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
        } else if (mess_type == Client::send_clients) {
          GetClientsList(in);
          // next_block_size = 0;
        } else if (mess_type == Client::reg_ok) {
          QString tmp;
          in >> tmp;
          QMessageBox msg_box;
          msg_box.setText(
              "вы успешно зарегистрировались, нажмите на кнопку авторизация");
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
          if (!names_to_items_[from]->isSelected()) {
            names_to_items_[from]->setBackground(Qt::blue);
          }
        }
        next_block_size = 0;
      } else {
        mw->ui->textBrowser->append("read error");
      }
    }
  }
}

void Client::SendToServer(QString str, quint8 mess_type) {
  data_.clear();
  QDataStream out(&data_, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_6_4);
  out << uint(0) << mess_type << my_name_ << str;
  out.device()->seek(0);
  out << uint(data_.size() - sizeof(uint));
  socket_->write(data_);
}

void Client::SendToClient(QString msg) {
  data_.clear();
  QDataStream out(&data_, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_6_4);
  out << uint(0) << Client::send_msg << my_name_ << buddy_name_ << msg;
  out.device()->seek(0);
  out << uint(data_.size() - sizeof(uint));
  socket_->write(data_);
}

void Client::GetClientsList(QDataStream &in) {
  mw->ui->listWidget->clear();
  quint32 size;
  in >> size;
  int ind = 0;
  for (quint32 i = 0; i < size; ++i) {
    QString name;
    bool is_online;
    in >> name >> is_online;
    if (name != my_name_) {
      clients_list_[name] = is_online;
      mw->ui->listWidget->addItem(name);
      names_to_items_[name] = mw->ui->listWidget->item(ind);
      is_online ? mw->ui->listWidget->item(ind++)->setForeground(Qt::green)
                : mw->ui->listWidget->item(ind++)->setForeground(Qt::red);
      if (!buddy_name_.isNull() && buddy_name_ == name && !is_online) {
        mw->ui->send_btn->setEnabled(false);
      } else if (!buddy_name_.isNull() && buddy_name_ == name && is_online) {
        mw->ui->send_btn->setEnabled(true);
      }
    }
  }
}

bool Client::CheckName(const QString &name) {
  QRegExp rx("^[a-zA-Z0-9]+$");
  return rx.exactMatch(name) && name.size() > 3;
}

bool Client::CheckPass(const QString &pass) { return pass.size() > 3; }

void Client::WriteMessageToHistory(QString msg, QString name, bool is_my_msg) {
  QFile file(my_name_ + "-" + name);
  if (file.open(QIODevice::Append | QIODevice::Text)) {
    QTextStream write_stream(&file);
    if (is_my_msg) {
      write_stream << "me"
                   << ":" << msg << "\n";
    } else {
      write_stream << name << ":" << msg << "\n";
    }
    file.close();
  }
}

void Client::SetHistory() {
  QFile file(my_name_ + "-" + buddy_name_);
  mw->ui->textBrowser->clear();
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream read_stream(&file);
    QString temp;
    while (1) {
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
    QString hash_pass = (QString)(QString(
        QCryptographicHash::hash(aw->ui->pass_le->text().toUtf8(),
                                 QCryptographicHash::Sha1)
            .toHex()));
    SendToServer(hash_pass, Client::auth_mes);
  }
}

void Client::SlotRegButtonClicked() {
  if (CheckName(aw->ui->name_le->text()) &&
      CheckPass(aw->ui->pass_le->text())) {
    QString hash_pass = (QString)(QString(
        QCryptographicHash::hash(aw->ui->pass_le->text().toUtf8(),
                                 QCryptographicHash::Sha1)
            .toHex()));
    my_name_ = aw->ui->name_le->text();
    SendToServer(hash_pass, Client::reg_mes);
  } else {
    QMessageBox msg_box;
    msg_box.setText(
        "некорректные данные:\n-имя должно содеражать латинские "
        "буквы и цифры и быть длиннее 3-х символов\n-пароль "
        "должен быть длиннее 3-х символов");
    msg_box.exec();
  }
}

void Client::SlotBuddyChoosed(QListWidgetItem *item) {
  buddy_name_ = item->text();
  SetHistory();
  mw->ui->send_btn->setEnabled(true);
  item->setBackground(Qt::white);
  if (clients_list_[buddy_name_]) {
    mw->ui->send_btn->setEnabled(true);
  } else {
    mw->ui->send_btn->setEnabled(false);
  }
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
