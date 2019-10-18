#include "popdialog.h"
#include "ui_popdialog.h"

PopDialog::PopDialog(QWidget *parent) :
    CFramelessDialog(parent),
    ui(new Ui::PopDialog)
{
    ui->setupUi(this);

    this->setTitleBar(this);
    this->addSubTitleWidget(ui->widgetHeader);
    this->setEscForExit(true);
    this->setBorder(1, QColor(50,50,200));

    QPushButton *addBtn = new QPushButton("+", this);
    ui->tabWidget->setCornerWidget(addBtn, Qt::TopLeftCorner);

    setSizeGripEnabled(true);
}

PopDialog::~PopDialog()
{
    delete ui;
}

void PopDialog::setResizeable(bool resize)
{
    ui->pushButtonMin->setVisible(resize);
    ui->pushButtonMax->setVisible(resize);

    CFramelessDialog::setResizeable(resize);
}

void PopDialog::on_pushButtonMin_clicked()
{
    showMinimized();
}

void PopDialog::on_pushButtonMax_clicked()
{
    if (isMaximized()) {
        showNormal();
    }
    else {
        showMaximized();
    }
}

void PopDialog::on_pushButtonClose2_clicked()
{
    close();
}
