#pragma once

#include <QGraphicsEffect>
#include <QPropertyAnimation>
#include <QTimer>
#include <QWidget>
#include "FluentQt/Core/FluentState.h"

#include <concepts>
#include <memory>

namespace FluentQt::Core {

class FluentComponent : public QWidget {
    Q_OBJECT
    Q_PROPERTY(FluentState state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(bool animated READ isAnimated WRITE setAnimated)
    Q_PROPERTY(int cornerRadius READ cornerRadius WRITE setCornerRadius)

    // ElaWidgetTools-inspired properties
    Q_PROPERTY(bool smoothTransitions READ smoothTransitions WRITE
                   setSmoothTransitions NOTIFY smoothTransitionsChanged)
    Q_PROPERTY(int transitionDuration READ transitionDuration WRITE
                   setTransitionDuration NOTIFY transitionDurationChanged)

public:
    explicit FluentComponent(QWidget *parent = nullptr);
    virtual ~FluentComponent() = default;

    // State management
    FluentState state() const noexcept { return m_state; }
    void setState(FluentState state);

    // Animation properties
    bool isAnimated() const noexcept { return m_animated; }
    void setAnimated(bool animated) { m_animated = animated; }

    // Corner radius
    int cornerRadius() const noexcept { return m_cornerRadius; }
    void setCornerRadius(int radius);

    // ElaWidgetTools-inspired smooth transitions
    bool smoothTransitions() const noexcept { return m_smoothTransitions; }
    void setSmoothTransitions(bool enabled);

    int transitionDuration() const noexcept { return m_transitionDuration; }
    void setTransitionDuration(int duration);

    // Reactive state management
    template <typename T>
    void bindProperty(const QString &property, std::function<T()> getter) {
        // Provide a minimal inline template implementation so callers in tests
        // link correctly. For now, simply invoke the getter once and set a
        // dynamic property to the value. Future work: connect to change signals
        // and keep it reactive.
        QVariant value = QVariant::fromValue(getter());
        this->setProperty(property.toUtf8().constData(), value);
    }

signals:
    void stateChanged(FluentState state);
    void themeChanged();
    void smoothTransitionsChanged(bool enabled);
    void transitionDurationChanged(int duration);

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

    virtual void updateStateStyle();
    virtual void performStateTransition(FluentState from, FluentState to);

private:
    FluentState m_state{FluentState::Normal};
    bool m_animated{true};
    int m_cornerRadius{static_cast<int>(FluentCornerRadius::Medium)};
    std::unique_ptr<QPropertyAnimation> m_stateAnimation;

    // ElaWidgetTools-inspired properties
    bool m_smoothTransitions{true};
    int m_transitionDuration{200};
};

}  // namespace FluentQt::Core
