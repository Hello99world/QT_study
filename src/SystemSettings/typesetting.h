#ifndef TYPESETTING_H
#define TYPESETTING_H

#include <QWidget>
#include <QSqlTableModel>
#include <QItemSelectionModel>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace Ui {
class TypeSetting;
}

class TypeSetting : public QWidget
{
    Q_OBJECT

public:
    explicit TypeSetting(QWidget *parent = nullptr);
    ~TypeSetting();

private slots:
    void onAddClicked();
    void onSaveClicked();
    void onDeleteClicked();
    void onRevertClicked();
    void onClearClicked();
    void onImportClicked();
    void onExportClicked();

private:
    Ui::TypeSetting *ui;
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

#endif // TYPESETTING_H
