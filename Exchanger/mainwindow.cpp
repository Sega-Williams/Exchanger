#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include "client.h"
#include <limits>

bool authorization;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    RegistrationWindow* regWindow = new RegistrationWindow();
    regWindow->exec();

    if(authorization) ui->setupUi(this);

//    ui->scrollAreaActiveQuotes->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateLabels);
    updateLabels();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateQuotesWindows(){
//    // Очистка окна выполненных заявок перед обновлением
//    while (QLayoutItem* item = ui->LayoutExecQuotes->takeAt(0)) {
//        delete item->widget();
//        delete item;
//    }


    std::cout << "Окно выполненных заявок пользователя" << std::endl;
    // Окно выполненных заявок пользователя
    if(!pullExecBids.empty()){
        for (auto&& push : pullExecBids) { //берем результаты запроса
            ClickableLabel *newLabel = new ClickableLabel(this, push.at(0), push.at(1), push.at(2), push.at(3), push.at(4));
            pullExecBids.erase(std::find(pullExecBids.begin(), pullExecBids.end(), push));
            ui->LayoutExecQuotes->addWidget(newLabel,0);
        }
    }

    std::cout << "Удаление исполненных заявок из пулла активных заявок" << std::endl;
    // Удаление исполненных заявок из пулла активных заявок
    for(auto&& activeId : idsActiveBids){
        for(auto&& execId : idsExecBids){
            if(activeId == execId){
                for(auto&& activeBid : pullActiveBids){
                    if(activeId == activeBid.at(0))
                        pullActiveBids.erase(std::find(pullActiveBids.begin(), pullActiveBids.end(), activeBid));
                }
            }
        }
    }


    std::cout << "Очистка окна активных заявок перед обновлением" << std::endl;
    // Очистка окна активных заявок перед обновлением
    while (QLayoutItem* item = ui->LayoutActiveQuotes->takeAt(0)) {
        delete item->widget();
        delete item;
    }


    std::cout << "Окно активных заявок пользователя" << std::endl;
    // Окно активных заявок пользователя
    if(!pullActiveBids.empty()){
        for (auto&& push : pullActiveBids) { //берем результаты запроса
            ClickableLabel *newLabel = new ClickableLabel(this, push.at(0), push.at(1), push.at(2), push.at(3), push.at(4));
            ui->LayoutActiveQuotes->addWidget(newLabel,0);
        }
    }
}

void MainWindow::updateLabels(){
    // Обновление пользовательских данных
    ui->labelId->setText(QString::fromStdString(id));
    ui->labelShowName->setText(QString::fromStdString(name));
    ui->labelShowRub->setText(QString::fromStdString(rub));
    ui->labelShowUsd->setText(QString::fromStdString(usd));


    std::cout << "quotes.size()= " << quotes.size() << std::endl;
    // Обновление котировок
    ui->buyQuote_1->setText(QString::fromStdString(quotes.at(0)));
    ui->buyQuote_2->setText(QString::fromStdString(quotes.at(1)));
    ui->buyQuote_3->setText(QString::fromStdString(quotes.at(2)));
    ui->buyQuote_4->setText(QString::fromStdString(quotes.at(3)));
    ui->buyQuote_5->setText(QString::fromStdString(quotes.at(4)));
    ui->sellQuote_1->setText(QString::fromStdString(quotes.at(5)));
    ui->sellQuote_2->setText(QString::fromStdString(quotes.at(6)));
    ui->sellQuote_3->setText(QString::fromStdString(quotes.at(7)));
    ui->sellQuote_4->setText(QString::fromStdString(quotes.at(8)));
    ui->sellQuote_5->setText(QString::fromStdString(quotes.at(9)));

    updateQuotesWindows();

    // Частота обновления данных
    timer->start(5000);
}

void MainWindow::on_buyButton_clicked()
{
    if(!ui->buyValue->text().isEmpty() && !ui->buyCost->text().isEmpty()){
        SendBid("buy", ui->buyValue->text().toStdString(), ui->buyCost->text().toStdString());
//        std::string message = "Заявка: " + answerSendBid.at(0)
//                + "\nТип: " + answerSendBid.at(1)
//                + "\nОбъём: " + answerSendBid.at(2)
//                + "\nЦена: " + answerSendBid.at(3)
//                + "\nСтатус: " + answerSendBid.at(4);
//        answerSendBid.clear();
        QMessageBox::information(this, "Успешно", "Ваша заявка размещена");
    }else QMessageBox::warning(this, "Ошибка", "Проверьте правильность введённых данных");
}

void MainWindow::on_sellButton_clicked()
{
    if(!ui->sellValue->text().isEmpty() && !ui->sellCost->text().isEmpty()){
        SendBid("sell", ui->sellValue->text().toStdString(), ui->sellCost->text().toStdString());
//        std::string message = "Заявка: " + answerSendBid.at(0)
//                + "\nТип: " + answerSendBid.at(1)
//                + "\nОбъём: " + answerSendBid.at(2)
//                + "\nЦена: " + answerSendBid.at(3)
//                + "\nСтатус: " + answerSendBid.at(4);
//        answerSendBid.clear();
        QMessageBox::information(this, "Успешно", "Ваша заявка размещена");
    }else QMessageBox::warning(this, "Ошибка", "Проверьте правильность введённых данных");
}




