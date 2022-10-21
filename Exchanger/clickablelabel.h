#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QWidget>
#include <Qt>

class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget* parent = Q_NULLPTR, std::string id = "", std::string type = "",
                            std::string value = "", std::string price = "", std::string status = "", Qt::WindowFlags f = Qt::WindowFlags());
    ~ClickableLabel();

private:
    std::string id = "";
    std::string type = "";
    std::string value = "";
    std::string price = "";
    std::string status = "";

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event);

};

#endif // CLICKABLELABEL_H
