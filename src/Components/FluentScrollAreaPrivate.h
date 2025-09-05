// src/Components/FluentScrollAreaPrivate.h
#pragma once

#include <QColor>
#include <QEnterEvent>
#include <QEvent>
#include <QFont>
#include <QMouseEvent>
#include <QObject>
#include <QPaintEvent>
#include <QPainter>
#include <QScrollBar>
#include <QString>
#include <QTimer>
#include <QWidget>

namespace FluentQt::Components {

// Custom scroll bar implementation with Fluent UI design system compliance
class FluentScrollBar : public QScrollBar {
    Q_OBJECT

public:
    explicit FluentScrollBar(Qt::Orientation orientation,
                             QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private slots:
    void updateThemeColors();

private:
    void updateOpacity();
    QRect handleRect() const;

    qreal m_opacity{1.0};
    QString m_accessibleDescription;

    // Cached theme colors for performance
    QColor m_grooveColor;
    QColor m_handleColor;
    QColor m_handleHoverColor;
    QColor m_handlePressedColor;
    QColor m_handleDisabledColor;
};

// Enhanced scroll position indicator with Fluent UI design system compliance
class FluentScrollIndicator : public QWidget {
    Q_OBJECT

public:
    explicit FluentScrollIndicator(QWidget* parent = nullptr);

    void showIndicator(const QString& text, const QPoint& position);

protected:
    void paintEvent(QPaintEvent* event) override;
    QSize sizeHint() const override;

private slots:
    void updateThemeProperties();

private:
    QString m_text;
    QTimer* m_hideTimer{nullptr};

    // Cached theme properties for performance
    QColor m_backgroundColor;
    QColor m_textColor;
    QFont m_font;
    int m_cornerRadius{4};
};

}  // namespace FluentQt::Components
