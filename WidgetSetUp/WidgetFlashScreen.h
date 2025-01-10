#ifndef WIDGETFLASHSCREEN_H 
#define WIDGETFLASHSCREEN_H

#include <QDialog>
#include <QPixmap>
#include <QFont>
#include <QProgressBar>
#include <QRect>
#include <QLabel>

class  WidgetFlashScreen :public QWidget
{
public:
	static WidgetFlashScreen* GetInstance();

	void SetProgressValue(int nValue);
	int GetProgressValue();

	void SetIconName(QString iconName);
	void SetLableText(QString);

protected:
	virtual void paintEvent(QPaintEvent *);

private:
	WidgetFlashScreen(QWidget* pParent = NULL);
	~WidgetFlashScreen(void);

private:
	int m_nValue;
	QFont m_font;
	QPixmap m_pixmap;
	QProgressBar m_bar;
	QLabel m_lable;
	QRect m_screenRect;
};

#endif //WIDGETFLASHSCREEN_H
