#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include "lutfileholder.h"

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

void MainWindow::on_pushButton_open_clicked()
{
    QFileDialog d(this);
    d.setFileMode(QFileDialog::ExistingFile);
    d.exec();
    if(d.result() == QFileDialog::Accepted){
        ui->lineEdit_filename->setText(d.selectedFiles()[0]);
        LUTFileHolder::load_file(d.selectedFiles()[0]);
        ui->label->setText(LUTFileHolder::get_info_string());

        ui->glwidget->initGeom(LUTFileHolder::get_points());
    }
}
