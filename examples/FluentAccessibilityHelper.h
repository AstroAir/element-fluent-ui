// examples/FluentAccessibilityHelper.h
// Accessibility helper for FluentQt components and theme switching

#pragma once

#include <QObject>
#include <QWidget>
#include <QAccessible>
#include <QAccessibleWidget>
#include <QKeyEvent>
#include <QString>
#include <QTimer>
#include <QApplication>

namespace FluentQt::Examples {

/**
 * @brief Accessibility helper class for FluentQt components
 * 
 * Provides comprehensive accessibility support including:
 * - Screen reader announcements
 * - Keyboard navigation
 * - High contrast mode detection
 * - ARIA-like properties for Qt widgets
 * - Theme change announcements
 */
class FluentAccessibilityHelper : public QObject {
    Q_OBJECT

public:
    explicit FluentAccessibilityHelper(QObject* parent = nullptr);
    ~FluentAccessibilityHelper() override;

    // Screen reader announcements
    static void announceToScreenReader(const QString& message, QAccessible::Event event = QAccessible::Alert);
    static void announceThemeChange(const QString& newTheme);
    static void announceComponentStateChange(QWidget* widget, const QString& state);

    // Widget accessibility enhancement
    static void enhanceWidgetAccessibility(QWidget* widget, const QString& role = QString());
    static void setAccessibleName(QWidget* widget, const QString& name);
    static void setAccessibleDescription(QWidget* widget, const QString& description);
    static void setAccessibleRole(QWidget* widget, QAccessible::Role role);

    // Keyboard navigation
    static bool handleKeyboardNavigation(QWidget* widget, QKeyEvent* event);
    static void setupKeyboardNavigation(QWidget* container);
    static void setTabOrder(const QList<QWidget*>& widgets);

    // High contrast mode
    static bool isHighContrastMode();
    static void applyHighContrastStyles(QWidget* widget);
    static QString getHighContrastStyleSheet(const QString& baseStyle = QString());

    // Focus management
    static void setFocusIndicator(QWidget* widget, bool visible = true);
    static void ensureVisibleFocus(QWidget* widget);
    static void announceFocusChange(QWidget* widget);

    // Theme accessibility
    static void setupThemeAccessibility(QWidget* themeControls);
    static void announceAccentColorChange(const QString& colorName);
    static void announceContrastModeChange(bool highContrast);

    // Component-specific accessibility
    static void setupButtonAccessibility(QWidget* button, const QString& action = QString());
    static void setupInputAccessibility(QWidget* input, const QString& label = QString());
    static void setupSliderAccessibility(QWidget* slider, const QString& valueDescription = QString());
    static void setupProgressAccessibility(QWidget* progress, const QString& description = QString());

    // Live regions for dynamic content
    static void createLiveRegion(QWidget* parent, const QString& regionId);
    static void updateLiveRegion(const QString& regionId, const QString& content);
    static void removeLiveRegion(const QString& regionId);

    // Validation and error handling
    static void announceValidationError(QWidget* field, const QString& error);
    static void announceValidationSuccess(QWidget* field);
    static void setFieldError(QWidget* field, const QString& error);
    static void clearFieldError(QWidget* field);

public slots:
    void onThemeChanged();
    void onFocusChanged(QWidget* old, QWidget* now);
    void onWidgetStateChanged();

signals:
    void accessibilityAnnouncement(const QString& message);
    void focusIndicatorRequested(QWidget* widget);
    void highContrastModeChanged(bool enabled);

private:
    static void installEventFilter(QWidget* widget);
    static QString getWidgetDescription(QWidget* widget);
    static QString getThemeDescription(const QString& mode, const QString& accent);
    static void updateAccessibleProperties(QWidget* widget);

    // Static members for global state
    static QHash<QString, QWidget*> s_liveRegions;
    static QTimer* s_announcementTimer;
    static QString s_pendingAnnouncement;
    static bool s_highContrastMode;

    // Instance members
    QWidget* m_lastFocusedWidget{nullptr};
    QTimer* m_focusTimer{nullptr};
};

/**
 * @brief Custom accessible interface for FluentQt components
 */
class FluentAccessibleWidget : public QAccessibleWidget {
public:
    explicit FluentAccessibleWidget(QWidget* widget, QAccessible::Role role = QAccessible::NoRole);

    // QAccessibleInterface implementation
    QString text(QAccessible::Text t) const override;
    void setText(QAccessible::Text t, const QString& text) override;
    QAccessible::State state() const override;
    QAccessible::Role role() const override;

    // Enhanced functionality
    void announceStateChange(const QString& newState);
    void setLiveRegion(bool isLive = true);

private:
    QAccessible::Role m_role;
    QString m_customDescription;
    QString m_customName;
    bool m_isLiveRegion{false};
};

/**
 * @brief Keyboard navigation helper for complex widgets
 */
class FluentKeyboardNavigator : public QObject {
    Q_OBJECT

public:
    explicit FluentKeyboardNavigator(QWidget* container, QObject* parent = nullptr);

    void addNavigableWidget(QWidget* widget, int priority = 0);
    void removeNavigableWidget(QWidget* widget);
    void setNavigationOrder(const QList<QWidget*>& order);

    bool handleKeyPress(QKeyEvent* event);
    void focusNext();
    void focusPrevious();
    void focusFirst();
    void focusLast();

signals:
    void navigationChanged(QWidget* current, QWidget* previous);
    void navigationWrapped(bool forward);

private slots:
    void onWidgetDestroyed(QObject* widget);

private:
    void updateTabOrder();
    QWidget* findNextWidget(QWidget* current, bool forward);
    bool isWidgetNavigable(QWidget* widget);

    QWidget* m_container;
    QList<QWidget*> m_navigableWidgets;
    QHash<QWidget*, int> m_priorities;
    int m_currentIndex{-1};
};

} // namespace FluentQt::Examples
