#ifndef EMBEDWINDOW_H
#define EMBEDWINDOW_H

#include <QWidget>
#include <windows.h>

class EmbedWindow : public QWidget
{
    Q_OBJECT

public:
    explicit EmbedWindow(QWidget *parent = nullptr);
    ~EmbedWindow();

    // 嵌入目标窗口
    bool embedWindow(const QString &windowTitle);

private:
    HWND targetHwnd; // 目标窗口句柄
    HWND qtHwnd;     // Qt 窗口句柄

    // 修改窗口样式
    void adjustWindowStyle();
};

#endif // EMBEDWINDOW_H
