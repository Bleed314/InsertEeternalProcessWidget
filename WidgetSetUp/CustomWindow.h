#ifndef CUSTOMWINDOW_H
#define CUSTOMWINDOW_H

#include <QWidget>
#include <QMenu>
#include <QPropertyAnimation>
#include <QTimer>
#include <QPainterPath>

class CustomWindow : public QWidget
{
    Q_OBJECT
        Q_PROPERTY(qreal menuOpacity READ menuOpacity WRITE setMenuOpacity)

signals:
    void signalWgtName(QString wgtName);

private slots:
    void slotMueuClick(bool check);

public:
    explicit CustomWindow(const QStringList& menuLst,QWidget* parent = nullptr);

    qreal menuOpacity() const;
    void setMenuOpacity(qreal opacity);

protected:
    void paintEvent(QPaintEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QMenu* menu;
    QPropertyAnimation* fadeInAnimation;
    QPropertyAnimation* fadeOutAnimation;
    bool _isMouseInThis; // �����������
   // bool _isMouseInMenuWgt; // ����ڸ�����

    qreal m_menuOpacity;

    bool m_isDragging;       // �Ƿ�������ק
    QPoint m_dragStartPos;   // ��ק��ʼλ��

    QPainterPath customShapePath() const;

    void showMenu();
    void hideMenu();
};

#endif // CUSTOMWINDOW_H
