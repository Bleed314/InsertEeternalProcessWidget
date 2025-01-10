#ifndef DTUWidget_H
#define DTUWidget_H

#include <QWidget>
#include <QProcess>
#include <QTimer>
#include <QWindow>
#include <QPointer>
#include <memory>
#include <QHBoxLayout>

namespace evqt {
    class EVGlobeControl;
    class INetWorkStreaming;
    class EVNNGClient;
    class IEVDesktopInterface;
}

//#define DEUBUG_USE_EDITOR
//#define USE_THRIFT

class DTUWidget : public QWidget
{
    Q_OBJECT

public:
    DTUWidget(QString& exePath, const QString& wgtName, QWidget *parent = nullptr);
    ~DTUWidget();

    //void close();

	/// <summary>
	/// 显示之后需要reszie一次保持视口和Exe大小一致
	/// </summary>
	/// <returns></returns>
	Q_INVOKABLE void resizeParentWindow();

protected:
    void closeEvent(QCloseEvent*) override;

private slots:
    void slotFindWindowComplete();
    void startProcess(QString& strID);

signals:
    void sigFindWindowComplete();
    void sigEmbedComplete();

#ifdef linux
public:
    unsigned long getWinIdFromWindowName();

    bool search(unsigned long window, void* display);
#endif

protected:
	void timerShow();


public:
    //后端运行时程序窗口名
    QString _WindowName;

private:
    QProcess* _process {Q_NULLPTR};
#ifdef _WIN32
    HWND _hwnWindow {Q_NULLPTR};
#endif
    QWidget* _windowWidget {Q_NULLPTR};
    QWindow* _window {Q_NULLPTR};
    QTimer* _timer {Q_NULLPTR};

    QPointer<QHBoxLayout> DTUWidgetHorizontalLayout = nullptr;

    //linux下窗口id
    unsigned long winId = 0;
};
#endif // DTUWidget_H
