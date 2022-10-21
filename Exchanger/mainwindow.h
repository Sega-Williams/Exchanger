#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "registrationwindow.h"
#include "clickablelabel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE



extern std::string name;
extern std::string id;
extern std::string rub;
extern std::string usd;
extern std::vector<std::vector<std::string>> pullExecBids;
extern std::vector<std::vector<std::string>> pullActiveBids;
extern std::vector<std::string> idsExecBids;
extern std::vector<std::string> idsActiveBids;
extern std::vector<std::string> answerSendBid;
extern std::vector<std::string> quotes;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QTimer *timer;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void updateQuotesWindows();
    void updateLabels();
    void clearLayout(QLayout*);
private slots:
    void on_buyButton_clicked();

    void on_sellButton_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
