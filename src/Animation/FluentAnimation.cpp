#include "FluentQt/Animation/FluentAnimation.h"
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include "FluentQt/Animation/FluentTransformEffect.h"

namespace FluentQt::Animation {

FluentAnimation::FluentAnimation(QWidget* target, QObject* parent)
    : QObject(parent), m_target(target) {
    m_root = new QSequentialAnimationGroup(this);
}

FluentAnimation::~FluentAnimation() {}

FluentAnimation& FluentAnimation::fadeIn(int duration, FluentEasing easing) {
    auto* opacityEffect =
        qobject_cast<QGraphicsOpacityEffect*>(m_target->graphicsEffect());
    if (!opacityEffect) {
        opacityEffect = new QGraphicsOpacityEffect(m_target);
        m_target->setGraphicsEffect(opacityEffect);
    }
    opacityEffect->setOpacity(0.0);
    m_target->show();

    auto* animation = new QPropertyAnimation(opacityEffect, "opacity");
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->setDuration(duration);
    animation->setEasingCurve(FluentAnimator::toQtEasing(easing));

    addAnimation(animation);
    return *this;
}

FluentAnimation& FluentAnimation::fadeOut(int duration, FluentEasing easing) {
    auto* opacityEffect =
        qobject_cast<QGraphicsOpacityEffect*>(m_target->graphicsEffect());
    if (!opacityEffect) {
        opacityEffect = new QGraphicsOpacityEffect(m_target);
        m_target->setGraphicsEffect(opacityEffect);
    }

    auto* animation = new QPropertyAnimation(opacityEffect, "opacity");
    animation->setStartValue(m_target->windowOpacity());
    animation->setEndValue(0.0);
    animation->setDuration(duration);
    animation->setEasingCurve(FluentAnimator::toQtEasing(easing));

    connect(animation, &QPropertyAnimation::finished,
            [=]() { m_target->hide(); });

    addAnimation(animation);
    return *this;
}

FluentAnimation& FluentAnimation::scaleTo(qreal scale, int duration,
                                          FluentEasing easing) {
    auto* effect =
        qobject_cast<FluentTransformEffect*>(m_target->graphicsEffect());
    if (!effect) {
        effect = new FluentTransformEffect(m_target);
        m_target->setGraphicsEffect(effect);
    }

    auto* animation = new QPropertyAnimation(effect, "scale");
    animation->setStartValue(effect->scale());
    animation->setEndValue(scale);
    animation->setDuration(duration);
    animation->setEasingCurve(FluentAnimator::toQtEasing(easing));

    addAnimation(animation);
    return *this;
}

FluentAnimation& FluentAnimation::slideTo(const QPointF& pos, int duration,
                                          FluentEasing easing) {
    auto* effect =
        qobject_cast<FluentTransformEffect*>(m_target->graphicsEffect());
    if (!effect) {
        effect = new FluentTransformEffect(m_target);
        m_target->setGraphicsEffect(effect);
    }

    auto* animation = new QPropertyAnimation(effect, "translation");
    animation->setStartValue(effect->translation());
    animation->setEndValue(pos);
    animation->setDuration(duration);
    animation->setEasingCurve(FluentAnimator::toQtEasing(easing));

    addAnimation(animation);
    return *this;
}

FluentAnimation& FluentAnimation::rotateTo(qreal angle, int duration,
                                           FluentEasing easing) {
    auto* effect =
        qobject_cast<FluentTransformEffect*>(m_target->graphicsEffect());
    if (!effect) {
        effect = new FluentTransformEffect(m_target);
        m_target->setGraphicsEffect(effect);
    }

    auto* animation = new QPropertyAnimation(effect, "rotation");
    animation->setStartValue(effect->rotation());
    animation->setEndValue(angle);
    animation->setDuration(duration);
    animation->setEasingCurve(FluentAnimator::toQtEasing(easing));

    addAnimation(animation);
    return *this;
}

FluentAnimation& FluentAnimation::pause(int duration) {
    m_root->addPause(duration);
    return *this;
}

FluentAnimation& FluentAnimation::with() {
    m_parallel = true;
    return *this;
}

void FluentAnimation::start() {
    if (m_autoDelete) {
        connect(m_root, &QSequentialAnimationGroup::finished, this,
                &FluentAnimation::deleteLater);
    }
    m_root->start();
}

void FluentAnimation::addAnimation(QAbstractAnimation* animation) {
    if (m_parallel && m_lastAnimation) {
        m_parallel = false;

        if (auto* lastParallelGroup =
                qobject_cast<QParallelAnimationGroup*>(m_lastAnimation)) {
            lastParallelGroup->addAnimation(animation);
        } else {
            int index = m_root->animationCount() - 1;
            m_root->takeAnimation(index);

            auto* parallelGroup = new QParallelAnimationGroup();
            parallelGroup->addAnimation(m_lastAnimation);
            parallelGroup->addAnimation(animation);

            m_root->insertAnimation(index, parallelGroup);
            m_lastAnimation = parallelGroup;
        }
    } else {
        m_root->addAnimation(animation);
        m_lastAnimation = animation;
    }
}

}  // namespace FluentQt::Animation
