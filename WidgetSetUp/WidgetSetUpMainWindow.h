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

//����λ�úʹ�С��һ����ͳһ��С����ʾ
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

    //�������ļ��л�ȡҪ�����Ĵ���
    QList<MyWidget*> getNeedWgtByReadXml(const QString& xmlPath);

    WgtPosAndSize getWgtInfoByConfigStr(const QString& str);

    //�������ִ���
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
     QString _23DMode;//��ǰ����άģʽ

private:
    Ui::WidgetSetUpMainWindowClass ui;
};


#endif  //WIDGESETUPMAINWINDOWT_H