#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include "frameless/frameless_dialog.h"
#include "popdialog.h"
#include "normaldialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainDialog; }
QT_END_NAMESPACE

class MainDialog : public CFramelessDialog
{
    Q_OBJECT

public:
    MainDialog(QWidget *parent = nullptr);
    ~MainDialog();

    virtual void closeEvent(QCloseEvent *event);

private slots:
    void on_pushButtonMin_clicked();

    void on_pushButtonMax_clicked();

    void on_pushButtonClose_clicked();

    void on_pushButtonDialog_clicked();

    void on_checkBoxResize_stateChanged(int arg1);

    void on_checkBoxShowBorder_stateChanged(int arg1);

    void on_pushButtonFullScreen_clicked();

    void on_checkBoxMove_stateChanged(int arg1);

    void on_pushButtonDialog_2_clicked();

    void on_pushButtonDialog_3_clicked();

    void on_pushButtonDialog_5_clicked();

    void on_pushButtonMemDilog_clicked();

    void on_pushButtonMin_2_clicked();

private:
    Ui::MainDialog *ui;
    NormalDialog m_memberDialog;
};
#endif // MAINDIALOG_H
