#ifndef MYQUOTES_H
#define MYQUOTES_H

#include <QDialog>

namespace Ui {
class MyQuotes;
}

class MyQuotes : public QDialog
{
    Q_OBJECT

public:
    explicit MyQuotes(std::string id = "", std::string type = "",
                      std::string value = "", std::string price = "", std::string status = "", QWidget *parent = nullptr);
    ~MyQuotes();
private:
    std::string id;
private slots:

    void on_AcceptButton_clicked();

    void on_RejectButton_clicked();

private:
    Ui::MyQuotes *ui;
};

#endif // MYQUOTES_H
