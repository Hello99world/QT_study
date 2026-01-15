#include "datamonitoring.h"
#include "qidentityproxymodel.h"
#include "ui_datamonitoring.h"

#include "qss.h"
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

DataMonitoring::DataMonitoring(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DataMonitoring)
{
    ui->setupUi(this);
    Qss::loadStyleSheetFromResource(":/rec/qss/DataMonitoring.qss", this);
    initUI();           // 初始化界面
    initDatabase();     // 初始化数据库
}

DataMonitoring::~DataMonitoring()
{
    delete ui;

}

//
class TableProxyModel : public QIdentityProxyModel
{
public:
    TableProxyModel(QObject *parent = nullptr) : QIdentityProxyModel(parent) {}

    void setHighlightColumns(const QList<int> &columns)
    {
        m_highlightColumns = columns;
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        // 所有列居中
        if (role == Qt::TextAlignmentRole)
        {
            return Qt::AlignCenter;
        }

        // 特定列设置颜色
        if (role == Qt::ForegroundRole && m_highlightColumns.contains(index.column()))
        {
            return QColor("#7CFCFF");
        }

        // 特定列设置字体加粗
        if (role == Qt::FontRole && m_highlightColumns.contains(index.column()))
        {
            QFont font;
            font.setBold(true);
            return font;
        }

        return QIdentityProxyModel::data(index, role);
    }

private:
    QList<int> m_highlightColumns;
};

// 初始化界面
void DataMonitoring::initUI()
{
    this->setObjectName("DataMonitoring");
    // 创建表格视图
    tableView = new QTableView(this);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setAlternatingRowColors(true);
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->verticalHeader()->setVisible(false); // 隐藏原本行号
    // 垂直布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tableView);
    setLayout(mainLayout);
}

// 初始化数据库连接
void DataMonitoring::initDatabase()
{
    QSqlDatabase db = DbHelper::getDatabase("iot.db");
    if (db.open())
    {
        model = new QSqlTableModel(this, db);
        model->setTable("MonitorInfo");
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
void DataMonitoring::setupTable()
{
    // 设置表头显示名称
    model->setHeaderData(model->fieldIndex("NodeID"), Qt::Horizontal, "序号");
    model->setHeaderData(model->fieldIndex("positionID"), Qt::Horizontal, "位号");
    model->setHeaderData(model->fieldIndex("DeviceName"), Qt::Horizontal, "控制器名称");
    model->setHeaderData(model->fieldIndex("DeviceType"), Qt::Horizontal, "控制器型号");
    model->setHeaderData(model->fieldIndex("NodeName"), Qt::Horizontal, "探测器名称");
    model->setHeaderData(model->fieldIndex("NodeType"), Qt::Horizontal, "探测器型号");
    model->setHeaderData(model->fieldIndex("NodeClass"), Qt::Horizontal, "气体种类");
    model->setHeaderData(model->fieldIndex("RealDensity"), Qt::Horizontal, "实时浓度");
    model->setHeaderData(model->fieldIndex("NodeSign"), Qt::Horizontal, "单位");
    model->setHeaderData(model->fieldIndex("NodeStatus"), Qt::Horizontal, "状态");
    // // 设置选择模型
    // selectionModel = new QItemSelectionModel(model, this);
    // // 绑定到表格
    // tableView->setModel(model);
    // tableView->setSelectionModel(selectionModel);
    // 创建代理模型
    TableProxyModel *proxyModel = new TableProxyModel(this);
    proxyModel->setSourceModel(model);

    // 设置高亮列
    proxyModel->setHighlightColumns({
        model->fieldIndex("RealDensity"),
        model->fieldIndex("NodeStatus")
    });

    // 设置选择模型
    selectionModel = new QItemSelectionModel(proxyModel, this);

    // 绑定代理模型到表格
    tableView->setModel(proxyModel);
    tableView->setSelectionModel(selectionModel);


}
