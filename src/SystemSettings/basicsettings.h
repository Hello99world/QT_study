#ifndef BASICSETTINGS_H
#define BASICSETTINGS_H

#include <QWidget>

class QLineEdit;
class QPushButton;
class QComboBox;
class QLabel;
class SwitchButton;
#include "switchbutton.h"

namespace Ui {
class BasicSettings;
}

class BasicSettings : public QWidget
{
    Q_OBJECT

public:
    explicit BasicSettings(QWidget *parent = nullptr);
    ~BasicSettings();

private:
    Ui::BasicSettings *ui;
    // ===== 本地数据库 =====
    QComboBox  *localConnType;
    QLineEdit  *localDbNameEdit;
    QPushButton *localTestBtn;

    // ===== 云端数据库 =====
    // QLabel     *syncSwitch;   // 仅 UI 占位
    SwitchButton *syncSwitch;

    QComboBox  *remoteConnType;
    QComboBox  *remoteDbType;

    QLineEdit  *remoteDbNameEdit;
    QLineEdit  *hostEdit;
    QLineEdit  *userEdit;
    QLineEdit  *passwordEdit;
    QPushButton *remoteTestBtn;
private:
    SwitchButton *autoLoginBtn;


private:
    void initUI();

private slots:
    void testLocalDatabase();
    void testRemoteDatabase();
};

#endif // BASICSETTINGS_H

