#ifndef EQUIPMENTMONITOR_H
#define EQUIPMENTMONITOR_H

#include <QWidget>

namespace Ui {
class EquipmentMonitor;
}

class EquipmentMonitor : public QWidget
{
    Q_OBJECT

public:
    explicit EquipmentMonitor(QWidget *parent = nullptr);
    ~EquipmentMonitor();

private:
    Ui::EquipmentMonitor *ui;
};

#endif // EQUIPMENTMONITOR_H
