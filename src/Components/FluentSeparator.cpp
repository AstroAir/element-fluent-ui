// src/Components/FluentSeparator.cpp
#include "FluentQt/Components/FluentSeparator.h"
#include <QAccessible>
#include <QApplication>
#include <QFontMetrics>
#include <QPainter>
#include "FluentQt/Styling/FluentTheme.h"

namespace FluentQt::Components {

FluentSeparator::FluentSeparator(QWidget* parent)
    : Core::FluentComponent(parent),
      m_animator(std::make_unique<Animation::FluentAnimator>(this)) {
    setupAnimations();
    updateColors();
    updateAccessibility();

    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this,
            &FluentSeparator::updateColors);
}

FluentSeparator::FluentSeparator(Qt::Orientation orientation, QWidget* parent)
    : FluentSeparator(parent) {
    setOrientation(orientation);
}

FluentSeparator::FluentSeparator(const QString& text, QWidget* parent)
    : FluentSeparator(parent) {
    setText(text);
}

FluentSeparator::FluentSeparator(Qt::Orientation orientation,
                                 const QString& text, QWidget* parent)
    : FluentSeparator(orientation, parent) {
    setText(text);
}

FluentSeparator::~FluentSeparator() = default;

Qt::Orientation FluentSeparator::orientation() const { return m_orientation; }

void FluentSeparator::setOrientation(Qt::Orientation orientation) {
    if (m_orientation == orientation)
        return;

    m_orientation = orientation;
    updateGeometry();
    update();
    emit orientationChanged(m_orientation);
}

QString FluentSeparator::text() const { return m_text; }

void FluentSeparator::setText(const QString& text) {
    if (m_text == text)
        return;

    m_text = text;
    updateGeometry();
    update();
    emit textChanged(m_text);
    updateAccessibility();
}

FluentSeparatorStyle FluentSeparator::separatorStyle() const {
    return m_separatorStyle;
}

void FluentSeparator::setSeparatorStyle(FluentSeparatorStyle style) {
    if (m_separatorStyle == style)
        return;

    m_separatorStyle = style;
    update();
    emit separatorStyleChanged(m_separatorStyle);
}

Qt::Alignment FluentSeparator::textAlignment() const { return m_textAlignment; }

void FluentSeparator::setTextAlignment(Qt::Alignment alignment) {
    if (m_textAlignment == alignment)
        return;

    m_textAlignment = alignment;
    update();
    emit textAlignmentChanged(m_textAlignment);
}

int FluentSeparator::thickness() const { return m_thickness; }

void FluentSeparator::setThickness(int thickness) {
    if (m_thickness == thickness)
        return;

    m_thickness = qMax(1, thickness);
    updateGeometry();
    update();
    emit thicknessChanged(m_thickness);
}

int FluentSeparator::spacing() const { return m_spacing; }

void FluentSeparator::setSpacing(int spacing) {
    if (m_spacing == spacing)
        return;

    m_spacing = qMax(0, spacing);
    update();
    emit spacingChanged(m_spacing);
}

QColor FluentSeparator::lineColor() const { return m_lineColor; }

void FluentSeparator::setLineColor(const QColor& color) {
    if (m_lineColor == color)
        return;

    m_lineColor = color;
    m_hasCustomLineColor = true;
    update();
    emit lineColorChanged(m_lineColor);
}

QColor FluentSeparator::textColor() const { return m_textColor; }

void FluentSeparator::setTextColor(const QColor& color) {
    if (m_textColor == color)
        return;

    m_textColor = color;
    m_hasCustomTextColor = true;
    update();
    emit textColorChanged(m_textColor);
}

bool FluentSeparator::isAnimated() const { return m_animated; }

void FluentSeparator::setAnimated(bool animated) {
    if (m_animated == animated)
        return;

    m_animated = animated;
    emit animatedChanged(m_animated);
}

qreal FluentSeparator::opacity() const { return m_opacity; }

void FluentSeparator::setOpacity(qreal opacity) {
    if (qFuzzyCompare(m_opacity, opacity))
        return;

    m_opacity = qBound(0.0, opacity, 1.0);
    update();
    emit opacityChanged(m_opacity);
}

QSize FluentSeparator::sizeHint() const {
    if (m_text.isEmpty()) {
        // Simple line separator
        if (m_orientation == Qt::Horizontal) {
            return QSize(100, m_thickness);
        } else {
            return QSize(m_thickness, 100);
        }
    } else {
        // Text separator
        const QFontMetrics fm(font());
        const QSize textSize = fm.size(Qt::TextSingleLine, m_text);

        if (m_orientation == Qt::Horizontal) {
            const int width = textSize.width() + 2 * m_spacing +
                              40;  // 40px for lines on sides
            const int height = qMax(textSize.height(), m_thickness);
            return QSize(width, height);
        } else {
            const int width = qMax(textSize.width(), m_thickness);
            const int height = textSize.height() + 2 * m_spacing +
                               40;  // 40px for lines on top/bottom
            return QSize(width, height);
        }
    }
}

QSize FluentSeparator::minimumSizeHint() const {
    if (m_text.isEmpty()) {
        if (m_orientation == Qt::Horizontal) {
            return QSize(10, m_thickness);
        } else {
            return QSize(m_thickness, 10);
        }
    } else {
        const QFontMetrics fm(font());
        const QSize textSize = fm.size(Qt::TextSingleLine, m_text);

        if (m_orientation == Qt::Horizontal) {
            return QSize(textSize.width() + 2 * m_spacing,
                         qMax(textSize.height(), m_thickness));
        } else {
            return QSize(qMax(textSize.width(), m_thickness),
                         textSize.height() + 2 * m_spacing);
        }
    }
}

FluentSeparator* FluentSeparator::createHorizontal(QWidget* parent) {
    return new FluentSeparator(Qt::Horizontal, parent);
}

FluentSeparator* FluentSeparator::createVertical(QWidget* parent) {
    return new FluentSeparator(Qt::Vertical, parent);
}

FluentSeparator* FluentSeparator::createWithText(const QString& text,
                                                 QWidget* parent) {
    return new FluentSeparator(text, parent);
}

FluentSeparator* FluentSeparator::createDotted(Qt::Orientation orientation,
                                               QWidget* parent) {
    auto* separator = new FluentSeparator(orientation, parent);
    separator->setSeparatorStyle(FluentSeparatorStyle::Dotted);
    return separator;
}

FluentSeparator* FluentSeparator::createDashed(Qt::Orientation orientation,
                                               QWidget* parent) {
    auto* separator = new FluentSeparator(orientation, parent);
    separator->setSeparatorStyle(FluentSeparatorStyle::Dashed);
    return separator;
}

FluentSeparator* FluentSeparator::createDouble(Qt::Orientation orientation,
                                               QWidget* parent) {
    auto* separator = new FluentSeparator(orientation, parent);
    separator->setSeparatorStyle(FluentSeparatorStyle::Double);
    return separator;
}

void FluentSeparator::fadeIn() {
    if (m_animated && m_fadeAnimation) {
        m_fadeAnimation->stop();
        m_fadeAnimation->setStartValue(m_opacity);
        m_fadeAnimation->setEndValue(1.0);
        m_fadeAnimation->start();
    } else {
        setOpacity(1.0);
    }
}

void FluentSeparator::fadeOut() {
    if (m_animated && m_fadeAnimation) {
        m_fadeAnimation->stop();
        m_fadeAnimation->setStartValue(m_opacity);
        m_fadeAnimation->setEndValue(0.0);
        m_fadeAnimation->start();
    } else {
        setOpacity(0.0);
    }
}

void FluentSeparator::setupAnimations() {
    // Fade animation
    m_fadeAnimation = new QPropertyAnimation(this, "opacity", this);
    m_fadeAnimation->setDuration(300);
    m_fadeAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_fadeAnimation, &QPropertyAnimation::finished, this,
            &FluentSeparator::onFadeAnimationFinished);
}

void FluentSeparator::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(m_opacity);

    if (m_text.isEmpty()) {
        paintLine(&painter);
    } else {
        paintTextSeparator(&painter);
    }
}

void FluentSeparator::paintLine(QPainter* painter) {
    const QRect rect = this->rect();

    QPen pen(m_lineColor, m_thickness);

    switch (m_separatorStyle) {
        case FluentSeparatorStyle::Solid:
            pen.setStyle(Qt::SolidLine);
            break;
        case FluentSeparatorStyle::Dotted:
            pen.setStyle(Qt::DotLine);
            break;
        case FluentSeparatorStyle::Dashed:
            pen.setStyle(Qt::DashLine);
            break;
        case FluentSeparatorStyle::Double:
            pen.setStyle(Qt::SolidLine);
            break;
    }

    pen.setCapStyle(Qt::RoundCap);
    painter->setPen(pen);

    if (m_separatorStyle == FluentSeparatorStyle::Double) {
        // Draw double line
        if (m_orientation == Qt::Horizontal) {
            const int y1 = rect.center().y() - m_thickness / 2;
            const int y2 = rect.center().y() + m_thickness / 2;
            painter->drawLine(rect.left(), y1, rect.right(), y1);
            painter->drawLine(rect.left(), y2, rect.right(), y2);
        } else {
            const int x1 = rect.center().x() - m_thickness / 2;
            const int x2 = rect.center().x() + m_thickness / 2;
            painter->drawLine(x1, rect.top(), x1, rect.bottom());
            painter->drawLine(x2, rect.top(), x2, rect.bottom());
        }
    } else {
        // Draw single line
        if (m_orientation == Qt::Horizontal) {
            const int y = rect.center().y();
            painter->drawLine(rect.left(), y, rect.right(), y);
        } else {
            const int x = rect.center().x();
            painter->drawLine(x, rect.top(), x, rect.bottom());
        }
    }
}

void FluentSeparator::paintTextSeparator(QPainter* painter) {
    const QRect rect = this->rect();
    const QFontMetrics fm(font());
    const QSize textSize = fm.size(Qt::TextSingleLine, m_text);

    // Calculate text position
    QRect textRect;
    if (m_orientation == Qt::Horizontal) {
        int textX;
        if (m_textAlignment & Qt::AlignLeft) {
            textX = rect.left() + m_spacing;
        } else if (m_textAlignment & Qt::AlignRight) {
            textX = rect.right() - textSize.width() - m_spacing;
        } else {
            textX = rect.center().x() - textSize.width() / 2;
        }

        const int textY = rect.center().y() - textSize.height() / 2;
        textRect = QRect(textX, textY, textSize.width(), textSize.height());
    } else {
        const int textX = rect.center().x() - textSize.width() / 2;

        int textY;
        if (m_textAlignment & Qt::AlignTop) {
            textY = rect.top() + m_spacing;
        } else if (m_textAlignment & Qt::AlignBottom) {
            textY = rect.bottom() - textSize.height() - m_spacing;
        } else {
            textY = rect.center().y() - textSize.height() / 2;
        }

        textRect = QRect(textX, textY, textSize.width(), textSize.height());
    }

    // Draw lines
    QPen linePen(m_lineColor, m_thickness);
    linePen.setCapStyle(Qt::RoundCap);
    painter->setPen(linePen);

    if (m_orientation == Qt::Horizontal) {
        const int y = rect.center().y();

        // Left line
        if (textRect.left() > rect.left() + m_spacing) {
            painter->drawLine(rect.left(), y, textRect.left() - m_spacing, y);
        }

        // Right line
        if (textRect.right() < rect.right() - m_spacing) {
            painter->drawLine(textRect.right() + m_spacing, y, rect.right(), y);
        }
    } else {
        const int x = rect.center().x();

        // Top line
        if (textRect.top() > rect.top() + m_spacing) {
            painter->drawLine(x, rect.top(), x, textRect.top() - m_spacing);
        }

        // Bottom line
        if (textRect.bottom() < rect.bottom() - m_spacing) {
            painter->drawLine(x, textRect.bottom() + m_spacing, x,
                              rect.bottom());
        }
    }

    // Draw text
    painter->setPen(m_textColor);
    painter->setFont(font());
    painter->drawText(textRect, Qt::AlignCenter, m_text);
}

void FluentSeparator::onFadeAnimationFinished() {
    // Animation finished
}

void FluentSeparator::updateColors() {
    const auto& theme = Styling::FluentTheme::instance();

    if (!m_hasCustomLineColor) {
        m_lineColor =
            theme.color(Styling::FluentThemeColor::ControlStrokeDefault);
    }

    if (!m_hasCustomTextColor) {
        m_textColor = theme.color(Styling::FluentThemeColor::TextFillSecondary);
    }

    update();
}

void FluentSeparator::updateAccessibility() {
#ifndef QT_NO_ACCESSIBILITY
    if (QAccessible::isActive()) {
        QString accessibleText;
        if (m_text.isEmpty()) {
            accessibleText = tr("Separator");
        } else {
            accessibleText = tr("Separator: %1").arg(m_text);
        }

        setAccessibleName(accessibleText);
        setAccessibleDescription(accessibleText);

        QAccessibleEvent event(this, QAccessible::NameChanged);
        QAccessible::updateAccessibility(&event);
    }
#endif
}

}  // namespace FluentQt::Components
