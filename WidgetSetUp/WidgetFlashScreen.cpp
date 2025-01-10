#include "WidgetFlashScreen.h"
#include <QApplication>
#include <QPainter>
#include <QFontMetrics>
#include <QScreen>

//#pragma comment(lib, "BaseUtility.lib")

WidgetFlashScreen* WidgetFlashScreen::GetInstance()
{
	static WidgetFlashScreen widgetFlashScreen;
	return &widgetFlashScreen;
}

WidgetFlashScreen::WidgetFlashScreen(QWidget* pParent)
	:QWidget(pParent)
{
	setWindowFlags(Qt::FramelessWindowHint);
	QString strPath = ("./../icon/flashScreen.png");
	m_pixmap.load(strPath);
	m_nValue = 0;

	// 获取屏幕的大小
	QScreen* screen = QApplication::primaryScreen();
	m_screenRect = screen->availableGeometry();

	setGeometry(m_screenRect);
	m_bar.setParent(this);
	QString strStlyeSheet = " QProgressBar {"
							" border: 2px solid grey;"
							" border-radius: 5px;"
							" text-align: center;"
							" background-color:rgba(36, 31, 72,100);"
							" }"

							" QProgressBar::chunk {"
							" background-color: rgb(0,114,174);"
							" width: 10px;"
							" margin: 0.5px;"
							" }";
	m_bar.setStyleSheet(strStlyeSheet);
	m_bar.setGeometry(0,m_screenRect.height()-80,m_screenRect.width(),30);
	m_bar.setValue(m_nValue);
	m_bar.setTextVisible(false);

	m_lable.setParent(this);
	m_lable.setStyleSheet("font-size:20px;");
	//系统正在初始化......
	m_lable.setText(QString::fromLocal8Bit("系统正在初始化中..."));

	m_lable.setGeometry(10,m_screenRect.height()-124,m_screenRect.width()-10,40);
}
WidgetFlashScreen::~WidgetFlashScreen(void)
{
}
	
void WidgetFlashScreen::SetLableText(QString sValue)
{
	m_lable.setText(sValue);
}

void WidgetFlashScreen::SetProgressValue( int nValue )
{
	m_nValue = nValue;
	m_bar.setValue(nValue);
	update(m_screenRect);
	QApplication::processEvents();
}

void WidgetFlashScreen::paintEvent(QPaintEvent*  paintEvent)
{
	QPainter painter(this);
	QRect rect(0,0,m_screenRect.width(),m_screenRect.height()-20);
	painter.drawPixmap(rect,m_pixmap);
	return QWidget::paintEvent(paintEvent);
}

int WidgetFlashScreen::GetProgressValue()
{
	return m_nValue;
}

void WidgetFlashScreen::SetIconName(QString iconPath)
{
	m_pixmap.load(iconPath);
	repaint();
}
