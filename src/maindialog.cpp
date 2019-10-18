#include "maindialog.h"
#include "ui_maindialog.h"
#include "popdialog.h"
#include "normaldialog.h"
#include <QDebug>

MainDialog::MainDialog(QWidget *parent)
    : CFramelessDialog(parent)
    , ui(new Ui::MainDialog)
    , m_memberDialog(this) // 此处会导致根据相同控件名绑定的按钮消息混乱
{
    ui->setupUi(this);

    this->setTitleBar(this);
    this->addSubTitleWidget(ui->widgetHeader);
    this->addSubTitleWidget(ui->labelLogo);
    this->addSubTitleWidget(ui->label);
    this->setBorder(1, QColor(200,50,200));

    setSizeGripEnabled(true);
}

MainDialog::~MainDialog()
{
    delete ui;
}

void MainDialog::closeEvent(QCloseEvent *event)
{
    QDialog::closeEvent(event);
}

void MainDialog::on_pushButtonMin_clicked()
{
    showMinimized();
}

void MainDialog::on_pushButtonMax_clicked()
{
    if (isMaximized()) {
        showNormal();
    }
    else {
        showMaximized();
    }
}

void MainDialog::on_pushButtonClose_clicked()
{
    close();
}

void MainDialog::on_pushButtonDialog_clicked()
{
    PopDialog dlg(this);
    dlg.exec();
}

void MainDialog::on_pushButtonDialog_2_clicked()
{
    static PopDialog *dlg = nullptr;
    if (!dlg) {
        dlg = new PopDialog(this);
    }
    dlg->show();
}

void MainDialog::on_pushButtonDialog_3_clicked()
{
    PopDialog dlg(this);
    dlg.setResizeable(false);
    dlg.exec();
}


void MainDialog::on_checkBoxResize_stateChanged(int arg1)
{
    setSizeGripEnabled(arg1 == Qt::Checked);
    this->setResizeable(arg1 == Qt::Checked);
}

void MainDialog::on_checkBoxShowBorder_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) {
        this->setBorder(ui->spinBoxBoderWidth->value(), QColor(0, 0, 230));
    }
    else {
        this->setBorder(0);
    }
}

void MainDialog::on_checkBoxShowShadow_stateChanged(int arg1)
{
//    this->showShadow(arg1 == Qt::Checked);
}

void MainDialog::on_pushButtonFullScreen_clicked()
{
    if (isFullScreen()) {
        showNormal();
        this->setMoveable(ui->checkBoxMove->isChecked());
        this->setResizeable(ui->checkBoxResize->isChecked());
        ui->pushButtonMax->show();
    }
    else {
        showFullScreen();
        this->setMoveable(false);
        this->setResizeable(false);
        ui->pushButtonMax->hide();
    }
}

void MainDialog::on_checkBoxMove_stateChanged(int arg1)
{
    this->setMoveable(arg1 == Qt::Checked);
}

void MainDialog::on_pushButtonDialog_5_clicked()
{
    NormalDialog dlg(this);
    dlg.exec();
}

void MainDialog::on_pushButtonMemDilog_clicked()
{
    m_memberDialog.show();
}
