#pragma once

#include <QObject>
#include <QWidget>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <memory>
#include "FluentAnimator.h"

namespace FluentQt::Animation {

class FluentAnimation : public QObject {
    Q_OBJECT

public:
    explicit FluentAnimation(QWidget* target, QObject* parent = nullptr);
    ~FluentAnimation();

    // Animation methods
    FluentAnimation& fadeIn(int duration = 200, FluentEasing easing = FluentEasing::EaseOut);
    FluentAnimation& fadeOut(int duration = 200, FluentEasing easing = FluentEasing::EaseIn);
    FluentAnimation& scaleTo(qreal scale, int duration = 200, FluentEasing easing = FluentEasing::EaseOut);
    FluentAnimation& slideTo(const QPointF& pos, int duration = 200, FluentEasing easing = FluentEasing::EaseOut);
    FluentAnimation& rotateTo(qreal angle, int duration = 200, FluentEasing easing = FluentEasing::EaseOut);
    FluentAnimation& pause(int duration);

    // Control methods
    FluentAnimation& with(); // Next animation will be parallel with the previous one

    // Execution
    void start();

private:
    void addAnimation(QAbstractAnimation* animation);

    QWidget* m_target;
    QSequentialAnimationGroup* m_root;
    QAbstractAnimation* m_lastAnimation = nullptr;
    bool m_parallel = false;
};

} // namespace FluentQt::Animation
