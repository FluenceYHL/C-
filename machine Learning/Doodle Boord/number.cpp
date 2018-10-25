#include "number.h"
#include "ui_number.h"
#include <QMessageBox>

number::number(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::number)
{
    ui->setupUi(this);
}

number::~number()
{
    delete ui;
    ui = nullptr;
}

void number::on_pushButton_clicked()
{
    auto content = ui->lineEdit->text ();
    if(content.isEmpty ()) {
        QMessageBox::warning (this, "警告", "不能为空", QMessageBox::Yes);
        return;
    }
    bool flag = true;
    auto _number = content.toInt (&flag, 10);
    if(_number < 0 || _number > 9) {
        QMessageBox::warning (this, "警告", "数字非法", QMessageBox::Yes);
        return;
    }
    emit this->send_number (_number);
    this->hide ();
    ui->lineEdit->clear ();
}
