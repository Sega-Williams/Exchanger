#include "client.h"
#include "myquotes.h"
#include "ui_myquotes.h"
#include <QMessageBox>

MyQuotes::MyQuotes(std::string aId, std::string aType,
                   std::string aValue, std::string aPrice, std::string aStatus, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MyQuotes)
{
    ui->setupUi(this);
    id = aId;
    ui->labelQuoteId->setText(QString::fromStdString(aId));
    ui->labelQuoteType->setText(QString::fromStdString(aType));
    ui->labelQuoteValue->setText(QString::fromStdString(aValue));
    ui->labelQuotePrice->setText(QString::fromStdString(aPrice));
    if(aStatus == "executed") delete ui->RejectButton;
}

MyQuotes::~MyQuotes()
{
    delete ui;
}

void MyQuotes::on_AcceptButton_clicked()
{
    close();
}


void MyQuotes::on_RejectButton_clicked()
{
    RejectBid(id);
    QMessageBox::information(this, "Успешно", "Ваша заявка отменена");
    close();
}

