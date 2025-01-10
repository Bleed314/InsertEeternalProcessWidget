#include "WidgetSetUpMainWindow.h"

//#include "ui_WidgetSetUpMainWindow.h"

#include <QProcess>
#include <QThread>
#include <QString>
#include <QWindow>
#include <QFile>
#include <QDomDocument>
#include <QDebug>
#include <QScreen>

#include "MyWebWidget.h"
#include "CustomWindow.h"
#include "WidgetFlashScreen.h"

WidgetSetUpMainWindow::WidgetSetUpMainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// 获取屏幕的大小
	QScreen* screen = QApplication::primaryScreen();
	QRect screenRect = screen->availableGeometry();
	setGeometry(screenRect);

	WidgetFlashScreen::GetInstance()->show();

	_allWgtName << QString::fromLocal8Bit("仅二维") << QString::fromLocal8Bit("仅三维") << QString::fromLocal8Bit("二三维");

	_23DMode = QString::fromLocal8Bit("二三维");

	WidgetFlashScreen::GetInstance()->SetLableText(QString::fromLocal8Bit("正在获取窗口信息..."));
	WidgetFlashScreen::GetInstance()->SetProgressValue(10);

	QString xmlPtah = "./../config/widget.xml";
	QList<MyWidget*> allWgt = getNeedWgtByReadXml(xmlPtah);
	//网页窗口优先加载
	std::sort(allWgt.begin(), allWgt.end(), [](const MyWidget* a, const MyWidget* b) 
		{
			if ((a->_wgtType== "2D_Map"&& b->_wgtType == "3D_Map")|| (a->_wgtType == "3D_Map" && b->_wgtType == "2D_Map"))
			{
				if (a->_wgtType == "2D_Map")
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			if (b->_wgtType == "WebPage"&& a->_wgtType != "WebPage")
			{
				return false;
			}
			else
			{
				return true;
			}
		});
	_myConfigWgts = allWgt;
	creatWgt(allWgt);
}

WidgetSetUpMainWindow::~WidgetSetUpMainWindow()
{

}

QList<MyWidget*> WidgetSetUpMainWindow::getNeedWgtByReadXml(const QString& xmlPath)
{
	QList<MyWidget*> needAddWgts;

	QFile file(xmlPath);

	// 打开文件
	if (!file.open(QIODevice::ReadOnly)) {
		qWarning() << "Cannot open file for reading";
		return needAddWgts;
	}

	QDomDocument doc;
	if (!doc.setContent(&file)) {
		qWarning() << "Failed to parse XML";
		file.close();
		return needAddWgts;
	}
	file.close();

	// 获取 XML 文档的根元素
	QDomElement root = doc.documentElement();

	// 遍历根元素的子元素
	QDomNodeList nodeList = root.elementsByTagName("dockWidget"); // 假设要遍历标签为 "item" 的节点
	for (int i = 0; i < nodeList.count(); ++i) {
		QDomElement element = nodeList.at(i).toElement();
		if (!element.isNull()) {
			QString wgtType = element.firstChildElement("dockStyle").text();
			QString wgtUrl = element.firstChildElement("url").text();
			QString wgtPos = element.firstChildElement("pos").text();
			QString wgtIfFloat = element.firstChildElement("style").text();
			QString wgtTitle = element.firstChildElement("name").text();//自定义窗口标题
			QString wgtName= element.firstChildElement("wgtName").text();//窗口名称
			MyWidget* wgt = new MyWidget;
			wgt->_wgtType = wgtType;
			wgt->_posStr = wgtPos;
			wgt->_wgtUrl = wgtUrl;
			wgt->_isFloating = (wgtIfFloat == "isFloating") ? 1 : 0;
			wgt->_title = wgtTitle;
			wgt->_wgtName = wgtName;
			needAddWgts.push_back(wgt);
			if (!wgtTitle.isEmpty()&&wgt->_isFloating)
			{
				_allWgtName.push_back(wgtTitle);
			}
		}
	}

	return needAddWgts;
}


WgtPosAndSize WidgetSetUpMainWindow::getWgtInfoByConfigStr(const QString& str)
{
	WgtPosAndSize info;
	QStringList lst = str.split("-");
	if (lst.size() != 4)
	{
		return info;
	}
	info._x = lst[0].toDouble();
	info._y = lst[1].toDouble();
	info._width = lst[2].toDouble();
	info._height = lst[3].toDouble();
	return info;
}

void WidgetSetUpMainWindow::creatWgt(QList<MyWidget*>& wgts)
{
	for (MyWidget* it : wgts)
	{
		WgtPosAndSize wgtInfo = getWgtInfoByConfigStr(it->_posStr);
		if (it->_wgtType == "2D_Map")
		{
			WidgetFlashScreen::GetInstance()->SetProgressValue(30);
			WidgetFlashScreen::GetInstance()->SetLableText(QString::fromLocal8Bit("正在创建二维窗口..."));

			//it->_wgtName为进程窗口名称，可通过进程id获取,在此就不用传入窗口名称
			//由于有些进程启动可能不止一个窗口，通过进程id获取窗口的名称并不一定是你想要获取的窗口
			//建议直接使用窗口名称（可以先通过进程id获取你想要添加窗口的窗口名称）
			_2DWidget = new DTUWidget(it->_wgtUrl,it->_wgtName,this);
			_2DWidget->setFixedSize(wgtInfo._width * this->width(), wgtInfo._height * this->height());
			it->_wgt = _2DWidget;

			if (it->_isFloating)
			{
				_2DWidget->setWindowFlags(Qt::WindowFlags(
					(windowFlags() | Qt::WindowStaysOnTopHint | Qt::WindowCloseButtonHint) &
					~Qt::WindowMinimizeButtonHint &
					~Qt::WindowMaximizeButtonHint));
				_2DWidget->move(wgtInfo._x * this->width() + pos().x(), wgtInfo._y * this->height() + pos().y());
			}
			else
			{
				_2DWidget->move(wgtInfo._x * this->width(), wgtInfo._y * this->height());
			}

			WidgetFlashScreen::GetInstance()->SetProgressValue(50);
			//setFocusPolicy(Qt::StrongFocus);
		}
		else if (it->_wgtType == "3D_Map")
		{
			WidgetFlashScreen::GetInstance()->SetLableText(QString::fromLocal8Bit("正在创建三维窗口..."));

			_3DWidget = new DTUWidget(it->_wgtUrl, it->_wgtName, this);
			QThread::msleep(3000);
			_3DWidget->setFixedSize(wgtInfo._width * this->width(), wgtInfo._height * this->height());
			_3DWidget->move(wgtInfo._x * this->width(), wgtInfo._y * this->height());
			it->_wgt = _3DWidget;

			if (it->_isFloating)
			{
				_3DWidget->setWindowFlags(Qt::WindowFlags(
					(windowFlags() | Qt::WindowStaysOnTopHint | Qt::WindowCloseButtonHint) &
					~Qt::WindowMinimizeButtonHint &
					~Qt::WindowMaximizeButtonHint));
			}

			connect(_3DWidget, &DTUWidget::sigEmbedComplete, this, [&]
				{
					showMaximized();
					_custonWgt=new CustomWindow(_allWgtName,this);
					connect(_custonWgt,&CustomWindow::signalWgtName,this,&WidgetSetUpMainWindow::slotWgtControl);
					_custonWgt->show();

					for (MyWidget* it: _myConfigWgts)
					{
						if (it->_wgt)
						{
							it->_wgt->show();
						}
					}      
					WidgetFlashScreen::GetInstance()->SetProgressValue(100);
					WidgetFlashScreen::GetInstance()->hide();
				});

			WidgetFlashScreen::GetInstance()->SetProgressValue(70);
			WidgetFlashScreen::GetInstance()->show();


		}
		else if (it->_wgtType == "WebPage")
		{
			WidgetFlashScreen::GetInstance()->SetLableText(QString::fromLocal8Bit("正在创建网页窗口..."));

			MyWebWidget* webWgt = new MyWebWidget(it->_wgtUrl,it->_title,this);
			it->_wgt = webWgt;

			webWgt->setFixedSize(wgtInfo._width * this->width(), wgtInfo._height * this->height());

			//webWgt->setWindowFlags(Qt::WindowFlags(windowFlags()|Qt::FramelessWindowHint|           // 无边框窗口
			//	Qt::WindowStaysOnTopHint |           // 窗口置顶
			//	Qt::WindowCloseButtonHint &          // 保留关闭按钮
			//	~Qt::WindowMinimizeButtonHint &      // 移除最小化按钮
			//	~Qt::WindowMaximizeButtonHint));      // 移除最大化按钮

			if (it->_isFloating)
			{
				webWgt->setWindowFlags(Qt::WindowFlags(
					(windowFlags() | Qt::WindowStaysOnTopHint | Qt::WindowCloseButtonHint) &
					~Qt::WindowMinimizeButtonHint &
					~Qt::WindowMaximizeButtonHint));
			}
			else
			{
				
			}
		}
	}
}

void WidgetSetUpMainWindow::slotWgtControl(QString wgtName)
{
	if (_23DMode == wgtName)
	{
		return;
	}
	if (wgtName== QString::fromLocal8Bit("仅二维"))
	{
		_23DMode = wgtName;
		_3DWidget->hide();
		_2DWidget->setFixedSize(this->width(), this->height());
		_2DWidget->move(0,0);
		_2DWidget->show();
	}
	else if (wgtName == QString::fromLocal8Bit("仅三维"))
	{
		_23DMode = wgtName;
		_2DWidget->hide();
		_3DWidget->setFixedSize(this->width(), this->height());
		_3DWidget->move(0, 0);
		_3DWidget->show();
	}
	else if (wgtName == QString::fromLocal8Bit("二三维"))
	{
		_23DMode = wgtName;
		for (MyWidget* it : _myConfigWgts)
		{
			if (it->_wgt)
			{
				if (it->_wgtType != "3D_Map"&& it->_wgtType != "2D_Map")
				{
					continue;
				}
				WgtPosAndSize wgtInfo = getWgtInfoByConfigStr(it->_posStr);
				it->_wgt->setFixedSize(wgtInfo._width * this->width(), wgtInfo._height * this->height());
				if (it->_isFloating)
				{
					QPoint parentPos = this->pos();
					it->_wgt->move(wgtInfo._x * this->width() + parentPos.x(), wgtInfo._y * this->height() + parentPos.y());
					it->_wgt->show();
				}
				else
				{
					it->_wgt->move(wgtInfo._x * this->width(), wgtInfo._y * this->height());
					it->_wgt->show();
				}
			}
		}
	}
	else
	{
		for (MyWidget* it : _myConfigWgts)
		{
			if (it->_title == wgtName)
			{
				if (it->_wgt->isVisible())
				{
					it->_wgt->hide();
				}
				else
				{
					it->_wgt->show();
				}
				return;
			}
		}
	}
}

void WidgetSetUpMainWindow::resizeEvent(QResizeEvent* event)
{
	Q_UNUSED(event);
	bool isMove = true;//二三位窗口是否更新大小
	if (_23DMode == QString::fromLocal8Bit("仅二维")|| _23DMode == QString::fromLocal8Bit("仅三维"))
	{
		isMove = false;
	}
	for (MyWidget* it : _myConfigWgts)
	{
		if (it->_wgt)
		{
			if (!isMove&&(it->_wgtType == "3D_Map" || it->_wgtType == "2D_Map"))
			{
				continue;
			}
			WgtPosAndSize wgtInfo = getWgtInfoByConfigStr(it->_posStr);
			it->_wgt->setFixedSize(wgtInfo._width * this->width(), wgtInfo._height * this->height());
			if (it->_isFloating)
			{
				QPoint parentPos = this->pos();
				it->_wgt->move(wgtInfo._x * this->width() + parentPos.x(), wgtInfo._y * this->height() + parentPos.y());
			}
			else
			{
				it->_wgt->move(wgtInfo._x * this->width(), wgtInfo._y * this->height());
			}
		}
	}
}
