// include/FluentQt/Components/FluentToastManager.h
#pragma once

#include "FluentQt/Components/FluentToast.h"
#include <QObject>
#include <QWidget>
#include <QApplication>
#include <QScreen>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QTimer>
#include <QQueue>
#include <QList>
#include <QRect>
#include <QMargins>
#include <memory>

namespace FluentQt::Components {

struct FluentToastManagerConfig {
    FluentToastPosition defaultPosition{FluentToastPosition::TopRight};
    FluentToastAnimation defaultAnimation{FluentToastAnimation::Slide};
    
    int maxVisible{5};           // Maximum number of visible toasts
    int maxQueued{20};           // Maximum number of queued toasts
    int stackSpacing{8};         // Spacing between stacked toasts
    int screenMargin{16};        // Margin from screen edges
    int animationDuration{300};  // Default animation duration
    
    bool allowDuplicates{false}; // Allow duplicate toasts
    bool stackToasts{true};      // Stack toasts or replace
    bool pauseOnHover{true};     // Pause all timers on hover
    bool respectScreenBounds{true}; // Keep toasts within screen bounds
    
    QWidget* parentWidget{nullptr}; // Parent widget for positioning (null = screen)
    QEasingCurve easingCurve{QEasingCurve::OutCubic};
};

class FluentToastManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(FluentToastPosition defaultPosition READ defaultPosition WRITE setDefaultPosition NOTIFY defaultPositionChanged)
    Q_PROPERTY(int maxVisible READ maxVisible WRITE setMaxVisible NOTIFY maxVisibleChanged)
    Q_PROPERTY(int maxQueued READ maxQueued WRITE setMaxQueued NOTIFY maxQueuedChanged)
    Q_PROPERTY(int stackSpacing READ stackSpacing WRITE setStackSpacing NOTIFY stackSpacingChanged)
    Q_PROPERTY(int screenMargin READ screenMargin WRITE setScreenMargin NOTIFY screenMarginChanged)
    Q_PROPERTY(bool allowDuplicates READ allowDuplicates WRITE setAllowDuplicates NOTIFY allowDuplicatesChanged)
    Q_PROPERTY(bool stackToasts READ stackToasts WRITE setStackToasts NOTIFY stackToastsChanged)

public:
    explicit FluentToastManager(QObject* parent = nullptr);
    explicit FluentToastManager(const FluentToastManagerConfig& config, QObject* parent = nullptr);
    ~FluentToastManager() override;

    // Singleton access
    static FluentToastManager& instance();
    static void setGlobalInstance(FluentToastManager* manager);

    // Configuration properties
    FluentToastPosition defaultPosition() const { return m_config.defaultPosition; }
    void setDefaultPosition(FluentToastPosition position);

    int maxVisible() const { return m_config.maxVisible; }
    void setMaxVisible(int max);

    int maxQueued() const { return m_config.maxQueued; }
    void setMaxQueued(int max);

    int stackSpacing() const { return m_config.stackSpacing; }
    void setStackSpacing(int spacing);

    int screenMargin() const { return m_config.screenMargin; }
    void setScreenMargin(int margin);

    bool allowDuplicates() const { return m_config.allowDuplicates; }
    void setAllowDuplicates(bool allow);

    bool stackToasts() const { return m_config.stackToasts; }
    void setStackToasts(bool stack);

    // Configuration management
    FluentToastManagerConfig configuration() const { return m_config; }
    void setConfiguration(const FluentToastManagerConfig& config);

    // Parent widget for positioning
    QWidget* parentWidget() const { return m_config.parentWidget; }
    void setParentWidget(QWidget* parent);

    // Toast management
    FluentToast* show(FluentToastType type, const QString& title, const QString& message = QString());
    FluentToast* show(FluentToastType type, const QString& title, const QString& message, FluentToastPosition position);
    FluentToast* show(FluentToastType type, const QString& title, const QString& message, const FluentToastConfig& config);
    FluentToast* show(FluentToast* toast);
    FluentToast* show(FluentToast* toast, FluentToastPosition position);

    // Convenience methods
    FluentToast* showInfo(const QString& title, const QString& message = QString());
    FluentToast* showSuccess(const QString& title, const QString& message = QString());
    FluentToast* showWarning(const QString& title, const QString& message = QString());
    FluentToast* showError(const QString& title, const QString& message = QString());
    FluentToast* showCustom(const QIcon& icon, const QString& title, const QString& message = QString());

    // Position-specific convenience methods
    FluentToast* showInfoAt(FluentToastPosition position, const QString& title, const QString& message = QString());
    FluentToast* showSuccessAt(FluentToastPosition position, const QString& title, const QString& message = QString());
    FluentToast* showWarningAt(FluentToastPosition position, const QString& title, const QString& message = QString());
    FluentToast* showErrorAt(FluentToastPosition position, const QString& title, const QString& message = QString());

    // Toast control
    void hide(FluentToast* toast);
    void hideAll();
    void hideAllAt(FluentToastPosition position);
    void clear();
    void clearAt(FluentToastPosition position);

    // Queue management
    void pauseQueue();
    void resumeQueue();
    void clearQueue();
    bool isQueuePaused() const { return m_queuePaused; }

    // Information
    int visibleCount() const;
    int visibleCountAt(FluentToastPosition position) const;
    int queuedCount() const { return m_queuedToasts.size(); }
    QList<FluentToast*> visibleToasts() const;
    QList<FluentToast*> visibleToastsAt(FluentToastPosition position) const;

    // Positioning utilities
    QRect getToastRect(FluentToastPosition position, const QSize& toastSize, int index = 0) const;
    QPoint getToastPosition(FluentToastPosition position, const QSize& toastSize, int index = 0) const;
    QRect getAvailableArea(FluentToastPosition position) const;
    QRect getScreenRect() const;

public slots:
    void processQueue();
    void repositionToasts();
    void repositionToastsAt(FluentToastPosition position);

signals:
    void defaultPositionChanged(FluentToastPosition position);
    void maxVisibleChanged(int max);
    void maxQueuedChanged(int max);
    void stackSpacingChanged(int spacing);
    void screenMarginChanged(int margin);
    void allowDuplicatesChanged(bool allow);
    void stackToastsChanged(bool stack);
    
    void toastShown(FluentToast* toast);
    void toastHidden(FluentToast* toast);
    void toastDismissed(FluentToast* toast);
    void queueProcessed();
    void allToastsHidden();

private slots:
    void onToastShown();
    void onToastHidden();
    void onToastDismissed();
    void onScreenChanged();
    void onParentWidgetDestroyed();

private:
    void setupConnections();
    void connectToast(FluentToast* toast);
    void disconnectToast(FluentToast* toast);
    
    bool canShowToast(FluentToast* toast) const;
    bool isDuplicate(FluentToast* toast) const;
    void addToQueue(FluentToast* toast);
    void removeFromQueue(FluentToast* toast);
    void showNextFromQueue();
    
    void addToVisible(FluentToast* toast, FluentToastPosition position);
    void removeFromVisible(FluentToast* toast);
    void updateToastPositions(FluentToastPosition position);
    void animateToastToPosition(FluentToast* toast, const QPoint& position);
    
    QRect calculateToastRect(FluentToastPosition position, const QSize& toastSize, int stackIndex) const;
    QPoint calculateToastPosition(FluentToastPosition position, const QSize& toastSize, int stackIndex) const;
    QRect getPositionArea(FluentToastPosition position) const;
    
    void cleanupToast(FluentToast* toast);
    void updateScreenGeometry();

private:
    FluentToastManagerConfig m_config;
    
    // Toast storage by position
    QHash<FluentToastPosition, QList<FluentToast*>> m_visibleToasts;
    QQueue<FluentToast*> m_queuedToasts;
    
    // State
    bool m_queuePaused{false};
    QRect m_screenGeometry;
    QMargins m_screenMargins;
    
    // Animation
    std::unique_ptr<QParallelAnimationGroup> m_repositionAnimation;
    QTimer* m_queueTimer{nullptr};
    
    // Static instance
    static FluentToastManager* s_globalInstance;
};

// Global convenience functions
namespace FluentToastGlobal {
    FluentToast* showInfo(const QString& title, const QString& message = QString());
    FluentToast* showSuccess(const QString& title, const QString& message = QString());
    FluentToast* showWarning(const QString& title, const QString& message = QString());
    FluentToast* showError(const QString& title, const QString& message = QString());
    FluentToast* showCustom(const QIcon& icon, const QString& title, const QString& message = QString());
    
    void hideAll();
    void clear();
    
    FluentToastManager& manager();
}

} // namespace FluentQt::Components
