// include/FluentQt/Components/FluentTimeline.h
#pragma once

#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QScrollArea>
#include <QSequentialAnimationGroup>
#include <QTimer>
#include <QVBoxLayout>
#include <memory>
#include <vector>
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

// Forward declarations
class FluentTimelineItem;

// Forward declare enums from FluentTimelineItem.h
enum class FluentTimelineItemState;

enum class FluentTimelineOrientation { Vertical, Horizontal };

enum class FluentTimelineAlignment { Left, Right, Center, Alternate };

enum class FluentTimelineStyle { Default, Compact, Detailed, Minimal };

enum class FluentTimelineConnectorStyle { Solid, Dashed, Dotted, None };

struct FluentTimelineConfig {
    FluentTimelineOrientation orientation{FluentTimelineOrientation::Vertical};
    FluentTimelineAlignment alignment{FluentTimelineAlignment::Left};
    FluentTimelineStyle style{FluentTimelineStyle::Default};
    FluentTimelineConnectorStyle connectorStyle{
        FluentTimelineConnectorStyle::Solid};

    int itemSpacing{16};
    int connectorWidth{2};
    int indicatorSize{12};
    bool animated{true};
    bool scrollable{false};
    bool interactive{true};
    bool showConnectors{true};
    bool showIndicators{true};

    QColor connectorColor;
    QColor indicatorColor;
    QColor backgroundColor;

    // Auto-calculated if not set
    bool autoCalculateColors{true};
};

class FluentTimeline : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(FluentTimelineOrientation orientation READ orientation WRITE
                   setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(FluentTimelineAlignment alignment READ alignment WRITE
                   setAlignment NOTIFY alignmentChanged)
    Q_PROPERTY(FluentTimelineStyle timelineStyle READ timelineStyle WRITE
                   setTimelineStyle NOTIFY styleChanged)
    Q_PROPERTY(FluentTimelineConnectorStyle connectorStyle READ connectorStyle
                   WRITE setConnectorStyle NOTIFY connectorStyleChanged)
    Q_PROPERTY(int itemSpacing READ itemSpacing WRITE setItemSpacing NOTIFY
                   itemSpacingChanged)
    Q_PROPERTY(int connectorWidth READ connectorWidth WRITE setConnectorWidth
                   NOTIFY connectorWidthChanged)
    Q_PROPERTY(int indicatorSize READ indicatorSize WRITE setIndicatorSize
                   NOTIFY indicatorSizeChanged)
    Q_PROPERTY(
        bool animated READ isAnimated WRITE setAnimated NOTIFY animatedChanged)
    Q_PROPERTY(bool scrollable READ isScrollable WRITE setScrollable NOTIFY
                   scrollableChanged)
    Q_PROPERTY(bool interactive READ isInteractive WRITE setInteractive NOTIFY
                   interactiveChanged)
    Q_PROPERTY(bool showConnectors READ showConnectors WRITE setShowConnectors
                   NOTIFY showConnectorsChanged)
    Q_PROPERTY(bool showIndicators READ showIndicators WRITE setShowIndicators
                   NOTIFY showIndicatorsChanged)

public:
    explicit FluentTimeline(QWidget* parent = nullptr);
    explicit FluentTimeline(FluentTimelineOrientation orientation,
                            QWidget* parent = nullptr);
    explicit FluentTimeline(const FluentTimelineConfig& config,
                            QWidget* parent = nullptr);
    ~FluentTimeline() override;

    // Configuration properties
    FluentTimelineOrientation orientation() const noexcept {
        return m_config.orientation;
    }
    void setOrientation(FluentTimelineOrientation orientation);

    FluentTimelineAlignment alignment() const noexcept {
        return m_config.alignment;
    }
    void setAlignment(FluentTimelineAlignment alignment);

    FluentTimelineStyle timelineStyle() const noexcept {
        return m_config.style;
    }
    void setTimelineStyle(FluentTimelineStyle style);

    FluentTimelineConnectorStyle connectorStyle() const noexcept {
        return m_config.connectorStyle;
    }
    void setConnectorStyle(FluentTimelineConnectorStyle style);

    // Spacing and sizing
    int itemSpacing() const noexcept { return m_config.itemSpacing; }
    void setItemSpacing(int spacing);

    int connectorWidth() const noexcept { return m_config.connectorWidth; }
    void setConnectorWidth(int width);

    int indicatorSize() const noexcept { return m_config.indicatorSize; }
    void setIndicatorSize(int size);

    // Behavior properties
    bool isAnimated() const noexcept { return m_config.animated; }
    void setAnimated(bool animated);

    bool isScrollable() const noexcept { return m_config.scrollable; }
    void setScrollable(bool scrollable);

    bool isInteractive() const noexcept { return m_config.interactive; }
    void setInteractive(bool interactive);

    bool showConnectors() const noexcept { return m_config.showConnectors; }
    void setShowConnectors(bool show);

    bool showIndicators() const noexcept { return m_config.showIndicators; }
    void setShowIndicators(bool show);

    // Configuration management
    FluentTimelineConfig configuration() const { return m_config; }
    void setConfiguration(const FluentTimelineConfig& config);

    // Item management
    void addItem(FluentTimelineItem* item);
    void insertItem(int index, FluentTimelineItem* item);
    void removeItem(FluentTimelineItem* item);
    void removeItem(int index);
    void clearItems();

    FluentTimelineItem* item(int index) const;
    int itemCount() const;
    QList<FluentTimelineItem*> items() const;

    int indexOf(FluentTimelineItem* item) const;

    // Selection and navigation
    FluentTimelineItem* currentItem() const { return m_currentItem; }
    int currentIndex() const;
    void setCurrentItem(FluentTimelineItem* item);
    void setCurrentIndex(int index);

    // Convenience methods for creating items
    FluentTimelineItem* addTextItem(const QString& title,
                                    const QString& description = QString());
    FluentTimelineItem* addIconItem(const QIcon& icon, const QString& title,
                                    const QString& description = QString());
    FluentTimelineItem* addDateTimeItem(const QDateTime& dateTime,
                                        const QString& title,
                                        const QString& description = QString());

    // Animation control
    void animateToItem(FluentTimelineItem* item);
    void animateToIndex(int index);
    void scrollToItem(FluentTimelineItem* item);
    void scrollToIndex(int index);

    // Static factory methods
    static FluentTimeline* createVerticalTimeline(QWidget* parent = nullptr);
    static FluentTimeline* createHorizontalTimeline(QWidget* parent = nullptr);
    static FluentTimeline* createCompactTimeline(QWidget* parent = nullptr);
    static FluentTimeline* createDetailedTimeline(QWidget* parent = nullptr);

public slots:
    void expandAll();
    void collapseAll();
    void refresh();

signals:
    void orientationChanged(FluentTimelineOrientation orientation);
    void alignmentChanged(FluentTimelineAlignment alignment);
    void styleChanged(FluentTimelineStyle style);
    void connectorStyleChanged(FluentTimelineConnectorStyle style);
    void itemSpacingChanged(int spacing);
    void connectorWidthChanged(int width);
    void indicatorSizeChanged(int size);
    void animatedChanged(bool animated);
    void scrollableChanged(bool scrollable);
    void interactiveChanged(bool interactive);
    void showConnectorsChanged(bool show);
    void showIndicatorsChanged(bool show);

    void itemAdded(FluentTimelineItem* item, int index);
    void itemRemoved(FluentTimelineItem* item, int index);
    void itemClicked(FluentTimelineItem* item, int index);
    void itemDoubleClicked(FluentTimelineItem* item, int index);
    void currentItemChanged(FluentTimelineItem* current,
                            FluentTimelineItem* previous);
    void currentIndexChanged(int index);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

    virtual void updateStateStyle() override;
    virtual void performStateTransition(Core::FluentState from,
                                        Core::FluentState to) override;

private slots:
    void onItemClicked();
    void onItemDoubleClicked();
    void onItemStateChanged(FluentTimelineItemState state);
    void onThemeChanged();
    void onScrollValueChanged(int value);

private:
    void setupLayout();
    void setupScrollArea();
    void setupAnimations();
    void updateLayout();
    void updateItemPositions();
    void updateConnectors();
    void updateColors();
    void updateAccessibility();

    void paintConnectors(QPainter& painter);
    void paintIndicators(QPainter& painter);
    void paintBackground(QPainter& painter);

    QRect getItemRect(int index) const;
    QPoint getItemPosition(int index) const;
    QPoint getConnectorStart(int index) const;
    QPoint getConnectorEnd(int index) const;

    void ensureItemVisible(FluentTimelineItem* item);
    void updateItemStates();

    // Layout helpers
    void layoutVertical();
    void layoutHorizontal();
    void calculateItemPositions();

    // Animation helpers
    void animateItemAppearance(FluentTimelineItem* item);
    void animateItemRemoval(FluentTimelineItem* item);
    void animateLayoutChange();

private:
    FluentTimelineConfig m_config;

    // Layout and containers
    QVBoxLayout* m_mainLayout{nullptr};
    QScrollArea* m_scrollArea{nullptr};
    QWidget* m_contentWidget{nullptr};
    QLayout* m_contentLayout{nullptr};

    // Items and state
    QList<FluentTimelineItem*> m_items;
    FluentTimelineItem* m_currentItem{nullptr};
    int m_hoveredIndex{-1};

    // Animation
    std::unique_ptr<QPropertyAnimation> m_scrollAnimation;
    std::unique_ptr<QParallelAnimationGroup> m_layoutAnimation;
    std::unique_ptr<QSequentialAnimationGroup> m_itemAnimation;

    // Cached values
    QList<QPoint> m_itemPositions;
    QList<QRect> m_itemRects;
    bool m_layoutDirty{true};
    QSize m_lastSize;

    // Colors (cached from theme)
    QColor m_connectorColor;
    QColor m_indicatorColor;
    QColor m_backgroundColor;
    QColor m_focusColor;
};

}  // namespace FluentQt::Components
