// src/Components/FluentCheckBox.cpp
#include "FluentQt/Components/FluentCheckBox.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QApplication>
#include <QEnterEvent>
#include <QFocusEvent>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

namespace FluentQt::Components {

FluentCheckBox::FluentCheckBox(QWidget* parent)
    : FluentComponent(parent),
      m_animator(std::make_unique<Animation::FluentAnimator>(this)) {
    setObjectName("FluentCheckBox");
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover, true);

    setupAnimations();
    updateColors();
    updateFonts();

    // Set minimum size
    setMinimumSize(m_iconSize.width() + 8, m_iconSize.height());
}

FluentCheckBox::FluentCheckBox(const QString& text, QWidget* parent)
    : FluentCheckBox(parent) {
    setText(text);
}

FluentCheckBox::~FluentCheckBox() = default;

void FluentCheckBox::setupAnimations() {
    FLUENT_PROFILE("FluentCheckBox::setupAnimations");

    // Create opacity effect for check mark
    m_checkOpacityEffect = new QGraphicsOpacityEffect(this);
    m_checkOpacityEffect->setOpacity(0.0);

    // Create check animation
    m_checkAnimation = new QPropertyAnimation(this, "checkProgress", this);
    m_checkAnimation->setDuration(200);
    m_checkAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // Create state animation
    m_stateAnimation = new QPropertyAnimation(this, "geometry", this);
    m_stateAnimation->setDuration(150);
    m_stateAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // Connect theme changes
    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this,
            &FluentCheckBox::updateAnimations);
}

QString FluentCheckBox::text() const { return m_text; }

void FluentCheckBox::setText(const QString& text) {
    if (m_text != text) {
        m_text = text;
        updateGeometry();
        update();
        emit textChanged(text);
    }
}

FluentCheckState FluentCheckBox::checkState() const { return m_checkState; }

void FluentCheckBox::setCheckState(FluentCheckState state) {
    if (m_checkState != state) {
        m_checkState = state;
        animateCheckChange();
        emit checkStateChanged(state);
        emit stateChanged(static_cast<int>(state));

        if (state == FluentCheckState::Checked ||
            state == FluentCheckState::Unchecked) {
            emit toggled(state == FluentCheckState::Checked);
        }
    }
}

bool FluentCheckBox::isChecked() const {
    return m_checkState == FluentCheckState::Checked;
}

void FluentCheckBox::setChecked(bool checked) {
    setCheckState(checked ? FluentCheckState::Checked
                          : FluentCheckState::Unchecked);
}

bool FluentCheckBox::isTristate() const { return m_tristate; }

void FluentCheckBox::setTristate(bool tristate) {
    if (m_tristate != tristate) {
        m_tristate = tristate;
        emit tristateChanged(tristate);
    }
}

bool FluentCheckBox::autoExclusive() const { return m_autoExclusive; }

void FluentCheckBox::setAutoExclusive(bool autoExclusive) {
    if (m_autoExclusive != autoExclusive) {
        m_autoExclusive = autoExclusive;
        emit autoExclusiveChanged(autoExclusive);
    }
}

QSize FluentCheckBox::iconSize() const { return m_iconSize; }

void FluentCheckBox::setIconSize(const QSize& size) {
    if (m_iconSize != size) {
        m_iconSize = size;
        updateGeometry();
        update();
        emit iconSizeChanged(size);
    }
}

QSize FluentCheckBox::sizeHint() const {
    const QFontMetrics fm(font());
    const int textWidth =
        m_text.isEmpty() ? 0 : fm.horizontalAdvance(m_text) + 8;
    const int width = m_iconSize.width() + textWidth;
    const int height = qMax(m_iconSize.height(), fm.height());

    return QSize(width, height);
}

QSize FluentCheckBox::minimumSizeHint() const {
    return QSize(m_iconSize.width(), m_iconSize.height());
}

void FluentCheckBox::toggle() { nextCheckState(); }

void FluentCheckBox::click() {
    nextCheckState();
    emit clicked(isChecked());
}

void FluentCheckBox::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRect checkRect = checkBoxRect();
    const QRect textRect = this->textRect();

    // Draw checkbox
    drawCheckBox(painter, checkRect);

    // Draw check mark or partial mark
    if (m_checkState == FluentCheckState::Checked) {
        drawCheckMark(painter, checkRect);
    } else if (m_checkState == FluentCheckState::PartiallyChecked) {
        drawPartialMark(painter, checkRect);
    }

    // Draw text
    if (!m_text.isEmpty()) {
        const auto& theme = Styling::FluentTheme::instance();
        painter.setPen(theme.color("textPrimary"));
        painter.setFont(font());
        painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, m_text);
    }

    // Draw focus indicator
    if (hasFocus()) {
        const auto& theme = Styling::FluentTheme::instance();
        QPen focusPen(theme.color("accent"), 2);
        focusPen.setStyle(Qt::DashLine);
        painter.setPen(focusPen);
        painter.drawRect(rect().adjusted(1, 1, -1, -1));
    }
}

void FluentCheckBox::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_pressed = true;
        setState(Core::FluentState::Pressed);
        animateStateChange();
        update();
    }
    FluentComponent::mousePressEvent(event);
}

void FluentCheckBox::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_pressed) {
        m_pressed = false;
        setState(Core::FluentState::Hovered);

        if (rect().contains(event->pos())) {
            click();
        }

        animateStateChange();
        update();
    }
    FluentComponent::mouseReleaseEvent(event);
}

void FluentCheckBox::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return) {
        click();
        event->accept();
        return;
    }
    FluentComponent::keyPressEvent(event);
}

void FluentCheckBox::focusInEvent(QFocusEvent* event) {
    FluentComponent::focusInEvent(event);
    setState(Core::FluentState::Focused);
    update();
}

void FluentCheckBox::focusOutEvent(QFocusEvent* event) {
    FluentComponent::focusOutEvent(event);
    setState(Core::FluentState::Normal);
    update();
}

void FluentCheckBox::enterEvent(QEnterEvent* event) {
    FluentComponent::enterEvent(event);
    if (!m_pressed) {
        setState(Core::FluentState::Hovered);
        animateStateChange();
        update();
    }
}

void FluentCheckBox::leaveEvent(QEvent* event) {
    FluentComponent::leaveEvent(event);
    if (!m_pressed && !hasFocus()) {
        setState(Core::FluentState::Normal);
        animateStateChange();
        update();
    }
}

void FluentCheckBox::changeEvent(QEvent* event) {
    FluentComponent::changeEvent(event);

    if (event->type() == QEvent::EnabledChange) {
        setState(isEnabled() ? Core::FluentState::Normal
                             : Core::FluentState::Disabled);
        update();
    } else if (event->type() == QEvent::FontChange) {
        updateGeometry();
    }
}

void FluentCheckBox::updateAnimations() {
    updateColors();
    updateFonts();
    update();
}

void FluentCheckBox::updateColors() {
    // Colors are handled in paintEvent based on current theme
}

void FluentCheckBox::updateFonts() {
    const auto& theme = Styling::FluentTheme::instance();
    setFont(theme.bodyFont());
}

void FluentCheckBox::animateStateChange() {
    if (m_stateAnimation->state() == QAbstractAnimation::Running) {
        m_stateAnimation->stop();
    }

    m_stateAnimation->setStartValue(geometry());
    m_stateAnimation->setEndValue(geometry());
    m_stateAnimation->start();
}

void FluentCheckBox::animateCheckChange() {
    if (m_checkAnimation->state() == QAbstractAnimation::Running) {
        m_checkAnimation->stop();
    }

    qreal targetProgress = 0.0;
    if (m_checkState == FluentCheckState::Checked) {
        targetProgress = 1.0;
    } else if (m_checkState == FluentCheckState::PartiallyChecked) {
        targetProgress = 0.5;
    }

    m_checkAnimation->setStartValue(m_checkProgress);
    m_checkAnimation->setEndValue(targetProgress);
    m_checkAnimation->start();
}

void FluentCheckBox::drawCheckBox(QPainter& painter, const QRect& rect) const {
    const auto& theme = Styling::FluentTheme::instance();

    // Determine colors based on state
    QColor backgroundColor;
    QColor borderColor;
    int borderWidth = 1;

    if (!isEnabled()) {
        backgroundColor = theme.color("controlFillDisabled");
        borderColor = theme.color("controlStrokeDisabled");
    } else if (m_checkState == FluentCheckState::Checked ||
               m_checkState == FluentCheckState::PartiallyChecked) {
        backgroundColor = theme.color("accent");
        borderColor = theme.color("accent");
        borderWidth = 2;
    } else {
        switch (state()) {
            case Core::FluentState::Pressed:
                backgroundColor = theme.color("controlFillTertiary");
                borderColor = theme.color("controlStrokeDefault");
                borderWidth = 2;
                break;
            case Core::FluentState::Hovered:
                backgroundColor = theme.color("controlFillSecondary");
                borderColor = theme.color("controlStrokeSecondary");
                break;
            default:
                backgroundColor = theme.color("controlFillDefault");
                borderColor = theme.color("controlStrokeDefault");
                break;
        }
    }

    // Draw background
    const int radius = theme.borderRadius("small");
    QPainterPath path;
    path.addRoundedRect(rect, radius, radius);

    painter.fillPath(path, backgroundColor);

    // Draw border
    QPen borderPen(borderColor, borderWidth);
    painter.setPen(borderPen);
    painter.drawPath(path);
}

void FluentCheckBox::drawCheckMark(QPainter& painter, const QRect& rect) const {
    const auto& theme = Styling::FluentTheme::instance();

    // Use white color for check mark on accent background
    QColor checkColor = theme.color("textOnAccentPrimary");
    if (!isEnabled()) {
        checkColor = theme.color("textDisabled");
    }

    painter.setPen(
        QPen(checkColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    // Draw check mark path
    const QPointF center = rect.center();
    const qreal size = rect.width() * 0.6;
    const qreal offset = size * 0.3;

    QPainterPath checkPath;
    checkPath.moveTo(center.x() - offset, center.y());
    checkPath.lineTo(center.x() - offset * 0.3, center.y() + offset * 0.6);
    checkPath.lineTo(center.x() + offset, center.y() - offset * 0.4);

    // Apply check progress for animation
    painter.setOpacity(m_checkProgress);
    painter.drawPath(checkPath);
    painter.setOpacity(1.0);
}

void FluentCheckBox::drawPartialMark(QPainter& painter,
                                     const QRect& rect) const {
    const auto& theme = Styling::FluentTheme::instance();

    // Use white color for partial mark on accent background
    QColor markColor = theme.color("textOnAccentPrimary");
    if (!isEnabled()) {
        markColor = theme.color("textDisabled");
    }

    painter.setPen(QPen(markColor, 2, Qt::SolidLine, Qt::RoundCap));

    // Draw horizontal line for partial state
    const QPointF center = rect.center();
    const qreal width = rect.width() * 0.5;

    painter.drawLine(center.x() - width / 2, center.y(), center.x() + width / 2,
                     center.y());
}

QRect FluentCheckBox::checkBoxRect() const {
    const int y = (height() - m_iconSize.height()) / 2;
    return QRect(0, y, m_iconSize.width(), m_iconSize.height());
}

QRect FluentCheckBox::textRect() const {
    if (m_text.isEmpty()) {
        return QRect();
    }

    const int x = m_iconSize.width() + 8;
    const int y = 0;
    const int width = this->width() - x;
    const int height = this->height();

    return QRect(x, y, width, height);
}

void FluentCheckBox::nextCheckState() {
    if (m_autoExclusive && m_checkState != FluentCheckState::Checked) {
        // For auto-exclusive checkboxes, uncheck siblings
        if (parentWidget()) {
            const auto siblings =
                parentWidget()->findChildren<FluentCheckBox*>();
            for (auto* sibling : siblings) {
                if (sibling != this && sibling->autoExclusive()) {
                    sibling->setChecked(false);
                }
            }
        }
        setCheckState(FluentCheckState::Checked);
        return;
    }

    // Normal toggle behavior
    switch (m_checkState) {
        case FluentCheckState::Unchecked:
            setCheckState(FluentCheckState::Checked);
            break;
        case FluentCheckState::Checked:
            if (m_tristate) {
                setCheckState(FluentCheckState::PartiallyChecked);
            } else {
                setCheckState(FluentCheckState::Unchecked);
            }
            break;
        case FluentCheckState::PartiallyChecked:
            setCheckState(FluentCheckState::Unchecked);
            break;
    }
}

}  // namespace FluentQt::Components
