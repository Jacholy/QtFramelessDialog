#include "frameless_dialog.h"
#include <windows.h>
#include <winuser.h>
#include <windowsx.h>
#include <QStyle>
#include <QDebug>
#include <QDesktopWidget>
#include <QApplication>

CFramelessDialog::CFramelessDialog(QWidget *parent)
    : QDialog((parent))
{
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    limitMaximumSize();
}

void CFramelessDialog::limitMaximumSize()
{
    // 限制主窗体最大的大小
    QDesktopWidget * desk = QApplication::desktop();
    if(desk){
        QRect rc = desk->availableGeometry();
        this->setMaximumSize(rc.width(), rc.height());
    }
}

void CFramelessDialog::setTitleBar(QWidget* titlebar)
{
    m_titlebar = titlebar;
    if (titlebar) {
        connect(titlebar, SIGNAL(destroyed(QObject*)), this, SLOT(onTitleBarDestroyed()));
    }
}

void CFramelessDialog::onTitleBarDestroyed()
{
    if (m_titlebar == QObject::sender()) {
        m_titlebar = Q_NULLPTR;
    }
}

void CFramelessDialog::addSubTitleWidget(QWidget* widget)
{
    if (!widget) return;
    if (m_whiteList.contains(widget)) return;
    m_whiteList.append(widget);
}

void CFramelessDialog::setResizeable(bool resizeable)
{
    m_bResizeable = resizeable;
}

void CFramelessDialog::setMoveable(bool move)
{
    m_bMoveable = move;
}

void CFramelessDialog::setEscForExit(bool escExit)
{
    m_bEscForExit = escExit;
}

void CFramelessDialog::setBorder(int borderWidth, QColor borderColor)
{
    m_borderWidth = borderWidth;
    m_borderColor = borderColor;
    m_deactiveBorderColor = m_borderColor;
    m_deactiveBorderColor.setAlphaF(0.5);

    if (borderWidth > 0) {
        setContentsMargins(borderWidth,borderWidth,borderWidth,borderWidth);
        internalShowBorder(true, m_borderWidth, m_borderColor);
    }
    else {
        setContentsMargins(0,0,0,0);
        internalShowBorder(false, m_borderWidth, m_borderColor);
    }
}

void CFramelessDialog::internalShowBorder(bool show, int borderWidth, QColor color)
{
    if (m_staticSytles.isEmpty()) {
        m_staticSytles = styleSheet();
    }

    if (show && borderWidth > 0) {
        setStyleSheet(m_staticSytles + QString("QDialog{border:%1px solid #%2;}")
                      .arg(borderWidth).arg(color.rgba(), 0, 16));
    }
    else {
        setStyleSheet(m_staticSytles + "QDialog{border:none;}");
    }

    style()->unpolish(this);
    style()->polish(this);
}

bool CFramelessDialog::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    MSG* msg = reinterpret_cast<MSG*>(message);

    switch (msg->message)
    {
    case WM_NCHITTEST:
    {
        *result = 0;

        const LONG border_width = m_resizeAreaWidth;
        RECT winrect;
        GetWindowRect(HWND(winId()), &winrect);

        long x = GET_X_LPARAM(msg->lParam);
        long y = GET_Y_LPARAM(msg->lParam);

        if(m_bResizeable && !isMaximized())
        {
            bool resizeWidth = minimumWidth() != maximumWidth();
            bool resizeHeight = minimumHeight() != maximumHeight();

            if(resizeWidth)
            {
                //left border
                if (x >= winrect.left && x < winrect.left + border_width)
                {
                    *result = HTLEFT;
                }
                //right border
                if (x < winrect.right && x >= winrect.right - border_width)
                {
                    *result = HTRIGHT;
                }
            }
            if(resizeHeight)
            {
                //bottom border
                if (y < winrect.bottom && y >= winrect.bottom - border_width)
                {
                    *result = HTBOTTOM;
                }
                //top border
                if (y >= winrect.top && y < winrect.top + border_width)
                {
                    *result = HTTOP;
                }
            }
            if(resizeWidth && resizeHeight)
            {
                //bottom left corner
                if (x >= winrect.left && x < winrect.left + border_width &&
                        y < winrect.bottom && y >= winrect.bottom - border_width)
                {
                    *result = HTBOTTOMLEFT;
                }
                //bottom right corner
                if (x < winrect.right && x >= winrect.right - border_width &&
                        y < winrect.bottom && y >= winrect.bottom - border_width)
                {
                    *result = HTBOTTOMRIGHT;
                }
                //top left corner
                if (x >= winrect.left && x < winrect.left + border_width &&
                        y >= winrect.top && y < winrect.top + border_width)
                {
                    *result = HTTOPLEFT;
                }
                //top right corner
                if (x < winrect.right && x >= winrect.right - border_width &&
                        y >= winrect.top && y < winrect.top + border_width)
                {
                    *result = HTTOPRIGHT;
                }
            }
        }

        if (0 != *result) {
            return true;
        }

        if (m_bMoveable) {
            if (!m_titlebar) {
                return false;
            }

            //support highdpi
            double dpr = this->devicePixelRatioF();
            QPoint pos = m_titlebar->mapFromGlobal(QPoint(x/dpr, y/dpr));

            if (!m_titlebar->rect().contains(pos)) return false;
            QWidget* child = m_titlebar->childAt(pos);
            if (!child) {
                *result = HTCAPTION;
                return true;
            }else{
                if (m_whiteList.contains(child))
                {
                    *result = HTCAPTION;
                    return true;
                }
            }
        }
        return false;
    }
    case WM_NCLBUTTONDBLCLK:
    {
        if (!m_bMoveable || !m_bResizeable) {
            break;
        }

        if (isMaximized()) {
            showNormal();
        }
        else {
            showMaximized();
        }
        return true;
    }
    case WM_NCACTIVATE:
    {
        if (/*isModal() &&*/ m_borderWidth > 0) {
            unsigned int bactive = msg->wParam;
            if (bactive) {
                internalShowBorder(true, m_borderWidth, m_borderColor);
            }
            else {
                internalShowBorder(true, m_borderWidth, m_deactiveBorderColor);
            }
        }
        break;
    }
    }

    return QDialog::nativeEvent(eventType, message, result);
}

void CFramelessDialog::resizeEvent(QResizeEvent *event)
{
    // 防止虚拟机中屏幕大小变化
    limitMaximumSize();

    QDialog::resizeEvent(event);
}

void CFramelessDialog::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape) {
        if (!m_bEscForExit) {
            return;
        }
    }

    QDialog::keyPressEvent(e);
}
