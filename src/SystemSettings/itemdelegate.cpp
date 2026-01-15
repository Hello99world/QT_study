#include "itemdelegate.h"
#include <QApplication>
#include <QCheckBox>
#include <QPainter>
ItemDelegate::ItemDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{}

/**
 * @brief 自定义委托的绘制函数，负责渲染列表/表格中的单元格
 * @param painter 用于绘制的QPainter对象
 * @param option 包含绘制选项的结构体，如单元格状态、尺寸等
 * @param index 指向当前要绘制单元格的模型索引
 *
 * 此函数会根据单元格的状态（选中、悬停等）和数据内容，
 * 使用QPainter绘制自定义组件。
 */
void ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    //获取单元格的数据
    QString data = index.data(Qt::EditRole).toString();
    //复选框是否勾选
    bool checked = (data == "启用");

    // QStyleOptionButton用于定义按钮样式的类，设置绘制按钮所需的参数
    QStyleOptionButton styleOption;
    // 根据checked的值设置复选框的状态，QStyle::State_On/State_Off：开关状态（复选框、单选框）
    styleOption.state = checked ? QStyle::State_On : QStyle::State_Off;


    /**
     * option.rect 是单元格的矩形区域，当前要绘制的单元格在界面上的位置和大小：
        option.rect.x()：单元格左上角的 X 坐标
        option.rect.y()：单元格左上角的 Y 坐标
        option.rect.width()：单元格的宽度
        option.rect.height()：单元格的高度
        option.rect.center()：单元格的中心点坐标（返回一个 QPoint 对象，包含 x() 和 y() 两个属性）
     */

    // 计算复选框大小（适配单元格）
    int size = qMin(option.rect.width(), option.rect.height()) * 0.8; // 缩小一点，避免超出边界
    // 居中绘制（水平+垂直）
    //option.rect.center().x()：单元格水平方向的中心点 X 坐标（比如单元格宽度是 100，中心点 X 就是 50）。
    //size 是复选框的边长,复选框宽度的一半（比如复选框边长是 30，size/2 就是 15）,两者相减，得到的是复选框左上角的 X 坐标
    int x = option.rect.center().x() - size / 2;
    int y = option.rect.center().y() - size / 2;
    styleOption.rect = QRect(x, y, size, size);

    // 用调色板强行改颜色
    QPalette pal = styleOption.palette;
    pal.setColor(QPalette::WindowText, QColor("#0E1A32"));
    pal.setColor(QPalette::ButtonText, QColor("#0E1A32"));
    pal.setColor(QPalette::Button, QColor("#498fbc"));
    pal.setColor(QPalette::Highlight, QColor("#0E1A32"));
    styleOption.palette = pal;
    // 绘制复选框
    QApplication::style()->drawControl(QStyle::CE_CheckBox, &styleOption, painter);
}

//处理 “编辑器相关的所有事件” ,当用户在单元格中操作（比如点击、双击）时，Qt 会将这些事件传递给editorEvent函数
bool ItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{

    // qDebug() << "ok";

    // 鼠标松开事件不需要处理，不然会导致复选框状态的重复切换
    if (event->type() == QEvent::MouseButtonRelease) {
        return false;
    }

    //获取模型中索引项的数据
    QString data = index.data(Qt::EditRole).toString();

    QString checkedText = (data == "启用" ? "禁用" : "启用");

    //更新模型中的数据
    model->setData(index, checkedText, Qt::EditRole);

    //让父类继续处理其他的编辑事件
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QWidget *ItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return nullptr;
}
