#include "common.h"
#include "ui_common.h"

Common::Common(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Common)
{
    ui->setupUi(this);
}

Common::~Common()
{
    delete ui;
}
