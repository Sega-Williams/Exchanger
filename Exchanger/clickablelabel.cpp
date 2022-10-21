#include "clickablelabel.h"
#include "myquotes.h"

ClickableLabel::ClickableLabel(QWidget* parent, std::string aId, std::string aType,
                               std::string aValue, std::string aPrice, std::string aStatus, Qt::WindowFlags f)
    : QLabel(parent) {

    id = aId;
    type = aType;
    value = aValue;
    price = aPrice;
    status = aStatus;

    std::string message;
    message = "№: " + id
            + " Объём: " + value
            + " Цена: " + price;
    this->setBaseSize(35,35);
    this->setMinimumHeight(35);
    this->setMaximumHeight(35);
    if(type == "sell") this->setStyleSheet("QLabel { background-color: rgba(248, 189, 196, 1) ; color : black; }");
    else if(type == "buy") this->setStyleSheet("QLabel { background-color: rgba(222, 246, 202, 1) ; color : black; }");
    this->setText(QString::fromStdString(message));
    this->setObjectName(QString::fromStdString("ActiveQuery_" + id));

}

ClickableLabel::~ClickableLabel() {}

void ClickableLabel::mousePressEvent(QMouseEvent* event) {
    emit clicked();
    MyQuotes* quoteWindow = new MyQuotes(id,type,value,price,status);
    quoteWindow->exec();
}
