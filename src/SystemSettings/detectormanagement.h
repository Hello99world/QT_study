#ifndef DETECTORMANAGEMENT_H
#define DETECTORMANAGEMENT_H

#include <QWidget>
#include <QWidget>
#include <QSqlTableModel>
#include <QItemSelectionModel>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace Ui {
class DetectorManagement;
}

class DetectorManagement : public QWidget
{
    Q_OBJECT

private slots:
    void onAddClicked();
    void onSaveClicked();
    void onDeleteClicked();
    void onRevertClicked();
    void onClearClicked();

public:
    explicit DetectorManagement(QWidget *parent = nullptr);
    ~DetectorManagement();

private:
    Ui::DetectorManagement *ui;
    // UI控件
    QTableView *tableView;
    QPushButton *btnAdd;
    QPushButton *btnSave;
    QPushButton *btnDelete;
    QPushButton *btnRevert;
    QPushButton *btnClear;
    // 数据模型
    QSqlTableModel *model;
    QItemSelectionModel *selectionModel;
    // 函数声明
    void initUI();
    void initDatabase();
    void setupTable();
    void updateButtonState();
};

#endif // DETECTORMANAGEMENT_H
