#ifndef NORMALDIALOG_H
#define NORMALDIALOG_H

#include <QDialog>

namespace Ui {
class NormalDialog;
}

class NormalDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NormalDialog(QWidget *parent = nullptr);
    ~NormalDialog();

private slots:
    void on_pushButtonMin_clicked();

private:
    Ui::NormalDialog *ui;
};

#endif // NORMALDIALOG_H
