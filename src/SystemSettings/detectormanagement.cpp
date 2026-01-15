#include "detectormanagement.h"
#include "itemdelegate.h"
#include "ui_detectormanagement.h"

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

DetectorManagement::DetectorManagement(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DetectorManagement)
{
    ui->setupUi(this);
    initUI();           // 初始化界面
    initDatabase();     // 初始化数据库
}

DetectorManagement::~DetectorManagement()
{
    delete ui;
}

// 初始化界面
void DetectorManagement::initUI()
{
    this->setObjectName("PortManagement");
    // 创建按钮
    btnAdd = new QPushButton("添加", this);
    btnSave = new QPushButton("保存", this);
    btnDelete = new QPushButton("删除", this);
    btnRevert = new QPushButton("撤销", this);
    btnClear = new QPushButton("清空", this);
    // 添加图标
    btnAdd->setIcon(QIcon(":/rec/image/portadd.png"));
    btnSave->setIcon(QIcon(":/rec/image/portsave.png"));
    btnDelete->setIcon(QIcon(":/rec/image/portdele.png"));
    btnRevert->setIcon(QIcon(":/rec/image/portre.png"));
    btnClear->setIcon(QIcon(":/rec/image/portclear.png"));
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
    btnLayout->addWidget(btnClear, 1);
    // 主垂直布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(btnLayout);
    mainLayout->addWidget(tableView);
    setLayout(mainLayout);
    // 链接信号槽
    connect(btnAdd, &QPushButton::clicked, this, &DetectorManagement::onAddClicked);
    connect(btnSave, &QPushButton::clicked, this, &DetectorManagement::onSaveClicked);
    connect(btnDelete, &QPushButton::clicked, this, &DetectorManagement::onDeleteClicked);
    connect(btnRevert, &QPushButton::clicked, this, &DetectorManagement::onRevertClicked);
    connect(btnClear, &QPushButton::clicked, this, &DetectorManagement::onClearClicked);
    // qDebug() << "表格设置完成";
}

// 初始化数据库连接
void DetectorManagement::initDatabase()
{
    QSqlDatabase db = DbHelper::getDatabase("iot.db");
    if (db.open())
    {
        model = new QSqlTableModel(this, db);
        model->setTable("nodeinfo");
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
void DetectorManagement::setupTable()
{
    // 设置表头显示名称
    model->setHeaderData(model->fieldIndex("NodeID"), Qt::Horizontal, "编号");
    model->setHeaderData(model->fieldIndex("NodeName"), Qt::Horizontal, "探测器名称");
    model->setHeaderData(model->fieldIndex("NodeType"), Qt::Horizontal, "型号");
    model->setHeaderData(model->fieldIndex("NodeClass"), Qt::Horizontal, "气体种类");
    model->setHeaderData(model->fieldIndex("NodeUpper"), Qt::Horizontal, "上限值");
    model->setHeaderData(model->fieldIndex("NodeLimit"), Qt::Horizontal, "下限值");
    model->setHeaderData(model->fieldIndex("NodeEnable"), Qt::Horizontal, "启用");
    model->setHeaderData(model->fieldIndex("NodeMark"), Qt::Horizontal, "备注");
    // 设置选择模型
    selectionModel = new QItemSelectionModel(model, this);
    // 绑定到表格
    tableView->setModel(model);
    tableView->setSelectionModel(selectionModel);
    ItemDelegate *delegate = new ItemDelegate(this);
    tableView->setItemDelegateForColumn(6, delegate);
    connect(selectionModel, &QItemSelectionModel::currentChanged, this, [=]() {
        updateButtonState();
    });
    connect(model, &QSqlTableModel::dataChanged, this, [=]() {
        updateButtonState();
    });
}

// 更新按钮状态
void DetectorManagement::updateButtonState()
{
    bool isDirty = model->isDirty();
    btnSave->setEnabled(isDirty);
    btnRevert->setEnabled(isDirty);
}

// 添加
void DetectorManagement::onAddClicked()
{
    int rowCount = model->rowCount();
    QSqlRecord record = model->record();
    model->insertRecord(rowCount, record);

    // 选中新添加的行
    QModelIndex index = model->index(rowCount, 0);
    selectionModel->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

    updateButtonState();
}

// 保存
void DetectorManagement::onSaveClicked()
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
void DetectorManagement::onDeleteClicked()
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
void DetectorManagement::onRevertClicked()
{
    model->revertAll();
    btnSave->setEnabled(false);
    btnRevert->setEnabled(false);
}

// 清空
void DetectorManagement::onClearClicked()
{
    if (model->rowCount() == 0)
    {
        QMessageBox::information(this, "提示", "表格中没有数据");
        return;
    }

    int result = QMessageBox::warning(this, "清空数据",
                                      "确定清空所有数据？此操作不可恢复！",
                                      QMessageBox::Ok | QMessageBox::Cancel);
    if (result == QMessageBox::Ok)
    {
        QSqlQuery query(DbHelper::getDatabase("iot.db"));
        if (query.exec("DELETE FROM portinfo"))
        {
            model->select();  // 刷新表格
            QMessageBox::information(this, "提示", "数据已清空");
        }
        else
        {
            QMessageBox::critical(this, "错误", "清空失败：" + query.lastError().text());
        }
    }
}
