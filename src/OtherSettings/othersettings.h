#ifndef OTHERSETTINGS_H
#define OTHERSETTINGS_H

#include <QWidget>

namespace Ui {
class OtherSettings;
}

class OtherSettings : public QWidget
{
    Q_OBJECT

public:
    explicit OtherSettings(QWidget *parent = nullptr);
    ~OtherSettings();

private:
    Ui::OtherSettings *ui;
};

#endif // OTHERSETTINGS_H
