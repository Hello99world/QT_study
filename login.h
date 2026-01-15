#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QPoint>

class QLabel;
class QLineEdit;
class QCheckBox;
class QPushButton;

namespace Ui {
class login;
}

// 当前登录用户信息
struct CurrentUser
{
    QString userName;
    QString userType;
    bool permissionSystemSetting = false; // Permission1
    bool permissionReboot = false;        // Permission2
};
// 全局变量声明
extern CurrentUser g_currentUser;

class login : public QWidget
{
    Q_OBJECT
public:
    explicit login(QWidget *parent = nullptr);
    ~login();
    bool tryLogin();



signals:
    void accept();   // 登录成功信号

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    Ui::login *ui;
    // ===== UI 成员 =====
    QLabel *closeLabel;
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QCheckBox *rememberCheck;
    QCheckBox *autoLoginCheck;

    QPoint dragPos;

    // ===== 业务逻辑 =====
    void initUI();
    void writeFile();
    void readFile();
    // bool tryLogin();
    bool checkUser(const QString &username, const QString &pwd);
    void startLoginLogThread(const QString& username);
};

#endif // LOGIN_H
