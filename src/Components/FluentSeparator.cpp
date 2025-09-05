// src/Components/FluentSeparator.cpp
#include "FluentQt/Components/FluentSeparator.h"
#include <QAccessible>
#include <QApplication>
#include <QFontMetrics>
#include <QLinearGradient>
#include <QPainter>
#include "FluentQt/Styling/FluentTheme.h"

namespace FluentQt::Components {

FluentSeparator::FluentSeparator(QWidget* parent)
    : Core::FluentComponent(parent),
      m_animator(std::make_unique<Animation::FluentAnimator>(this)) {
    setupLayout();
    setupAnimations();
    updateColors();
    updateAccessibility();

    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this,
            &FluentSeparator::updateColors);
}

FluentSeparator::FluentSeparator(FluentSeparatorOrientation orientation,
                                 QWidget* parent)
    : FluentSeparator(parent) {
    setOrientation(orientation);
}

FluentSeparator::FluentSeparator(const QString& text, QWidget* parent)
    : FluentSeparator(parent) {
    setText(text);
    setSeparatorType(FluentSeparatorType::Text);
}

FluentSeparator::FluentSeparator(const QIcon& icon, QWidget* parent)
    : FluentSeparator(parent) {
    setIcon(icon);
    setSeparatorType(FluentSeparatorType::Icon);
}

FluentSeparator::FluentSeparator(FluentSeparatorType type, QWidget* parent)
    : FluentSeparator(parent) {
    setSeparatorType(type);
}

FluentSeparator::~FluentSeparator() = default;

// Content properties
QString FluentSeparator::text() const { return m_text; }

void FluentSeparator::setText(const QString& text) {
    if (m_text == text)
        return;

    m_text = text;
    m_layoutDirty = true;
    updateGeometry();
    update();
    emit textChanged(m_text);
    updateAccessibility();
}

QIcon FluentSeparator::icon() const { return m_icon; }

void FluentSeparator::setIcon(const QIcon& icon) {
    if (m_icon.cacheKey() == icon.cacheKey())
        return;

    m_icon = icon;
    m_layoutDirty = true;
    updateGeometry();
    update();
    emit iconChanged(m_icon);
    updateAccessibility();
}

// Type and appearance
FluentSeparatorType FluentSeparator::separatorType() const {
    return m_separatorType;
}

void FluentSeparator::setSeparatorType(FluentSeparatorType type) {
    if (m_separatorType == type)
        return;

    m_separatorType = type;
    m_layoutDirty = true;
    updateGeometry();
    update();
    emit separatorTypeChanged(m_separatorType);
    updateAccessibility();
}

FluentSeparatorOrientation FluentSeparator::orientation() const {
    return m_orientation;
}

void FluentSeparator::setOrientation(FluentSeparatorOrientation orientation) {
    if (m_orientation == orientation)
        return;

    m_orientation = orientation;
    m_layoutDirty = true;
    updateGeometry();
    update();
    emit orientationChanged(m_orientation);
}

FluentSeparatorAlignment FluentSeparator::alignment() const {
    return m_alignment;
}

void FluentSeparator::setAlignment(FluentSeparatorAlignment alignment) {
    if (m_alignment == alignment)
        return;

    m_alignment = alignment;
    m_layoutDirty = true;
    update();
    emit alignmentChanged(m_alignment);
}

FluentSeparatorStyle FluentSeparator::separatorStyle() const {
    return m_separatorStyle;
}

void FluentSeparator::setSeparatorStyle(FluentSeparatorStyle style) {
    if (m_separatorStyle == style)
        return;

    m_separatorStyle = style;
    updateColors();
    update();
    emit separatorStyleChanged(m_separatorStyle);
}

// Color properties
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

// Size properties
int FluentSeparator::lineWidth() const { return m_lineWidth; }

void FluentSeparator::setLineWidth(int width) {
    if (m_lineWidth == width)
        return;

    m_lineWidth = qMax(1, width);
    updateGeometry();
    update();
    emit lineWidthChanged(m_lineWidth);
}

int FluentSeparator::spacing() const { return m_spacing; }

void FluentSeparator::setSpacing(int spacing) {
    if (m_spacing == spacing)
        return;

    m_spacing = qMax(0, spacing);
    m_layoutDirty = true;
    update();
    emit spacingChanged(m_spacing);
}

int FluentSeparator::margins() const {
    // Return the maximum margin for compatibility
    return qMax({m_leftMargin, m_topMargin, m_rightMargin, m_bottomMargin});
}

void FluentSeparator::setMargins(int margins) {
    setMargins(margins, margins, margins, margins);
}

void FluentSeparator::setMargins(int left, int top, int right, int bottom) {
    if (m_leftMargin == left && m_topMargin == top && m_rightMargin == right &&
        m_bottomMargin == bottom)
        return;

    m_leftMargin = qMax(0, left);
    m_topMargin = qMax(0, top);
    m_rightMargin = qMax(0, right);
    m_bottomMargin = qMax(0, bottom);

    m_layoutDirty = true;
    updateGeometry();
    update();
    emit marginsChanged(margins());
}

// Behavior properties
bool FluentSeparator::isAnimated() const { return m_animated; }

void FluentSeparator::setAnimated(bool animated) {
    if (m_animated == animated)
        return;

    m_animated = animated;
    emit animatedChanged(m_animated);
}

bool FluentSeparator::isVisible() const { return QWidget::isVisible(); }

void FluentSeparator::setVisible(bool visible) {
    QWidget::setVisible(visible);
    emit visibilityChanged(visible);
}

// Utility methods
QSize FluentSeparator::contentSize() const {
    updateContentLayout();
    return m_contentRect.size();
}

QRect FluentSeparator::lineRect() const {
    updateContentLayout();
    return m_lineRect;
}

QRect FluentSeparator::textRect() const {
    updateContentLayout();
    return m_textRect;
}

QRect FluentSeparator::iconRect() const {
    updateContentLayout();
    return m_iconRect;
}

bool FluentSeparator::hasContent() const {
    return !m_text.isEmpty() || !m_icon.isNull();
}

bool FluentSeparator::isHorizontal() const {
    return m_orientation == FluentSeparatorOrientation::Horizontal;
}

bool FluentSeparator::isVertical() const {
    return m_orientation == FluentSeparatorOrientation::Vertical;
}

// Size hints with Fluent UI design tokens
QSize FluentSeparator::sizeHint() const {
    const auto& theme = Styling::FluentTheme::instance();
    const int defaultSpacing = theme.spacing("m");    // 8px
    const int minLineLength = theme.spacing("xxxl");  // 24px

    QSize contentSize = calculateContentSize();
    QMargins margins(m_leftMargin, m_topMargin, m_rightMargin, m_bottomMargin);

    if (isHorizontal()) {
        int width = contentSize.width() + margins.left() + margins.right();
        int height = qMax(contentSize.height(), m_lineWidth) + margins.top() +
                     margins.bottom();

        // Ensure minimum line length for visual balance
        if (!hasContent()) {
            width = qMax(width, minLineLength * 4);  // 96px minimum
        }

        return QSize(width, height);
    } else {
        int width = qMax(contentSize.width(), m_lineWidth) + margins.left() +
                    margins.right();
        int height = contentSize.height() + margins.top() + margins.bottom();

        // Ensure minimum line length for visual balance
        if (!hasContent()) {
            height = qMax(height, minLineLength * 4);  // 96px minimum
        }

        return QSize(width, height);
    }
}

QSize FluentSeparator::minimumSizeHint() const {
    const auto& theme = Styling::FluentTheme::instance();
    const int minSpacing = theme.spacing("s");  // 4px

    QSize contentSize = calculateContentSize();
    QMargins margins(m_leftMargin, m_topMargin, m_rightMargin, m_bottomMargin);

    if (isHorizontal()) {
        int width = contentSize.width() + margins.left() + margins.right();
        int height = qMax(contentSize.height(), m_lineWidth) + margins.top() +
                     margins.bottom();

        // Minimum width for line separators
        if (!hasContent()) {
            width = qMax(width, minSpacing * 2);  // 8px minimum
        }

        return QSize(width, height);
    } else {
        int width = qMax(contentSize.width(), m_lineWidth) + margins.left() +
                    margins.right();
        int height = contentSize.height() + margins.top() + margins.bottom();

        // Minimum height for line separators
        if (!hasContent()) {
            height = qMax(height, minSpacing * 2);  // 8px minimum
        }

        return QSize(width, height);
    }
}

QSize FluentSeparator::calculateContentSize() const {
    const auto& theme = Styling::FluentTheme::instance();

    switch (m_separatorType) {
        case FluentSeparatorType::Text:
            return calculateTextSize();
        case FluentSeparatorType::Icon:
            return calculateIconSize();
        case FluentSeparatorType::Line:
        case FluentSeparatorType::Gradient:
        case FluentSeparatorType::Dashed:
        case FluentSeparatorType::Dotted:
        case FluentSeparatorType::Double:
        default:
            return calculateLineSize();
    }
}

// Static convenience methods
FluentSeparator* FluentSeparator::createHorizontalLine(QWidget* parent) {
    auto* separator = new FluentSeparator(parent);
    separator->setOrientation(FluentSeparatorOrientation::Horizontal);
    separator->setSeparatorType(FluentSeparatorType::Line);
    return separator;
}

FluentSeparator* FluentSeparator::createVerticalLine(QWidget* parent) {
    auto* separator = new FluentSeparator(parent);
    separator->setOrientation(FluentSeparatorOrientation::Vertical);
    separator->setSeparatorType(FluentSeparatorType::Line);
    return separator;
}

FluentSeparator* FluentSeparator::createTextSeparator(const QString& text,
                                                      QWidget* parent) {
    auto* separator = new FluentSeparator(text, parent);
    return separator;
}

FluentSeparator* FluentSeparator::createIconSeparator(const QIcon& icon,
                                                      QWidget* parent) {
    auto* separator = new FluentSeparator(icon, parent);
    return separator;
}

FluentSeparator* FluentSeparator::createGradientSeparator(
    FluentSeparatorOrientation orientation, QWidget* parent) {
    auto* separator = new FluentSeparator(parent);
    separator->setOrientation(orientation);
    separator->setSeparatorType(FluentSeparatorType::Gradient);
    return separator;
}

// Public slots
void FluentSeparator::show() {
    if (m_animated) {
        animateIn();
    } else {
        QWidget::show();
    }
}

void FluentSeparator::hide() {
    if (m_animated) {
        animateOut();
    } else {
        QWidget::hide();
    }
}

void FluentSeparator::animateIn() {
    if (m_showAnimation) {
        stopAllAnimations();
        startShowAnimation();
    } else {
        show();
    }
}

void FluentSeparator::animateOut() {
    if (m_hideAnimation) {
        stopAllAnimations();
        startHideAnimation();
    } else {
        hide();
    }
}

void FluentSeparator::fadeIn() {
    if (m_animated && m_fadeAnimation) {
        m_fadeAnimation->stop();
        m_fadeAnimation->setStartValue(m_separatorOpacity);
        m_fadeAnimation->setEndValue(1.0);
        m_fadeAnimation->start();
    } else {
        setSeparatorOpacity(1.0);
    }
}

void FluentSeparator::fadeOut() {
    if (m_animated && m_fadeAnimation) {
        m_fadeAnimation->stop();
        m_fadeAnimation->setStartValue(m_separatorOpacity);
        m_fadeAnimation->setEndValue(0.0);
        m_fadeAnimation->start();
    } else {
        setSeparatorOpacity(0.0);
    }
}

// Protected methods
void FluentSeparator::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(m_separatorOpacity);

    updateContentLayout();

    switch (m_separatorType) {
        case FluentSeparatorType::Line:
            paintLine(&painter);
            break;
        case FluentSeparatorType::Text:
            paintText(&painter);
            break;
        case FluentSeparatorType::Icon:
            paintIcon(&painter);
            break;
        case FluentSeparatorType::Gradient:
            paintGradientLine(&painter);
            break;
        case FluentSeparatorType::Dashed:
            paintDashedLine(&painter);
            break;
        case FluentSeparatorType::Dotted:
            paintDottedLine(&painter);
            break;
        case FluentSeparatorType::Double:
            paintDoubleLine(&painter);
            break;
    }
}

void FluentSeparator::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    m_layoutDirty = true;
    updateContentLayout();
}

void FluentSeparator::changeEvent(QEvent* event) {
    QWidget::changeEvent(event);

    if (event->type() == QEvent::FontChange ||
        event->type() == QEvent::StyleChange) {
        m_layoutDirty = true;
        updateGeometry();
        update();
    }
}

// Private slots
void FluentSeparator::onShowAnimationFinished() { QWidget::show(); }

void FluentSeparator::onHideAnimationFinished() { QWidget::hide(); }

void FluentSeparator::updateColors() {
    const auto& theme = Styling::FluentTheme::instance();

    if (!m_hasCustomLineColor) {
        switch (m_separatorStyle) {
            case FluentSeparatorStyle::Subtle:
                m_lineColor = theme.color("neutralTertiary");
                break;
            case FluentSeparatorStyle::Standard:
                m_lineColor = theme.color("neutralSecondary");
                break;
            case FluentSeparatorStyle::Prominent:
                m_lineColor = theme.color("neutralPrimary");
                break;
            case FluentSeparatorStyle::Accent:
                m_lineColor = theme.color("accent");
                break;
        }
    }

    if (!m_hasCustomTextColor) {
        switch (m_separatorStyle) {
            case FluentSeparatorStyle::Subtle:
                m_textColor = theme.color("neutralSecondary");
                break;
            case FluentSeparatorStyle::Standard:
                m_textColor = theme.color("neutralPrimary");
                break;
            case FluentSeparatorStyle::Prominent:
                m_textColor = theme.color("neutralPrimary");
                break;
            case FluentSeparatorStyle::Accent:
                m_textColor = theme.color("accent");
                break;
        }
    }

    update();
}

// Private methods
void FluentSeparator::setupLayout() {
    // Initialize layout properties
    m_layoutDirty = true;
}

void FluentSeparator::setupAnimations() {
    const auto& theme = Styling::FluentTheme::instance();

    // Fade animation with Fluent UI timing
    m_fadeAnimation = new QPropertyAnimation(this, "separatorOpacity", this);
    m_fadeAnimation->setDuration(200);  // Fluent UI fast animation
    m_fadeAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // Show animation
    m_showAnimation = new QPropertyAnimation(this, "separatorScale", this);
    m_showAnimation->setDuration(300);  // Fluent UI normal animation
    m_showAnimation->setEasingCurve(QEasingCurve::OutBack);
    connect(m_showAnimation, &QPropertyAnimation::finished, this,
            &FluentSeparator::onShowAnimationFinished);

    // Hide animation
    m_hideAnimation = new QPropertyAnimation(this, "separatorScale", this);
    m_hideAnimation->setDuration(200);  // Fluent UI fast animation
    m_hideAnimation->setEasingCurve(QEasingCurve::InCubic);
    connect(m_hideAnimation, &QPropertyAnimation::finished, this,
            &FluentSeparator::onHideAnimationFinished);
}

void FluentSeparator::updateLayout() {
    m_layoutDirty = true;
    updateContentLayout();
}

void FluentSeparator::updateSizeMetrics() {
    const auto& theme = Styling::FluentTheme::instance();

    // Update default values based on theme
    m_defaultSpacing = theme.spacing("m");          // 8px
    m_iconSize = theme.iconSize("medium").width();  // 16px
    m_textPadding = theme.spacing("s");             // 4px
}

void FluentSeparator::updateGeometry() {
    QWidget::updateGeometry();
    m_layoutDirty = true;
}

// Painting methods
void FluentSeparator::paintLine(QPainter* painter) {
    if (m_lineRect.isEmpty())
        return;

    QPen pen = getLinePen();
    painter->setPen(pen);

    if (isHorizontal()) {
        const int y = m_lineRect.center().y();
        painter->drawLine(m_lineRect.left(), y, m_lineRect.right(), y);
    } else {
        const int x = m_lineRect.center().x();
        painter->drawLine(x, m_lineRect.top(), x, m_lineRect.bottom());
    }
}

void FluentSeparator::paintText(QPainter* painter) {
    // Draw lines first
    paintLine(painter);

    // Draw text
    if (!m_text.isEmpty() && !m_textRect.isEmpty()) {
        painter->setPen(getTextColor());
        painter->setFont(getTextFont());
        painter->drawText(m_textRect, Qt::AlignCenter, m_text);
    }
}

void FluentSeparator::paintIcon(QPainter* painter) {
    // Draw lines first
    paintLine(painter);

    // Draw icon
    if (!m_icon.isNull() && !m_iconRect.isEmpty()) {
        QIcon::Mode mode = isEnabled() ? QIcon::Normal : QIcon::Disabled;
        QPixmap pixmap = m_icon.pixmap(m_iconRect.size(), mode);
        painter->drawPixmap(m_iconRect, pixmap);
    }
}

void FluentSeparator::paintGradientLine(QPainter* painter) {
    if (m_lineRect.isEmpty())
        return;

    QLinearGradient gradient;
    const auto& theme = Styling::FluentTheme::instance();

    if (isHorizontal()) {
        gradient.setStart(m_lineRect.left(), m_lineRect.center().y());
        gradient.setFinalStop(m_lineRect.right(), m_lineRect.center().y());
    } else {
        gradient.setStart(m_lineRect.center().x(), m_lineRect.top());
        gradient.setFinalStop(m_lineRect.center().x(), m_lineRect.bottom());
    }

    // Create gradient with fade-in/fade-out effect
    QColor lineColor = getLineColor();
    QColor transparent = lineColor;
    transparent.setAlpha(0);

    gradient.setColorAt(0.0, transparent);
    gradient.setColorAt(0.2, lineColor);
    gradient.setColorAt(0.8, lineColor);
    gradient.setColorAt(1.0, transparent);

    QPen pen(QBrush(gradient), m_lineWidth);
    pen.setCapStyle(Qt::RoundCap);
    painter->setPen(pen);

    if (isHorizontal()) {
        const int y = m_lineRect.center().y();
        painter->drawLine(m_lineRect.left(), y, m_lineRect.right(), y);
    } else {
        const int x = m_lineRect.center().x();
        painter->drawLine(x, m_lineRect.top(), x, m_lineRect.bottom());
    }
}

void FluentSeparator::paintDashedLine(QPainter* painter) {
    if (m_lineRect.isEmpty())
        return;

    QPen pen = getLinePen();
    pen.setStyle(Qt::DashLine);
    painter->setPen(pen);

    if (isHorizontal()) {
        const int y = m_lineRect.center().y();
        painter->drawLine(m_lineRect.left(), y, m_lineRect.right(), y);
    } else {
        const int x = m_lineRect.center().x();
        painter->drawLine(x, m_lineRect.top(), x, m_lineRect.bottom());
    }
}

void FluentSeparator::paintDottedLine(QPainter* painter) {
    if (m_lineRect.isEmpty())
        return;

    QPen pen = getLinePen();
    pen.setStyle(Qt::DotLine);
    painter->setPen(pen);

    if (isHorizontal()) {
        const int y = m_lineRect.center().y();
        painter->drawLine(m_lineRect.left(), y, m_lineRect.right(), y);
    } else {
        const int x = m_lineRect.center().x();
        painter->drawLine(x, m_lineRect.top(), x, m_lineRect.bottom());
    }
}

void FluentSeparator::paintDoubleLine(QPainter* painter) {
    if (m_lineRect.isEmpty())
        return;

    QPen pen = getLinePen();
    painter->setPen(pen);

    const int spacing = qMax(2, m_lineWidth);

    if (isHorizontal()) {
        const int y1 = m_lineRect.center().y() - spacing / 2;
        const int y2 = m_lineRect.center().y() + spacing / 2;
        painter->drawLine(m_lineRect.left(), y1, m_lineRect.right(), y1);
        painter->drawLine(m_lineRect.left(), y2, m_lineRect.right(), y2);
    } else {
        const int x1 = m_lineRect.center().x() - spacing / 2;
        const int x2 = m_lineRect.center().x() + spacing / 2;
        painter->drawLine(x1, m_lineRect.top(), x1, m_lineRect.bottom());
        painter->drawLine(x2, m_lineRect.top(), x2, m_lineRect.bottom());
    }
}

// Animation methods
void FluentSeparator::startShowAnimation() {
    if (m_showAnimation) {
        m_showAnimation->setStartValue(0.0);
        m_showAnimation->setEndValue(1.0);
        m_showAnimation->start();
    }
}

void FluentSeparator::startHideAnimation() {
    if (m_hideAnimation) {
        m_hideAnimation->setStartValue(1.0);
        m_hideAnimation->setEndValue(0.0);
        m_hideAnimation->start();
    }
}

void FluentSeparator::startFadeAnimation(bool fadeIn) {
    if (m_fadeAnimation) {
        m_fadeAnimation->setStartValue(m_separatorOpacity);
        m_fadeAnimation->setEndValue(fadeIn ? 1.0 : 0.0);
        m_fadeAnimation->start();
    }
}

void FluentSeparator::stopAllAnimations() {
    if (m_showAnimation)
        m_showAnimation->stop();
    if (m_hideAnimation)
        m_hideAnimation->stop();
    if (m_fadeAnimation)
        m_fadeAnimation->stop();
}

// Utility methods
QRect FluentSeparator::calculateLineRect() const {
    const QRect contentRect = rect().adjusted(m_leftMargin, m_topMargin,
                                              -m_rightMargin, -m_bottomMargin);

    if (!hasContent()) {
        return contentRect;
    }

    const auto& theme = Styling::FluentTheme::instance();
    const int spacing = theme.spacing("m");

    if (isHorizontal()) {
        QRect textOrIconRect = m_separatorType == FluentSeparatorType::Text
                                   ? calculateTextRect()
                                   : calculateIconRect();

        // Calculate line segments around content
        QRect leftLine, rightLine;

        switch (m_alignment) {
            case FluentSeparatorAlignment::Left:
                rightLine = QRect(
                    textOrIconRect.right() + spacing, contentRect.top(),
                    contentRect.right() - textOrIconRect.right() - spacing,
                    contentRect.height());
                return rightLine;

            case FluentSeparatorAlignment::Right:
                leftLine =
                    QRect(contentRect.left(), contentRect.top(),
                          textOrIconRect.left() - contentRect.left() - spacing,
                          contentRect.height());
                return leftLine;

            case FluentSeparatorAlignment::Center:
            default:
                // Return combined line area for center alignment
                return contentRect;
        }
    } else {
        QRect textOrIconRect = m_separatorType == FluentSeparatorType::Text
                                   ? calculateTextRect()
                                   : calculateIconRect();

        // Calculate line segments around content
        QRect topLine, bottomLine;

        switch (m_alignment) {
            case FluentSeparatorAlignment::Left:  // Top for vertical
                bottomLine = QRect(
                    contentRect.left(), textOrIconRect.bottom() + spacing,
                    contentRect.width(),
                    contentRect.bottom() - textOrIconRect.bottom() - spacing);
                return bottomLine;

            case FluentSeparatorAlignment::Right:  // Bottom for vertical
                topLine = QRect(
                    contentRect.left(), contentRect.top(), contentRect.width(),
                    textOrIconRect.top() - contentRect.top() - spacing);
                return topLine;

            case FluentSeparatorAlignment::Center:
            default:
                // Return combined line area for center alignment
                return contentRect;
        }
    }
}

QRect FluentSeparator::calculateTextRect() const {
    if (m_text.isEmpty())
        return QRect();

    const QFontMetrics fm(getTextFont());
    const QSize textSize = fm.size(Qt::TextSingleLine, m_text);
    const QRect contentRect = rect().adjusted(m_leftMargin, m_topMargin,
                                              -m_rightMargin, -m_bottomMargin);

    QRect textRect;

    if (isHorizontal()) {
        int x;
        switch (m_alignment) {
            case FluentSeparatorAlignment::Left:
                x = contentRect.left() + m_spacing;
                break;
            case FluentSeparatorAlignment::Right:
                x = contentRect.right() - textSize.width() - m_spacing;
                break;
            case FluentSeparatorAlignment::Center:
            default:
                x = contentRect.center().x() - textSize.width() / 2;
                break;
        }

        int y = contentRect.center().y() - textSize.height() / 2;
        textRect = QRect(x, y, textSize.width(), textSize.height());
    } else {
        int x = contentRect.center().x() - textSize.width() / 2;

        int y;
        switch (m_alignment) {
            case FluentSeparatorAlignment::Left:  // Top for vertical
                y = contentRect.top() + m_spacing;
                break;
            case FluentSeparatorAlignment::Right:  // Bottom for vertical
                y = contentRect.bottom() - textSize.height() - m_spacing;
                break;
            case FluentSeparatorAlignment::Center:
            default:
                y = contentRect.center().y() - textSize.height() / 2;
                break;
        }

        textRect = QRect(x, y, textSize.width(), textSize.height());
    }

    return textRect;
}

QRect FluentSeparator::calculateIconRect() const {
    if (m_icon.isNull())
        return QRect();

    const QSize iconSize(m_iconSize, m_iconSize);
    const QRect contentRect = rect().adjusted(m_leftMargin, m_topMargin,
                                              -m_rightMargin, -m_bottomMargin);

    QRect iconRect;

    if (isHorizontal()) {
        int x;
        switch (m_alignment) {
            case FluentSeparatorAlignment::Left:
                x = contentRect.left() + m_spacing;
                break;
            case FluentSeparatorAlignment::Right:
                x = contentRect.right() - iconSize.width() - m_spacing;
                break;
            case FluentSeparatorAlignment::Center:
            default:
                x = contentRect.center().x() - iconSize.width() / 2;
                break;
        }

        int y = contentRect.center().y() - iconSize.height() / 2;
        iconRect = QRect(x, y, iconSize.width(), iconSize.height());
    } else {
        int x = contentRect.center().x() - iconSize.width() / 2;

        int y;
        switch (m_alignment) {
            case FluentSeparatorAlignment::Left:  // Top for vertical
                y = contentRect.top() + m_spacing;
                break;
            case FluentSeparatorAlignment::Right:  // Bottom for vertical
                y = contentRect.bottom() - iconSize.height() - m_spacing;
                break;
            case FluentSeparatorAlignment::Center:
            default:
                y = contentRect.center().y() - iconSize.height() / 2;
                break;
        }

        iconRect = QRect(x, y, iconSize.width(), iconSize.height());
    }

    return iconRect;
}

QRect FluentSeparator::calculateContentRect() const {
    return rect().adjusted(m_leftMargin, m_topMargin, -m_rightMargin,
                           -m_bottomMargin);
}

QColor FluentSeparator::getLineColor() const { return m_lineColor; }

QColor FluentSeparator::getTextColor() const { return m_textColor; }

QFont FluentSeparator::getTextFont() const {
    const auto& theme = Styling::FluentTheme::instance();

    switch (m_separatorStyle) {
        case FluentSeparatorStyle::Subtle:
            return theme.captionFont();
        case FluentSeparatorStyle::Standard:
            return theme.bodyFont();
        case FluentSeparatorStyle::Prominent:
            return theme.bodyLargeFont();
        case FluentSeparatorStyle::Accent:
            return theme.subtitleFont();
        default:
            return theme.bodyFont();
    }
}

QPen FluentSeparator::getLinePen() const {
    QPen pen(getLineColor(), m_lineWidth);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    return pen;
}

void FluentSeparator::layoutHorizontalSeparator() { updateContentLayout(); }

void FluentSeparator::layoutVerticalSeparator() { updateContentLayout(); }

void FluentSeparator::updateContentLayout() const {
    if (!m_layoutDirty)
        return;

    m_contentRect = calculateContentRect();
    m_lineRect = calculateLineRect();
    m_textRect = calculateTextRect();
    m_iconRect = calculateIconRect();

    m_layoutDirty = false;
}

QSize FluentSeparator::calculateTextSize() const {
    if (m_text.isEmpty())
        return QSize();

    const QFontMetrics fm(getTextFont());
    QSize textSize = fm.size(Qt::TextSingleLine, m_text);

    // Add padding around text
    textSize.rwidth() += 2 * m_textPadding;
    textSize.rheight() += 2 * m_textPadding;

    // Add spacing for lines
    const auto& theme = Styling::FluentTheme::instance();
    const int lineSpacing = theme.spacing("l");  // 12px

    if (isHorizontal()) {
        textSize.rwidth() += 2 * lineSpacing;  // Space for lines on sides
    } else {
        textSize.rheight() += 2 * lineSpacing;  // Space for lines on top/bottom
    }

    return textSize;
}

QSize FluentSeparator::calculateIconSize() const {
    if (m_icon.isNull())
        return QSize();

    QSize iconSize(m_iconSize, m_iconSize);

    // Add spacing for lines
    const auto& theme = Styling::FluentTheme::instance();
    const int lineSpacing = theme.spacing("l");  // 12px

    if (isHorizontal()) {
        iconSize.rwidth() += 2 * lineSpacing;  // Space for lines on sides
    } else {
        iconSize.rheight() += 2 * lineSpacing;  // Space for lines on top/bottom
    }

    return iconSize;
}

QSize FluentSeparator::calculateLineSize() const {
    const auto& theme = Styling::FluentTheme::instance();
    const int minSize = theme.spacing("m");  // 8px

    if (isHorizontal()) {
        return QSize(minSize, m_lineWidth);
    } else {
        return QSize(m_lineWidth, minSize);
    }
}

// Property accessors for animations
void FluentSeparator::setSeparatorOpacity(qreal opacity) {
    if (qFuzzyCompare(m_separatorOpacity, opacity))
        return;

    m_separatorOpacity = qBound(0.0, opacity, 1.0);
    update();
}

void FluentSeparator::setSeparatorScale(qreal scale) {
    if (qFuzzyCompare(m_separatorScale, scale))
        return;

    m_separatorScale = qMax(0.0, scale);

    // Apply scale transform
    QTransform transform;
    transform.scale(m_separatorScale, m_separatorScale);
    setTransform(transform);

    update();
}

void FluentSeparator::setAnimatedLineWidth(int width) {
    if (m_animatedLineWidth == width)
        return;

    m_animatedLineWidth = qMax(1, width);
    update();
}

void FluentSeparator::updateAccessibility() {
#ifndef QT_NO_ACCESSIBILITY
    if (QAccessible::isActive()) {
        QString accessibleText;
        QString roleText;

        switch (m_separatorType) {
            case FluentSeparatorType::Text:
                if (!m_text.isEmpty()) {
                    accessibleText = tr("Text separator: %1").arg(m_text);
                    roleText = tr("Text separator");
                } else {
                    accessibleText = tr("Text separator");
                    roleText = tr("Text separator");
                }
                break;
            case FluentSeparatorType::Icon:
                accessibleText = tr("Icon separator");
                roleText = tr("Icon separator");
                break;
            case FluentSeparatorType::Gradient:
                accessibleText = tr("Gradient separator");
                roleText = tr("Gradient separator");
                break;
            case FluentSeparatorType::Dashed:
                accessibleText = tr("Dashed separator");
                roleText = tr("Dashed separator");
                break;
            case FluentSeparatorType::Dotted:
                accessibleText = tr("Dotted separator");
                roleText = tr("Dotted separator");
                break;
            case FluentSeparatorType::Double:
                accessibleText = tr("Double line separator");
                roleText = tr("Double line separator");
                break;
            case FluentSeparatorType::Line:
            default:
                accessibleText = tr("Separator");
                roleText = tr("Separator");
                break;
        }

        // Add orientation information
        if (isHorizontal()) {
            accessibleText += tr(" (horizontal)");
        } else {
            accessibleText += tr(" (vertical)");
        }

        setAccessibleName(accessibleText);
        setAccessibleDescription(roleText);

        QAccessibleEvent event(this, QAccessible::NameChanged);
        QAccessible::updateAccessibility(&event);
    }
#endif
}

}  // namespace FluentQt::Components
