#include "policerecord.h"
#include "ui_policerecord.h"

#include "dbhelper.h"
#include "qss.h"

#include <QFrame>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QMessageBox>
#include <QHeaderView>
#include <QDebug>
#include <QElapsedTimer>

PoliceRecord::PoliceRecord(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PoliceRecord)
    , model(nullptr)
    , selectionModel(nullptr)
    , pageSize(15)
    , currentPage(1)
{
    ui->setupUi(this);
    this->setObjectName("PoliceRecord");
    Qss::loadStyleSheetFromResource(":/rec/qss/PoliceRecord.qss", this);
    initUI();
    connectSignals();
    initDatabase();
}

PoliceRecord::~PoliceRecord()
{
    delete ui;
}

// 初始化界面
void PoliceRecord::initUI()
{
    // 左侧表格
    tableView = new QTableView(this);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setAlternatingRowColors(true);
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->verticalHeader()->setVisible(false); // 隐藏原本行号

    // 右侧面板
    QWidget *rightPanel = new QWidget(this);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);

    // 查询条件区域
    QGridLayout *queryLayout = new QGridLayout();

    // 1. 时间范围
    chkTime = new QCheckBox("开始时间~结束时间", this);
    dtStart = new QDateTimeEdit(this);
    dtEnd = new QDateTimeEdit(this);
    dtStart->setCalendarPopup(true);
    dtEnd->setCalendarPopup(true);
    dtStart->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    dtEnd->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    dtStart->setDateTime(QDateTime::currentDateTime().addDays(-7));
    dtEnd->setDateTime(QDateTime::currentDateTime());

    queryLayout->addWidget(chkTime, 0, 0, 1, 2);
    queryLayout->addWidget(dtStart, 1, 0, 1, 2);
    queryLayout->addWidget(dtEnd, 2, 0, 1, 2);

    // 2. 控制器
    chkDevice = new QCheckBox("控制器", this);
    cmbDevice = new QComboBox(this);
    queryLayout->addWidget(chkDevice, 3, 0);
    queryLayout->addWidget(cmbDevice, 3, 1);

    // 3. 探测器
    chkNode = new QCheckBox("探测器", this);
    cmbNode = new QComboBox(this);
    queryLayout->addWidget(chkNode, 4, 0);
    queryLayout->addWidget(cmbNode, 4, 1);

    // 4. 报警类型
    chkAlarmType = new QCheckBox("报警类型", this);
    cmbAlarmType = new QComboBox(this);
    queryLayout->addWidget(chkAlarmType, 5, 0);
    queryLayout->addWidget(cmbAlarmType, 5, 1);

    // 5. 报警值
    chkAlarmValue = new QCheckBox("报警值", this);
    editAlarmValue = new QLineEdit(this);
    queryLayout->addWidget(chkAlarmValue, 6, 0);
    queryLayout->addWidget(editAlarmValue, 6, 1);

    // 6. 位置编号
    chkPosition = new QCheckBox("位置编号", this);
    editPosition = new QLineEdit(this);
    queryLayout->addWidget(chkPosition, 7, 0);
    queryLayout->addWidget(editPosition, 7, 1);

    // 查询和删除按钮
    btnQuery = new QPushButton("查询", this);
    btnDelete = new QPushButton("删除", this);
    QHBoxLayout *btnQueryLayout = new QHBoxLayout();
    btnQueryLayout->addWidget(btnQuery);
    btnQueryLayout->addWidget(btnDelete);
    queryLayout->addLayout(btnQueryLayout, 8, 0, 1, 2);
    rightLayout->addLayout(queryLayout);

    // 添加分隔线
    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    rightLayout->addWidget(line);

    // 页面控制区域
    QVBoxLayout *pageLayout = new QVBoxLayout();

    // 上/下一页
    btnPrev = new QPushButton("上一页", this);
    btnNext = new QPushButton("下一页", this);
    QHBoxLayout *navLayout1 = new QHBoxLayout();
    navLayout1->addWidget(btnPrev);
    navLayout1->addWidget(btnNext);
    pageLayout->addLayout(navLayout1);

    // 首/尾页
    btnFirst = new QPushButton("首页", this);
    btnLast = new QPushButton("尾页", this);
    QHBoxLayout *navLayout2 = new QHBoxLayout();
    navLayout2->addWidget(btnFirst);
    navLayout2->addWidget(btnLast);
    pageLayout->addLayout(navLayout2);

    // 当前页
    lblCurrentPage = new QLabel("第 1 页", this);
    lblCurrentPage->setObjectName("lblPageInfo");
    lblCurrentPage->setAlignment(Qt::AlignCenter);
    pageLayout->addWidget(lblCurrentPage);

    // 总页数
    lblTotalPage = new QLabel("共 0 页", this);
    lblTotalPage->setObjectName("lblPageInfo");
    lblTotalPage->setAlignment(Qt::AlignCenter);
    pageLayout->addWidget(lblTotalPage);

    // 总条数
    lblTotalCount = new QLabel("共 0 条", this);
    lblTotalCount->setObjectName("lblPageInfo");
    lblTotalCount->setAlignment(Qt::AlignCenter);
    pageLayout->addWidget(lblTotalCount);

    // 每页条数
    lblPageSize = new QLabel(QString("每页 %1 条").arg(pageSize), this);
    lblPageSize->setObjectName("lblPageInfo");
    lblPageSize->setAlignment(Qt::AlignCenter);
    pageLayout->addWidget(lblPageSize);

    // 查询时间
    lblQueryTime = new QLabel("耗时 0 ms", this);
    lblQueryTime->setObjectName("lblPageInfo");
    lblQueryTime->setAlignment(Qt::AlignCenter);
    pageLayout->addWidget(lblQueryTime);

    rightLayout->addLayout(pageLayout);
    rightLayout->addStretch();

    // 合并主布局
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(tableView, 3);
    mainLayout->addWidget(rightPanel, 1);
}

// 连接槽函数
void PoliceRecord::connectSignals()
{
    connect(btnQuery, &QPushButton::clicked, this, &PoliceRecord::onQueryClicked);
    connect(btnDelete, &QPushButton::clicked, this, &PoliceRecord::onDeleteClicked);
    connect(btnFirst, &QPushButton::clicked, this, &PoliceRecord::onFirstPageClicked);
    connect(btnPrev, &QPushButton::clicked, this, &PoliceRecord::onPrevPageClicked);
    connect(btnNext, &QPushButton::clicked, this, &PoliceRecord::onNextPageClicked);
    connect(btnLast, &QPushButton::clicked, this, &PoliceRecord::onLastPageClicked);
}

// 连接数据库
void PoliceRecord::initDatabase()
{
    QSqlDatabase db = DbHelper::getDatabase("iot.db");

    if (!db.isValid())
    {
        qDebug() << "数据库无效";
        return;
    }
    if (!db.open())
    {
        qDebug() << "数据库打开失败：" << db.lastError().text();
        return;
    }
    model = new QSqlQueryModel(this);
    tableView->setModel(model);
    selectionModel = tableView->selectionModel();
    // 加载下拉框数据
    loadComboBoxData();
    pageQuery(); // 查询一次
}

// 加载下拉框数据
void PoliceRecord::loadComboBoxData()
{
    QSqlDatabase db = DbHelper::getDatabase("iot.db");
    QSqlQuery query(db);

    // 控制器列表
    cmbDevice->clear();
    cmbDevice->addItem("全部");
    query.exec("SELECT DISTINCT DeviceName FROM alarmlog WHERE DeviceName IS NOT NULL");
    while (query.next())
    {
        cmbDevice->addItem(query.value(0).toString());
    }

    // 探测器列表
    cmbNode->clear();
    cmbNode->addItem("全部");
    query.exec("SELECT DISTINCT NodeName FROM alarmlog WHERE NodeName IS NOT NULL");
    while (query.next())
    {
        cmbNode->addItem(query.value(0).toString());
    }

    // 报警类型列表
    cmbAlarmType->clear();
    cmbAlarmType->addItem("全部");
    query.exec("SELECT DISTINCT Content FROM alarmlog WHERE Content IS NOT NULL");
    while (query.next())
    {
        cmbAlarmType->addItem(query.value(0).toString());
    }
}

// 构建查询SQL
QString PoliceRecord::buildQuerySql(bool isCount)
{
    QString sql;
    // 插叙条件为空
    if (isCount)
    {
        sql = "SELECT COUNT(*) FROM alarmlog WHERE 1=1";
    }
    else
    {
        sql = "SELECT LogID, PositionID, DeviceName, NodeName, NodeValue, Content, "
              "StartTime, ConfirmUser, ConfirmTime, ConfirmContent FROM alarmlog WHERE 1=1";
    }
    // 时间
    if (chkTime->isChecked())
    {
        sql += " AND StartTime >= :startTime AND StartTime <= :endTime";
    }
    // 控制器
    if (chkDevice->isChecked() && cmbDevice->currentIndex() > 0)
    {
        sql += " AND DeviceName = :deviceName";
    }
    // 探测器
    if (chkNode->isChecked() && cmbNode->currentIndex() > 0)
    {
        sql += " AND NodeName = :nodeName";
    }
    // 报警类型
    if (chkAlarmType->isChecked() && cmbAlarmType->currentIndex() > 0)
    {
        sql += " AND Content = :alarmType";
    }
    // 报警值
    if (chkAlarmValue->isChecked() && !editAlarmValue->text().trimmed().isEmpty())
    {
        sql += " AND NodeValue = :alarmValue";
    }
    // 位置编号
    if (chkPosition->isChecked() && !editPosition->text().trimmed().isEmpty())
    {
        sql += " AND PositionID LIKE :positionId";
    }
    // 分页
    if (!isCount)
    {
        sql += " ORDER BY LogID ASC LIMIT :limit OFFSET :offset";
    }
    return sql;
}

// 构建查询参数
void PoliceRecord::bindQueryParams(QSqlQuery &query)
{
    // 时间
    if (chkTime->isChecked())
    {
        query.bindValue(":startTime", dtStart->dateTime().toString("yyyy-MM-dd HH:mm:ss"));
        query.bindValue(":endTime", dtEnd->dateTime().toString("yyyy-MM-dd HH:mm:ss"));
    }
    // 控制器
    if (chkDevice->isChecked() && cmbDevice->currentIndex() > 0)
    {
        query.bindValue(":deviceName", cmbDevice->currentText());
    }
    // 探测器
    if (chkNode->isChecked() && cmbNode->currentIndex() > 0)
    {
        query.bindValue(":nodeName", cmbNode->currentText());
    }
    // 报警类型
    if (chkAlarmType->isChecked() && cmbAlarmType->currentIndex() > 0)
    {
        query.bindValue(":alarmType", cmbAlarmType->currentText());
    }
    // 报警值
    if (chkAlarmValue->isChecked() && !editAlarmValue->text().trimmed().isEmpty())
    {
        query.bindValue(":alarmValue", editAlarmValue->text().trimmed());
    }
    // 位置编号
    if (chkPosition->isChecked() && !editPosition->text().trimmed().isEmpty())
    {
        query.bindValue(":positionId", "%" + editPosition->text().trimmed() + "%");
    }
}

// 分页查询
void PoliceRecord::pageQuery()
{
    // 创建定时器
    QElapsedTimer timer;
    timer.start();
    // 整体查询
    QString sql = buildQuerySql(false);
    QSqlQuery query(DbHelper::getDatabase("iot.db"));
    query.prepare(sql);
    bindQueryParams(query);
    query.bindValue(":limit", pageSize);
    query.bindValue(":offset", (currentPage - 1) * pageSize);
    if (query.exec())
    {
        model->setQuery(query);
        // 设置表头
        model->setHeaderData(0, Qt::Horizontal, "编号");
        model->setHeaderData(1, Qt::Horizontal, "位号");
        model->setHeaderData(2, Qt::Horizontal, "控制器名称");
        model->setHeaderData(3, Qt::Horizontal, "探测器名称");
        model->setHeaderData(4, Qt::Horizontal, "报警值");
        model->setHeaderData(5, Qt::Horizontal, "报警类型");
        model->setHeaderData(6, Qt::Horizontal, "触发时间");
        model->setHeaderData(7, Qt::Horizontal, "确认用户");
        model->setHeaderData(8, Qt::Horizontal, "确认时间");
        model->setHeaderData(9, Qt::Horizontal, "确认信息");

        tableView->resizeColumnsToContents();
    }
    else
    {
        qDebug() << "查询失败：" << query.lastError().text();
    }
    // 结算查询时间
    qint64 elapsed = timer.elapsed();
    lblQueryTime->setText(QString("耗时 %1 ms").arg(elapsed));
    updatePageStatus(); // 更新按钮
    showPageInfo();     // 显示信息
}
// 更新分页按钮的状态
void PoliceRecord::updatePageStatus()
{
    int totalPage = getTotalPage();
    btnFirst->setEnabled(currentPage > 1);
    btnPrev->setEnabled(currentPage > 1);
    btnNext->setEnabled(currentPage < totalPage);
    btnLast->setEnabled(currentPage < totalPage);
}

// 提取显示分页信息
void PoliceRecord::showPageInfo()
{
    lblCurrentPage->setText(QString("第 %1 页").arg(currentPage));
    lblTotalPage->setText(QString("共 %1 页").arg(getTotalPage()));
    lblTotalCount->setText(QString("共 %1 条").arg(getCount()));
}

// 获取符合条件的总记录数
int PoliceRecord::getCount()
{
    QString sql = buildQuerySql(true);
    QSqlQuery query(DbHelper::getDatabase("iot.db"));
    query.prepare(sql);
    bindQueryParams(query);
    if (query.exec() && query.next())
    {
        return query.value(0).toInt();
    }
    return 0;
}

// 获取总页数
int PoliceRecord::getTotalPage()
{
    int count = getCount();
    if (count == 0) return 1;
    return (count % pageSize == 0) ? (count / pageSize) : (count / pageSize + 1);
}

// 查询按钮点击槽函数
void PoliceRecord::onQueryClicked()
{
    currentPage = 1;
    pageQuery();
}

// 删除符合条件的记录槽函数
void PoliceRecord::onDeleteClicked()
{
    // 获取符合条件的记录数
    int count = getCount();
    if (count == 0)
    {
        QMessageBox::information(this, "提示", "没有符合条件的记录");
        return;
    }
    int result = QMessageBox::question(this, "删除数据",
                                       QString("确认删除符合条件的 %1 条记录？\n此操作不可恢复！").arg(count),
                                       QMessageBox::Yes | QMessageBox::No);
    if (result != QMessageBox::Yes)
    {
        return;
    }
    // 构建 DELETE
    QString sql = "DELETE FROM alarmlog WHERE 1=1";
    // 时间
    if (chkTime->isChecked())
    {
        sql += " AND StartTime >= :startTime AND StartTime <= :endTime";
    }
    // 控制器
    if (chkDevice->isChecked() && cmbDevice->currentIndex() > 0)
    {
        sql += " AND DeviceName = :deviceName";
    }
    // 探测器
    if (chkNode->isChecked() && cmbNode->currentIndex() > 0)
    {
        sql += " AND NodeName = :nodeName";
    }
    // 报警类型
    if (chkAlarmType->isChecked() && cmbAlarmType->currentIndex() > 0)
    {
        sql += " AND Content = :alarmType";
    }
    // 报警值
    if (chkAlarmValue->isChecked() && !editAlarmValue->text().trimmed().isEmpty())
    {
        sql += " AND NodeValue = :alarmValue";
    }
    // 位置编号
    if (chkPosition->isChecked() && !editPosition->text().trimmed().isEmpty())
    {
        sql += " AND PositionID LIKE :positionId";
    }
    QSqlQuery query(DbHelper::getDatabase("iot.db"));
    query.prepare(sql);
    bindQueryParams(query);

    if (query.exec())
    {
        QMessageBox::information(this, "提示", QString("成功删除 %1 条记录").arg(count));
        // 重置到第一页
        currentPage = 1;
        pageQuery();
        loadComboBoxData();
    }
    else
    {
        QMessageBox::critical(this, "错误", "删除失败：" + query.lastError().text());
    }
}

// 首页槽函数
void PoliceRecord::onFirstPageClicked()
{
    currentPage = 1;
    pageQuery();
}

// 上一页槽函数
void PoliceRecord::onPrevPageClicked()
{
    if (currentPage > 1)
    {
        currentPage--;
        pageQuery();
    }
}

// 下一页槽函数
void PoliceRecord::onNextPageClicked()
{
    if (currentPage < getTotalPage())
    {
        currentPage++;
        pageQuery();
    }
}

// 尾页槽函数
void PoliceRecord::onLastPageClicked()
{
    currentPage = getTotalPage();
    pageQuery();
}
