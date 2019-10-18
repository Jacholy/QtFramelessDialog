#ifndef FRAMELESSSDIALOG_H
#define FRAMELESSSDIALOG_H

#include <QDialog>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QList>

class CFramelessDialog : public QDialog
{
    Q_OBJECT
public:
    CFramelessDialog(QWidget *parent);

    // 设置标题栏, 可以拖拽和双击，可以设置为自己
    void setTitleBar(QWidget* titlebar);

    // 添加标题栏的子控件也可拖拽
    void addSubTitleWidget(QWidget* widget);

    // 设置是否可改变窗体大小
    void setResizeable(bool resizeable);

    // 设置是否可以移动窗体
    void setMoveable(bool move);

    // 设置是否可以通过Esc键退出
    void setEscForExit(bool escExit);

    // 设置边框，borderWidth为0时取消边框
    void setBorder(int borderWidth, QColor borderColor = QColor(0,0,100));

private:
    void limitMaximumSize();
    void internalShowBorder(bool show, int borderWidth, QColor color);

private slots:
    void onTitleBarDestroyed();

protected:
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *e) override;

protected:
    QWidget* m_titlebar = nullptr;
    QList<QWidget*> m_whiteList;
    int m_resizeAreaWidth = 5;
    bool m_bResizeable = true;
    bool m_bEscForExit = false;
    bool m_bMoveable = true;
    int m_borderWidth = 0;
    QColor m_borderColor;
    QColor m_deactiveBorderColor;
    QString m_staticSytles;
};

#endif // FRAMELESSSDIALOG_H
