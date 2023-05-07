#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "auth_window.h"
#include "ui_authwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    aw = new AuthWindow(this);
    aw->setModal(true);
    socket_ = new QTcpSocket;
    connect(ui->connect_btn, SIGNAL(clicked()), this, SLOT(SlotConnnectButtonClicked()));
    connect(ui->send_btn, SIGNAL(clicked()), this, SLOT(SlotSendButtonClicked()));
    connect(socket_, SIGNAL(readyRead()), this, SLOT(SlotReadyRead()));
    connect(socket_, SIGNAL(connected()), this, SLOT(SlotSocketConnected()));
    connect(socket_, SIGNAL(disconnected()), socket_, SLOT(deleteLater()));
    connect(aw->ui->auth_button, SIGNAL(clicked()), this, SLOT(SlotAuthButtonClicked()));
    connect(aw->ui->reg_button, SIGNAL(clicked()), this, SLOT(SlotRegButtonClicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SlotConnnectButtonClicked() {
    socket_->connectToHost("127.0.0.1", 1234);
    if (socket_->waitForConnected(-1))
        {
            qDebug("Connected!");
        }
}

void MainWindow::SlotSendButtonClicked() {
    SendToServer(ui->lineEdit->text(), 22);
}

void MainWindow::SlotSocketConnected() {
    qDebug() << "connected";

    aw->show();
}

void MainWindow::SlotReadyRead() {
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
                QString str;
                in >> str;
                ui->textBrowser->append(str);
                next_block_size = 0;
            } else {
                ui->textBrowser->append("read error");
            }
        }
    }
}

void MainWindow::SendToServer(QString str, quint8 mess_type){
    data_.clear();
    QDataStream out(&data_, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    out << quint16(0) << mess_type << name_<< str;
    out.device()->seek(0);
    out <<quint16(data_.size() - sizeof(quint16));
    socket_->write(data_);
    qDebug() << data_;
}

void MainWindow::SlotAuthButtonClicked() {
    if (aw->ui->name_le->text().size() != 0) {
        name_ = aw->ui->name_le->text();
        SendToServer(aw->ui->pass_le->text(), MainWindow::auth_mes);
    }
}

void MainWindow::SlotRegButtonClicked() {
    if (aw->ui->name_le->text().size() != 0) {
        name_ = aw->ui->name_le->text();
        SendToServer(aw->ui->pass_le->text(), MainWindow::reg_mes);
    }
}

