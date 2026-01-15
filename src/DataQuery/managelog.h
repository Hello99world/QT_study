#ifndef MANAGELOG_H
#define MANAGELOG_H

#include <QWidget>
#include <QTableView>
#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QItemSelectionModel>

namespace Ui {
class ManageLog;
}

class ManageLog : public QWidget
{
    Q_OBJECT

public:
    explicit ManageLog(QWidget *parent = nullptr);
    ~ManageLog();
private slots:
    void onQueryClicked();      // 查询
    void onDeleteClicked();     // 删除
    void onFirstPageClicked();  // 首页
    void onPrevPageClicked();   // 上一页
    void onNextPageClicked();   // 下一页
    void onLastPageClicked();   // 尾页

private:
    Ui::ManageLog *ui;
    // 左侧表格
    QTableView *tableView;
    QSqlQueryModel *model;
    QItemSelectionModel *selectionModel;

    // 查询复选框
    QCheckBox *chkTime;
    QCheckBox *chkDevice;
    QCheckBox *chkNode;
    QCheckBox *chkAlarmType;
    // 查询输入
    QDateTimeEdit *dtStart;
    QDateTimeEdit *dtEnd;
    QComboBox *cmbDevice;
    QComboBox *cmbNode;
    QComboBox *cmbAlarmType;
    // 查询和删除按钮
    QPushButton *btnQuery;
    QPushButton *btnDelete;

    // 分页按钮
    QPushButton *btnFirst;
    QPushButton *btnPrev;
    QPushButton *btnNext;
    QPushButton *btnLast;

    // 分页信息显示
    QLabel *lblCurrentPage;
    QLabel *lblTotalPage;
    QLabel *lblTotalCount;
    QLabel *lblPageSize;
    QLabel *lblQueryTime;

    // 分页参数
    int pageSize;
    int currentPage;

    // 初始化函数
    void initUI();
    void initDatabase();
    void connectSignals();
    void loadComboBoxData();

    // 分页相关函数
    int getCount();
    int getTotalPage();
    void pageQuery();
    void updatePageStatus();
    void showPageInfo();

    // 查询SQL
    QString buildQuerySql(bool isCount = false);
    void bindQueryParams(QSqlQuery &query);

};

#endif // MANAGELOG_H
