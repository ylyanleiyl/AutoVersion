#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../version.h"

#if defined( Compile_WindServer )
#define MAIN_NAME QString( "WinServer" )
#else
#define MAIN_NAME QString( "WinClient" )
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(COMPANY_NAME);

    ui->labelAppVersion->setText(QString("程序版本：%1").arg(FILE_VERSION_DOT));
    ui->labelMainName->setText(QString("宏定义：%1").arg(MAIN_NAME));
}

MainWindow::~MainWindow()
{
    delete ui;
}

