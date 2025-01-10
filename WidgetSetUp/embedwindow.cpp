#include "EmbedWindow.h"
#include <QDebug>
#include <QVBoxLayout>

EmbedWindow::EmbedWindow(QWidget *parent)
    : QWidget(parent), targetHwnd(nullptr), qtHwnd(nullptr)
{
    setWindowTitle("Qt Embed Window");
    setGeometry(100, 100, 800, 600);
}

EmbedWindow::~EmbedWindow()
{
    // 如果需要释放嵌入资源，可以在这里处理
}

bool EmbedWindow::embedWindow(const QString &windowTitle)
{
    // 查找目标窗口
    targetHwnd = FindWindow(nullptr, (LPCWSTR)windowTitle.utf16());
    if (!targetHwnd) {
        qDebug() << "Target window not found!";
        return false;
    }

    // 获取 Qt 窗口句柄
    qtHwnd = reinterpret_cast<HWND>(winId());

    if (!IsWindow(targetHwnd)) {
        qDebug() << "Invalid window handle!";
        return false;
    }

    // 设置目标窗口为 Qt 窗口的子窗口
    //SetParent(targetHwnd, qtHwnd);

    if (!SetParent(targetHwnd, qtHwnd)) {
        qDebug() << "Failed to set parent window!";
        return false;
    }

    // 调整目标窗口的样式
    adjustWindowStyle();

    // 调整目标窗口大小以适配 Qt 窗口
    RECT rect;
    GetClientRect(qtHwnd, &rect);
    qDebug()<<rect.bottom<<rect.left
             <<    rect. right<<    rect. top;

    SetWindowPos(targetHwnd, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top,
                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

    qDebug() << "Successfully embedded the target window!";
    return true;
}

void EmbedWindow::adjustWindowStyle()
{
    if (!targetHwnd) return;

    // 获取目标窗口的样式
    LONG_PTR style = GetWindowLongPtr(targetHwnd, GWL_STYLE);
    style &= ~WS_POPUP; // 移除 WS_POPUP 样式
    style |= WS_CHILD;  // 添加 WS_CHILD 样式
    SetWindowLongPtr(targetHwnd, GWL_STYLE, style);

    // 强制刷新窗口样式
    SetWindowPos(targetHwnd, nullptr, 0, 0, 0, 0,
                 SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
}
