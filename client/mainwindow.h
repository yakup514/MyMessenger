#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "auth_window.h"
#include <QMainWindow>
#include <QTcpSocket>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    static const quint8 auth_mes = 1;
    static const quint8 auth_ok = 10;
    static const quint8 auth_not = 11;
    static const quint8 reg_mes = 2;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void SlotConnnectButtonClicked();
    void SlotSendButtonClicked();
    void SlotSocketConnected();
    void SlotAuthButtonClicked();
    void SlotRegButtonClicked();
public slots:
    void SlotReadyRead();
private:
    Ui::MainWindow *ui;
    AuthWindow* aw;
    QTcpSocket *socket_;
    QByteArray data_;
    void SendToServer(QString str, quint8 mess_type);
    quint16 next_block_size;
    QString name_;
    bool is_auth = false;
};
#endif // MAINWINDOW_H
