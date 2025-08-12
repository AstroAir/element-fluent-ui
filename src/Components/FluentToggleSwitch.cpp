// src/Components/FluentToggleSwitch.cpp
#include "FluentQt/Components/FluentToggleSwitch.h"
#include <QApplication>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QtMath>
#include "FluentQt/Styling/FluentTheme.h"

namespace FluentQt::Components {

FluentToggleSwitch::FluentToggleSwitch(QWidget* parent)
    : Core::FluentComponent(parent),
      m_animator(std::make_unique<Animation::FluentAnimator>(this)) {
    setupAnimations();
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover, true);

    updateSizeMetrics();
    updateLayout();

    // Connect to theme changes
    auto& theme = Styling::FluentTheme::instance();
    connect(&theme, &Styling::FluentTheme::themeChanged, this,
            &FluentToggleSwitch::updateColors);
    connect(&theme, &Styling::FluentTheme::accentColorChanged, this,
            &FluentToggleSwitch::updateColors);
}

FluentToggleSwitch::FluentToggleSwitch(const QString& text, QWidget* parent)
    : FluentToggleSwitch(parent) {
    setText(text);
}

FluentToggleSwitch::FluentToggleSwitch(const QString& text, bool checked,
                                       QWidget* parent)
    : FluentToggleSwitch(text, parent) {
    setChecked(checked);
}

FluentToggleSwitch::~FluentToggleSwitch() = default;

bool FluentToggleSwitch::isChecked() const { return m_checked; }

void FluentToggleSwitch::setChecked(bool checked) {
    if (m_checked != checked) {
        m_checked = checked;

        if (m_animated) {
            startToggleAnimation();
        } else {
            m_thumbPosition = checked ? 1.0 : 0.0;
            updateLayout();
            update();
        }

        emit checkedChanged(checked);
        emit toggled(checked);
    }
}

void FluentToggleSwitch::toggle() { setChecked(!m_checked); }

QString FluentToggleSwitch::text() const { return m_text; }

void FluentToggleSwitch::setText(const QString& text) {
    if (m_text != text) {
        m_text = text;
        m_layoutDirty = true;
        updateLayout();
        update();
        emit textChanged(text);
    }
}

QString FluentToggleSwitch::onText() const { return m_onText; }

void FluentToggleSwitch::setOnText(const QString& text) {
    if (m_onText != text) {
        m_onText = text;
        if (m_showStateText) {
            update();
        }
        emit onTextChanged(text);
    }
}

QString FluentToggleSwitch::offText() const { return m_offText; }

void FluentToggleSwitch::setOffText(const QString& text) {
    if (m_offText != text) {
        m_offText = text;
        if (m_showStateText) {
            update();
        }
        emit offTextChanged(text);
    }
}

QIcon FluentToggleSwitch::onIcon() const { return m_onIcon; }

void FluentToggleSwitch::setOnIcon(const QIcon& icon) {
    m_onIcon = icon;
    if (m_showIcons) {
        update();
    }
    emit onIconChanged(icon);
}

QIcon FluentToggleSwitch::offIcon() const { return m_offIcon; }

void FluentToggleSwitch::setOffIcon(const QIcon& icon) {
    m_offIcon = icon;
    if (m_showIcons) {
        update();
    }
    emit offIconChanged(icon);
}

bool FluentToggleSwitch::showIcons() const { return m_showIcons; }

void FluentToggleSwitch::setShowIcons(bool show) {
    if (m_showIcons != show) {
        m_showIcons = show;
        m_layoutDirty = true;
        updateLayout();
        update();
        emit showIconsChanged(show);
    }
}

FluentToggleSwitchSize FluentToggleSwitch::size() const { return m_size; }

void FluentToggleSwitch::setSize(FluentToggleSwitchSize size) {
    if (m_size != size) {
        m_size = size;
        updateSizeMetrics();
        m_layoutDirty = true;
        updateLayout();
        update();
        emit sizeChanged(size);
    }
}

FluentToggleSwitchLabelPosition FluentToggleSwitch::labelPosition() const {
    return m_labelPosition;
}

void FluentToggleSwitch::setLabelPosition(
    FluentToggleSwitchLabelPosition position) {
    if (m_labelPosition != position) {
        m_labelPosition = position;
        m_layoutDirty = true;
        updateLayout();
        update();
        emit labelPositionChanged(position);
    }
}

bool FluentToggleSwitch::showStateText() const { return m_showStateText; }

void FluentToggleSwitch::setShowStateText(bool show) {
    if (m_showStateText != show) {
        m_showStateText = show;
        m_layoutDirty = true;
        updateLayout();
        update();
        emit showStateTextChanged(show);
    }
}

bool FluentToggleSwitch::isAnimated() const { return m_animated; }

void FluentToggleSwitch::setAnimated(bool animated) {
    if (m_animated != animated) {
        m_animated = animated;
        emit animatedChanged(animated);
    }
}

int FluentToggleSwitch::animationDuration() const {
    return m_animationDuration;
}

void FluentToggleSwitch::setAnimationDuration(int duration) {
    if (duration > 0 && m_animationDuration != duration) {
        m_animationDuration = duration;
        emit animationDurationChanged(duration);
    }
}

QSize FluentToggleSwitch::switchSize() const {
    return QSize(m_switchWidth, m_switchHeight);
}

QSize FluentToggleSwitch::thumbSize() const {
    return QSize(m_thumbSize, m_thumbSize);
}

QRect FluentToggleSwitch::switchRect() const { return m_switchRect; }

QRect FluentToggleSwitch::thumbRect() const { return m_thumbRect; }

QRect FluentToggleSwitch::labelRect() const { return m_labelRect; }

QRect FluentToggleSwitch::stateTextRect() const { return m_stateTextRect; }

QString FluentToggleSwitch::currentStateText() const {
    return m_checked ? m_onText : m_offText;
}

QIcon FluentToggleSwitch::currentIcon() const {
    return m_checked ? m_onIcon : m_offIcon;
}

QSize FluentToggleSwitch::sizeHint() const {
    auto& theme = Styling::FluentTheme::instance();
    QFontMetrics fm(theme.bodyFont());

    int totalWidth = m_switchWidth;
    int totalHeight = m_switchHeight;

    // Add space for main label
    if (!m_text.isEmpty() &&
        m_labelPosition != FluentToggleSwitchLabelPosition::None) {
        QRect textRect = fm.boundingRect(m_text);

        if (m_labelPosition == FluentToggleSwitchLabelPosition::Left ||
            m_labelPosition == FluentToggleSwitchLabelPosition::Right) {
            totalWidth += textRect.width() + 8;
            totalHeight = qMax(totalHeight, textRect.height());
        } else {
            totalWidth = qMax(totalWidth, textRect.width());
            totalHeight += textRect.height() + 4;
        }
    }

    // Add space for state text
    if (m_showStateText) {
        QString stateText =
            qMax(m_onText.length(), m_offText.length()) > 0
                ? (m_onText.length() > m_offText.length() ? m_onText
                                                          : m_offText)
                : "Off";
        QRect stateRect = fm.boundingRect(stateText);

        if (m_labelPosition == FluentToggleSwitchLabelPosition::Left ||
            m_labelPosition == FluentToggleSwitchLabelPosition::Right) {
            totalWidth += stateRect.width() + 8;
            totalHeight = qMax(totalHeight, stateRect.height());
        } else {
            totalWidth = qMax(totalWidth, stateRect.width());
            totalHeight += stateRect.height() + 4;
        }
    }

    return QSize(totalWidth, totalHeight);
}

QSize FluentToggleSwitch::minimumSizeHint() const {
    return QSize(m_switchWidth, m_switchHeight);
}

void FluentToggleSwitch::animateToggle() { setChecked(!m_checked); }

void FluentToggleSwitch::setupAnimations() {
    // Initialize animation properties
    m_thumbPosition = m_checked ? 1.0 : 0.0;
    updateColors();
}

void FluentToggleSwitch::updateSizeMetrics() {
    switch (m_size) {
        case FluentToggleSwitchSize::Small:
            m_switchWidth = 32;
            m_switchHeight = 16;
            m_thumbSize = 12;
            m_padding = 2;
            break;
        case FluentToggleSwitchSize::Medium:
            m_switchWidth = 40;
            m_switchHeight = 20;
            m_thumbSize = 16;
            m_padding = 2;
            break;
        case FluentToggleSwitchSize::Large:
            m_switchWidth = 48;
            m_switchHeight = 24;
            m_thumbSize = 20;
            m_padding = 2;
            break;
    }
}

void FluentToggleSwitch::updateLayout() {
    if (!m_layoutDirty)
        return;

    layoutComponents();
    m_layoutDirty = false;
}

void FluentToggleSwitch::updateThumbPosition() {
    m_thumbRect = calculateThumbRect();
    update();
}

void FluentToggleSwitch::startToggleAnimation() {
    if (m_toggleAnimation) {
        m_toggleAnimation->stop();
        m_toggleAnimation->deleteLater();
    }

    m_toggleAnimation = new QParallelAnimationGroup(this);

    // Thumb position animation
    m_thumbAnimation = new QPropertyAnimation(this, "thumbPosition", this);
    m_thumbAnimation->setDuration(m_animationDuration);
    m_thumbAnimation->setStartValue(m_thumbPosition);
    m_thumbAnimation->setEndValue(m_checked ? 1.0 : 0.0);
    m_thumbAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // Color animation
    m_colorAnimation = new QPropertyAnimation(this, "switchColor", this);
    m_colorAnimation->setDuration(m_animationDuration);
    m_colorAnimation->setStartValue(m_switchColor);
    m_colorAnimation->setEndValue(getSwitchColor());
    m_colorAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // Scale animation for press feedback
    if (m_pressed) {
        m_scaleAnimation = new QPropertyAnimation(this, "scale", this);
        m_scaleAnimation->setDuration(m_animationDuration / 2);
        m_scaleAnimation->setStartValue(1.0);
        m_scaleAnimation->setEndValue(0.95);
        m_scaleAnimation->setEasingCurve(QEasingCurve::OutCubic);
        m_toggleAnimation->addAnimation(m_scaleAnimation);
    }

    m_toggleAnimation->addAnimation(m_thumbAnimation);
    m_toggleAnimation->addAnimation(m_colorAnimation);

    connect(m_toggleAnimation, &QParallelAnimationGroup::finished, this,
            &FluentToggleSwitch::onToggleAnimationFinished);

    m_toggleAnimation->start();
}

void FluentToggleSwitch::onToggleAnimationFinished() {
    if (m_toggleAnimation) {
        m_toggleAnimation->deleteLater();
        m_toggleAnimation = nullptr;
    }

    // Reset scale
    if (m_scale != 1.0) {
        m_scale = 1.0;
        update();
    }
}

void FluentToggleSwitch::onThumbPositionChanged() { updateThumbPosition(); }

void FluentToggleSwitch::updateColors() {
    m_switchColor = getSwitchColor();
    update();
}

void FluentToggleSwitch::setThumbPosition(qreal position) {
    if (!qFuzzyCompare(m_thumbPosition, position)) {
        m_thumbPosition = position;
        updateThumbPosition();
    }
}

void FluentToggleSwitch::setSwitchColor(const QColor& color) {
    if (m_switchColor != color) {
        m_switchColor = color;
        update();
    }
}

void FluentToggleSwitch::setScale(qreal scale) {
    if (!qFuzzyCompare(m_scale, scale)) {
        m_scale = scale;
        update();
    }
}

QRect FluentToggleSwitch::calculateSwitchRect() const {
    QRect rect = this->rect();

    switch (m_labelPosition) {
        case FluentToggleSwitchLabelPosition::Left:
            if (!m_text.isEmpty()) {
                auto& theme = Styling::FluentTheme::instance();
                QFontMetrics fm(theme.bodyFont());
                int textWidth = fm.boundingRect(m_text).width();
                rect.setLeft(rect.left() + textWidth + 8);
            }
            break;
        case FluentToggleSwitchLabelPosition::Above:
            if (!m_text.isEmpty()) {
                auto& theme = Styling::FluentTheme::instance();
                QFontMetrics fm(theme.bodyFont());
                int textHeight = fm.height();
                rect.setTop(rect.top() + textHeight + 4);
            }
            break;
        default:
            break;
    }

    // Center the switch in the available space
    int x = rect.left() + (rect.width() - m_switchWidth) / 2;
    int y = rect.top() + (rect.height() - m_switchHeight) / 2;

    return QRect(x, y, m_switchWidth, m_switchHeight);
}

QRect FluentToggleSwitch::calculateThumbRect() const {
    QRect switchRect = m_switchRect;

    // Calculate thumb position based on animation progress
    int thumbX = switchRect.left() + m_padding +
                 (int)((switchRect.width() - m_thumbSize - 2 * m_padding) *
                       m_thumbPosition);
    int thumbY = switchRect.top() + (switchRect.height() - m_thumbSize) / 2;

    return QRect(thumbX, thumbY, m_thumbSize, m_thumbSize);
}

QRect FluentToggleSwitch::calculateLabelRect() const {
    if (m_text.isEmpty() ||
        m_labelPosition == FluentToggleSwitchLabelPosition::None) {
        return QRect();
    }

    auto& theme = Styling::FluentTheme::instance();
    QFontMetrics fm(theme.bodyFont());
    QRect textRect = fm.boundingRect(m_text);
    QRect rect = this->rect();

    switch (m_labelPosition) {
        case FluentToggleSwitchLabelPosition::Left:
            return QRect(rect.left(),
                         rect.top() + (rect.height() - textRect.height()) / 2,
                         textRect.width(), textRect.height());
        case FluentToggleSwitchLabelPosition::Right:
            return QRect(m_switchRect.right() + 8,
                         rect.top() + (rect.height() - textRect.height()) / 2,
                         textRect.width(), textRect.height());
        case FluentToggleSwitchLabelPosition::Above:
            return QRect(rect.left() + (rect.width() - textRect.width()) / 2,
                         rect.top(), textRect.width(), textRect.height());
        case FluentToggleSwitchLabelPosition::Below:
            return QRect(rect.left() + (rect.width() - textRect.width()) / 2,
                         m_switchRect.bottom() + 4, textRect.width(),
                         textRect.height());
        default:
            return QRect();
    }
}

QRect FluentToggleSwitch::calculateStateTextRect() const {
    if (!m_showStateText) {
        return QRect();
    }

    auto& theme = Styling::FluentTheme::instance();
    QFontMetrics fm(theme.captionFont());
    QString stateText = currentStateText();
    QRect textRect = fm.boundingRect(stateText);

    // Position state text inside the switch
    QRect switchRect = m_switchRect;
    int x = switchRect.left() + (switchRect.width() - textRect.width()) / 2;
    int y = switchRect.top() + (switchRect.height() - textRect.height()) / 2;

    return QRect(x, y, textRect.width(), textRect.height());
}

QRect FluentToggleSwitch::calculateIconRect() const {
    if (!m_showIcons || currentIcon().isNull()) {
        return QRect();
    }

    QRect switchRect = m_switchRect;
    int iconSize = qMin(m_thumbSize - 4, 16);
    int x = switchRect.left() + (switchRect.width() - iconSize) / 2;
    int y = switchRect.top() + (switchRect.height() - iconSize) / 2;

    return QRect(x, y, iconSize, iconSize);
}

QPoint FluentToggleSwitch::getThumbPosition(bool checked) const {
    QRect switchRect = m_switchRect;
    int x = checked ? switchRect.right() - m_padding - m_thumbSize
                    : switchRect.left() + m_padding;
    int y = switchRect.top() + (switchRect.height() - m_thumbSize) / 2;

    return QPoint(x, y);
}

QColor FluentToggleSwitch::getSwitchColor() const {
    auto& theme = Styling::FluentTheme::instance();

    if (!isEnabled()) {
        return theme.color("controlStrokeColorDefault");
    }

    if (m_checked) {
        if (m_pressed) {
            return theme.color("accentDark1");
        } else if (m_hovered) {
            return theme.color("accentLight1");
        } else {
            return theme.color("accent");
        }
    } else {
        if (m_pressed) {
            return theme.color("controlStrokeColorDefault").darker(110);
        } else if (m_hovered) {
            return theme.color("controlStrokeColorSecondary");
        } else {
            return theme.color("controlStrokeColorDefault");
        }
    }
}

QColor FluentToggleSwitch::getThumbColor() const {
    auto& theme = Styling::FluentTheme::instance();

    if (!isEnabled()) {
        return theme.color("textFillColorDisabled");
    }

    if (m_checked) {
        return theme.color("textOnAccentFillColorPrimary");
    } else {
        if (m_pressed) {
            return theme.color("textFillColorSecondary");
        } else {
            return theme.color("textFillColorPrimary");
        }
    }
}

QColor FluentToggleSwitch::getTrackColor() const {
    auto& theme = Styling::FluentTheme::instance();

    if (!isEnabled()) {
        return theme.color("controlFillColorDisabled");
    }

    if (m_checked) {
        return getSwitchColor();
    } else {
        if (m_pressed) {
            return theme.color("controlFillColorTertiary");
        } else if (m_hovered) {
            return theme.color("controlFillColorSecondary");
        } else {
            return theme.color("controlFillColorDefault");
        }
    }
}

void FluentToggleSwitch::layoutComponents() {
    m_switchRect = calculateSwitchRect();
    m_thumbRect = calculateThumbRect();
    m_labelRect = calculateLabelRect();
    m_stateTextRect = calculateStateTextRect();
    m_iconRect = calculateIconRect();
}

bool FluentToggleSwitch::hitTestSwitch(const QPoint& position) const {
    return m_switchRect.contains(position);
}

void FluentToggleSwitch::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Apply scale transformation if needed
    if (!qFuzzyCompare(m_scale, 1.0)) {
        painter.save();
        QPointF center = m_switchRect.center();
        painter.translate(center);
        painter.scale(m_scale, m_scale);
        painter.translate(-center);
    }

    paintSwitch(&painter);
    paintThumb(&painter);

    if (m_showIcons && !currentIcon().isNull()) {
        paintIcon(&painter);
    }

    if (m_showStateText && !currentStateText().isEmpty()) {
        paintStateText(&painter);
    }

    if (!qFuzzyCompare(m_scale, 1.0)) {
        painter.restore();
    }

    paintLabel(&painter);

    if (hasFocus()) {
        paintFocusIndicator(&painter);
    }
}

void FluentToggleSwitch::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && hitTestSwitch(event->pos())) {
        m_pressed = true;
        update();
        emit clicked();
    }

    Core::FluentComponent::mousePressEvent(event);
}

void FluentToggleSwitch::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_pressed) {
        m_pressed = false;

        if (hitTestSwitch(event->pos())) {
            toggle();
        }

        update();
    }

    Core::FluentComponent::mouseReleaseEvent(event);
}

void FluentToggleSwitch::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_Space:
        case Qt::Key_Return:
        case Qt::Key_Enter:
            toggle();
            event->accept();
            return;
        default:
            break;
    }

    Core::FluentComponent::keyPressEvent(event);
}

void FluentToggleSwitch::focusInEvent(QFocusEvent* event) {
    update();
    Core::FluentComponent::focusInEvent(event);
}

void FluentToggleSwitch::focusOutEvent(QFocusEvent* event) {
    update();
    Core::FluentComponent::focusOutEvent(event);
}

void FluentToggleSwitch::enterEvent(QEnterEvent* event) {
    m_hovered = true;
    update();
    Core::FluentComponent::enterEvent(event);
}

void FluentToggleSwitch::leaveEvent(QEvent* event) {
    m_hovered = false;
    update();
    Core::FluentComponent::leaveEvent(event);
}

void FluentToggleSwitch::resizeEvent(QResizeEvent* event) {
    m_layoutDirty = true;
    updateLayout();
    Core::FluentComponent::resizeEvent(event);
}

void FluentToggleSwitch::paintSwitch(QPainter* painter) {
    painter->save();

    // Draw track background
    QColor trackColor = getTrackColor();
    painter->setBrush(trackColor);
    painter->setPen(Qt::NoPen);

    int radius = m_switchHeight / 2;
    painter->drawRoundedRect(m_switchRect, radius, radius);

    painter->restore();
}

void FluentToggleSwitch::paintThumb(QPainter* painter) {
    painter->save();

    // Draw thumb
    QColor thumbColor = getThumbColor();
    painter->setBrush(thumbColor);
    painter->setPen(Qt::NoPen);

    int radius = m_thumbSize / 2;
    painter->drawEllipse(m_thumbRect.center(), radius, radius);

    // Add subtle shadow
    if (m_pressed || m_hovered) {
        painter->setBrush(QColor(0, 0, 0, 20));
        QRect shadowRect = m_thumbRect.adjusted(1, 1, 1, 1);
        painter->drawEllipse(shadowRect.center(), radius, radius);
    }

    painter->restore();
}

void FluentToggleSwitch::paintLabel(QPainter* painter) {
    if (m_text.isEmpty() ||
        m_labelPosition == FluentToggleSwitchLabelPosition::None) {
        return;
    }

    painter->save();

    auto& theme = Styling::FluentTheme::instance();
    painter->setFont(theme.bodyFont());
    painter->setPen(theme.color("textFillColorPrimary"));

    painter->drawText(m_labelRect, Qt::AlignLeft | Qt::AlignVCenter, m_text);

    painter->restore();
}

void FluentToggleSwitch::paintStateText(QPainter* painter) {
    painter->save();

    auto& theme = Styling::FluentTheme::instance();
    painter->setFont(theme.captionFont());
    painter->setPen(m_checked ? theme.color("textOnAccentFillColorPrimary")
                              : theme.color("textFillColorSecondary"));

    painter->drawText(m_stateTextRect, Qt::AlignCenter, currentStateText());

    painter->restore();
}

void FluentToggleSwitch::paintIcon(QPainter* painter) {
    QIcon icon = currentIcon();
    if (icon.isNull())
        return;

    painter->save();

    QIcon::Mode mode = isEnabled() ? QIcon::Normal : QIcon::Disabled;
    QIcon::State state = m_checked ? QIcon::On : QIcon::Off;

    icon.paint(painter, m_iconRect, Qt::AlignCenter, mode, state);

    painter->restore();
}

void FluentToggleSwitch::paintFocusIndicator(QPainter* painter) {
    painter->save();

    auto& theme = Styling::FluentTheme::instance();
    QPen focusPen(theme.color("accent"), 2);
    focusPen.setStyle(Qt::DashLine);
    painter->setPen(focusPen);
    painter->setBrush(Qt::NoBrush);

    QRect focusRect = m_switchRect.adjusted(-3, -3, 3, 3);
    int radius = focusRect.height() / 2;
    painter->drawRoundedRect(focusRect, radius, radius);

    painter->restore();
}

}  // namespace FluentQt::Components
