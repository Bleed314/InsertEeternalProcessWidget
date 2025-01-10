#include "MyWebWidget.h"

#include <QVBoxLayout>
#include <QWebEngineView>
#include <QUrl>
#include <QEvent>


MyWebWidget::MyWebWidget(const QString& webUrl, const QString& wgtName, QWidget* parent):QWidget(parent)
{
    QVBoxLayout* pLayout = new QVBoxLayout();
    this->setLayout(pLayout);

    QWebEngineView* webEngineView = new QWebEngineView(this);
    pLayout->addWidget(webEngineView);
    webEngineView->load(QUrl(webUrl));

    this->setWindowTitle(wgtName);

}

MyWebWidget::~MyWebWidget()
{

}

void MyWebWidget::closeEvent(QCloseEvent*)
{
    hide();
}



