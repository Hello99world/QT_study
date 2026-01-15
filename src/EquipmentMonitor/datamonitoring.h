#ifndef DATAMONITORING_H
#define DATAMONITORING_H

#include <QWidget>
#include <QSqlTableModel>
#include <QItemSelectionModel>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace Ui {
class DataMonitoring;
}

class DataMonitoring : public QWidget
{
    Q_OBJECT

public:
    explicit DataMonitoring(QWidget *parent = nullptr);
    ~DataMonitoring();

private:
    Ui::DataMonitoring *ui;
    // UI控件
    QTableView *tableView;
    QPushButton *btnAdd;
    QPushButton *btnSave;
    QPushButton *btnDelete;
    QPushButton *btnRevert;
    QPushButton *btnClear;
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

#endif // DATAMONITORING_H
