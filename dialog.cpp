#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QString msg,QWidget *parent,bool exitProgram) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->label->setText(msg);
    this->exitProgram=exitProgram;
    //去掉对话框的问号并置顶
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    show();

}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    delete ui;
    if(exitProgram) //如果需要退出程序
        exit(-1);
}
