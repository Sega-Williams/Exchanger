#include <QMessageBox>
#include "registrationwindow.h"
#include "ui_registrationwindow.h"
#include "client.h"

RegistrationWindow::RegistrationWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegistrationWindow)
{
    ui->setupUi(this);
}

RegistrationWindow::~RegistrationWindow()
{
    delete ui;
}

void RegistrationWindow::on_buttonBox_accepted()
{
    QString login = ui->lineEnterLogin->text();
    QString pass = ui->lineEnterPassword->text();
    if(ProcessRegistration(login.toStdString(), pass.toStdString())){
        name = login.toStdString();
        QMessageBox::information(this, "Авторизация", "Вы успешно авторизовались");
        authorization = true;
    }else {
        QMessageBox::warning(this, "Авторизация", "Неправильный логин или пароль");
        authorization = false;
    }
}

void RegistrationWindow::on_buttonBox_rejected()
{
    authorization = false;
}

