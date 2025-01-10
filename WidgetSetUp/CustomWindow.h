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
    bool _isMouseInThis; // 鼠标在主窗口
   // bool _isMouseInMenuWgt; // 鼠标在副窗口

    qreal m_menuOpacity;

    bool m_isDragging;       // 是否正在拖拽
    QPoint m_dragStartPos;   // 拖拽起始位置

    QPainterPath customShapePath() const;

    void showMenu();
    void hideMenu();
};

#endif // CUSTOMWINDOW_H
