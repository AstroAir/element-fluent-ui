// include/FluentQt/Accessibility/FluentKeyboardNavigationManager.h
#pragma once

#include <QObject>
#include <QWidget>
#include <QKeyEvent>
#include <QKeySequence>
#include <QTimer>
#include <QMutex>
#include <QRect>
#include <QList>
#include <QStringList>
#include <unordered_map>
#include <memory>

namespace FluentQt::Accessibility {

// Keyboard navigation modes
enum class FluentNavigationMode {
    Tab,            // Standard tab navigation
    Arrow,          // Arrow key navigation
    Spatial,        // Spatial navigation (2D grid)
    Custom,         // Custom navigation patterns
    Roving,         // Roving tabindex pattern
    Modal           // Modal dialog navigation
};

// Focus movement directions
enum class FluentFocusDirection {
    Next,
    Previous,
    Up,
    Down,
    Left,
    Right,
    First,
    Last,
    Home,
    End
};

// Keyboard shortcut types
enum class FluentShortcutType {
    Global,         // Application-wide shortcuts
    Local,          // Widget-specific shortcuts
    Context,        // Context menu shortcuts
    Navigation,     // Navigation shortcuts
    Action,         // Action shortcuts
    Toggle          // Toggle state shortcuts
};

// Focus indicator styles
enum class FluentFocusIndicatorStyle {
    Default,        // System default
    HighContrast,   // High contrast mode
    Custom,         // Custom styling
    Animated,       // Animated focus ring
    Subtle,         // Subtle indication
    Bold            // Bold indication
};

// Keyboard navigation configuration
struct FluentKeyboardConfig {
    FluentNavigationMode mode{FluentNavigationMode::Tab};
    bool enableArrowNavigation{true};
    bool enableHomeEndNavigation{true};
    bool enablePageUpDownNavigation{false};
    bool enableSpatialNavigation{false};
    bool enableRovingTabindex{false};
    bool enableSkipLinks{true};
    bool enableFocusTrapping{false};
    bool enableFocusRestoration{true};
    bool enableCustomShortcuts{true};
    int focusIndicatorWidth{2};
    FluentFocusIndicatorStyle focusStyle{FluentFocusIndicatorStyle::Default};
    QColor focusColor{Qt::blue};
    int navigationDelay{0}; // ms delay for navigation
    bool announceNavigation{true};
};

// Focus trap configuration
struct FluentFocusTrap {
    QWidget* container;
    QWidget* firstFocusable;
    QWidget* lastFocusable;
    QWidget* initialFocus;
    bool isActive{false};
    bool restoreFocus{true};
    QWidget* previousFocus{nullptr};
};

// Keyboard shortcut definition
struct FluentKeyboardShortcut {
    QKeySequence sequence;
    QString description;
    FluentShortcutType type;
    QWidget* target{nullptr};
    std::function<void()> action;
    bool enabled{true};
    QString context;
    int priority{0};
};

// Skip link definition
struct FluentSkipLink {
    QString text;
    QWidget* target;
    QKeySequence shortcut;
    bool visible{false};
    QString description;
};

// Comprehensive keyboard navigation manager
class FluentKeyboardNavigationManager : public QObject {
    Q_OBJECT

public:
    static FluentKeyboardNavigationManager& instance();
    
    // Configuration
    void setNavigationConfig(const FluentKeyboardConfig& config);
    FluentKeyboardConfig getNavigationConfig() const { return m_config; }
    
    // Widget registration for navigation
    void registerNavigableWidget(QWidget* widget, int tabOrder = -1);
    void unregisterNavigableWidget(QWidget* widget);
    void setWidgetNavigationMode(QWidget* widget, FluentNavigationMode mode);
    void setWidgetTabOrder(QWidget* widget, int order);
    
    // Focus management
    bool moveFocus(FluentFocusDirection direction);
    void setFocus(QWidget* widget, bool announce = true);
    QWidget* getCurrentFocus() const;
    QWidget* getNextFocusWidget(QWidget* current, FluentFocusDirection direction);
    
    // Focus trapping for modal dialogs
    void installFocusTrap(QWidget* container, QWidget* initialFocus = nullptr);
    void removeFocusTrap(QWidget* container);
    bool isFocusTrapped(QWidget* widget) const;
    void activateFocusTrap(QWidget* container);
    void deactivateFocusTrap(QWidget* container);
    
    // Keyboard shortcuts
    void registerShortcut(const QKeySequence& sequence, std::function<void()> action,
                         const QString& description = "", FluentShortcutType type = FluentShortcutType::Global);
    void registerShortcut(const FluentKeyboardShortcut& shortcut);
    void unregisterShortcut(const QKeySequence& sequence);
    void enableShortcut(const QKeySequence& sequence, bool enabled = true);
    QList<FluentKeyboardShortcut> getShortcuts(FluentShortcutType type = FluentShortcutType::Global) const;
    
    // Skip links
    void addSkipLink(const QString& text, QWidget* target, const QKeySequence& shortcut = QKeySequence());
    void removeSkipLink(QWidget* target);
    void showSkipLinks();
    void hideSkipLinks();
    QList<FluentSkipLink> getSkipLinks() const;
    
    // Spatial navigation (2D grid)
    void enableSpatialNavigation(QWidget* container);
    void disableSpatialNavigation(QWidget* container);
    void setSpatialNavigationGrid(QWidget* container, int rows, int columns);
    QWidget* findSpatialNeighbor(QWidget* current, FluentFocusDirection direction);
    
    // Roving tabindex pattern
    void enableRovingTabindex(QWidget* container);
    void disableRovingTabindex(QWidget* container);
    void setRovingTabindexGroup(const QList<QWidget*>& widgets);
    
    // Focus indicators
    void setFocusIndicatorStyle(FluentFocusIndicatorStyle style);
    void setCustomFocusIndicator(QWidget* widget, const QString& styleSheet);
    void showFocusIndicator(QWidget* widget);
    void hideFocusIndicator(QWidget* widget);
    void updateFocusIndicator(QWidget* widget);
    
    // Event handling
    bool handleKeyEvent(QKeyEvent* event);
    bool handleTabNavigation(QKeyEvent* event);
    bool handleArrowNavigation(QKeyEvent* event);
    bool handleShortcut(QKeyEvent* event);
    
    // Accessibility integration
    void announceNavigation(QWidget* widget, const QString& action = "");
    void announceShortcut(const QKeySequence& sequence, const QString& description);
    
    // Validation and testing
    QStringList validateKeyboardNavigation(QWidget* rootWidget);
    QList<QWidget*> findKeyboardTraps(QWidget* rootWidget);
    QList<QWidget*> findUnreachableWidgets(QWidget* rootWidget);
    bool isKeyboardAccessible(QWidget* widget);
    
    // Navigation history
    void enableNavigationHistory(bool enabled = true);
    QWidget* getPreviousFocus() const;
    void restorePreviousFocus();
    QList<QWidget*> getNavigationHistory() const;
    void clearNavigationHistory();

signals:
    void focusChanged(QWidget* newFocus, QWidget* oldFocus);
    void navigationModeChanged(FluentNavigationMode mode);
    void shortcutActivated(const QKeySequence& sequence, const QString& description);
    void focusTrapActivated(QWidget* container);
    void focusTrapDeactivated(QWidget* container);
    void skipLinkActivated(const FluentSkipLink& skipLink);
    void keyboardNavigationIssue(QWidget* widget, const QString& issue);
    void focusIndicatorChanged(QWidget* widget, FluentFocusIndicatorStyle style);

private slots:
    void onWidgetDestroyed(QObject* widget);
    void onApplicationFocusChanged(QWidget* old, QWidget* now);
    void updateFocusIndicators();

private:
    FluentKeyboardNavigationManager();
    
    // Navigation helpers
    QWidget* findNextTabWidget(QWidget* current, bool forward = true);
    QWidget* findFirstFocusableWidget(QWidget* container);
    QWidget* findLastFocusableWidget(QWidget* container);
    bool isWidgetFocusable(QWidget* widget);
    bool isWidgetVisible(QWidget* widget);
    
    // Focus trap helpers
    void trapFocus(QWidget* widget, FluentFocusTrap& trap);
    void releaseFocus(FluentFocusTrap& trap);
    
    // Spatial navigation helpers
    QRect getWidgetRect(QWidget* widget);
    double calculateDistance(const QRect& from, const QRect& to, FluentFocusDirection direction);
    QWidget* findBestSpatialMatch(QWidget* current, const QList<QWidget*>& candidates, FluentFocusDirection direction);
    
    // Shortcut helpers
    bool matchesShortcut(QKeyEvent* event, const QKeySequence& sequence);
    void executeShortcut(const FluentKeyboardShortcut& shortcut);
    
    // Focus indicator helpers
    void createFocusIndicator(QWidget* widget);
    void positionFocusIndicator(QWidget* widget);
    void styleFocusIndicator(QWidget* widget);

private:
    FluentKeyboardConfig m_config;
    
    // Widget management
    QList<QWidget*> m_navigableWidgets;
    std::unordered_map<QWidget*, int> m_tabOrders;
    std::unordered_map<QWidget*, FluentNavigationMode> m_widgetModes;
    
    // Focus management
    QWidget* m_currentFocus{nullptr};
    QList<QWidget*> m_navigationHistory;
    bool m_navigationHistoryEnabled{true};
    int m_maxHistorySize{50};
    
    // Focus traps
    std::unordered_map<QWidget*, FluentFocusTrap> m_focusTraps;
    QWidget* m_activeTrap{nullptr};
    
    // Shortcuts
    QList<FluentKeyboardShortcut> m_shortcuts;
    
    // Skip links
    QList<FluentSkipLink> m_skipLinks;
    QWidget* m_skipLinksContainer{nullptr};
    
    // Spatial navigation
    std::unordered_map<QWidget*, QList<QWidget*>> m_spatialGroups;
    std::unordered_map<QWidget*, std::pair<int, int>> m_spatialGrids; // rows, columns
    
    // Roving tabindex
    std::unordered_map<QWidget*, QList<QWidget*>> m_rovingGroups;
    std::unordered_map<QWidget*, QWidget*> m_rovingActive; // container -> active widget
    
    // Focus indicators
    std::unordered_map<QWidget*, QWidget*> m_focusIndicators;
    QTimer* m_focusUpdateTimer;
    
    // Thread safety
    mutable QMutex m_mutex;
};

} // namespace FluentQt::Accessibility
