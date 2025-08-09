#include "FluentQt/Animation/FluentTransformEffect.h"
#include <QPainter>

FluentTransformEffect::FluentTransformEffect(QObject *parent)
    : QGraphicsEffect(parent)
{
}

qreal FluentTransformEffect::scale() const
{
    return m_scale;
}

void FluentTransformEffect::setScale(qreal scale)
{
    if (m_scale != scale) {
        m_scale = scale;
        update();
    }
}

qreal FluentTransformEffect::rotation() const
{
    return m_rotation;
}

void FluentTransformEffect::setRotation(qreal rotation)
{
    if (m_rotation != rotation) {
        m_rotation = rotation;
        update();
    }
}

QPointF FluentTransformEffect::translation() const
{
    return m_translation;
}

void FluentTransformEffect::setTranslation(const QPointF &translation)
{
    if (m_translation != translation) {
        m_translation = translation;
        update();
    }
}

void FluentTransformEffect::draw(QPainter *painter)
{
    QPoint offset;
    const QPixmap pixmap = sourcePixmap(Qt::DeviceCoordinates, &offset, QGraphicsEffect::PadToEffectiveBoundingRect);

    if (pixmap.isNull()) {
        return;
    }

    painter->save();

    QPointF center = QPointF(pixmap.rect().center());
    painter->translate(center + m_translation);
    painter->rotate(m_rotation);
    painter->scale(m_scale, m_scale);
    painter->translate(-center);

    painter->drawPixmap(0, 0, pixmap);

    painter->restore();
}
