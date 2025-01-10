#include "customwindow.h"
#include <QVBoxLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPoint>

void CustomWindow::slotMueuClick(bool check)
{

}

CustomWindow::CustomWindow(const QStringList& menuLst, QWidget* parent)
    : QWidget(parent), menu(new QMenu(this)), m_menuOpacity(0.0), _isMouseInThis(false), m_isDragging(false)
{
    // 设置窗口属性
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool| Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setFixedSize(50, 50);
    setWindowTitle("Custom Window");

    // 配置菜单窗口
    menu->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    menu->setAttribute(Qt::WA_TranslucentBackground);
    menu->setWindowOpacity(0.0); // 初始透明度为0

    for (QString it: menuLst)
    {
        if (it.isEmpty())
        {
            continue;
        }

        if (it!=QString::fromLocal8Bit("仅二维")&& it != QString::fromLocal8Bit("仅三维")&& it != QString::fromLocal8Bit("二三维"))
        {
            it += QString::fromLocal8Bit("窗口显隐");
        }
        QAction* action = new QAction(it, menu);
        menu->addAction(action);
        connect(action, &QAction::triggered, this, [&]()
            {
                QString txt = qobject_cast<QAction*>(sender())->text().remove(QString::fromLocal8Bit("窗口显隐"));
                emit signalWgtName(txt);
            });
    }

    // 初始化淡入动画
    fadeInAnimation = new QPropertyAnimation(this, "menuOpacity", this);
    fadeInAnimation->setDuration(300);
    fadeInAnimation->setStartValue(0.0);
    fadeInAnimation->setEndValue(1.0);

    // 初始化淡出动画
    fadeOutAnimation = new QPropertyAnimation(this, "menuOpacity", this);
    fadeOutAnimation->setDuration(300);
    fadeOutAnimation->setStartValue(1.0);
    fadeOutAnimation->setEndValue(0.0);
    connect(fadeOutAnimation, &QPropertyAnimation::finished, menu, &QMenu::hide);

    // 安装事件过滤器
    installEventFilter(this);
    menu->installEventFilter(this);
}

qreal CustomWindow::menuOpacity() const
{
    return m_menuOpacity;
}

void CustomWindow::setMenuOpacity(qreal opacity)
{
    m_menuOpacity = opacity;
    menu->setWindowOpacity(opacity);
    menu->update();
}

void CustomWindow::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 自定义形状路径
    QPainterPath path = customShapePath();

    // 设置颜色和绘制形状
    painter.fillPath(path, QColor(100, 150, 200, 200));
}

QPainterPath CustomWindow::customShapePath() const
{
    QPainterPath path;
    path.addEllipse(0, 0, 50, 50);
    //path.addRoundedRect(rect(), 30, 30); // 自定义为圆角矩形
    path.moveTo(width() / 2 - 20, height());
    path.lineTo(width() / 2 + 20, height());
    path.lineTo(width() / 2, height() + 20); // 添加下方小三角
    path.closeSubpath();
    return path;
}

bool CustomWindow::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == this) {
        if (event->type() == QEvent::Enter) {
            // 鼠标进入主窗口，显示菜单
            _isMouseInThis = true;
            showMenu();
        }
        else if (event->type() == QEvent::Leave) 
        {
            // 鼠标离开主窗口，检查是否进入菜单
            QPoint globalPos = QCursor::pos(); // 获取鼠标全局位置
            if (!this->geometry().contains(globalPos) && // 不在主窗口内 // 也不在菜单内
                !menu->geometry().contains(globalPos))
            { 
                _isMouseInThis = false;
               
            }
            QTimer::singleShot(100, this, [this]() {
                if ( !_isMouseInThis) hideMenu();
                });
        }
    }
    else if (watched == menu) {
        if (event->type() == QEvent::Enter) {
            // 鼠标进入菜单窗口
            _isMouseInThis = true;
        }
        else if (event->type() == QEvent::Leave)
        {
            QTimer::singleShot(100, this, [this]() 
            {
                    QPoint globalPos = QCursor::pos();
                    if (!this->geometry().contains(globalPos) &&
                        !menu->geometry().contains(globalPos))
                    {
                        _isMouseInThis = false;
                        // 也不在菜单内

                        //QTimer::singleShot(100, this, [this]() {
                        //    if (!_isMouseInThis) hideMenu();
                        //    });
                    }
                   if (!_isMouseInThis) hideMenu();
            });

            //// 鼠标离开主窗口，检查是否进入菜单
            //QPoint globalPos = QCursor::pos(); 
            //if (!this->geometry().contains(globalPos) && 
            //    !menu->geometry().contains(globalPos))
            //{
            //    _isMouseInThis = false;
            //    // 也不在菜单内

            //    //QTimer::singleShot(100, this, [this]() {
            //    //    if (!_isMouseInThis) hideMenu();
            //    //    });
            //}

        }
    }

    return QWidget::eventFilter(watched, event);
}

void CustomWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;  // 开始拖拽
        m_dragStartPos = event->globalPosition().toPoint() - frameGeometry().topLeft();
    }
}

void CustomWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (m_isDragging) {
        // 移动窗口
        move(event->globalPosition().toPoint() - m_dragStartPos);
    }
}

void CustomWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;  // 结束拖拽
    }
}

void CustomWindow::showMenu()
{
    if (fadeOutAnimation->state() == QPropertyAnimation::Running) {
        fadeOutAnimation->stop();
    }

    // 将菜单窗口显示在主窗口右上角
    menu->move(this->geometry().topRight() + QPoint(0, 10));
    menu->show();
    fadeInAnimation->start();
}

void CustomWindow::hideMenu()
{
    if (fadeInAnimation->state() == QPropertyAnimation::Running) {
        fadeInAnimation->stop();
    }

    fadeOutAnimation->start();
}
