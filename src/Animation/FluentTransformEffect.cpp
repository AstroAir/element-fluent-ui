#include "FluentQt/Animation/FluentTransformEffect.h"
#include <QApplication>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QPainter>
#include <QWidget>

FluentTransformEffect::FluentTransformEffect(QObject *parent)
    : QGraphicsEffect(parent),
      m_useHardwareAcceleration(true),
      m_cacheEnabled(true) {
    // Check if hardware acceleration is available
    if (QOpenGLContext::currentContext()) {
        m_hardwareAccelAvailable = true;
    } else {
        m_hardwareAccelAvailable = false;
        m_useHardwareAcceleration = false;
    }
}

qreal FluentTransformEffect::scale() const { return m_scale; }

void FluentTransformEffect::setScale(qreal scale) {
    if (m_scale != scale) {
        m_scale = scale;
        m_cacheValid = false;  // Invalidate cache
        update();
    }
}

qreal FluentTransformEffect::rotation() const { return m_rotation; }

void FluentTransformEffect::setRotation(qreal rotation) {
    if (m_rotation != rotation) {
        m_rotation = rotation;
        m_cacheValid = false;  // Invalidate cache
        update();
    }
}

QPointF FluentTransformEffect::translation() const { return m_translation; }

void FluentTransformEffect::setTranslation(const QPointF &translation) {
    if (m_translation != translation) {
        m_translation = translation;
        m_cacheValid = false;  // Invalidate cache
        update();
    }
}

void FluentTransformEffect::draw(QPainter *painter) {
    QPoint offset;
    const QPixmap pixmap =
        sourcePixmap(Qt::DeviceCoordinates, &offset,
                     QGraphicsEffect::PadToEffectiveBoundingRect);

    if (pixmap.isNull()) {
        return;
    }

    // Use cached pixmap if available and valid
    if (m_cacheEnabled && m_cacheValid && !m_cachedPixmap.isNull()) {
        painter->drawPixmap(offset, m_cachedPixmap);
        return;
    }

    painter->save();

    // Enable hardware acceleration hints
    if (m_useHardwareAcceleration && m_hardwareAccelAvailable) {
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
        // HighQualityAntialiasing isn't available on all Qt builds;
        // Antialiasing covers primary need
        painter->setRenderHint(QPainter::Antialiasing, true);
    }

    QPointF center = QPointF(pixmap.rect().center());

    // Use QTransform for better performance with complex transformations
    QTransform transform;
    transform.translate(center.x() + m_translation.x(),
                        center.y() + m_translation.y());
    transform.rotate(m_rotation);
    transform.scale(m_scale, m_scale);
    transform.translate(-center.x(), -center.y());

    painter->setTransform(transform, true);
    painter->drawPixmap(0, 0, pixmap);

    painter->restore();

    // Cache the result if caching is enabled
    if (m_cacheEnabled) {
        m_cachedPixmap = pixmap;
        m_cacheValid = true;
    }
}
