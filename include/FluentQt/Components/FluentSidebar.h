#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QToolButton>
#include <QLabel>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QTimer>
#include <QIcon>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QResizeEvent>
#include <functional>

namespace FluentQt::Components {

/**
 * @brief Sidebar display modes
 */
enum class FluentSidebarMode {
    Expanded,       // Full sidebar with text and icons
    Compact,        // Icons only, no text
    Overlay,        // Overlay mode for mobile/small screens
    Hidden,         // Completely hidden
    Auto            // Automatically choose based on screen size
};

/**
 * @brief Sidebar item configuration
 */
struct FluentSidebarItem {
    QString id;                                 // Unique identifier
    QString text;                               // Display text
    QIcon icon;                                 // Item icon
    QString tooltip;                            // Tooltip text
    bool enabled = true;                        // Whether item is enabled
    bool separator = false;                     // Whether this is a separator
    QWidget* customWidget = nullptr;            // Custom widget for item
    std::function<void()> onClicked;           // Click callback
    QVariantMap data;                          // Custom data
    
    // Accessibility
    QString accessibleName;                     // Accessible name
    QString accessibleDescription;              // Accessible description
    QString ariaRole = "button";               // ARIA role
};

/**
 * @brief Enhanced sidebar component with comprehensive navigation support
 */
class FluentSidebar : public QWidget {
    Q_OBJECT
    Q_PROPERTY(FluentSidebarMode mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(int expandedWidth READ expandedWidth WRITE setExpandedWidth)
    Q_PROPERTY(int compactWidth READ compactWidth WRITE setCompactWidth)
    Q_PROPERTY(bool collapsible READ isCollapsible WRITE setCollapsible)
    Q_PROPERTY(bool autoHide READ autoHide WRITE setAutoHide)

public:
    explicit FluentSidebar(QWidget* parent = nullptr);
    ~FluentSidebar() override;

    // Mode management
    FluentSidebarMode mode() const { return m_mode; }
    void setMode(FluentSidebarMode mode);

    // Size configuration
    int expandedWidth() const { return m_expandedWidth; }
    void setExpandedWidth(int width);
    
    int compactWidth() const { return m_compactWidth; }
    void setCompactWidth(int width);

    // Behavior configuration
    bool isCollapsible() const { return m_collapsible; }
    void setCollapsible(bool collapsible);
    
    bool autoHide() const { return m_autoHide; }
    void setAutoHide(bool autoHide);

    // Item management
    void addItem(const FluentSidebarItem& item);
    void insertItem(int index, const FluentSidebarItem& item);
    void removeItem(const QString& id);
    void removeItem(int index);
    void clearItems();
    
    FluentSidebarItem* findItem(const QString& id);
    int itemCount() const;

    // Selection
    QString selectedItemId() const { return m_selectedItemId; }
    void setSelectedItem(const QString& id);
    void clearSelection();

    // Header and footer
    void setHeader(QWidget* header);
    void setFooter(QWidget* footer);
    QWidget* header() const { return m_header; }
    QWidget* footer() const { return m_footer; }

    // Responsive behavior
    void setResponsiveBreakpoints(int compactBreakpoint, int hiddenBreakpoint);
    void enableResponsiveBehavior(bool enable);

    // Animation configuration
    void setAnimationDuration(int duration);
    void setAnimationEasing(QEasingCurve::Type easing);

    // Accessibility
    void setAccessibleName(const QString& name);
    void setAccessibleDescription(const QString& description);

public slots:
    void expand();
    void collapse();
    void toggle();
    void showOverlay();
    void hideOverlay();

signals:
    void modeChanged(FluentSidebarMode mode);
    void itemClicked(const QString& id);
    void itemSelected(const QString& id);
    void expanded();
    void collapsed();
    void overlayShown();
    void overlayHidden();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    bool eventFilter(QObject* object, QEvent* event) override;

private slots:
    void onItemClicked();
    void onAnimationFinished();
    void onResponsiveTimer();
    void updateResponsiveMode();

private:
    void setupUI();
    void setupAnimations();
    void setupAccessibility();
    void updateLayout();
    void updateItemVisibility();
    void updateItemStyles();
    void animateToMode(FluentSidebarMode newMode);
    void createItemWidget(const FluentSidebarItem& item, int index);
    void removeItemWidget(int index);
    void updateItemWidget(int index);
    
    // Responsive helpers
    void checkResponsiveBreakpoints();
    void handleResponsiveChange();
    
    // Accessibility helpers
    void updateAccessibilityProperties();
    void announceStateChange(const QString& message);

    FluentSidebarMode m_mode = FluentSidebarMode::Expanded;
    FluentSidebarMode m_previousMode = FluentSidebarMode::Expanded;
    
    // Size configuration
    int m_expandedWidth = 280;
    int m_compactWidth = 48;
    int m_currentWidth = 280;
    
    // Behavior flags
    bool m_collapsible = true;
    bool m_autoHide = false;
    bool m_responsiveEnabled = true;
    bool m_animating = false;
    
    // Responsive breakpoints
    int m_compactBreakpoint = 768;
    int m_hiddenBreakpoint = 480;
    
    // Animation configuration
    int m_animationDuration = 250;
    QEasingCurve::Type m_animationEasing = QEasingCurve::OutCubic;
    
    // UI components
    QVBoxLayout* m_mainLayout = nullptr;
    QScrollArea* m_scrollArea = nullptr;
    QWidget* m_contentWidget = nullptr;
    QVBoxLayout* m_contentLayout = nullptr;
    QWidget* m_header = nullptr;
    QWidget* m_footer = nullptr;
    
    // Items
    QList<FluentSidebarItem> m_items;
    QList<QWidget*> m_itemWidgets;
    QString m_selectedItemId;
    
    // Animations
    QPropertyAnimation* m_widthAnimation = nullptr;
    QParallelAnimationGroup* m_modeAnimation = nullptr;
    
    // Responsive behavior
    QTimer* m_responsiveTimer = nullptr;
    QSize m_lastParentSize;
    
    // Accessibility
    QString m_accessibleName;
    QString m_accessibleDescription;
};

} // namespace FluentQt::Components
