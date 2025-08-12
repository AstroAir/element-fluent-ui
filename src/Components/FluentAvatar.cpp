// src/Components/FluentAvatar.cpp
#include "FluentQt/Components/FluentAvatar.h"
#include <QAccessible>
#include <QApplication>
#include <QBuffer>
#include <QFileInfo>
#include <QFontMetrics>
#include <QImageReader>
#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPainter>
#include <QTimer>
#include <cmath>
#include "FluentQt/Styling/FluentTheme.h"

namespace FluentQt::Components {

FluentAvatar::FluentAvatar(QWidget* parent)
    : Core::FluentComponent(parent),
      m_animator(std::make_unique<Animation::FluentAnimator>(this)),
      m_networkManager(new QNetworkAccessManager(this)) {
    setupAnimations();
    updateColors();
    updateInitials();
    updateAccessibility();
    setFixedSize(40, 40);  // Default size

    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this,
            &FluentAvatar::updateColors);

    connect(m_networkManager, &QNetworkAccessManager::finished, this,
            &FluentAvatar::onImageDownloaded);
}

FluentAvatar::FluentAvatar(const QString& name, QWidget* parent)
    : FluentAvatar(parent) {
    setName(name);
}

FluentAvatar::FluentAvatar(const QPixmap& image, QWidget* parent)
    : FluentAvatar(parent) {
    setImage(image);
}

FluentAvatar::FluentAvatar(const QIcon& icon, QWidget* parent)
    : FluentAvatar(parent) {
    setIcon(icon);
}

FluentAvatar::FluentAvatar(FluentAvatarSize size, QWidget* parent)
    : FluentAvatar(parent) {
    setAvatarSize(size);
}

FluentAvatar::~FluentAvatar() = default;

QString FluentAvatar::name() const { return m_name; }

void FluentAvatar::setName(const QString& name) {
    if (m_name == name)
        return;

    m_name = name;
    updateInitials();
    update();
    emit nameChanged(m_name);
    updateAccessibility();
}

QPixmap FluentAvatar::image() const { return m_image; }

void FluentAvatar::setImage(const QPixmap& image) {
    if (m_image.cacheKey() == image.cacheKey())
        return;

    m_image = image;
    m_scaledImage = QPixmap();  // Clear cached scaled image
    update();
    emit imageChanged(m_image);
}

QIcon FluentAvatar::icon() const { return m_icon; }

void FluentAvatar::setIcon(const QIcon& icon) {
    if (m_icon.cacheKey() == icon.cacheKey())
        return;

    m_icon = icon;
    update();
    emit iconChanged(m_icon);
}

QString FluentAvatar::initials() const { return m_initials; }

void FluentAvatar::setInitials(const QString& initials) {
    if (m_initials == initials)
        return;

    m_initials = initials.left(3);  // Limit to 3 characters
    update();
    emit initialsChanged(m_initials);
}

FluentAvatarSize FluentAvatar::avatarSize() const { return m_avatarSize; }

void FluentAvatar::setAvatarSize(FluentAvatarSize size) {
    if (m_avatarSize == size)
        return;

    m_avatarSize = size;
    updateSizeMetrics();
    updateGeometry();
    update();
    emit avatarSizeChanged(m_avatarSize);
}

FluentAvatarShape FluentAvatar::shape() const { return m_shape; }

void FluentAvatar::setShape(FluentAvatarShape shape) {
    if (m_shape == shape)
        return;

    m_shape = shape;
    update();
    emit shapeChanged(m_shape);
}

FluentAvatarStatus FluentAvatar::status() const { return m_status; }

void FluentAvatar::setStatus(FluentAvatarStatus status) {
    if (m_status == status)
        return;

    m_status = status;
    update();
    emit statusChanged(m_status);
}

QColor FluentAvatar::backgroundColor() const { return m_backgroundColor; }

void FluentAvatar::setBackgroundColor(const QColor& color) {
    if (m_backgroundColor == color)
        return;

    m_backgroundColor = color;
    m_hasCustomBackgroundColor = true;
    update();
    emit backgroundColorChanged(m_backgroundColor);
}

QColor FluentAvatar::textColor() const { return m_textColor; }

void FluentAvatar::setTextColor(const QColor& color) {
    if (m_textColor == color)
        return;

    m_textColor = color;
    m_hasCustomTextColor = true;
    update();
    emit textColorChanged(m_textColor);
}

bool FluentAvatar::showBorder() const { return m_showBorder; }

void FluentAvatar::setShowBorder(bool show) {
    if (m_showBorder == show)
        return;

    m_showBorder = show;
    update();
    emit showBorderChanged(m_showBorder);
}

bool FluentAvatar::showStatus() const { return m_showStatus; }

void FluentAvatar::setShowStatus(bool show) {
    if (m_showStatus == show)
        return;

    m_showStatus = show;
    update();
    emit showStatusChanged(m_showStatus);
}

bool FluentAvatar::isClickable() const { return m_clickable; }

void FluentAvatar::setClickable(bool clickable) {
    if (m_clickable == clickable)
        return;

    m_clickable = clickable;
    setCursor(m_clickable ? Qt::PointingHandCursor : Qt::ArrowCursor);
    setAttribute(Qt::WA_Hover, m_clickable);
    emit clickableChanged(m_clickable);
}

bool FluentAvatar::isAnimated() const { return m_animated; }

void FluentAvatar::setAnimated(bool animated) {
    if (m_animated == animated)
        return;

    m_animated = animated;
    emit animatedChanged(m_animated);
}

QString FluentAvatar::imageUrl() const { return m_imageUrl; }

void FluentAvatar::setImageUrl(const QString& url) {
    if (m_imageUrl == url)
        return;

    m_imageUrl = url;

    if (!url.isEmpty()) {
        loadImageFromUrl(url);
    }

    emit imageUrlChanged(m_imageUrl);
}

QSize FluentAvatar::sizeHint() const { return QSize(m_size, m_size); }

QSize FluentAvatar::minimumSizeHint() const { return QSize(16, 16); }

FluentAvatar* FluentAvatar::createWithName(const QString& name,
                                           QWidget* parent) {
    return new FluentAvatar(name, parent);
}

FluentAvatar* FluentAvatar::createWithImage(const QPixmap& image,
                                            QWidget* parent) {
    return new FluentAvatar(image, parent);
}

FluentAvatar* FluentAvatar::createWithIcon(const QIcon& icon, QWidget* parent) {
    return new FluentAvatar(icon, parent);
}

FluentAvatar* FluentAvatar::createSmall(QWidget* parent) {
    return new FluentAvatar(FluentAvatarSize::Small, parent);
}

FluentAvatar* FluentAvatar::createMedium(QWidget* parent) {
    return new FluentAvatar(FluentAvatarSize::Medium, parent);
}

FluentAvatar* FluentAvatar::createLarge(QWidget* parent) {
    return new FluentAvatar(FluentAvatarSize::Large, parent);
}

FluentAvatar* FluentAvatar::createExtraLarge(QWidget* parent) {
    return new FluentAvatar(FluentAvatarSize::ExtraLarge, parent);
}

void FluentAvatar::loadImageFromFile(const QString& filePath) {
    QPixmap pixmap(filePath);
    if (!pixmap.isNull()) {
        setImage(pixmap);
    }
}

void FluentAvatar::loadImageFromUrl(const QString& url) {
    if (url.isEmpty())
        return;

    QNetworkRequest request(QUrl(url));
    request.setRawHeader("User-Agent", "FluentQt Avatar Component");
    m_networkManager->get(request);
}

void FluentAvatar::clearImage() { setImage(QPixmap()); }

void FluentAvatar::clearIcon() { setIcon(QIcon()); }

void FluentAvatar::reset() {
    clearImage();
    clearIcon();
    setName(QString());
    setInitials(QString());
}

void FluentAvatar::setupAnimations() {
    // Hover animation
    m_hoverAnimation = new QPropertyAnimation(this, "hoverOpacity", this);
    m_hoverAnimation->setDuration(150);
    m_hoverAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // Press animation
    m_pressAnimation = new QPropertyAnimation(this, "pressScale", this);
    m_pressAnimation->setDuration(100);
    m_pressAnimation->setEasingCurve(QEasingCurve::OutBack);

    // Status pulse animation
    m_statusAnimation = new QPropertyAnimation(this, "statusOpacity", this);
    m_statusAnimation->setDuration(1000);
    m_statusAnimation->setEasingCurve(QEasingCurve::InOutSine);
    m_statusAnimation->setLoopCount(-1);
    m_statusAnimation->setStartValue(0.5);
    m_statusAnimation->setEndValue(1.0);
}

void FluentAvatar::updateSizeMetrics() {
    switch (m_avatarSize) {
        case FluentAvatarSize::Small:
            m_size = 24;
            break;
        case FluentAvatarSize::Medium:
            m_size = 40;
            break;
        case FluentAvatarSize::Large:
            m_size = 56;
            break;
        case FluentAvatarSize::ExtraLarge:
            m_size = 72;
            break;
    }

    setFixedSize(m_size, m_size);
}

void FluentAvatar::updateInitials() {
    if (!m_name.isEmpty() && m_initials.isEmpty()) {
        QStringList words = m_name.split(' ', Qt::SkipEmptyParts);
        QString generatedInitials;

        for (const QString& word : words) {
            if (!word.isEmpty()) {
                generatedInitials += word.at(0).toUpper();
                if (generatedInitials.length() >= 2)
                    break;
            }
        }

        m_initials = generatedInitials;
    }
}

QPixmap FluentAvatar::getScaledImage() const {
    if (m_image.isNull())
        return QPixmap();

    if (m_scaledImage.isNull() ||
        m_scaledImage.size() != QSize(m_size, m_size)) {
        const int devicePixelRatio = devicePixelRatioF();
        const QSize scaledSize(m_size * devicePixelRatio,
                               m_size * devicePixelRatio);

        m_scaledImage =
            m_image.scaled(scaledSize, Qt::KeepAspectRatioByExpanding,
                           Qt::SmoothTransformation);
        m_scaledImage.setDevicePixelRatio(devicePixelRatio);

        // Crop to circle if needed
        if (m_shape == FluentAvatarShape::Circle) {
            QPixmap circularImage(scaledSize);
            circularImage.fill(Qt::transparent);
            circularImage.setDevicePixelRatio(devicePixelRatio);

            QPainter painter(&circularImage);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setBrush(QBrush(m_scaledImage));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(circularImage.rect());

            m_scaledImage = circularImage;
        }
    }

    return m_scaledImage;
}

QColor FluentAvatar::generateBackgroundColor() const {
    if (m_hasCustomBackgroundColor) {
        return m_backgroundColor;
    }

    // Generate color based on name hash
    if (!m_name.isEmpty()) {
        const auto& theme = Styling::FluentTheme::instance();

        // Simple hash function
        uint hash = 0;
        for (const QChar& ch : m_name) {
            hash = hash * 31 + ch.unicode();
        }

        // Predefined color palette
        const QList<QColor> colors = {
            QColor(232, 17, 35),   // Red
            QColor(234, 0, 94),    // Pink
            QColor(136, 23, 152),  // Purple
            QColor(0, 120, 215),   // Blue
            QColor(0, 153, 188),   // Cyan
            QColor(16, 124, 16),   // Green
            QColor(255, 140, 0),   // Orange
            QColor(255, 185, 0),   // Yellow
        };

        return colors[hash % colors.size()];
    }

    const auto& theme = Styling::FluentTheme::instance();
    return theme.color(Styling::FluentThemeColor::AccentFillDefault);
}

void FluentAvatar::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Apply transformations
    const QPointF center = rect().center();
    painter.translate(center);
    painter.scale(m_pressScale, m_pressScale);
    painter.translate(-center);

    // Set opacity
    painter.setOpacity(m_hoverOpacity);

    // Paint avatar content
    paintAvatar(&painter);

    // Paint border
    if (m_showBorder) {
        paintBorder(&painter);
    }

    // Paint status indicator
    if (m_showStatus && m_status != FluentAvatarStatus::None) {
        paintStatus(&painter);
    }
}

void FluentAvatar::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_clickable && isEnabled()) {
        if (m_animated) {
            startPressAnimation();
        }
        emit clicked();
    }

    Core::FluentComponent::mousePressEvent(event);
}

void FluentAvatar::enterEvent(QEnterEvent* event) {
    if (m_clickable && isEnabled()) {
        setState(Core::FluentState::Hovered);

        if (m_animated) {
            startHoverAnimation(true);
        }
    }

    Core::FluentComponent::enterEvent(event);
}

void FluentAvatar::leaveEvent(QEvent* event) {
    setState(Core::FluentState::Normal);

    if (m_animated) {
        startHoverAnimation(false);
    }

    Core::FluentComponent::leaveEvent(event);
}

void FluentAvatar::onImageDownloaded(QNetworkReply* reply) {
    reply->deleteLater();

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QPixmap pixmap;

        if (pixmap.loadFromData(data)) {
            setImage(pixmap);
        }
    }
}

void FluentAvatar::paintAvatar(QPainter* painter) {
    const QRect rect = this->rect();

    if (!m_image.isNull()) {
        paintImage(painter, rect);
    } else if (!m_icon.isNull()) {
        paintIcon(painter, rect);
    } else {
        paintInitials(painter, rect);
    }
}

void FluentAvatar::paintImage(QPainter* painter, const QRect& rect) {
    const QPixmap scaledImage = getScaledImage();
    if (scaledImage.isNull())
        return;

    painter->save();

    if (m_shape == FluentAvatarShape::Circle) {
        // Create circular clipping path
        QPainterPath path;
        path.addEllipse(rect);
        painter->setClipPath(path);
    } else {
        // Rounded rectangle
        const qreal radius = rect.width() * 0.1;  // 10% of width
        QPainterPath path;
        path.addRoundedRect(rect, radius, radius);
        painter->setClipPath(path);
    }

    // Draw image
    const QRect imageRect = scaledImage.rect();
    const QRect targetRect = rect;

    // Center the image
    const QPoint offset((targetRect.width() - imageRect.width()) / 2,
                        (targetRect.height() - imageRect.height()) / 2);

    painter->drawPixmap(targetRect.topLeft() + offset, scaledImage);

    painter->restore();
}

void FluentAvatar::paintIcon(QPainter* painter, const QRect& rect) {
    painter->save();

    // Draw background
    const QColor bgColor = generateBackgroundColor();
    painter->setPen(Qt::NoPen);
    painter->setBrush(bgColor);

    if (m_shape == FluentAvatarShape::Circle) {
        painter->drawEllipse(rect);
    } else {
        const qreal radius = rect.width() * 0.1;
        painter->drawRoundedRect(rect, radius, radius);
    }

    // Draw icon
    const int iconSize = rect.width() * 0.6;  // 60% of avatar size
    const QRect iconRect(rect.center().x() - iconSize / 2,
                         rect.center().y() - iconSize / 2, iconSize, iconSize);

    QIcon::Mode mode = isEnabled() ? QIcon::Normal : QIcon::Disabled;
    QPixmap iconPixmap = m_icon.pixmap(QSize(iconSize, iconSize), mode);
    painter->drawPixmap(iconRect, iconPixmap);

    painter->restore();
}

void FluentAvatar::paintInitials(QPainter* painter, const QRect& rect) {
    painter->save();

    // Draw background
    const QColor bgColor = generateBackgroundColor();
    painter->setPen(Qt::NoPen);
    painter->setBrush(bgColor);

    if (m_shape == FluentAvatarShape::Circle) {
        painter->drawEllipse(rect);
    } else {
        const qreal radius = rect.width() * 0.1;
        painter->drawRoundedRect(rect, radius, radius);
    }

    // Draw initials
    if (!m_initials.isEmpty()) {
        painter->setPen(m_textColor);

        // Calculate font size based on avatar size
        QFont font = this->font();
        const int fontSize = rect.width() * 0.4;  // 40% of avatar size
        font.setPixelSize(fontSize);
        font.setBold(true);
        painter->setFont(font);

        painter->drawText(rect, Qt::AlignCenter, m_initials);
    }

    painter->restore();
}

void FluentAvatar::paintBorder(QPainter* painter) {
    const QRect rect = this->rect();
    const auto& theme = Styling::FluentTheme::instance();
    const QColor borderColor =
        theme.color(Styling::FluentThemeColor::ControlStrokeDefault);

    painter->save();

    QPen borderPen(borderColor, 2);
    painter->setPen(borderPen);
    painter->setBrush(Qt::NoBrush);

    if (m_shape == FluentAvatarShape::Circle) {
        painter->drawEllipse(rect.adjusted(1, 1, -1, -1));
    } else {
        const qreal radius = rect.width() * 0.1;
        painter->drawRoundedRect(rect.adjusted(1, 1, -1, -1), radius, radius);
    }

    painter->restore();
}

void FluentAvatar::paintStatus(QPainter* painter) {
    const QRect rect = this->rect();
    const int statusSize = rect.width() * 0.25;  // 25% of avatar size

    // Position status indicator at bottom-right
    const QRect statusRect(rect.right() - statusSize,
                           rect.bottom() - statusSize, statusSize, statusSize);

    painter->save();
    painter->setOpacity(m_statusOpacity);

    QColor statusColor;
    switch (m_status) {
        case FluentAvatarStatus::Online:
            statusColor = QColor(16, 124, 16);  // Green
            break;
        case FluentAvatarStatus::Away:
            statusColor = QColor(255, 140, 0);  // Orange
            break;
        case FluentAvatarStatus::Busy:
            statusColor = QColor(196, 43, 28);  // Red
            break;
        case FluentAvatarStatus::Offline:
            statusColor = QColor(128, 128, 128);  // Gray
            break;
        case FluentAvatarStatus::None:
            painter->restore();
            return;
    }

    // Draw status background (white border)
    painter->setPen(QPen(Qt::white, 2));
    painter->setBrush(statusColor);
    painter->drawEllipse(statusRect);

    painter->restore();
}

void FluentAvatar::startHoverAnimation(bool hover) {
    if (!m_hoverAnimation)
        return;

    m_hoverAnimation->stop();
    m_hoverAnimation->setStartValue(m_hoverOpacity);
    m_hoverAnimation->setEndValue(hover ? 0.8 : 1.0);
    m_hoverAnimation->start();
}

void FluentAvatar::startPressAnimation() {
    if (!m_pressAnimation)
        return;

    m_pressAnimation->stop();
    m_pressAnimation->setStartValue(m_pressScale);
    m_pressAnimation->setEndValue(0.95);
    m_pressAnimation->start();

    // Return to normal scale
    QTimer::singleShot(m_pressAnimation->duration(), this, [this]() {
        if (m_pressAnimation) {
            m_pressAnimation->setStartValue(m_pressScale);
            m_pressAnimation->setEndValue(1.0);
            m_pressAnimation->start();
        }
    });
}

void FluentAvatar::startStatusAnimation() {
    if (m_status == FluentAvatarStatus::Online && m_statusAnimation) {
        m_statusAnimation->start();
    } else if (m_statusAnimation) {
        m_statusAnimation->stop();
        m_statusOpacity = 1.0;
    }
}

void FluentAvatar::updateColors() {
    const auto& theme = Styling::FluentTheme::instance();

    if (!m_hasCustomBackgroundColor) {
        m_backgroundColor = generateBackgroundColor();
    }

    if (!m_hasCustomTextColor) {
        m_textColor =
            theme.color(Styling::FluentThemeColor::TextOnAccentFillPrimary);
    }

    update();
}

void FluentAvatar::updateAccessibility() {
#ifndef QT_NO_ACCESSIBILITY
    if (QAccessible::isActive()) {
        QString accessibleText;

        if (!m_name.isEmpty()) {
            accessibleText = tr("Avatar for %1").arg(m_name);
        } else if (!m_initials.isEmpty()) {
            accessibleText = tr("Avatar with initials %1").arg(m_initials);
        } else {
            accessibleText = tr("Avatar");
        }

        setAccessibleName(accessibleText);
        setAccessibleDescription(accessibleText);

        QAccessibleEvent event(this, QAccessible::NameChanged);
        QAccessible::updateAccessibility(&event);
    }
#endif
}

// Property accessors for animations
qreal FluentAvatar::hoverOpacity() const { return m_hoverOpacity; }

void FluentAvatar::setHoverOpacity(qreal opacity) {
    if (qFuzzyCompare(m_hoverOpacity, opacity))
        return;
    m_hoverOpacity = opacity;
    update();
}

qreal FluentAvatar::pressScale() const { return m_pressScale; }

void FluentAvatar::setPressScale(qreal scale) {
    if (qFuzzyCompare(m_pressScale, scale))
        return;
    m_pressScale = scale;
    update();
}

qreal FluentAvatar::statusOpacity() const { return m_statusOpacity; }

void FluentAvatar::setStatusOpacity(qreal opacity) {
    if (qFuzzyCompare(m_statusOpacity, opacity))
        return;
    m_statusOpacity = opacity;
    update();
}

}  // namespace FluentQt::Components
