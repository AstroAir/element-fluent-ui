// src/Components/FluentRadioButton.cpp
#include "FluentQt/Components/FluentRadioButton.h"
#include <QAccessible>
#include <QApplication>
#include <QButtonGroup>
#include <QFocusEvent>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include "FluentQt/Styling/FluentTheme.h"

namespace FluentQt::Components {

FluentRadioButton::FluentRadioButton(QWidget* parent)
    : QAbstractButton(parent),
      m_animator(std::make_unique<Animation::FluentAnimator>(this)) {
    setupAnimations();
    setupButtonGroup();
    updateSizeMetrics();
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_Hover, true);

    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this,
            &FluentRadioButton::updateColors);

    updateColors();
    updateAccessibility();
}

FluentRadioButton::FluentRadioButton(const QString& text, QWidget* parent)
    : FluentRadioButton(parent) {
    setText(text);
}

FluentRadioButton::FluentRadioButton(const QString& text, const QString& value,
                                     QWidget* parent)
    : FluentRadioButton(text, parent) {
    setValue(value);
}

FluentRadioButton::~FluentRadioButton() {
    if (m_ownButtonGroup && m_buttonGroup) {
        delete m_buttonGroup;
    }
}

bool FluentRadioButton::isChecked() const { return m_checked; }

void FluentRadioButton::setChecked(bool checked) {
    if (m_checked == checked)
        return;

    m_checked = checked;

    if (m_checked && m_autoExclusive && m_buttonGroup) {
        // Uncheck other radio buttons in the group
        for (auto* button : m_buttonGroup->buttons()) {
            if (auto* radioButton = qobject_cast<FluentRadioButton*>(button)) {
                if (radioButton != this && radioButton->isChecked()) {
                    radioButton->setChecked(false);
                }
            }
        }
    }

    if (m_animated) {
        startCheckAnimation();
    } else {
        m_indicatorOpacity = m_checked ? 1.0 : 0.0;
        update();
    }

    emit checkedChanged(m_checked);
    emit toggled(m_checked);
    updateAccessibility();
}

void FluentRadioButton::toggle() {
    if (!m_checked) {  // Radio buttons can only be turned on, not off
        setChecked(true);
    }
}

QString FluentRadioButton::text() const { return m_text; }

void FluentRadioButton::setText(const QString& text) {
    if (m_text == text)
        return;

    m_text = text;
    m_layoutDirty = true;
    updateGeometry();
    update();
    emit textChanged(m_text);
    updateAccessibility();
}

QString FluentRadioButton::value() const {
    return m_value.isEmpty() ? m_text : m_value;
}

void FluentRadioButton::setValue(const QString& value) {
    if (m_value == value)
        return;

    m_value = value;
    emit valueChanged(m_value);
}

QIcon FluentRadioButton::icon() const { return m_icon; }

void FluentRadioButton::setIcon(const QIcon& icon) {
    if (m_icon.cacheKey() == icon.cacheKey())
        return;

    m_icon = icon;
    m_layoutDirty = true;
    updateGeometry();
    update();
    emit iconChanged(m_icon);
}

bool FluentRadioButton::showIcon() const { return m_showIcon; }

void FluentRadioButton::setShowIcon(bool show) {
    if (m_showIcon == show)
        return;

    m_showIcon = show;
    m_layoutDirty = true;
    updateGeometry();
    update();
    emit showIconChanged(m_showIcon);
}

FluentRadioButtonSize FluentRadioButton::size() const { return m_size; }

void FluentRadioButton::setSize(FluentRadioButtonSize size) {
    if (m_size == size)
        return;

    m_size = size;
    updateSizeMetrics();
    m_layoutDirty = true;
    updateGeometry();
    update();
    emit sizeChanged(m_size);
}

FluentRadioButtonComplexity FluentRadioButton::complexity() const {
    return m_complexity;
}

void FluentRadioButton::setComplexity(FluentRadioButtonComplexity complexity) {
    if (m_complexity != complexity) {
        m_complexity = complexity;

        // Adjust behavior based on complexity mode
        if (complexity == FluentRadioButtonComplexity::Simple) {
            // Simple mode: disable animations
            setAnimated(false);
        } else {
            // Full mode: enable animations
            setAnimated(true);
        }

        updateLayout();
        emit complexityChanged(complexity);
    }
}

FluentRadioButtonLabelPosition FluentRadioButton::labelPosition() const {
    return m_labelPosition;
}

void FluentRadioButton::setLabelPosition(
    FluentRadioButtonLabelPosition position) {
    if (m_labelPosition == position)
        return;

    m_labelPosition = position;
    m_layoutDirty = true;
    updateGeometry();
    update();
    emit labelPositionChanged(m_labelPosition);
}

bool FluentRadioButton::autoExclusive() const { return m_autoExclusive; }

void FluentRadioButton::setAutoExclusive(bool autoExclusive) {
    if (m_autoExclusive == autoExclusive)
        return;

    m_autoExclusive = autoExclusive;
    setupButtonGroup();
    emit autoExclusiveChanged(m_autoExclusive);
}

QButtonGroup* FluentRadioButton::buttonGroup() const { return m_buttonGroup; }

void FluentRadioButton::setButtonGroup(QButtonGroup* group) {
    if (m_buttonGroup == group)
        return;

    if (m_ownButtonGroup && m_buttonGroup) {
        delete m_buttonGroup;
        m_ownButtonGroup = false;
    }

    if (m_buttonGroup) {
        m_buttonGroup->removeButton(this);
        disconnect(m_buttonGroup, nullptr, this, nullptr);
    }

    m_buttonGroup = group;

    if (m_buttonGroup) {
        m_buttonGroup->addButton(this);
        connect(
            m_buttonGroup,
            QOverload<QAbstractButton*, bool>::of(&QButtonGroup::buttonToggled),
            this, &FluentRadioButton::onButtonGroupToggled);
    }
}

bool FluentRadioButton::isAnimated() const { return m_animated; }

void FluentRadioButton::setAnimated(bool animated) {
    if (m_animated == animated)
        return;

    m_animated = animated;
    emit animatedChanged(m_animated);
}

int FluentRadioButton::animationDuration() const { return m_animationDuration; }

void FluentRadioButton::setAnimationDuration(int duration) {
    if (m_animationDuration == duration)
        return;

    m_animationDuration = qMax(0, duration);

    if (m_checkAnimation) {
        m_checkAnimation->setDuration(m_animationDuration);
    }
    if (m_scaleAnimation) {
        m_scaleAnimation->setDuration(m_animationDuration / 2);
    }
    if (m_colorAnimation) {
        m_colorAnimation->setDuration(m_animationDuration);
    }
    if (m_indicatorAnimation) {
        m_indicatorAnimation->setDuration(m_animationDuration);
    }

    emit animationDurationChanged(m_animationDuration);
}

QSize FluentRadioButton::radioSize() const {
    return QSize(m_radioSize, m_radioSize);
}

QSize FluentRadioButton::iconSize() const {
    return QSize(m_iconSize, m_iconSize);
}

QRect FluentRadioButton::radioRect() const {
    if (m_layoutDirty) {
        const_cast<FluentRadioButton*>(this)->updateLayout();
    }
    return m_radioRect;
}

QRect FluentRadioButton::iconRect() const {
    if (m_layoutDirty) {
        const_cast<FluentRadioButton*>(this)->updateLayout();
    }
    return m_iconRect;
}

QRect FluentRadioButton::labelRect() const {
    if (m_layoutDirty) {
        const_cast<FluentRadioButton*>(this)->updateLayout();
    }
    return m_labelRect;
}

QSize FluentRadioButton::sizeHint() const {
    QFontMetrics fm(font());
    QSize textSize =
        m_text.isEmpty() ? QSize(0, 0) : fm.size(Qt::TextSingleLine, m_text);
    QSize iconSize = (m_showIcon && !m_icon.isNull())
                         ? QSize(m_iconSize, m_iconSize)
                         : QSize(0, 0);

    int width = m_radioSize;
    int height = m_radioSize;

    if (m_labelPosition == FluentRadioButtonLabelPosition::Left ||
        m_labelPosition == FluentRadioButtonLabelPosition::Right) {
        if (!m_text.isEmpty()) {
            width += m_spacing + textSize.width();
            height = qMax(height, textSize.height());
        }
        if (m_showIcon && !m_icon.isNull()) {
            width += m_spacing + iconSize.width();
            height = qMax(height, iconSize.height());
        }
    } else if (m_labelPosition == FluentRadioButtonLabelPosition::Above ||
               m_labelPosition == FluentRadioButtonLabelPosition::Below) {
        width = qMax(width, textSize.width());
        width = qMax(width, iconSize.width());
        if (!m_text.isEmpty()) {
            height += m_spacing + textSize.height();
        }
        if (m_showIcon && !m_icon.isNull()) {
            height += m_spacing + iconSize.height();
        }
    }

    return QSize(width, height);
}

QSize FluentRadioButton::minimumSizeHint() const {
    return QSize(m_radioSize, m_radioSize);
}

QButtonGroup* FluentRadioButton::createButtonGroup(const QStringList& options,
                                                   QWidget* parent) {
    auto* group = new QButtonGroup(parent);
    for (const QString& option : options) {
        auto* radio = new FluentRadioButton(option, parent);
        group->addButton(radio);
    }
    return group;
}

QList<FluentRadioButton*> FluentRadioButton::createRadioGroup(
    const QStringList& options, QWidget* parent) {
    QList<FluentRadioButton*> radios;
    auto* group = new QButtonGroup(parent);

    for (const QString& option : options) {
        auto* radio = new FluentRadioButton(option, parent);
        group->addButton(radio);
        radios.append(radio);
    }

    return radios;
}

void FluentRadioButton::animateCheck() {
    if (m_animated) {
        startCheckAnimation();
    }
}

void FluentRadioButton::click() {
    if (!isEnabled())
        return;

    setChecked(true);
    emit clicked();
    emit pressed();
    emit released();
}

void FluentRadioButton::setupAnimations() {
    // Check animation for indicator opacity
    m_indicatorAnimation =
        new QPropertyAnimation(this, "indicatorOpacity", this);
    m_indicatorAnimation->setDuration(m_animationDuration);
    m_indicatorAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_indicatorAnimation, &QPropertyAnimation::finished, this,
            &FluentRadioButton::onCheckAnimationFinished);

    // Scale animation for press effect
    m_scaleAnimation = new QPropertyAnimation(this, "radioScale", this);
    m_scaleAnimation->setDuration(m_animationDuration / 2);
    m_scaleAnimation->setEasingCurve(QEasingCurve::OutBack);

    // Color animation for state changes
    m_colorAnimation = new QPropertyAnimation(this, "radioColor", this);
    m_colorAnimation->setDuration(m_animationDuration);
    m_colorAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

void FluentRadioButton::setupButtonGroup() {
    if (m_autoExclusive && !m_buttonGroup) {
        // Create a button group automatically if we need exclusive behavior
        // but don't have one yet. Look for siblings that are also radio
        // buttons.
        if (parentWidget()) {
            auto siblings = parentWidget()->findChildren<FluentRadioButton*>(
                QString(), Qt::FindDirectChildrenOnly);
            if (siblings.size() > 1) {
                // Find if any sibling already has a button group
                QButtonGroup* existingGroup = nullptr;
                for (auto* sibling : siblings) {
                    if (sibling != this && sibling->buttonGroup()) {
                        existingGroup = sibling->buttonGroup();
                        break;
                    }
                }

                if (existingGroup) {
                    setButtonGroup(existingGroup);
                } else {
                    // Create new button group
                    m_buttonGroup = new QButtonGroup(this);
                    m_ownButtonGroup = true;
                    m_buttonGroup->addButton(this);

                    // Add other radio button siblings
                    for (auto* sibling : siblings) {
                        if (sibling != this && sibling->autoExclusive() &&
                            !sibling->buttonGroup()) {
                            sibling->setButtonGroup(m_buttonGroup);
                        }
                    }
                }
            }
        }
    }
}

void FluentRadioButton::updateLayout() {
    if (!m_layoutDirty)
        return;

    const QRect rect = this->rect();
    const QFontMetrics fm(font());
    const QSize textSize =
        m_text.isEmpty() ? QSize(0, 0) : fm.size(Qt::TextSingleLine, m_text);
    const QSize iconSize = (m_showIcon && !m_icon.isNull())
                               ? QSize(m_iconSize, m_iconSize)
                               : QSize(0, 0);

    // Calculate radio button position
    m_radioRect = calculateRadioRect();

    // Calculate icon and label positions based on label position
    if (m_labelPosition == FluentRadioButtonLabelPosition::Right) {
        int x = m_radioRect.right() + m_spacing;
        int y = rect.center().y();

        if (m_showIcon && !m_icon.isNull()) {
            m_iconRect = QRect(x, y - iconSize.height() / 2, iconSize.width(),
                               iconSize.height());
            x += iconSize.width() + m_spacing;
        } else {
            m_iconRect = QRect();
        }

        if (!m_text.isEmpty()) {
            m_labelRect = QRect(x, y - textSize.height() / 2, textSize.width(),
                                textSize.height());
        } else {
            m_labelRect = QRect();
        }
    } else if (m_labelPosition == FluentRadioButtonLabelPosition::Left) {
        int x = rect.left();
        int y = rect.center().y();

        if (!m_text.isEmpty()) {
            m_labelRect = QRect(x, y - textSize.height() / 2, textSize.width(),
                                textSize.height());
            x += textSize.width() + m_spacing;
        } else {
            m_labelRect = QRect();
        }

        if (m_showIcon && !m_icon.isNull()) {
            m_iconRect = QRect(x, y - iconSize.height() / 2, iconSize.width(),
                               iconSize.height());
        } else {
            m_iconRect = QRect();
        }
    } else if (m_labelPosition == FluentRadioButtonLabelPosition::Above) {
        int x = rect.center().x();
        int y = rect.top();

        if (!m_text.isEmpty()) {
            m_labelRect = QRect(x - textSize.width() / 2, y, textSize.width(),
                                textSize.height());
            y += textSize.height() + m_spacing;
        } else {
            m_labelRect = QRect();
        }

        if (m_showIcon && !m_icon.isNull()) {
            m_iconRect = QRect(x - iconSize.width() / 2, y, iconSize.width(),
                               iconSize.height());
        } else {
            m_iconRect = QRect();
        }
    } else if (m_labelPosition == FluentRadioButtonLabelPosition::Below) {
        int x = rect.center().x();
        int y = m_radioRect.bottom() + m_spacing;

        if (m_showIcon && !m_icon.isNull()) {
            m_iconRect = QRect(x - iconSize.width() / 2, y, iconSize.width(),
                               iconSize.height());
            y += iconSize.height() + m_spacing;
        } else {
            m_iconRect = QRect();
        }

        if (!m_text.isEmpty()) {
            m_labelRect = QRect(x - textSize.width() / 2, y, textSize.width(),
                                textSize.height());
        } else {
            m_labelRect = QRect();
        }
    }

    m_layoutDirty = false;
}

void FluentRadioButton::updateRadioPosition() {
    m_layoutDirty = true;
    update();
}

void FluentRadioButton::updateSizeMetrics() {
    switch (m_size) {
        case FluentRadioButtonSize::Small:
            m_radioSize = 16;
            m_indicatorSize = 6;
            m_iconSize = 12;
            m_spacing = 6;
            break;
        case FluentRadioButtonSize::Medium:
            m_radioSize = 20;
            m_indicatorSize = 8;
            m_iconSize = 16;
            m_spacing = 8;
            break;
        case FluentRadioButtonSize::Large:
            m_radioSize = 24;
            m_indicatorSize = 10;
            m_iconSize = 20;
            m_spacing = 10;
            break;
    }
}

void FluentRadioButton::layoutComponents() { updateLayout(); }

QRect FluentRadioButton::calculateRadioRect() const {
    const QRect rect = this->rect();

    if (m_labelPosition == FluentRadioButtonLabelPosition::Right) {
        return QRect(rect.left(), rect.center().y() - m_radioSize / 2,
                     m_radioSize, m_radioSize);
    } else if (m_labelPosition == FluentRadioButtonLabelPosition::Left) {
        return QRect(rect.right() - m_radioSize,
                     rect.center().y() - m_radioSize / 2, m_radioSize,
                     m_radioSize);
    } else {
        return QRect(rect.center().x() - m_radioSize / 2,
                     rect.center().y() - m_radioSize / 2, m_radioSize,
                     m_radioSize);
    }
}

QRect FluentRadioButton::calculateIconRect() const {
    // This is handled in updateLayout()
    return m_iconRect;
}

QRect FluentRadioButton::calculateLabelRect() const {
    // This is handled in updateLayout()
    return m_labelRect;
}

void FluentRadioButton::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_layoutDirty) {
        updateLayout();
    }

    // Paint radio button
    paintRadioButton(&painter);

    // Paint icon
    if (m_showIcon && !m_icon.isNull() && !m_iconRect.isEmpty()) {
        paintIcon(&painter);
    }

    // Paint text
    if (!m_text.isEmpty() && !m_labelRect.isEmpty()) {
        paintText(&painter);
    }

    // Paint focus indicator
    if (hasFocus()) {
        paintFocusIndicator(&painter);
    }
}

void FluentRadioButton::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && isEnabled()) {
        m_pressed = true;
        if (m_animated) {
            startPressAnimation();
        }
        update();
        emit pressed();
    }
    QAbstractButton::mousePressEvent(event);
}

void FluentRadioButton::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_pressed) {
        m_pressed = false;
        if (m_animated) {
            startReleaseAnimation();
        }

        if (rect().contains(event->pos()) && isEnabled()) {
            setChecked(true);
            emit clicked();
        }

        update();
        emit released();
    }
    QAbstractButton::mouseReleaseEvent(event);
}

void FluentRadioButton::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return) {
        if (isEnabled()) {
            setChecked(true);
            emit clicked();
        }
        event->accept();
        return;
    }

    QAbstractButton::keyPressEvent(event);
}

void FluentRadioButton::focusInEvent(QFocusEvent* event) {
    update();
    QAbstractButton::focusInEvent(event);
}

void FluentRadioButton::focusOutEvent(QFocusEvent* event) {
    update();
    QAbstractButton::focusOutEvent(event);
}

void FluentRadioButton::resizeEvent(QResizeEvent* event) {
    m_layoutDirty = true;
    QAbstractButton::resizeEvent(event);
}

void FluentRadioButton::changeEvent(QEvent* event) {
    if (event->type() == QEvent::EnabledChange ||
        event->type() == QEvent::FontChange ||
        event->type() == QEvent::StyleChange) {
        m_layoutDirty = true;
        updateColors();
        update();
    }
    QAbstractButton::changeEvent(event);
}

void FluentRadioButton::enterEvent(QEnterEvent* event) {
    if (isEnabled()) {
        m_hovered = true;
        if (m_animated) {
            startHoverAnimation(true);
        }
        update();
    }
    QAbstractButton::enterEvent(event);
}

void FluentRadioButton::leaveEvent(QEvent* event) {
    m_hovered = false;
    if (m_animated) {
        startHoverAnimation(false);
    }
    update();
    QAbstractButton::leaveEvent(event);
}

void FluentRadioButton::onCheckAnimationFinished() { update(); }

void FluentRadioButton::onButtonGroupToggled(QAbstractButton* button,
                                             bool checked) {
    if (button == this && checked != m_checked) {
        setChecked(checked);
    }
}

void FluentRadioButton::updateColors() {
    const auto& theme = Styling::FluentTheme::instance();

    if (isEnabled()) {
        switch (state()) {
            case Core::FluentState::Normal:
                m_borderColor = theme.color(
                    Styling::FluentThemeColor::ControlStrokeDefault);
                m_backgroundColor =
                    theme.color(Styling::FluentThemeColor::ControlFillDefault);
                m_textColor =
                    theme.color(Styling::FluentThemeColor::TextFillPrimary);
                break;
            case Core::FluentState::Hovered:
                m_borderColor = theme.color(
                    Styling::FluentThemeColor::ControlStrokeSecondary);
                m_backgroundColor = theme.color(
                    Styling::FluentThemeColor::ControlFillSecondary);
                m_textColor =
                    theme.color(Styling::FluentThemeColor::TextFillPrimary);
                break;
            case Core::FluentState::Pressed:
                m_borderColor = theme.color(
                    Styling::FluentThemeColor::ControlStrokeDefault);
                m_backgroundColor =
                    theme.color(Styling::FluentThemeColor::ControlFillTertiary);
                m_textColor =
                    theme.color(Styling::FluentThemeColor::TextFillSecondary);
                break;
            case Core::FluentState::Focused:
                m_borderColor =
                    theme.color(Styling::FluentThemeColor::AccentFillDefault);
                m_backgroundColor =
                    theme.color(Styling::FluentThemeColor::ControlFillDefault);
                m_textColor =
                    theme.color(Styling::FluentThemeColor::TextFillPrimary);
                break;
            default:
                break;
        }

        if (m_checked) {
            m_borderColor =
                theme.color(Styling::FluentThemeColor::AccentFillDefault);
            m_indicatorColor =
                theme.color(Styling::FluentThemeColor::AccentFillDefault);
        } else {
            m_indicatorColor = Qt::transparent;
        }
    } else {
        m_borderColor =
            theme.color(Styling::FluentThemeColor::ControlStrokeDisabled);
        m_backgroundColor =
            theme.color(Styling::FluentThemeColor::ControlFillDisabled);
        m_textColor = theme.color(Styling::FluentThemeColor::TextFillDisabled);
        m_indicatorColor =
            theme.color(Styling::FluentThemeColor::TextFillDisabled);
    }

    update();
}

void FluentRadioButton::updateAccessibility() {
#ifndef QT_NO_ACCESSIBILITY
    if (QAccessible::isActive()) {
        QAccessibleEvent event(this, QAccessible::StateChanged);
        QAccessible::updateAccessibility(&event);
    }
#endif
}

void FluentRadioButton::paintRadioButton(QPainter* painter) {
    const QRect radioRect = m_radioRect;
    const qreal scale = m_radioScale;

    painter->save();

    // Apply scale transformation
    if (scale != 1.0) {
        const QPointF center = radioRect.center();
        painter->translate(center);
        painter->scale(scale, scale);
        painter->translate(-center);
    }

    // Draw outer circle (border)
    painter->setPen(QPen(m_borderColor, 2));
    painter->setBrush(m_backgroundColor);
    painter->drawEllipse(radioRect);

    // Draw inner indicator circle
    if (m_checked && m_indicatorOpacity > 0.0) {
        const int margin = (m_radioSize - m_indicatorSize) / 2;
        const QRect indicatorRect =
            radioRect.adjusted(margin, margin, -margin, -margin);

        QColor indicatorColor = m_indicatorColor;
        indicatorColor.setAlphaF(m_indicatorOpacity);

        painter->setPen(Qt::NoPen);
        painter->setBrush(indicatorColor);
        painter->drawEllipse(indicatorRect);
    }

    painter->restore();
}

void FluentRadioButton::paintIcon(QPainter* painter) {
    if (m_icon.isNull() || m_iconRect.isEmpty())
        return;

    painter->save();

    QIcon::Mode mode = QIcon::Normal;
    if (!isEnabled()) {
        mode = QIcon::Disabled;
    } else if (state() == Core::FluentState::Pressed) {
        mode = QIcon::Selected;
    }

    QIcon::State iconState = m_checked ? QIcon::On : QIcon::Off;
    QPixmap pixmap = m_icon.pixmap(m_iconRect.size(), mode, iconState);

    painter->drawPixmap(m_iconRect, pixmap);

    painter->restore();
}

void FluentRadioButton::paintText(QPainter* painter) {
    if (m_text.isEmpty() || m_labelRect.isEmpty())
        return;

    painter->save();

    painter->setPen(m_textColor);
    painter->setFont(font());

    const Qt::Alignment alignment = Qt::AlignLeft | Qt::AlignVCenter;
    painter->drawText(m_labelRect, alignment, m_text);

    painter->restore();
}

void FluentRadioButton::paintFocusIndicator(QPainter* painter) {
    if (!hasFocus())
        return;

    painter->save();

    const auto& theme = Styling::FluentTheme::instance();
    const QColor focusColor =
        theme.color(Styling::FluentThemeColor::AccentFillDefault);

    QPen focusPen(focusColor, 2);
    focusPen.setStyle(Qt::DashLine);
    painter->setPen(focusPen);
    painter->setBrush(Qt::NoBrush);

    const QRect focusRect = rect().adjusted(1, 1, -1, -1);
    painter->drawRect(focusRect);

    painter->restore();
}

void FluentRadioButton::startCheckAnimation() {
    if (!m_indicatorAnimation)
        return;

    m_indicatorAnimation->stop();
    m_indicatorAnimation->setStartValue(m_indicatorOpacity);
    m_indicatorAnimation->setEndValue(m_checked ? 1.0 : 0.0);
    m_indicatorAnimation->start();
}

void FluentRadioButton::startPressAnimation() {
    if (!m_scaleAnimation)
        return;

    m_scaleAnimation->stop();
    m_scaleAnimation->setStartValue(m_radioScale);
    m_scaleAnimation->setEndValue(0.95);
    m_scaleAnimation->start();
}

void FluentRadioButton::startReleaseAnimation() {
    if (!m_scaleAnimation)
        return;

    m_scaleAnimation->stop();
    m_scaleAnimation->setStartValue(m_radioScale);
    m_scaleAnimation->setEndValue(1.0);
    m_scaleAnimation->start();
}

void FluentRadioButton::startHoverAnimation(bool hover) {
    if (!m_colorAnimation)
        return;

    const auto& theme = Styling::FluentTheme::instance();
    QColor targetColor =
        hover ? theme.color(Styling::FluentThemeColor::ControlStrokeSecondary)
              : theme.color(Styling::FluentThemeColor::ControlStrokeDefault);

    m_colorAnimation->stop();
    m_colorAnimation->setStartValue(m_borderColor);
    m_colorAnimation->setEndValue(targetColor);
    m_colorAnimation->start();
}

// Property accessors for animations
qreal FluentRadioButton::indicatorOpacity() const { return m_indicatorOpacity; }

void FluentRadioButton::setIndicatorOpacity(qreal opacity) {
    if (qFuzzyCompare(m_indicatorOpacity, opacity))
        return;
    m_indicatorOpacity = opacity;
    update();
}

qreal FluentRadioButton::radioScale() const { return m_radioScale; }

void FluentRadioButton::setRadioScale(qreal scale) {
    if (qFuzzyCompare(m_radioScale, scale))
        return;
    m_radioScale = scale;
    update();
}

QColor FluentRadioButton::radioColor() const { return m_borderColor; }

void FluentRadioButton::setRadioColor(const QColor& color) {
    if (m_borderColor == color)
        return;
    m_borderColor = color;
    update();
}

// Factory methods for simple radio buttons (from Simple variant)
FluentRadioButton* FluentRadioButton::createSimple(const QString& text,
                                                   QWidget* parent) {
    auto* radioButton = new FluentRadioButton(text, parent);
    radioButton->setComplexity(FluentRadioButtonComplexity::Simple);
    return radioButton;
}

FluentRadioButton* FluentRadioButton::createSimple(const QString& text,
                                                   const QString& value,
                                                   QWidget* parent) {
    auto* radioButton = new FluentRadioButton(text, value, parent);
    radioButton->setComplexity(FluentRadioButtonComplexity::Simple);
    return radioButton;
}

}  // namespace FluentQt::Components
