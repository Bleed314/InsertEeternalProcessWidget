#ifndef MYWEBWIDGET_H
#define MYWEBWIDGET_H

#include <QWidget>

class MyWebWidget  : public QWidget
{
	Q_OBJECT

public:
	MyWebWidget(const QString& webUrl, const QString& wgtName, QWidget*parent);
	~MyWebWidget();

protected:
	void closeEvent(QCloseEvent*) override;
};

#endif //MYWEBWIDGET_H
