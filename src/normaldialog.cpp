#include "normaldialog.h"
#include "ui_normaldialog.h"

NormalDialog::NormalDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NormalDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);

    setSizeGripEnabled(true);
}

NormalDialog::~NormalDialog()
{
    delete ui;
}

void NormalDialog::on_pushButtonMin_clicked()
{
    showMinimized();
}
