#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this);
//    QGridLayout* layout = new QGridLayout;
//    lw = new QListWidget;
//    te = new QTextEdit;
//    layout->addWidget(te, 0, 1)
}

MainWindow::~MainWindow() {
    delete ui;
}
