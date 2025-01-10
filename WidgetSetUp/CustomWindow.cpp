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
    // ���ô�������
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool| Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setFixedSize(50, 50);
    setWindowTitle("Custom Window");

    // ���ò˵�����
    menu->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    menu->setAttribute(Qt::WA_TranslucentBackground);
    menu->setWindowOpacity(0.0); // ��ʼ͸����Ϊ0

    for (QString it: menuLst)
    {
        if (it.isEmpty())
        {
            continue;
        }

        if (it!=QString::fromLocal8Bit("����ά")&& it != QString::fromLocal8Bit("����ά")&& it != QString::fromLocal8Bit("����ά"))
        {
            it += QString::fromLocal8Bit("��������");
        }
        QAction* action = new QAction(it, menu);
        menu->addAction(action);
        connect(action, &QAction::triggered, this, [&]()
            {
                QString txt = qobject_cast<QAction*>(sender())->text().remove(QString::fromLocal8Bit("��������"));
                emit signalWgtName(txt);
            });
    }

    // ��ʼ�����붯��
    fadeInAnimation = new QPropertyAnimation(this, "menuOpacity", this);
    fadeInAnimation->setDuration(300);
    fadeInAnimation->setStartValue(0.0);
    fadeInAnimation->setEndValue(1.0);

    // ��ʼ����������
    fadeOutAnimation = new QPropertyAnimation(this, "menuOpacity", this);
    fadeOutAnimation->setDuration(300);
    fadeOutAnimation->setStartValue(1.0);
    fadeOutAnimation->setEndValue(0.0);
    connect(fadeOutAnimation, &QPropertyAnimation::finished, menu, &QMenu::hide);

    // ��װ�¼�������
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

    // �Զ�����״·��
    QPainterPath path = customShapePath();

    // ������ɫ�ͻ�����״
    painter.fillPath(path, QColor(100, 150, 200, 200));
}

QPainterPath CustomWindow::customShapePath() const
{
    QPainterPath path;
    path.addEllipse(0, 0, 50, 50);
    //path.addRoundedRect(rect(), 30, 30); // �Զ���ΪԲ�Ǿ���
    path.moveTo(width() / 2 - 20, height());
    path.lineTo(width() / 2 + 20, height());
    path.lineTo(width() / 2, height() + 20); // ����·�С����
    path.closeSubpath();
    return path;
}

bool CustomWindow::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == this) {
        if (event->type() == QEvent::Enter) {
            // �����������ڣ���ʾ�˵�
            _isMouseInThis = true;
            showMenu();
        }
        else if (event->type() == QEvent::Leave) 
        {
            // ����뿪�����ڣ�����Ƿ����˵�
            QPoint globalPos = QCursor::pos(); // ��ȡ���ȫ��λ��
            if (!this->geometry().contains(globalPos) && // ������������ // Ҳ���ڲ˵���
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
            // ������˵�����
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
                        // Ҳ���ڲ˵���

                        //QTimer::singleShot(100, this, [this]() {
                        //    if (!_isMouseInThis) hideMenu();
                        //    });
                    }
                   if (!_isMouseInThis) hideMenu();
            });

            //// ����뿪�����ڣ�����Ƿ����˵�
            //QPoint globalPos = QCursor::pos(); 
            //if (!this->geometry().contains(globalPos) && 
            //    !menu->geometry().contains(globalPos))
            //{
            //    _isMouseInThis = false;
            //    // Ҳ���ڲ˵���

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
        m_isDragging = true;  // ��ʼ��ק
        m_dragStartPos = event->globalPosition().toPoint() - frameGeometry().topLeft();
    }
}

void CustomWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (m_isDragging) {
        // �ƶ�����
        move(event->globalPosition().toPoint() - m_dragStartPos);
    }
}

void CustomWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;  // ������ק
    }
}

void CustomWindow::showMenu()
{
    if (fadeOutAnimation->state() == QPropertyAnimation::Running) {
        fadeOutAnimation->stop();
    }

    // ���˵�������ʾ�����������Ͻ�
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
