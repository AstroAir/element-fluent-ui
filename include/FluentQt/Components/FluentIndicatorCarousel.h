// include/FluentQt/Components/FluentIndicatorCarousel.h
#pragma once

#include "FluentQt/Components/FluentCarousel.h"
#include "FluentQt/Components/FluentButton.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QAbstractButton>
#include <QPainter>
#include <QPropertyAnimation>

namespace FluentQt::Components {

/**
 * @brief Indicator dot styles for carousel
 */
enum class FluentCarouselIndicatorStyle {
    Dots,           // Simple circular dots
    Lines,          // Horizontal/vertical lines
    Numbers,        // Numeric indicators
    Thumbnails,     // Small thumbnail previews
    Progress,       // Progress bar style
    Custom          // Custom indicator style
};

/**
 * @brief Indicator position relative to carousel
 */
enum class FluentCarouselIndicatorPosition {
    Bottom,         // Below the carousel
    Top,            // Above the carousel
    Left,           // Left side of carousel
    Right,          // Right side of carousel
    Overlay         // Overlaid on carousel content
};

/**
 * @brief Carousel with indicator dots showing current position
 * 
 * FluentIndicatorCarousel provides visual position indicators with:
 * - Multiple indicator styles (dots, lines, numbers, thumbnails)
 * - Clickable indicators for direct navigation
 * - Smooth animations between states
 * - Customizable positioning and styling
 * - Accessibility support for screen readers
 * - Responsive design for different screen sizes
 */
class FluentIndicatorCarousel : public FluentCarousel {
    Q_OBJECT
    Q_PROPERTY(FluentCarouselIndicatorStyle indicatorStyle READ indicatorStyle WRITE setIndicatorStyle NOTIFY indicatorStyleChanged)
    Q_PROPERTY(FluentCarouselIndicatorPosition indicatorPosition READ indicatorPosition WRITE setIndicatorPosition NOTIFY indicatorPositionChanged)
    Q_PROPERTY(bool showIndicators READ showIndicators WRITE setShowIndicators NOTIFY indicatorsVisibilityChanged)
    Q_PROPERTY(bool clickableIndicators READ clickableIndicators WRITE setClickableIndicators NOTIFY clickableIndicatorsChanged)
    Q_PROPERTY(int indicatorSize READ indicatorSize WRITE setIndicatorSize NOTIFY indicatorSizeChanged)
    Q_PROPERTY(int indicatorSpacing READ indicatorSpacing WRITE setIndicatorSpacing NOTIFY indicatorSpacingChanged)
    Q_PROPERTY(QColor activeIndicatorColor READ activeIndicatorColor WRITE setActiveIndicatorColor NOTIFY activeIndicatorColorChanged)
    Q_PROPERTY(QColor inactiveIndicatorColor READ inactiveIndicatorColor WRITE setInactiveIndicatorColor NOTIFY inactiveIndicatorColorChanged)

public:
    explicit FluentIndicatorCarousel(QWidget* parent = nullptr);
    explicit FluentIndicatorCarousel(const FluentCarouselConfig& config, QWidget* parent = nullptr);
    ~FluentIndicatorCarousel() override = default;

    // Indicator configuration
    FluentCarouselIndicatorStyle indicatorStyle() const noexcept { return m_indicatorStyle; }
    void setIndicatorStyle(FluentCarouselIndicatorStyle style);

    FluentCarouselIndicatorPosition indicatorPosition() const noexcept { return m_indicatorPosition; }
    void setIndicatorPosition(FluentCarouselIndicatorPosition position);

    bool showIndicators() const noexcept { return m_showIndicators; }
    void setShowIndicators(bool show);

    bool clickableIndicators() const noexcept { return m_clickableIndicators; }
    void setClickableIndicators(bool clickable);

    // Visual properties
    int indicatorSize() const noexcept { return m_indicatorSize; }
    void setIndicatorSize(int size);

    int indicatorSpacing() const noexcept { return m_indicatorSpacing; }
    void setIndicatorSpacing(int spacing);

    QColor activeIndicatorColor() const noexcept { return m_activeIndicatorColor; }
    void setActiveIndicatorColor(const QColor& color);

    QColor inactiveIndicatorColor() const noexcept { return m_inactiveIndicatorColor; }
    void setInactiveIndicatorColor(const QColor& color);

    // Indicator access
    QWidget* indicatorContainer() const noexcept { return m_indicatorContainer; }
    QList<QAbstractButton*> indicatorButtons() const;
    QAbstractButton* indicatorAt(int index) const;

    // Convenience factory methods
    static FluentIndicatorCarousel* createWithDots(QWidget* parent = nullptr);
    static FluentIndicatorCarousel* createWithNumbers(QWidget* parent = nullptr);
    static FluentIndicatorCarousel* createWithThumbnails(QWidget* parent = nullptr);
    static FluentIndicatorCarousel* createOverlay(QWidget* parent = nullptr);

public slots:
    void updateIndicators();
    void setIndicatorActive(int index, bool animated = true);
    void refreshIndicatorStyles();

signals:
    void indicatorStyleChanged(FluentCarouselIndicatorStyle style);
    void indicatorPositionChanged(FluentCarouselIndicatorPosition position);
    void indicatorsVisibilityChanged(bool visible);
    void clickableIndicatorsChanged(bool clickable);
    void indicatorSizeChanged(int size);
    void indicatorSpacingChanged(int spacing);
    void activeIndicatorColorChanged(const QColor& color);
    void inactiveIndicatorColorChanged(const QColor& color);
    void indicatorClicked(int index);
    void indicatorHovered(int index);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void changeEvent(QEvent* event) override;

private slots:
    void onIndicatorClicked();
    void onCurrentIndexChanged(int index);
    void onItemCountChanged(int count);
    void onIndicatorHovered();
    void onIndicatorAnimationFinished();

private:
    void initializeIndicatorCarousel();
    void createIndicatorContainer();
    void createIndicators();
    void updateIndicatorLayout();
    void updateIndicatorPositions();
    void updateIndicatorStates();
    void animateIndicatorTransition(int fromIndex, int toIndex);
    void setupIndicatorAccessibility();
    QAbstractButton* createIndicatorButton(int index);
    void styleIndicatorButton(QAbstractButton* button, bool active);
    QSize calculateIndicatorContainerSize() const;
    QPoint calculateIndicatorPosition() const;

private:
    // Configuration
    FluentCarouselIndicatorStyle m_indicatorStyle{FluentCarouselIndicatorStyle::Dots};
    FluentCarouselIndicatorPosition m_indicatorPosition{FluentCarouselIndicatorPosition::Bottom};
    bool m_showIndicators{true};
    bool m_clickableIndicators{true};

    // Visual properties
    int m_indicatorSize{8};
    int m_indicatorSpacing{8};
    QColor m_activeIndicatorColor;
    QColor m_inactiveIndicatorColor;

    // UI components
    QWidget* m_indicatorContainer{nullptr};
    QHBoxLayout* m_indicatorLayout{nullptr};
    QButtonGroup* m_indicatorGroup{nullptr};
    std::vector<QAbstractButton*> m_indicators;

    // Animation
    std::unique_ptr<QPropertyAnimation> m_indicatorAnimation;
    int m_animatingFromIndex{-1};
    int m_animatingToIndex{-1};
};

/**
 * @brief Custom indicator button for carousel
 */
class FluentCarouselIndicatorButton : public QAbstractButton {
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(FluentCarouselIndicatorStyle indicatorStyle READ indicatorStyle WRITE setIndicatorStyle)
    Q_PROPERTY(QColor activeColor READ activeColor WRITE setActiveColor)
    Q_PROPERTY(QColor inactiveColor READ inactiveColor WRITE setInactiveColor)

public:
    explicit FluentCarouselIndicatorButton(int index, QWidget* parent = nullptr);
    ~FluentCarouselIndicatorButton() override = default;

    int index() const noexcept { return m_index; }
    bool isActive() const noexcept { return m_active; }
    void setActive(bool active);

    FluentCarouselIndicatorStyle indicatorStyle() const noexcept { return m_indicatorStyle; }
    void setIndicatorStyle(FluentCarouselIndicatorStyle style);

    QColor activeColor() const noexcept { return m_activeColor; }
    void setActiveColor(const QColor& color);

    QColor inactiveColor() const noexcept { return m_inactiveColor; }
    void setInactiveColor(const QColor& color);

    void setThumbnail(const QPixmap& thumbnail);
    QPixmap thumbnail() const noexcept { return m_thumbnail; }

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void activeChanged(bool active);

protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private:
    void paintDotIndicator(QPainter& painter, const QRect& rect);
    void paintLineIndicator(QPainter& painter, const QRect& rect);
    void paintNumberIndicator(QPainter& painter, const QRect& rect);
    void paintThumbnailIndicator(QPainter& painter, const QRect& rect);
    void paintProgressIndicator(QPainter& painter, const QRect& rect);
    QColor getCurrentColor() const;
    void updateAccessibilityInfo();

private:
    int m_index;
    bool m_active{false};
    FluentCarouselIndicatorStyle m_indicatorStyle{FluentCarouselIndicatorStyle::Dots};
    QColor m_activeColor;
    QColor m_inactiveColor;
    QPixmap m_thumbnail;
    bool m_hovered{false};
    bool m_pressed{false};
};

} // namespace FluentQt::Components
