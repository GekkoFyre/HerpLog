#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button_create_new_db_clicked()
{}

void MainWindow::on_button_open_db_clicked()
{}

void MainWindow::on_button_exit_clicked()
{}
