// src/Core/FluentComponent.cpp
#include "FluentQt/Core/FluentComponent.h"
#include <QEnterEvent>
#include <QMouseEvent>
#include <QFocusEvent>
#include <QPropertyAnimation>

namespace FluentQt::Core {

FluentComponent::FluentComponent(QWidget *parent)
    : QWidget(parent)
    , m_stateAnimation(std::make_unique<QPropertyAnimation>(this, "geometry"))
{
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover, true);
    
    // Initialize animation
    m_stateAnimation->setDuration(200);
    m_stateAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

void FluentComponent::setState(FluentState state) {
    if (m_state == state) return;
    
    FluentState oldState = m_state;
    m_state = state;
    
    if (m_animated) {
        performStateTransition(oldState, state);
    }
    
    updateStateStyle();
    emit stateChanged(state);
}

void FluentComponent::setCornerRadius(int radius) {
    if (m_cornerRadius == radius) return;
    
    m_cornerRadius = radius;
    update();
}

void FluentComponent::enterEvent(QEnterEvent *event) {
    QWidget::enterEvent(event);
    if (isEnabled()) {
        setState(FluentState::Hovered);
    }
}

void FluentComponent::leaveEvent(QEvent *event) {
    QWidget::leaveEvent(event);
    if (isEnabled()) {
        setState(hasFocus() ? FluentState::Focused : FluentState::Normal);
    }
}

void FluentComponent::mousePressEvent(QMouseEvent *event) {
    QWidget::mousePressEvent(event);
    if (isEnabled() && event->button() == Qt::LeftButton) {
        setState(FluentState::Pressed);
    }
}

void FluentComponent::mouseReleaseEvent(QMouseEvent *event) {
    QWidget::mouseReleaseEvent(event);
    if (isEnabled()) {
        setState(underMouse() ? FluentState::Hovered : FluentState::Normal);
    }
}

void FluentComponent::focusInEvent(QFocusEvent *event) {
    QWidget::focusInEvent(event);
    if (isEnabled()) {
        setState(FluentState::Focused);
    }
}

void FluentComponent::focusOutEvent(QFocusEvent *event) {
    QWidget::focusOutEvent(event);
    if (isEnabled()) {
        setState(underMouse() ? FluentState::Hovered : FluentState::Normal);
    }
}

void FluentComponent::updateStateStyle() {
    // Base implementation - subclasses should override
    // This method should update the visual appearance based on current state
    update();
}

void FluentComponent::performStateTransition(FluentState from, FluentState to) {
    // Base implementation for state transitions
    // Subclasses can override for custom transition animations
    Q_UNUSED(from)
    Q_UNUSED(to)
    
    if (m_stateAnimation && m_stateAnimation->state() == QAbstractAnimation::Running) {
        m_stateAnimation->stop();
    }
}

} // namespace FluentQt::Core
