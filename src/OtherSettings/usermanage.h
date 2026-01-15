#ifndef USERMANAGE_H
#define USERMANAGE_H

#include <QWidget>
#include <QWidget>
#include <QSqlTableModel>
#include <QItemSelectionModel>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace Ui {
class UserManage;
}

class UserManage : public QWidget
{
    Q_OBJECT

public:
    explicit UserManage(QWidget *parent = nullptr);
    ~UserManage();

private slots:
    void onAddClicked();
    void onSaveClicked();
    void onDeleteClicked();
    void onRevertClicked();
    void onImportClicked();
    void onExportClicked();



private:
    Ui::UserManage *ui;
    // UI控件
    QTableView *tableView;
    QPushButton *btnAdd;
    QPushButton *btnSave;
    QPushButton *btnDelete;
    QPushButton *btnRevert;
    QPushButton *btnImport;
    QPushButton *btnExport;
    // 数据模型
    QSqlTableModel *model;
    QItemSelectionModel *selectionModel;
    // 函数声明
    void initUI();
    void initDatabase();
    void setupTable();
    void updateButtonState();
};

#endif // USERMANAGE_H
