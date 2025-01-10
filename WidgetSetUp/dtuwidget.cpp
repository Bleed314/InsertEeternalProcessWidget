#include "dtuwidget.h"

#include <QDebug>
#include <QtConcurrent>
#include <QFileInfo>
#include <QRegularExpression>
#include <QDir>

//放头文件里会报错
#ifdef _WIN32
#include <windows.h>
#endif


//放头文件里会报错
#ifdef linux
#include <X11/Xlib.h>
#include <X11/Xatom.h>

//获取窗口名称
#include <X11/Xutil.h>
#include <QX11Info>

#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#endif


#ifdef linux
static Display* mpDisplay = nullptr;
#endif

DTUWidget::DTUWidget(QString& exePath, const QString& wgtName, QWidget *parent)
    : QWidget(parent)
{
    _WindowName = wgtName;
    

	setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);

    connect(this, &DTUWidget::sigFindWindowComplete, this, &DTUWidget::slotFindWindowComplete);


#ifndef DEUBUG_USE_EDITOR
    startProcess(exePath);
#endif 
}

DTUWidget::~DTUWidget()
{
    if (_process)
    {
        QProcess::ProcessState stat = _process->state();
        if (stat == QProcess::Running)
        {
            _process->kill();
        }
        _process->waitForFinished();
    }

#ifdef linux
    if (mpDisplay)
    {
        XCloseDisplay(mpDisplay);
        mpDisplay = nullptr;
    }
#endif
}

void DTUWidget::slotFindWindowComplete()
{
#ifdef _WIN32
    //嵌入Qt窗口,需要设置焦点接受按键事件
    _window = QWindow::fromWinId((WId)_hwnWindow);
#endif

#ifdef linux
    _window = QWindow::fromWinId(winId);
#endif

    QWidget* windowWidget = QWidget::createWindowContainer(_window, this);
    windowWidget->setParent(this);
    DTUWidgetHorizontalLayout = new QHBoxLayout(this);
    DTUWidgetHorizontalLayout->setContentsMargins(0, 0, 0, 0);
    DTUWidgetHorizontalLayout->setSpacing(0);
    this->setLayout(DTUWidgetHorizontalLayout);
    DTUWidgetHorizontalLayout->addWidget(windowWidget);
    windowWidget->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    windowWidget->setFocusPolicy(Qt::StrongFocus);
    windowWidget->setFocus();
    this->setFocusPolicy(Qt::StrongFocus);
    _windowWidget = windowWidget;

    //_windowWidget->setMinimumSize(500, 200);

    //显示窗口
    this->show();

    //多视口的时候鼠标会被exe锁定出不来
    if (this->parentWidget())
    {
        QList<DTUWidget*> listWidget = this->parentWidget()->findChildren<DTUWidget*>();
        for (auto item : listWidget)
        {
            item->activateWindow();
        }
    }

	emit sigEmbedComplete();
    QMetaObject::invokeMethod(this, "resizeParentWindow", Qt::QueuedConnection);

#ifdef _WIN32
    //将窗口大小设置为_windowWidget大小（否则有时窗口大小不对）
    //QSize WidgetSize = _windowWidget->size();
    //LPARAM WindowSize = (WidgetSize.height() << 16) | (WidgetSize.width() & 0xffff);
    //PostMessage(_hwnWindow, WM_SIZE, 0, WindowSize);
#endif
}


#ifdef _WIN32

//通过进程id获取窗口名称----start
QString windowWgtTitle;

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    DWORD processId;
    GetWindowThreadProcessId(hwnd, &processId);

    // 判断进程ID是否匹配
    if (processId == (DWORD)lParam) {
        // 获取窗口的标题长度
        int length = GetWindowTextLengthW(hwnd);
        if (length > 0) {
            wchar_t* buffer = new wchar_t[length + 1];
            int titleLength = GetWindowTextW(hwnd, buffer, length + 1);  // 获取窗口标题
            if (titleLength > 0) {
                windowWgtTitle = QString::fromWCharArray(buffer);
                qDebug() << "Window Title: " << windowWgtTitle; // 输出窗口标题
            }
            else {
                qDebug() << "Window found but no title.";
            }
            delete[] buffer;
        }
        else {
            qDebug() << "Window found but no title length.";
        }
        return FALSE; // 找到窗口后停止枚举
    }
    return TRUE; // 继续枚举
}

QString getWindowTitleByProcessId(DWORD pid) {
    windowWgtTitle = "";
    EnumWindows(EnumWindowsProc, (LPARAM)pid);
    return windowWgtTitle;
}
//通过进程id获取窗口名称----end


std::vector<HWND> existingHwnWindows = {};
HWND targetHwnWindow = nullptr;

//回调函数的定义
BOOL CALLBACK FindWindowByName(HWND hwnd, LPARAM lParam)
{
    //若是之前启动的程序窗口，则跳过
    if (std::find(existingHwnWindows.begin(), existingHwnWindows.end(), hwnd) != existingHwnWindows.end())
    {
        //继续枚举
        return true;
    }

    LPWSTR lpWindowName = (LPWSTR)lParam;
    LPWSTR lpString = new WCHAR[50];
    if (GetWindowTextW(hwnd, lpString, 50))
    {
        //使用wcsstr函数查找子字符串
        LPWSTR found = wcsstr(lpString, lpWindowName);
        if (found)
        {
            targetHwnWindow = hwnd;

            delete[] lpString;

            //不再继续枚举
            return false;
        }
    }

    delete[] lpString;

    //继续枚举
    return true;
}
#endif

void DTUWidget::startProcess(QString& strID)
{   
    _process = new QProcess;

#ifdef _WIN32
    _process->setCreateProcessArgumentsModifier([](QProcess::CreateProcessArguments *args)
    {
        //进程执行时不显示窗口
        args->startupInfo->wShowWindow = SW_HIDE;
        args->flags = CREATE_NO_WINDOW;
        //输入输出句柄及 wShowWindow 字段有效
        args->startupInfo->dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    });
#endif

#ifdef linux
    QStringList arguments;
    arguments << "--start-minimized";
    _process->start(strID,arguments);
#endif

#ifdef _WIN32
     _process->start(strID);
#endif
    if (! _process->waitForStarted()) {
        qDebug() << "Failed to start process";
        return;
    }


    //_WindowName= getWindowTitleByProcessId(_process->processId());

    QProcess::ProcessState stat = _process->state();
    if (stat != QProcess::NotRunning)
    {
        QFuture<void> future = QtConcurrent::run([this]
        {
#ifdef _WIN32
            targetHwnWindow = nullptr;
            existingHwnWindows.clear();
            if (this->parentWidget())
            {
                QList<DTUWidget*> listWidget = this->parentWidget()->findChildren<DTUWidget*>();
                for (auto item : listWidget)
                {
                    if (item->_hwnWindow)
                    {
                        existingHwnWindows.push_back(item->_hwnWindow);
                    }
                }
            }

            while(true)
            {
                //通过窗口名称查找
                EnumWindows((WNDENUMPROC)FindWindowByName, (LPARAM)(_WindowName.toStdWString().c_str()));
                _hwnWindow = targetHwnWindow;

                if (_hwnWindow != NULL)
                {
                    //激活窗口并将其显示为最小化窗口
                    ShowWindow(_hwnWindow, SW_SHOWMINIMIZED);

                    qDebug()<<("Find Window");
                    emit sigFindWindowComplete();
                    break;
                }
            }
#endif

#ifdef linux
            while(true)
            {
                unsigned long mywinid = getWinIdFromWindowName();
                if (mywinid)
                {
                    qDebug()<<("Find Window   ")<<mywinid;

                    emit sigFindWindowComplete();
                    break;
                }
            }
#endif

        });
    }
    else
    {
        this->show();
        if (this->parentWidget())
        {
            this->parentWidget()->show();
        }
    }
}

//==================================show===============================
void DTUWidget::timerShow()
{
    if (_windowWidget)
    {

#ifdef _WIN32
        if (GetForegroundWindow() == _hwnWindow || this->isActiveWindow())
        {
            //窗口和_windowWidget的大小不一样时，设置窗口大小
            LPRECT Rect = new RECT();
            if (GetWindowRect(_hwnWindow, Rect))
            {
                int width = Rect->right - Rect->left;
                int height = Rect->bottom - Rect->top;

                if (_windowWidget->width() != width || _windowWidget->height() != height)
                {
                    //resize后窗口大小会正常
                    QSize size = this->size();
                    this->resize(size - QSize(1, 0));
                    this->resize(size);
                }
            }

            /*挂载为Qt子窗口后，SWindow::GetPositionInScreen()中返回的坐标是错误的，
            SWindow::GetPositionInScreen()中的ScreenPosition的值会由于WM_MOVE更新为0，
            原因：SetParent后，SWindow就无法接收到WM_MOVE消息了，所以要手动发送WM_MOVE消息给窗口*/
            //将窗口位置移到_windowWidget坐标
            QPoint WidgetPos = _windowWidget->pos();
            QPoint WidgetWindowPos = mapToGlobal(WidgetPos);
            LPARAM MoveParam = (WidgetWindowPos.y() << 16) | (WidgetWindowPos.x() & 0xffff);
            PostMessage(_hwnWindow, WM_MOVE, 0, MoveParam);//SendMessage(_hwnWindow, WM_MOVE, 0, MoveParam);

            //将鼠标位置设为当前鼠标的位置（有几次发现鼠标位置不对，但后面测试没有复现，不知道有没有用）
            QPoint CursorPos = this->cursor().pos();
            LPARAM MouseMoveParam = (CursorPos.y() << 16) | (CursorPos.x() & 0xffff);
            PostMessage(_hwnWindow, WM_MOUSEMOVE, 0, MouseMoveParam);

            //把焦点设在窗口后，才会接收按键事件
            //增加矩形判断适配多视口
			QPoint cursorPos = QCursor::pos();
			QRect widRect(WidgetWindowPos, QSize(_windowWidget->size()));
            if (widRect.contains(cursorPos))
            {
				SetFocus(_hwnWindow);
            }

            delete Rect;
        }
#endif

    }
}

#ifdef linux
unsigned long DTUWidget::getWinIdFromWindowName()
{
    Display* display = XOpenDisplay(nullptr);
    if (!display)
    {
        qDebug() << "Error: Unable to open X Display";
        return 0;
    }

    qDebug() << "Searching for window: " << _WindowName;

    Window wRoot = DefaultRootWindow(display);
    bool found = search(wRoot, display);

    qDebug() << (found ? "Window found, ID:" : "Window not found") << winId;

    XCloseDisplay(display);
    return winId;
}

bool DTUWidget::search(unsigned long window, void* display)
{
    if (!display)
    {
        return false;
    }

    Display* pdisplay = static_cast<Display*>(display);

    char* _WindowName = nullptr;
    if (XFetchName(pdisplay, window, &_WindowName) > 0 && _WindowName && strstr(_WindowName, _WindowName.toUtf8().data()) != nullptr)
    {
        winId = window;
        XFree(_WindowName);
        return true;  // 找到目标窗口，退出递归
    }
    if (_WindowName) XFree(_WindowName);

    Window wRoot, wParent;
    Window* wChild;
    unsigned int nChildren;
    if (XQueryTree(pdisplay, window, &wRoot, &wParent, &wChild, &nChildren))
    {
        for (unsigned int i = 0; i < nChildren; i++)
        {
            if (search(wChild[i], display))  // 如果找到目标窗口，直接返回
            {
                XFree(wChild);
                return true;
            }
        }

        XFree(wChild);
    }

    return false;
}
#endif


void DTUWidget::resizeParentWindow()
{
    QWidget* pParentWidget = parentWidget();
    if (pParentWidget)
    {
		//resize一次exe和视口大小一致
		QSize size = pParentWidget->size();
		pParentWidget->resize(size - QSize(1, 0));
		pParentWidget->resize(size);
    }
}

void DTUWidget::closeEvent(QCloseEvent*)
{
    hide();
}

