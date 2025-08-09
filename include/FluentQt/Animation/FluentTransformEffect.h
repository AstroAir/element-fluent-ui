#pragma once

#include <QGraphicsEffect>
#include <QObject>
#include <QPointF>

class FluentTransformEffect : public QGraphicsEffect
{
    Q_OBJECT
    Q_PROPERTY(qreal scale READ scale WRITE setScale)
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation)
    Q_PROPERTY(QPointF translation READ translation WRITE setTranslation)

public:
    explicit FluentTransformEffect(QObject *parent = nullptr);

    qreal scale() const;
    void setScale(qreal scale);

    qreal rotation() const;
    void setRotation(qreal rotation);

    QPointF translation() const;
    void setTranslation(const QPointF &translation);

protected:
    void draw(QPainter *painter) override;

private:
    qreal m_scale = 1.0;
    qreal m_rotation = 0.0;
    QPointF m_translation = QPointF(0, 0);
};
