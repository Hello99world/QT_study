#ifndef DATAQUERY_H
#define DATAQUERY_H

#include <QWidget>

namespace Ui {
class DataQuery;
}

class DataQuery : public QWidget
{
    Q_OBJECT

public:
    explicit DataQuery(QWidget *parent = nullptr);
    ~DataQuery();

private:
    Ui::DataQuery *ui;
};

#endif // DATAQUERY_H
