// include/FluentQt/Components/FluentBadge.h
#pragma once

#include "FluentQt/Core/FluentComponent.h"
#include "FluentQt/Animation/FluentAnimator.h"
#include <QWidget>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QIcon>

namespace FluentQt::Components {

enum class FluentBadgeType {
    Dot,            // Small dot indicator
    Count,          // Numeric count badge
    Text,           // Text-based badge
    Icon,           // Icon badge
    Status          // Status indicator with color
};

enum class FluentBadgeSize {
    Small,          // 16px height
    Medium,         // 20px height
    Large           // 24px height
};

enum class FluentBadgeStyle {
    Filled,         // Solid background
    Outline,        // Border only
    Subtle,         // Light background
    Ghost           // Minimal styling
};

enum class FluentBadgePosition {
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
    Center,
    Inline          // Not positioned, flows with content
};

enum class FluentBadgeStatus {
    None,
    Success,
    Warning,
    Error,
    Info,
    Neutral
};

class FluentBadge : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(int maxCount READ maxCount WRITE setMaxCount NOTIFY maxCountChanged)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(FluentBadgeType badgeType READ badgeType WRITE setBadgeType NOTIFY badgeTypeChanged)
    Q_PROPERTY(FluentBadgeSize badgeSize READ badgeSize WRITE setBadgeSize NOTIFY badgeSizeChanged)
    Q_PROPERTY(FluentBadgeStyle badgeStyle READ badgeStyle WRITE setBadgeStyle NOTIFY badgeStyleChanged)
    Q_PROPERTY(FluentBadgePosition position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(FluentBadgeStatus status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QColor customColor READ customColor WRITE setCustomColor NOTIFY customColorChanged)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibilityChanged)
    Q_PROPERTY(bool animated READ isAnimated WRITE setAnimated NOTIFY animatedChanged)
    Q_PROPERTY(bool showZero READ showZero WRITE setShowZero NOTIFY showZeroChanged)
    Q_PROPERTY(bool pulsing READ isPulsing WRITE setPulsing NOTIFY pulsingChanged)

public:
    explicit FluentBadge(QWidget* parent = nullptr);
    explicit FluentBadge(const QString& text, QWidget* parent = nullptr);
    explicit FluentBadge(int count, QWidget* parent = nullptr);
    explicit FluentBadge(const QIcon& icon, QWidget* parent = nullptr);
    explicit FluentBadge(FluentBadgeType type, QWidget* parent = nullptr);
    ~FluentBadge() override;

    // Content properties
    QString text() const;
    void setText(const QString& text);
    
    int count() const;
    void setCount(int count);
    
    int maxCount() const;
    void setMaxCount(int maxCount);
    
    QIcon icon() const;
    void setIcon(const QIcon& icon);

    // Type and appearance
    FluentBadgeType badgeType() const;
    void setBadgeType(FluentBadgeType type);
    
    FluentBadgeSize badgeSize() const;
    void setBadgeSize(FluentBadgeSize size);
    
    FluentBadgeStyle badgeStyle() const;
    void setBadgeStyle(FluentBadgeStyle style);
    
    FluentBadgePosition position() const;
    void setPosition(FluentBadgePosition position);
    
    FluentBadgeStatus status() const;
    void setStatus(FluentBadgeStatus status);
    
    QColor customColor() const;
    void setCustomColor(const QColor& color);

    // Behavior properties
    bool isVisible() const;
    void setVisible(bool visible) override;
    
    bool isAnimated() const;
    void setAnimated(bool animated);
    
    bool showZero() const;
    void setShowZero(bool show);
    
    bool isPulsing() const;
    void setPulsing(bool pulsing);

    // Utility methods
    QString displayText() const;
    QRect badgeRect() const;
    bool isEmpty() const;

    // Positioning for parent widget
    void attachTo(QWidget* parent, FluentBadgePosition position = FluentBadgePosition::TopRight);
    void detach();
    QWidget* attachedWidget() const { return m_attachedWidget; }

    // Size hints
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // Static convenience methods
    static FluentBadge* createCountBadge(int count, QWidget* parent = nullptr);
    static FluentBadge* createStatusBadge(FluentBadgeStatus status, QWidget* parent = nullptr);
    static FluentBadge* createDotBadge(QWidget* parent = nullptr);
    static FluentBadge* createTextBadge(const QString& text, QWidget* parent = nullptr);

public slots:
    void show();
    void hide();
    void animateIn();
    void animateOut();
    void pulse();
    void stopPulsing();
    void increment();
    void decrement();
    void reset();

signals:
    void textChanged(const QString& text);
    void countChanged(int count);
    void maxCountChanged(int maxCount);
    void iconChanged(const QIcon& icon);
    void badgeTypeChanged(FluentBadgeType type);
    void badgeSizeChanged(FluentBadgeSize size);
    void badgeStyleChanged(FluentBadgeStyle style);
    void positionChanged(FluentBadgePosition position);
    void statusChanged(FluentBadgeStatus status);
    void customColorChanged(const QColor& color);
    void visibilityChanged(bool visible);
    void animatedChanged(bool animated);
    void showZeroChanged(bool show);
    void pulsingChanged(bool pulsing);
    void offsetChanged(const QPoint& offset);
    void attachedWidgetChanged(QWidget* widget);
    void backgroundColorChanged(const QColor& color);
    void textColorChanged(const QColor& color);

    void clicked();
    void doubleClicked();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void moveEvent(QMoveEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void onPulseAnimationFinished();
    void onShowAnimationFinished();
    void onHideAnimationFinished();
    void onPulseTimer();
    void updatePosition();
    void updateColors();

private:
    void setupAnimations();
    void updateLayout();
    void updateSizeMetrics();
    void updateDisplayContent();
    void updateContent();
    void updateVisibility();
    void updateAccessibility();

    // Painting methods
    void paintBackground(QPainter* painter, const QRect& rect);
    void paintBorder(QPainter* painter, const QRect& rect);
    void paintContent(QPainter* painter, const QRect& rect);
    void paintText(QPainter* painter, const QRect& rect);
    void paintIcon(QPainter* painter, const QRect& rect);
    void paintDot(QPainter* painter);

    // Additional painting helpers matching implementation
    void paintTextBadge(QPainter* painter);
    void paintIconBadge(QPainter* painter);

    // Animation methods
    void startShowAnimation();
    void startHideAnimation();
    void startPulseAnimation();
    void stopAllAnimations();
    void startSinglePulse();

    // Utility methods
    QRect calculateBadgeRect() const;
    QRect calculateContentRect() const;
    QSize calculateTextSize() const;
    QSize calculateIconSize() const;
    QSize contentSize() const;

    QColor getBackgroundColor() const;
    QColor getTextColor() const;
    QColor getBorderColor() const;
    QColor getStatusColor() const;
    
    QFont getFont() const;
    int getBorderWidth() const;
    int getPadding() const;
    
    void updateAttachedPosition();
    void installAttachedEventFilter();
    void removeAttachedEventFilter();

private:
    // Content properties
    QString m_text;
    int m_count{0};
    int m_maxCount{99};
    QIcon m_icon;
    
    // Type and appearance
    FluentBadgeType m_badgeType{FluentBadgeType::Count};
    FluentBadgeSize m_badgeSize{FluentBadgeSize::Medium};
    FluentBadgeStyle m_badgeStyle{FluentBadgeStyle::Filled};
    FluentBadgePosition m_position{FluentBadgePosition::TopRight};
    FluentBadgeStatus m_status{FluentBadgeStatus::None};
    QColor m_customColor;
    
    // Behavior properties
    bool m_animated{true};
    bool m_showZero{false};
    bool m_pulsing{false};
    
    // State
    bool m_isEmpty{true};
    QString m_displayText;
    
    // Attachment
    QWidget* m_attachedWidget{nullptr};
    QPoint m_attachmentOffset;
    
    // Size metrics (updated based on m_badgeSize)
    int m_height{20};
    int m_minWidth{20};
    int m_minHeight{20};
    int m_padding{6};
    int m_iconSize{12};
    int m_fontSize{11};
    int m_dotSize{10};

    // Extra colors and flags used in implementation
    QColor m_backgroundColor;
    QColor m_textColor;
    bool m_hasCustomBackgroundColor{false};
    bool m_hasCustomTextColor{false};
    bool m_autoHide{false};
    QPoint m_offset;

    // Animation
    std::unique_ptr<Animation::FluentAnimator> m_animator;
    QPropertyAnimation* m_showAnimation{nullptr};
    QPropertyAnimation* m_hideAnimation{nullptr};
    QPropertyAnimation* m_pulseAnimation{nullptr};
    QPropertyAnimation* m_scaleAnimation{nullptr};
    QPropertyAnimation* m_opacityAnimation{nullptr};
    QTimer* m_pulseTimer{nullptr};

    // Animation properties
    Q_PROPERTY(qreal badgeScale READ badgeScale WRITE setBadgeScale)
    Q_PROPERTY(qreal badgeOpacity READ badgeOpacity WRITE setBadgeOpacity)

    qreal m_badgeScale{1.0};
    qreal m_badgeOpacity{1.0};
    qreal m_pulseScale{1.0};

    // Property accessors for animations
    qreal badgeScale() const { return m_badgeScale; }
    void setBadgeScale(qreal scale);

    qreal badgeOpacity() const { return m_badgeOpacity; }
    void setBadgeOpacity(qreal opacity);

    qreal pulseScale() const;
    void setPulseScale(qreal scale);
};

} // namespace FluentQt::Components
