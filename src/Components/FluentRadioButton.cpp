// src/Components/FluentRadioButton.cpp
#include "FluentQt/Components/FluentRadioButton.h"
#include <QAccessible>
#include <QApplication>
#include <QButtonGroup>
#include <QEnterEvent>
#include <QEvent>
#include <QFocusEvent>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QMutexLocker>
#include <QPainter>
#include <QResizeEvent>
#include <QTimer>
#include <exception>
#include <memory>
#include "FluentQt/Core/FluentState.h"
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
            &Styling::FluentTheme::themeChanged, this, [this]() {
                {
                    QMutexLocker locker(&m_mutex);
                    m_colorsCacheValid =
                        false;  // Thread-safe cache invalidation
                }
                updateColors();
            });

    // Enhanced accessibility setup
    setupAccessibility();

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
    // Enhanced error handling and boundary checks
    if (text.length() > 1000) {
        qWarning() << "FluentRadioButton::setText: Text too long ("
                   << text.length()
                   << " characters) - truncating to 1000 characters";
        QString truncatedText = text.left(1000);
        if (m_text == truncatedText)
            return;
        m_text = truncatedText;
    } else {
        if (m_text == text)
            return;
        m_text = text;
    }

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
            // Simple mode: disable animations and stop any running animations
            setAnimated(false);
            // Stop all running animations
            if (m_checkAnimation)
                m_checkAnimation->stop();
            if (m_scaleAnimation)
                m_scaleAnimation->stop();
            if (m_colorAnimation)
                m_colorAnimation->stop();
            if (m_indicatorAnimation)
                m_indicatorAnimation->stop();
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

    // Stop all running animations when animations are disabled
    if (!animated) {
        if (m_checkAnimation)
            m_checkAnimation->stop();
        if (m_scaleAnimation)
            m_scaleAnimation->stop();
        if (m_colorAnimation)
            m_colorAnimation->stop();
        if (m_indicatorAnimation)
            m_indicatorAnimation->stop();
    }

    emit animatedChanged(m_animated);
}

int FluentRadioButton::animationDuration() const { return m_animationDuration; }

void FluentRadioButton::setAnimationDuration(int duration) {
    // Enhanced error handling and boundary checks
    if (duration < 0) {
        qWarning()
            << "FluentRadioButton::setAnimationDuration: Invalid duration"
            << duration << "- must be non-negative, setting to 0";
        duration = 0;
    } else if (duration > 5000) {
        qWarning()
            << "FluentRadioButton::setAnimationDuration: Duration too long"
            << duration << "ms - clamping to 5000ms for better UX";
        duration = 5000;
    }

    if (m_animationDuration == duration)
        return;

    m_animationDuration = duration;

    // Safely update animation durations with null checks
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
    // Enhanced sizeHint with validation
    QFontMetrics fm(font());
    QSize textSize = m_text.isEmpty()
                         ? QSize(0, 0)
                         : fm.size(Qt::TextSingleLine,
                                   m_text.left(1000));  // Limit text length

    // Validate icon size
    QSize iconSize(0, 0);
    if (m_showIcon && !m_icon.isNull()) {
        int validIconSize = qBound(8, m_iconSize, 128);  // Reasonable limits
        iconSize = QSize(validIconSize, validIconSize);
    }

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
    // Check animation for indicator opacity (same as indicator animation)
    m_checkAnimation = new QPropertyAnimation(this, "indicatorOpacity", this);
    m_checkAnimation->setDuration(m_animationDuration);
    m_checkAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_checkAnimation, &QPropertyAnimation::finished, this,
            &FluentRadioButton::onCheckAnimationFinished);

    // Indicator animation (alias for check animation)
    m_indicatorAnimation = m_checkAnimation;

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

void FluentRadioButton::setupAccessibility() {
#ifndef QT_NO_ACCESSIBILITY
    // Enhanced accessibility setup for FluentUI compliance
    try {
        // Set focus policy for keyboard navigation
        setFocusPolicy(Qt::StrongFocus);

        // Enable tab navigation
        setProperty("accessibleTabOrder", true);

        // Set ARIA-like properties for better screen reader support
        setProperty("role", "radio");
        setProperty("aria-checked", m_checked);
        setProperty("aria-disabled", !isEnabled());

        // Set up keyboard shortcuts if text contains mnemonics
        if (m_text.contains('&')) {
            QString shortcut = m_text;
            shortcut.remove('&');
            if (!shortcut.isEmpty()) {
                setProperty("accessibleShortcut",
                            QString("Alt+%1").arg(shortcut.at(0)));
            }
        }

        // Set up group information for screen readers
        if (m_buttonGroup) {
            setProperty("accessibleGroup", m_buttonGroup->objectName());

            // Set position in group
            QList<QAbstractButton*> buttons = m_buttonGroup->buttons();
            int position = buttons.indexOf(this) + 1;
            int total = buttons.size();
            setProperty("accessiblePositionInSet", position);
            setProperty("accessibleSetSize", total);
        }

        // Set up live region for dynamic updates
        setProperty("accessibleLiveRegion", "polite");

    } catch (const std::exception& e) {
        qWarning() << "FluentRadioButton::setupAccessibility: Exception:"
                   << e.what();
    } catch (...) {
        qWarning()
            << "FluentRadioButton::setupAccessibility: Unknown exception";
    }
#endif
}

void FluentRadioButton::updateLayout() {
    if (!m_layoutDirty)
        return;

    // Enhanced error handling and boundary checks
    const QRect rect = this->rect();
    if (rect.isEmpty() || !rect.isValid()) {
        qWarning() << "FluentRadioButton::updateLayout: Invalid widget rect"
                   << rect;
        m_radioRect = QRect();
        m_iconRect = QRect();
        m_labelRect = QRect();
        return;
    }

    // Validate font and get metrics safely
    QFont currentFont = font();
    if (!currentFont.exactMatch()) {
        qWarning() << "FluentRadioButton::updateLayout: Font not available, "
                      "using default";
        currentFont = QFont();  // Use default font
    }

    const QFontMetrics fm(currentFont);
    const QSize textSize =
        m_text.isEmpty() ? QSize(0, 0)
                         : fm.size(Qt::TextSingleLine,
                                   m_text.left(1000));  // Limit text length

    // Validate icon size
    QSize iconSize(0, 0);
    if (m_showIcon && !m_icon.isNull()) {
        if (m_iconSize > 0 &&
            m_iconSize <= 128) {  // Reasonable icon size limits
            iconSize = QSize(m_iconSize, m_iconSize);
        } else {
            qWarning() << "FluentRadioButton::updateLayout: Invalid icon size"
                       << m_iconSize;
            iconSize = QSize(16, 16);  // Default icon size
        }
    }

    // Calculate radio button position with validation
    m_radioRect = calculateRadioRect();
    if (m_radioRect.isEmpty() || !rect.contains(m_radioRect)) {
        qWarning()
            << "FluentRadioButton::updateLayout: Invalid radio rect calculated";
        // Fallback to simple centered position
        int radioSize = qBound(8, m_radioSize, 64);
        m_radioRect = QRect(rect.left(), rect.center().y() - radioSize / 2,
                            radioSize, radioSize);
    }

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
    // Updated to match Microsoft FluentUI specifications
    switch (m_size) {
        case FluentRadioButtonSize::Small:
            m_radioSize = 16;
            m_indicatorSize = 6;
            m_iconSize = 12;
            m_spacing = 8;  // Increased for better FluentUI compliance
            break;
        case FluentRadioButtonSize::Medium:
            m_radioSize = 20;  // Standard FluentUI radio button size
            m_indicatorSize = 8;
            m_iconSize = 16;
            m_spacing = 12;  // Increased for better visual hierarchy
            break;
        case FluentRadioButtonSize::Large:
            m_radioSize = 24;
            m_indicatorSize = 10;
            m_iconSize = 20;
            m_spacing = 16;  // Increased for better accessibility
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
    paintRadio(&painter);

    // Paint icon
    if (m_showIcon && !m_icon.isNull() && !m_iconRect.isEmpty()) {
        paintIcon(&painter);
    }

    // Paint text
    if (!m_text.isEmpty() && !m_labelRect.isEmpty()) {
        paintLabel(&painter);
    }

    // Paint hover effect
    if (m_hovered && isEnabled()) {
        paintHoverEffect(&painter);
    }

    // Paint focus indicator
    if (hasFocus()) {
        paintFocusIndicator(&painter);
    }
}

void FluentRadioButton::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && isEnabled()) {
        m_pressed = true;

        // Enhanced visual feedback with state transition
        performStateTransition(
            m_hovered ? Core::FluentState::Hovered : Core::FluentState::Normal,
            Core::FluentState::Pressed);

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

        // Enhanced state transition on release
        Core::FluentState newState = Core::FluentState::Normal;
        if (hasFocus()) {
            newState = Core::FluentState::Focused;
        } else if (m_hovered && rect().contains(event->pos())) {
            newState = Core::FluentState::Hovered;
        }

        performStateTransition(Core::FluentState::Pressed, newState);

        if (m_animated) {
            startReleaseAnimation();
        }

        // Enhanced click detection with better hit testing
        if (rect().contains(event->pos()) && isEnabled()) {
            // Use hitTestRadio for more precise radio button area detection
            if (hitTestRadio(event->pos()) || !m_text.isEmpty()) {
                setChecked(true);
                emit clicked();
            }
        }

        update();
        emit released();
    }
    QAbstractButton::mouseReleaseEvent(event);
}

void FluentRadioButton::keyPressEvent(QKeyEvent* event) {
    // Enhanced keyboard interaction for FluentUI compliance
    if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return) {
        if (isEnabled()) {
            // Provide visual feedback for keyboard activation
            m_pressed = true;
            if (m_animated) {
                startPressAnimation();
            }
            update();

            setChecked(true);
            emit clicked();

            // Reset pressed state after a short delay for visual feedback
            QTimer::singleShot(100, this, [this]() {
                m_pressed = false;
                if (m_animated) {
                    startReleaseAnimation();
                }
                update();
            });
        }
        event->accept();
        return;
    }

    // Enhanced arrow key navigation for radio button groups
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Left ||
        event->key() == Qt::Key_Down || event->key() == Qt::Key_Right) {
        if (buttonGroup()) {
            // Find next/previous radio button in group
            QList<QAbstractButton*> buttons = buttonGroup()->buttons();
            int currentIndex = buttons.indexOf(this);

            if (currentIndex != -1) {
                int nextIndex = currentIndex;

                if (event->key() == Qt::Key_Up ||
                    event->key() == Qt::Key_Left) {
                    nextIndex =
                        (currentIndex - 1 + buttons.size()) % buttons.size();
                } else {
                    nextIndex = (currentIndex + 1) % buttons.size();
                }

                if (nextIndex != currentIndex && nextIndex < buttons.size()) {
                    QAbstractButton* nextButton = buttons[nextIndex];
                    if (nextButton && nextButton->isEnabled()) {
                        nextButton->setFocus();
                        nextButton->setChecked(true);
                    }
                }
            }
        }
        event->accept();
        return;
    }

    QAbstractButton::keyPressEvent(event);
}

void FluentRadioButton::focusInEvent(QFocusEvent* event) {
    // Enhanced focus handling with state transitions
    if (isEnabled()) {
        performStateTransition(
            m_hovered ? Core::FluentState::Hovered : Core::FluentState::Normal,
            Core::FluentState::Focused);
    }
    update();
    QAbstractButton::focusInEvent(event);
}

void FluentRadioButton::focusOutEvent(QFocusEvent* event) {
    // Enhanced focus out handling with state transitions
    Core::FluentState newState =
        m_hovered ? Core::FluentState::Hovered : Core::FluentState::Normal;
    performStateTransition(Core::FluentState::Focused, newState);
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
        // Trigger state transition manually
        performStateTransition(Core::FluentState::Normal,
                               Core::FluentState::Hovered);
    }
    QAbstractButton::enterEvent(event);
}

void FluentRadioButton::leaveEvent(QEvent* event) {
    m_hovered = false;
    // Trigger state transition manually
    Core::FluentState newState =
        hasFocus() ? Core::FluentState::Focused : Core::FluentState::Normal;
    performStateTransition(Core::FluentState::Hovered, newState);
    QAbstractButton::leaveEvent(event);
}

void FluentRadioButton::onCheckAnimationFinished() { update(); }

void FluentRadioButton::onButtonGroupToggled(QAbstractButton* button,
                                             bool checked) {
    // Cast to check if this button is the one being toggled
    if (qobject_cast<FluentRadioButton*>(button) == this &&
        checked != m_checked) {
        setChecked(checked);
    }
}

// Missing slot implementation for position changes
void FluentRadioButton::onRadioPositionChanged() { updateRadioPosition(); }

// Button-specific functionality implementation
void FluentRadioButton::setDown(bool down) {
    if (m_pressed == down)
        return;

    m_pressed = down;
    if (m_animated) {
        if (down) {
            startPressAnimation();
        } else {
            startReleaseAnimation();
        }
    }
    update();
}

// FluentComponent-style state management (manual implementation)
void FluentRadioButton::updateStateStyle() {
    // Update colors based on current state
    updateColors();
}

void FluentRadioButton::performStateTransition(Core::FluentState /* from */,
                                               Core::FluentState to) {
    // Handle radio button specific transitions
    if (m_animated) {
        switch (to) {
            case Core::FluentState::Hovered:
                // Hover animation will be handled by color updates
                break;
            case Core::FluentState::Normal:
                // Normal state animation handled by color updates
                break;
            case Core::FluentState::Pressed:
                // Press animation will be handled by scale animation
                animateRadioScale();
                break;
            case Core::FluentState::Focused:
                // Focus animation handled by focus indicator
                break;
            default:
                break;
        }
    }

    updateColors();
}

void FluentRadioButton::updateColors() {
    const auto& theme = Styling::FluentTheme::instance();

    // Thread-safe update of color cache for performance optimization
    {
        QMutexLocker locker(&m_mutex);
        m_cachedAccentColor = theme.color("accent");
        m_cachedHoverColor = theme.color("controlFillSecondary");
        m_cachedFocusColor = m_cachedAccentColor;
        m_colorsCacheValid = true;
    }

    if (isEnabled()) {
        // Determine current state manually since we don't inherit from
        // FluentComponent
        Core::FluentState currentState = Core::FluentState::Normal;
        if (m_pressed) {
            currentState = Core::FluentState::Pressed;
        } else if (hasFocus()) {
            currentState = Core::FluentState::Focused;
        } else if (m_hovered) {
            currentState = Core::FluentState::Hovered;
        }

        switch (currentState) {
            case Core::FluentState::Normal:
                m_borderColor = theme.color("controlStrokeDefault");
                m_backgroundColor = theme.color("controlFillDefault");
                m_textColor = theme.color("textPrimary");
                break;
            case Core::FluentState::Hovered:
                m_borderColor = theme.color("controlStrokeSecondary");
                m_backgroundColor = theme.color("controlFillSecondary");
                m_textColor = theme.color("textPrimary");
                break;
            case Core::FluentState::Pressed:
                m_borderColor = theme.color("controlStrokeDefault");
                m_backgroundColor = theme.color("controlFillTertiary");
                m_textColor = theme.color("textSecondary");
                break;
            case Core::FluentState::Focused:
                m_borderColor = theme.color("accent");
                m_backgroundColor = theme.color("controlFillDefault");
                m_textColor = theme.color("textPrimary");
                break;
            default:
                break;
        }

        if (m_checked) {
            m_borderColor = theme.color("accent");
            m_indicatorColor = theme.color("accent");
        } else {
            m_indicatorColor = Qt::transparent;
        }
    } else {
        m_borderColor = theme.color("controlStrokeDisabled");
        m_backgroundColor = theme.color("controlFillDisabled");
        m_textColor = theme.color("disabled");
        m_indicatorColor = theme.color("disabled");
    }

    // Keep radioColor in sync with borderColor for property animation
    m_radioColor = m_borderColor;

    update();
}

void FluentRadioButton::updateAccessibility() {
#ifndef QT_NO_ACCESSIBILITY
    if (!QAccessible::isActive()) {
        return;
    }

    // Enhanced accessibility implementation
    try {
        // Set accessible name and description
        QString accessibleName = m_text.isEmpty() ? objectName() : m_text;
        if (accessibleName.isEmpty()) {
            accessibleName = tr("Radio Button");
        }
        setAccessibleName(accessibleName);

        // Set accessible description with state information
        QString description = tr("Radio button");
        if (!m_value.isEmpty() && m_value != m_text) {
            description += tr(" with value: %1").arg(m_value);
        }
        if (m_checked) {
            description += tr(" (selected)");
        }
        if (!isEnabled()) {
            description += tr(" (disabled)");
        }
        setAccessibleDescription(description);

        // Set proper role
        setProperty("accessibleRole", QAccessible::RadioButton);

        // Update state information
        QAccessibleEvent stateEvent(this, QAccessible::StateChanged);
        QAccessible::updateAccessibility(&stateEvent);

        // If checked state changed, notify screen readers
        if (m_checked) {
            QAccessibleEvent checkedEvent(this, QAccessible::Selection);
            QAccessible::updateAccessibility(&checkedEvent);
        }

        // Update focus information if focused
        if (hasFocus()) {
            QAccessibleEvent focusEvent(this, QAccessible::Focus);
            QAccessible::updateAccessibility(&focusEvent);
        }

    } catch (const std::exception& e) {
        qWarning() << "FluentRadioButton::updateAccessibility: Exception:"
                   << e.what();
    } catch (...) {
        qWarning()
            << "FluentRadioButton::updateAccessibility: Unknown exception";
    }
#endif
}

// New primary painting method with enhanced FluentUI styling
void FluentRadioButton::paintRadio(QPainter* painter) {
    const QRect radioRect = m_radioRect;
    const qreal scale = m_radioScale;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    // Apply scale transformation
    if (scale != 1.0) {
        const QPointF center = radioRect.center();
        painter->translate(center);
        painter->scale(scale, scale);
        painter->translate(-center);
    }

    // Enhanced FluentUI styling with proper border width
    qreal borderWidth = 2.0;
    if (m_hovered && isEnabled()) {
        borderWidth = 2.5;  // Slightly thicker border on hover
    }
    if (hasFocus()) {
        borderWidth = 3.0;  // Thicker border for focus
    }

    // Draw outer circle (border) with enhanced styling
    QPen borderPen(m_borderColor, borderWidth);
    borderPen.setCapStyle(Qt::RoundCap);
    painter->setPen(borderPen);
    painter->setBrush(m_backgroundColor);

    // Adjust rect for border width to prevent clipping
    QRectF adjustedRect = radioRect.adjusted(
        borderWidth / 2, borderWidth / 2, -borderWidth / 2, -borderWidth / 2);
    painter->drawEllipse(adjustedRect);

    // Paint the indicator separately
    paintRadioIndicator(painter);

    painter->restore();
}

// Enhanced indicator painting method with FluentUI styling
void FluentRadioButton::paintRadioIndicator(QPainter* painter) {
    if (!m_checked || m_indicatorOpacity <= 0.0)
        return;

    const QRect radioRect = m_radioRect;
    const int margin = (m_radioSize - m_indicatorSize) / 2;
    const QRect indicatorRect =
        radioRect.adjusted(margin, margin, -margin, -margin);

    QColor indicatorColor = m_indicatorColor;
    indicatorColor.setAlphaF(m_indicatorOpacity);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);

    // Enhanced FluentUI indicator with subtle gradient effect
    QRadialGradient gradient(indicatorRect.center(), m_indicatorSize / 2.0);
    gradient.setColorAt(0.0, indicatorColor);
    gradient.setColorAt(0.8, indicatorColor);
    QColor fadeColor = indicatorColor;
    fadeColor.setAlphaF(indicatorColor.alphaF() * 0.8);
    gradient.setColorAt(1.0, fadeColor);

    painter->setBrush(gradient);
    painter->drawEllipse(indicatorRect);
    painter->restore();
}

// Legacy method for compatibility
void FluentRadioButton::paintRadioButton(QPainter* painter) {
    paintRadio(painter);
}

void FluentRadioButton::paintIcon(QPainter* painter) {
    if (m_icon.isNull() || m_iconRect.isEmpty())
        return;

    painter->save();

    QIcon::Mode mode = QIcon::Normal;
    if (!isEnabled()) {
        mode = QIcon::Disabled;
    } else if (m_pressed) {
        mode = QIcon::Selected;
    }

    QIcon::State iconState = m_checked ? QIcon::On : QIcon::Off;
    QPixmap pixmap = m_icon.pixmap(m_iconRect.size(), mode, iconState);

    painter->drawPixmap(m_iconRect, pixmap);

    painter->restore();
}

// New primary label painting method
void FluentRadioButton::paintLabel(QPainter* painter) {
    if (m_text.isEmpty() || m_labelRect.isEmpty())
        return;

    painter->save();

    painter->setPen(m_textColor);
    painter->setFont(font());

    const Qt::Alignment alignment = Qt::AlignLeft | Qt::AlignVCenter;
    painter->drawText(m_labelRect, alignment, m_text);

    painter->restore();
}

// Legacy method for compatibility
void FluentRadioButton::paintText(QPainter* painter) { paintLabel(painter); }

void FluentRadioButton::paintFocusIndicator(QPainter* painter) {
    if (!hasFocus())
        return;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    const auto& theme = Styling::FluentTheme::instance();
    const QColor focusColor = theme.color("accent");

    // Enhanced FluentUI focus indicator - subtle outer ring
    QPen focusPen(focusColor, 2);
    focusPen.setCapStyle(Qt::RoundCap);
    painter->setPen(focusPen);
    painter->setBrush(Qt::NoBrush);

    // Draw focus ring around the radio button with proper spacing
    const QRect radioRect = m_radioRect;
    const int focusMargin = 4;  // FluentUI standard focus margin
    const QRect focusRect = radioRect.adjusted(-focusMargin, -focusMargin,
                                               focusMargin, focusMargin);

    // Draw subtle focus ring
    QColor focusRingColor = focusColor;
    focusRingColor.setAlphaF(0.6);
    QPen focusRingPen(focusRingColor, 1.5);
    painter->setPen(focusRingPen);
    painter->drawEllipse(focusRect);

    painter->restore();
}

// Enhanced hover effect painting method with FluentUI styling
void FluentRadioButton::paintHoverEffect(QPainter* painter) {
    if (!m_hovered || !isEnabled())
        return;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    const auto& theme = Styling::FluentTheme::instance();
    QColor hoverColor = theme.color("controlFillSecondary");
    hoverColor.setAlphaF(0.08);  // Very subtle hover effect per FluentUI

    // Paint subtle background hover effect around the entire control
    painter->setPen(Qt::NoPen);
    painter->setBrush(hoverColor);

    // FluentUI style: rounded rectangle with proper corner radius
    const int cornerRadius = 6;  // FluentUI standard corner radius
    painter->drawRoundedRect(rect().adjusted(2, 2, -2, -2), cornerRadius,
                             cornerRadius);

    // Additional subtle highlight around radio button itself
    if (m_radioRect.isValid()) {
        QColor radioHoverColor = theme.color("controlStrokeSecondary");
        radioHoverColor.setAlphaF(0.15);

        QPen hoverPen(radioHoverColor, 1);
        painter->setPen(hoverPen);
        painter->setBrush(Qt::NoBrush);

        const int hoverMargin = 2;
        const QRect hoverRect = m_radioRect.adjusted(-hoverMargin, -hoverMargin,
                                                     hoverMargin, hoverMargin);
        painter->drawEllipse(hoverRect);
    }

    painter->restore();
}

void FluentRadioButton::startCheckAnimation() {
    if (!m_indicatorAnimation || !m_animated)
        return;

    m_indicatorAnimation->stop();
    m_indicatorAnimation->setStartValue(m_indicatorOpacity);
    m_indicatorAnimation->setEndValue(m_checked ? 1.0 : 0.0);
    m_indicatorAnimation->start();
}

void FluentRadioButton::startPressAnimation() {
    if (!m_scaleAnimation || !m_animated)
        return;

    m_scaleAnimation->stop();
    m_scaleAnimation->setStartValue(m_radioScale);
    m_scaleAnimation->setEndValue(0.95);
    m_scaleAnimation->start();
}

void FluentRadioButton::startReleaseAnimation() {
    if (!m_scaleAnimation || !m_animated)
        return;

    m_scaleAnimation->stop();
    m_scaleAnimation->setStartValue(m_radioScale);
    m_scaleAnimation->setEndValue(1.0);
    m_scaleAnimation->start();
}

// Duplicate methods removed - already implemented earlier in the file

void FluentRadioButton::startHoverAnimation(bool hover) {
    if (!m_colorAnimation || !m_animated)
        return;

    const auto& theme = Styling::FluentTheme::instance();
    QColor targetColor = hover ? theme.color("controlStrokeSecondary")
                               : theme.color("controlStrokeDefault");

    m_colorAnimation->stop();
    m_colorAnimation->setStartValue(m_borderColor);
    m_colorAnimation->setEndValue(targetColor);
    m_colorAnimation->start();
}

// Missing animation methods implementation
void FluentRadioButton::animateRadioScale() {
    if (!m_scaleAnimation || !m_animated)
        return;

    m_scaleAnimation->stop();
    m_scaleAnimation->setStartValue(m_radioScale);
    m_scaleAnimation->setEndValue(m_checked ? 1.05 : 1.0);
    m_scaleAnimation->start();
}

void FluentRadioButton::animateColors() {
    if (!m_colorAnimation || !m_animated)
        return;

    updateColors();  // Update target colors based on current state

    m_colorAnimation->stop();
    m_colorAnimation->setStartValue(m_borderColor);
    m_colorAnimation->setEndValue(m_borderColor);
    m_colorAnimation->start();
}

void FluentRadioButton::animateIndicatorOpacity() {
    if (!m_indicatorAnimation || !m_animated)
        return;

    m_indicatorAnimation->stop();
    m_indicatorAnimation->setStartValue(m_indicatorOpacity);
    m_indicatorAnimation->setEndValue(m_checked ? 1.0 : 0.0);
    m_indicatorAnimation->start();
}

// Color getter methods implementation
QColor FluentRadioButton::getRadioColor() const { return m_borderColor; }

QColor FluentRadioButton::getIndicatorColor() const { return m_indicatorColor; }

QColor FluentRadioButton::getTextColor() const { return m_textColor; }

QColor FluentRadioButton::getBorderColor() const { return m_borderColor; }

QColor FluentRadioButton::getHoverColor() const {
    // Thread-safe access to cached color for better performance
    QMutexLocker locker(&m_mutex);
    if (m_colorsCacheValid) {
        return m_cachedHoverColor;
    }
    const auto& theme = Styling::FluentTheme::instance();
    return theme.color("controlFillSecondary");
}

QColor FluentRadioButton::getFocusColor() const {
    // Thread-safe access to cached color for better performance
    QMutexLocker locker(&m_mutex);
    if (m_colorsCacheValid) {
        return m_cachedFocusColor;
    }
    const auto& theme = Styling::FluentTheme::instance();
    return theme.color("accent");
}

// Hit test method implementation
bool FluentRadioButton::hitTestRadio(const QPoint& position) const {
    if (m_layoutDirty) {
        const_cast<FluentRadioButton*>(this)->updateLayout();
    }
    return m_radioRect.contains(position);
}

// Property accessors for QPropertyAnimation
qreal FluentRadioButton::radioScale() const { return m_radioScale; }

void FluentRadioButton::setRadioScale(qreal scale) {
    if (qFuzzyCompare(m_radioScale, scale))
        return;
    m_radioScale = scale;
    update();
}

qreal FluentRadioButton::indicatorOpacity() const { return m_indicatorOpacity; }

void FluentRadioButton::setIndicatorOpacity(qreal opacity) {
    if (qFuzzyCompare(m_indicatorOpacity, opacity))
        return;
    m_indicatorOpacity = opacity;
    update();
}

QColor FluentRadioButton::radioColor() const { return m_radioColor; }

void FluentRadioButton::setRadioColor(const QColor& color) {
    if (m_radioColor == color)
        return;
    m_radioColor = color;
    m_borderColor = color;  // Keep border color in sync
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
