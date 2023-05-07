#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "server.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    Server s;
}

MainWindow::~MainWindow()
{
    delete ui;
}

