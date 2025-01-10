#ifndef WIDGESETUPMAINWINDOWT_H
#define WIDGESETUPMAINWINDOWT_H

#include <QtWidgets/QMainWindow>
#include <QWidget>
#include <QString>
#include <QList>
#include "ui_WidgetSetUpMainWindow.h"

#include "dtuwidget.h"

class CustomWindow;

struct MyWidget
{
    MyWidget()
    {
        _wgt = nullptr;
    }
    QString _wgtType;
    QString _wgtUrl;
    QString _posStr;
    QString _title;
    QString _wgtName;
    bool _isFloating;
    QWidget* _wgt;
};

//窗口位置和大小归一化，统一用小数表示
struct WgtPosAndSize
{
    WgtPosAndSize()
    {
        error = "";
    }
    double _x;
    double _y;
    double _width;
    double _height;
    QString error;

};

class WidgetSetUpMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    WidgetSetUpMainWindow(QWidget* parent = nullptr);
    ~WidgetSetUpMainWindow();

    //从配置文件中获取要创建的窗口
    QList<MyWidget*> getNeedWgtByReadXml(const QString& xmlPath);

    WgtPosAndSize getWgtInfoByConfigStr(const QString& str);

    //创建各种窗口
    void creatWgt(QList<MyWidget*>& wgts);

private slots:
    void slotWgtControl(QString wgtName);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
     DTUWidget *_2DWidget;
     DTUWidget* _3DWidget;
     QList<MyWidget*> _myConfigWgts;
     CustomWindow* _custonWgt;
     QStringList _allWgtName;
     QString _23DMode;//当前二三维模式

private:
    Ui::WidgetSetUpMainWindowClass ui;
};


#endif  //WIDGESETUPMAINWINDOWT_H