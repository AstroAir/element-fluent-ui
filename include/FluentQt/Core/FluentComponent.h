#pragma once

#include "FluentQt/Core/FluentState.h"
#include <QGraphicsEffect>
#include <QPropertyAnimation>
#include <QTimer>
#include <QWidget>

#include <concepts>
#include <memory>


namespace FluentQt::Core {

class FluentComponent : public QWidget {
  Q_OBJECT
  Q_PROPERTY(FluentState state READ state WRITE setState NOTIFY stateChanged)
  Q_PROPERTY(bool animated READ isAnimated WRITE setAnimated)
  Q_PROPERTY(int cornerRadius READ cornerRadius WRITE setCornerRadius)

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

  // Reactive state management
  template <typename T>
  void bindProperty(const QString &property, std::function<T()> getter) {
    // Provide a minimal inline template implementation so callers in tests link correctly.
    // For now, simply invoke the getter once and set a dynamic property to the value.
    // Future work: connect to change signals and keep it reactive.
    QVariant value = QVariant::fromValue(getter());
    this->setProperty(property.toUtf8().constData(), value);
  }

signals:
  void stateChanged(FluentState state);
  void themeChanged();

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
};

} // namespace FluentQt::Core