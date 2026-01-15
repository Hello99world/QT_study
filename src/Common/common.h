#ifndef COMMON_H
#define COMMON_H

#include <QWidget>

namespace Ui {
class Common;
}

class Common : public QWidget
{
    Q_OBJECT

public:
    explicit Common(QWidget *parent = nullptr);
    ~Common();

private:
    Ui::Common *ui;
};

#endif // COMMON_H
