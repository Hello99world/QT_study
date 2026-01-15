#include "usermanage.h"
#include "itemdelegate.h"
#include "ui_usermanage.h"

#include "dbhelper.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QHeaderView>
#include <QTextStream>
#include <QStringConverter>

UserManage::UserManage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UserManage)
{
    ui->setupUi(this);
    initUI();           // 初始化界面
    initDatabase();     // 初始化数据库
}

UserManage::~UserManage()
{
    delete ui;
}
// 初始化界面
void UserManage::initUI()
{
    this->setObjectName("PortManagement");
    // 创建按钮
    btnAdd = new QPushButton("添加", this);
    btnSave = new QPushButton("保存", this);
    btnDelete = new QPushButton("删除", this);
    btnRevert = new QPushButton("撤销", this);
    btnImport = new QPushButton("导入", this);
    btnExport = new QPushButton("导出", this);
    // 添加图标
    btnAdd->setIcon(QIcon(":/rec/image/portadd.png"));
    btnSave->setIcon(QIcon(":/rec/image/portsave.png"));
    btnDelete->setIcon(QIcon(":/rec/image/portdele.png"));
    btnRevert->setIcon(QIcon(":/rec/image/portre.png"));
    btnImport->setIcon(QIcon(":/rec/image/portdown.png"));
    btnExport->setIcon(QIcon(":/rec/image/portup.png"));
    // 初始状态：保存和撤销按钮禁用
    btnSave->setEnabled(false);
    btnRevert->setEnabled(false);
    // 创建表格视图
    tableView = new QTableView(this);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setAlternatingRowColors(true);
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->verticalHeader()->setVisible(false); // 隐藏原本行号
    // 按钮水平布局
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(btnAdd, 1);
    btnLayout->addWidget(btnSave, 1);
    btnLayout->addWidget(btnDelete,  1);
    btnLayout->addWidget(btnRevert,  1);
    btnLayout->addWidget(btnImport,  1);
    btnLayout->addWidget(btnExport, 1);
    // 主垂直布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(btnLayout);
    mainLayout->addWidget(tableView);
    setLayout(mainLayout);
    // 链接信号槽
    connect(btnAdd, &QPushButton::clicked, this, &UserManage::onAddClicked);
    connect(btnSave, &QPushButton::clicked, this, &UserManage::onSaveClicked);
    connect(btnDelete, &QPushButton::clicked, this, &UserManage::onDeleteClicked);
    connect(btnRevert, &QPushButton::clicked, this, &UserManage::onRevertClicked);
    connect(btnImport, &QPushButton::clicked, this, &UserManage::onImportClicked);
    connect(btnExport, &QPushButton::clicked, this, &UserManage::onExportClicked);
    // qDebug() << "表格设置完成";
}

// 初始化数据库连接
void UserManage::initDatabase()
{
    QSqlDatabase db = DbHelper::getDatabase("iot.db");
    if (db.open())
    {
        model = new QSqlTableModel(this, db);
        model->setTable("userinfo");
        model->setEditStrategy(QSqlTableModel::OnManualSubmit);

        if (model->select())
        {
            setupTable();
        }
        else
        {
            qDebug() << "数据加载失败：" << model->lastError().text();
            QMessageBox::critical(this, "错误", "无法加载数据：" + model->lastError().text());
        }
    }
    else
    {
        qDebug() << "数据库连接失败";
        QMessageBox::critical(this, "错误", "数据库连接失败");
    }
}

// 设置表格
void UserManage::setupTable()
{
    // 设置表头显示名称
    model->setHeaderData(model->fieldIndex("UserName"), Qt::Horizontal, "用户名称");
    model->setHeaderData(model->fieldIndex("UserPwd"), Qt::Horizontal, "用户密码");
    model->setHeaderData(model->fieldIndex("UserType"), Qt::Horizontal, "用户类型");
    model->setHeaderData(model->fieldIndex("Permission1"), Qt::Horizontal, "系统设置");
    model->setHeaderData(model->fieldIndex("Permission2"), Qt::Horizontal, "系统重启");
    // 设置选择模型
    selectionModel = new QItemSelectionModel(model, this);
    // 绑定到表格
    tableView->setModel(model);
    tableView->setSelectionModel(selectionModel);
    ItemDelegate *delegate = new ItemDelegate(this);
    tableView->setItemDelegateForColumn(3, delegate);
    ItemDelegate *delegate1 = new ItemDelegate(this);
    tableView->setItemDelegateForColumn(4, delegate1);
    // 隐藏后面
    tableView->hideColumn(model->fieldIndex("Permission3"));
    tableView->hideColumn(model->fieldIndex("Permission4"));
    tableView->hideColumn(model->fieldIndex("Permission5"));
    tableView->hideColumn(model->fieldIndex("Permission6"));
    tableView->hideColumn(model->fieldIndex("Permission7"));
    tableView->hideColumn(model->fieldIndex("UserMark"));
    connect(selectionModel, &QItemSelectionModel::currentChanged, this, [=]() {
        updateButtonState();
    });
    connect(model, &QSqlTableModel::dataChanged, this, [=]() {
        updateButtonState();
    });
}

// 更新按钮状态
void UserManage::updateButtonState()
{
    bool isDirty = model->isDirty();
    btnSave->setEnabled(isDirty);
    btnRevert->setEnabled(isDirty);
}

// 添加
void UserManage::onAddClicked()
{
    int rowCount = model->rowCount();
    QSqlRecord record = model->record();
    // 给隐藏的非空列设置默认值
    // record.setValue("ReadInterval", 1);
    // record.setValue("ReadTimeout", 3);
    // record.setValue("ReadMaxtime", 60);

    model->insertRecord(rowCount, record);

    // 选中新添加的行
    QModelIndex index = model->index(rowCount, 0);
    selectionModel->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

    updateButtonState();
}

// 保存
void UserManage::onSaveClicked()
{
    if (model->submitAll())
    {
        QMessageBox::information(this, "提示", "保存成功");
        btnSave->setEnabled(false);
        btnRevert->setEnabled(false);
    }
    else
    {
        QMessageBox::critical(this, "错误", "保存失败：" + model->lastError().text());
    }
}

// 删除
void UserManage::onDeleteClicked()
{
    QModelIndex index = selectionModel->currentIndex();
    if (!index.isValid())
    {
        QMessageBox::warning(this, "警告", "请先选择要删除的记录");
        return;
    }

    int result = QMessageBox::warning(this, "删除记录", "确定删除该记录？",
                                      QMessageBox::Ok | QMessageBox::Cancel);
    if (result == QMessageBox::Ok)
    {
        model->removeRow(index.row());
        if (model->submitAll())
        {
            QMessageBox::information(this, "提示", "删除成功");
        }
        else
        {
            QMessageBox::critical(this, "错误", "删除失败：" + model->lastError().text());
        }
    }
}

// 撤销
void UserManage::onRevertClicked()
{
    model->revertAll();
    btnSave->setEnabled(false);
    btnRevert->setEnabled(false);
}


// 导入CSV数据
void UserManage::onImportClicked()
{
    QString path = QFileDialog::getOpenFileName(this, "导入数据",
                                                QDir::currentPath(),
                                                "CSV文件 (*.csv);;所有文件 (*.*)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::critical(this, "错误", "无法打开文件");
        return;
    }

    QTextStream stream(&file);
    // 设置 UTF-8 编码
    stream.setEncoding(QStringConverter::Utf8);

    // 跳过表头行
    if (!stream.atEnd())
    {
        stream.readLine();
    }

    int importCount = 0;

    while (!stream.atEnd())
    {
        QString line = stream.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList fields = line.split(",");
        QSqlRecord record = model->record();

        // 设置字段值（跳过第一个ID字段，让数据库自动处理）
        for (int j = 1; j < fields.size() && j < record.count(); j++)
        {
            record.setValue(j, fields.at(j).trimmed());
        }

        model->insertRecord(model->rowCount(), record);
        importCount++;
    }

    file.close();

    if (model->submitAll())
    {
        QMessageBox::information(this, "提示", QString("成功导入 %1 条记录").arg(importCount));
    }
    else
    {
        QMessageBox::critical(this, "错误", "导入失败：" + model->lastError().text());
        model->revertAll();
    }
}

// 导出CSV数据
void UserManage::onExportClicked()
{
    if (model->rowCount() == 0)
    {
        QMessageBox::information(this, "提示", "没有数据可导出");
        return;
    }

    QString path = QFileDialog::getSaveFileName(this, "数据导出",
                                                QDir::currentPath() + "/端口数据.csv",
                                                "CSV文件 (*.csv)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::critical(this, "错误", "无法创建文件");
        return;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);


    int columns = model->columnCount();
    int rows = model->rowCount();

    // 写入表头
    for (int i = 0; i < columns; i++)
    {
        stream << model->headerData(i, Qt::Horizontal).toString();
        if (i != columns - 1)
            stream << ",";
    }
    stream << "\n";

    // 写入数据行
    for (int i = 0; i < rows; i++)
    {
        QSqlRecord record = model->record(i);
        for (int j = 0; j < columns; j++)
        {
            stream << record.value(j).toString();
            if (j != columns - 1)
                stream << ",";
        }
        stream << "\n";
    }

    file.close();
    QMessageBox::information(this, "提示", QString("成功导出 %1 条记录").arg(rows));
}
