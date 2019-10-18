#ifndef POPDIALOG_H
#define POPDIALOG_H

#include <QDialog>
#include "frameless/frameless_dialog.h"

namespace Ui {
class PopDialog;
}

class PopDialog : public CFramelessDialog
{
    Q_OBJECT

public:
    explicit PopDialog(QWidget *parent = nullptr);
    ~PopDialog();

    void setResizeable(bool resize);

private slots:
    void on_pushButtonMin_clicked();

    void on_pushButtonMax_clicked();

    void on_pushButtonClose2_clicked();

private:
    Ui::PopDialog *ui;
};

#endif // POPDIALOG_H
