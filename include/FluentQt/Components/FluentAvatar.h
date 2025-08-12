// include/FluentQt/Components/FluentAvatar.h
#pragma once

#include <QIcon>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QWidget>
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

enum class FluentAvatarType {
    Image,       // Display user image
    Initials,    // Display user initials
    Icon,        // Display icon
    Placeholder  // Display placeholder/default avatar
};

enum class FluentAvatarSize {
    ExtraSmall,  // 20px
    Small,       // 32px
    Medium,      // 40px
    Large,       // 56px
    ExtraLarge,  // 72px
    Huge         // 96px
};

enum class FluentAvatarShape {
    Circle,  // Circular avatar
    Square,  // Square avatar
    Rounded  // Rounded square avatar
};

enum class FluentAvatarStatus {
    None,         // No status indicator
    Online,       // Green dot
    Busy,         // Red dot
    Away,         // Yellow dot
    Offline,      // Gray dot
    DoNotDisturb  // Red with white line
};

class FluentAvatar : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(
        QString initials READ initials WRITE setInitials NOTIFY initialsChanged)
    Q_PROPERTY(QPixmap image READ image WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(FluentAvatarType avatarType READ avatarType WRITE setAvatarType
                   NOTIFY avatarTypeChanged)
    Q_PROPERTY(FluentAvatarSize avatarSize READ avatarSize WRITE setAvatarSize
                   NOTIFY avatarSizeChanged)
    Q_PROPERTY(
        FluentAvatarShape shape READ shape WRITE setShape NOTIFY shapeChanged)
    Q_PROPERTY(FluentAvatarStatus status READ status WRITE setStatus NOTIFY
                   statusChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE
                   setBackgroundColor NOTIFY backgroundColorChanged)
    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor NOTIFY
                   textColorChanged)
    Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor NOTIFY
                   borderColorChanged)
    Q_PROPERTY(int borderWidth READ borderWidth WRITE setBorderWidth NOTIFY
                   borderWidthChanged)
    Q_PROPERTY(bool showBorder READ showBorder WRITE setShowBorder NOTIFY
                   showBorderChanged)
    Q_PROPERTY(bool showStatus READ showStatus WRITE setShowStatus NOTIFY
                   showStatusChanged)
    Q_PROPERTY(bool clickable READ isClickable WRITE setClickable NOTIFY
                   clickableChanged)
    Q_PROPERTY(
        bool animated READ isAnimated WRITE setAnimated NOTIFY animatedChanged)
    Q_PROPERTY(
        QString tooltip READ tooltip WRITE setTooltip NOTIFY tooltipChanged)

public:
    explicit FluentAvatar(QWidget* parent = nullptr);
    explicit FluentAvatar(const QString& name, QWidget* parent = nullptr);
    explicit FluentAvatar(const QPixmap& image, QWidget* parent = nullptr);
    explicit FluentAvatar(const QIcon& icon, QWidget* parent = nullptr);
    explicit FluentAvatar(FluentAvatarSize size, QWidget* parent = nullptr);
    ~FluentAvatar() override;

    // Content properties
    QString name() const;
    void setName(const QString& name);

    QString initials() const;
    void setInitials(const QString& initials);

    QPixmap image() const;
    void setImage(const QPixmap& image);
    void setImage(const QString& imagePath);

    QIcon icon() const;
    void setIcon(const QIcon& icon);

    // Type and appearance
    FluentAvatarType avatarType() const;
    void setAvatarType(FluentAvatarType type);

    FluentAvatarSize avatarSize() const;
    void setAvatarSize(FluentAvatarSize size);

    FluentAvatarShape shape() const;
    void setShape(FluentAvatarShape shape);

    FluentAvatarStatus status() const;
    void setStatus(FluentAvatarStatus status);

    // Color properties
    QColor backgroundColor() const;
    void setBackgroundColor(const QColor& color);

    QColor textColor() const;
    void setTextColor(const QColor& color);

    QColor borderColor() const;
    void setBorderColor(const QColor& color);

    // Border properties
    int borderWidth() const;
    void setBorderWidth(int width);

    bool showBorder() const;
    void setShowBorder(bool show);

    // Status properties
    bool showStatus() const;
    void setShowStatus(bool show);

    // Behavior properties
    bool isClickable() const;
    void setClickable(bool clickable);

    bool isAnimated() const;
    void setAnimated(bool animated);

    QString tooltip() const;
    void setTooltip(const QString& tooltip);

    // Utility methods
    int avatarDiameter() const;
    QSize statusSize() const;
    QRect avatarRect() const;
    QRect statusRect() const;
    QRect contentRect() const;

    QString generateInitials(const QString& name) const;
    QColor generateBackgroundColor(const QString& text) const;
    bool hasValidImage() const;
    bool hasValidIcon() const;

    // Size hints
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // Static convenience methods
    static FluentAvatar* createImageAvatar(
        const QPixmap& image, FluentAvatarSize size = FluentAvatarSize::Medium,
        QWidget* parent = nullptr);
    static FluentAvatar* createInitialsAvatar(
        const QString& name, FluentAvatarSize size = FluentAvatarSize::Medium,
        QWidget* parent = nullptr);
    static FluentAvatar* createIconAvatar(
        const QIcon& icon, FluentAvatarSize size = FluentAvatarSize::Medium,
        QWidget* parent = nullptr);
    static FluentAvatar* createPlaceholderAvatar(
        FluentAvatarSize size = FluentAvatarSize::Medium,
        QWidget* parent = nullptr);

public slots:
    void refresh();
    void animateIn();
    void animateOut();
    void pulse();
    void bounce();
    void loadImageFromUrl(const QString& url);

signals:
    void nameChanged(const QString& name);
    void initialsChanged(const QString& initials);
    void imageChanged(const QPixmap& image);
    void iconChanged(const QIcon& icon);
    void avatarTypeChanged(FluentAvatarType type);
    void avatarSizeChanged(FluentAvatarSize size);
    void shapeChanged(FluentAvatarShape shape);
    void statusChanged(FluentAvatarStatus status);
    void backgroundColorChanged(const QColor& color);
    void textColorChanged(const QColor& color);
    void borderColorChanged(const QColor& color);
    void borderWidthChanged(int width);
    void showBorderChanged(bool show);
    void showStatusChanged(bool show);
    void clickableChanged(bool clickable);
    void animatedChanged(bool animated);
    void tooltipChanged(const QString& tooltip);

    void clicked();
    void doubleClicked();
    void imageLoaded(const QPixmap& image);
    void imageLoadFailed(const QString& error);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;

private slots:
    void onAnimationFinished();
    void onImageLoadFinished();
    void updateColors();
    void updateTooltip();

private:
    void setupAnimations();
    void updateLayout();
    void updateSizeMetrics();
    void updateContent();
    void updateAutoType();

    // Painting methods
    void paintAvatar(QPainter* painter);
    void paintBackground(QPainter* painter, const QRect& rect);
    void paintBorder(QPainter* painter, const QRect& rect);
    void paintImage(QPainter* painter, const QRect& rect);
    void paintInitials(QPainter* painter, const QRect& rect);
    void paintIcon(QPainter* painter, const QRect& rect);
    void paintPlaceholder(QPainter* painter, const QRect& rect);
    void paintStatus(QPainter* painter, const QRect& rect);
    void paintHoverEffect(QPainter* painter, const QRect& rect);
    void paintPressEffect(QPainter* painter, const QRect& rect);

    // Animation methods
    void startEnterAnimation();
    void startExitAnimation();
    void startPulseAnimation();
    void startBounceAnimation();
    void startHoverAnimation(bool hover);
    void stopAllAnimations();

    // Utility methods
    QRect calculateAvatarRect() const;
    QRect calculateStatusRect() const;
    QRect calculateContentRect() const;

    QColor getBackgroundColor() const;
    QColor getTextColor() const;
    QColor getBorderColor() const;
    QColor getStatusColor() const;

    QFont getInitialsFont() const;
    QPixmap getScaledImage() const;
    QIcon getScaledIcon() const;
    QPainterPath getAvatarPath(const QRect& rect) const;

    void updateInitialsFromName();
    void updateBackgroundFromInitials();
    void loadImageAsync(const QString& path);

private:
    // Content properties
    QString m_name;
    QString m_initials;
    QPixmap m_image;
    QIcon m_icon;
    QString m_tooltip;

    // Type and appearance
    FluentAvatarType m_avatarType{FluentAvatarType::Placeholder};
    FluentAvatarSize m_avatarSize{FluentAvatarSize::Medium};
    FluentAvatarShape m_shape{FluentAvatarShape::Circle};
    FluentAvatarStatus m_status{FluentAvatarStatus::None};

    // Color properties
    QColor m_backgroundColor;
    QColor m_textColor;
    QColor m_borderColor;

    // Border properties
    int m_borderWidth{0};
    bool m_showBorder{false};

    // Status properties
    bool m_showStatus{false};

    // Behavior properties
    bool m_clickable{false};
    bool m_animated{true};

    // State
    bool m_pressed{false};
    bool m_hovered{false};
    bool m_hasCustomBackgroundColor{false};
    bool m_hasCustomTextColor{false};
    bool m_hasCustomInitials{false};
    bool m_autoType{true};  // Automatically determine type based on content

    // Size metrics (updated based on m_avatarSize)
    int m_diameter{40};
    int m_statusDiameter{12};
    int m_cornerRadius{8};
    int m_fontSize{14};

    // Layout cache
    QRect m_avatarRect;
    QRect m_statusRect;
    QRect m_contentRect;
    bool m_layoutDirty{true};

    // Animation
    std::unique_ptr<Animation::FluentAnimator> m_animator;
    QPropertyAnimation* m_scaleAnimation{nullptr};
    QPropertyAnimation* m_opacityAnimation{nullptr};
    QSequentialAnimationGroup* m_pulseAnimation{nullptr};
    QSequentialAnimationGroup* m_bounceAnimation{nullptr};
    QPropertyAnimation* m_hoverAnimation{nullptr};

    // Animation properties
    Q_PROPERTY(qreal avatarScale READ avatarScale WRITE setAvatarScale)
    Q_PROPERTY(qreal avatarOpacity READ avatarOpacity WRITE setAvatarOpacity)
    Q_PROPERTY(qreal hoverOpacity READ hoverOpacity WRITE setHoverOpacity)

    qreal m_avatarScale{1.0};
    qreal m_avatarOpacity{1.0};
    qreal m_hoverOpacity{0.0};

    // Property accessors for animations
    qreal avatarScale() const { return m_avatarScale; }
    void setAvatarScale(qreal scale);

    qreal avatarOpacity() const { return m_avatarOpacity; }
    void setAvatarOpacity(qreal opacity);

    qreal hoverOpacity() const { return m_hoverOpacity; }
    void setHoverOpacity(qreal opacity);
};

}  // namespace FluentQt::Components
