#include "frameless_dialog.h"
#include <windows.h>
#include <winuser.h>
#include <windowsx.h>
#include <QStyle>
#include <QDebug>
#include <QDesktopWidget>
#include <QApplication>

// 使用native窗体，支持拖到桌面边缘放大，但是标题栏会出现闪烁
#define USE_NATIVE_WINDOW   0

CFramelessDialog::CFramelessDialog(QWidget *parent)
    : QDialog((parent))
{
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    setResizeable(m_bResizeable);
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

#if USE_NATIVE_WINDOW
    // 解决native父窗体消息异常问题
    setAttribute(Qt::WA_DontCreateNativeAncestors);
    QApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

    // 设置背景透明，解决标题栏会闪烁问题，但是样式边框不见了!
    //setAttribute(Qt::WA_TranslucentBackground);

    // native window
    HWND hwnd = (HWND)this->winId();
    DWORD style = ::GetWindowLong(hwnd, GWL_STYLE);

    if (m_bResizeable) {
        // 支持系统程序边框，同时也带回了标题栏和边框,在nativeEvent()的WM_NCCALCSIZE会再次去掉标题栏
        ::SetWindowLong(hwnd, GWL_STYLE, style | WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CAPTION);
    } else {
        ::SetWindowLong(hwnd, GWL_STYLE, (style | WS_THICKFRAME | WS_CAPTION) & ~WS_MAXIMIZEBOX);
    }
#endif
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
        setStyleSheet(m_staticSytles + QString("#%1{border:%2px solid #%3;}")
                      .arg(this->objectName()).arg(borderWidth).arg(color.rgba(), 0, 16));
    }
    else {
        setStyleSheet(m_staticSytles + QString("#%1{border:none;}").arg(this->objectName()));
    }

    style()->unpolish(this);
    style()->polish(this);
}

bool CFramelessDialog::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    MSG* msg = reinterpret_cast<MSG*>(message);

    switch (msg->message)
    {
#if USE_NATIVE_WINDOW
    case WM_NCCALCSIZE:
        return true;
    case WM_GETMINMAXINFO:
    {
        if (::IsZoomed(msg->hwnd)) {
            RECT frame = { 0, 0, 0, 0 };
            AdjustWindowRectEx(&frame, WS_OVERLAPPEDWINDOW, FALSE, 0);

            double dpr = this->devicePixelRatioF();

            QMargins mgn;
            if (m_bResizeable) {
                mgn.setLeft(abs(frame.left)/dpr+0.5);
                mgn.setTop(abs(frame.bottom)/dpr+0.5);
                mgn.setRight(abs(frame.right)/dpr+0.5);
                mgn.setBottom(abs(frame.bottom)/dpr+0.5);
            }

            // 最大化时隐藏边框
            QDialog::setContentsMargins(mgn);
            internalShowBorder(false, m_borderWidth, m_borderColor);
            m_bJustMaxSize = true;
        }else {
            if (m_bJustMaxSize) {
                int border = m_borderWidth > 0 ? m_borderWidth : 0;
                QDialog::setContentsMargins(border, border, border, border);
                internalShowBorder(m_borderWidth > 0, m_borderWidth, m_borderColor);
                m_bJustMaxSize = false;
            }
        }
        return false;
    }
#endif
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
