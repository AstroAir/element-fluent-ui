// include/FluentQt/Components/FluentNotification.h
#pragma once

#include <QGraphicsOpacityEffect>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QParallelAnimationGroup>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QQueue>
#include <QTimer>
#include <QWidget>
#include <memory>
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentComponent.h"
#include "FluentQt/Styling/FluentDesignTokenUtils.h"

namespace FluentQt::Components {

// Notification types
enum class FluentNotificationType { Info, Success, Warning, Error, Custom };

// Notification positions
enum class FluentNotificationPosition {
    TopLeft,
    TopCenter,
    TopRight,
    BottomLeft,
    BottomCenter,
    BottomRight,
    Center
};

// Notification animation types
enum class FluentNotificationAnimation { Slide, Fade, Scale, Bounce };

// Notification elevation levels
enum class FluentNotificationElevation {
    None,     // No shadow
    Low,      // Subtle shadow for basic notifications
    Medium,   // Standard shadow for important notifications
    High,     // Prominent shadow for critical notifications
    VeryHigh  // Maximum shadow for urgent notifications
};

// Notification complexity modes
enum class FluentNotificationComplexity {
    Simple,  // Lightweight mode with basic features (QWidget-based behavior)
    Full  // Full-featured mode with advanced animations and manager integration
};

// Notification action structure
struct FluentNotificationAction {
    QString text;
    QIcon icon;
    std::function<void()> callback;
    bool primary{false};

    FluentNotificationAction() = default;
    FluentNotificationAction(const QString& text,
                             std::function<void()> callback = nullptr)
        : text(text), callback(callback) {}
    FluentNotificationAction(const QString& text, const QIcon& icon,
                             std::function<void()> callback = nullptr)
        : text(text), icon(icon), callback(callback) {}
};

// Individual notification widget
class FluentNotification : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(
        FluentNotificationType type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(FluentNotificationComplexity complexity READ complexity WRITE
                   setComplexity NOTIFY complexityChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(
        QString message READ message WRITE setMessage NOTIFY messageChanged)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(
        int duration READ duration WRITE setDuration NOTIFY durationChanged)
    Q_PROPERTY(
        bool closable READ isClosable WRITE setClosable NOTIFY closableChanged)
    Q_PROPERTY(bool persistent READ isPersistent WRITE setPersistent NOTIFY
                   persistentChanged)
    Q_PROPERTY(
        qreal opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
    Q_PROPERTY(FluentNotificationElevation elevation READ elevation WRITE
                   setElevation NOTIFY elevationChanged)

public:
    explicit FluentNotification(QWidget* parent = nullptr);
    explicit FluentNotification(FluentNotificationType type,
                                const QString& title,
                                const QString& message = QString(),
                                QWidget* parent = nullptr);
    ~FluentNotification() override;

    // Type and content
    FluentNotificationType type() const;
    void setType(FluentNotificationType type);

    // Complexity mode
    FluentNotificationComplexity complexity() const;
    void setComplexity(FluentNotificationComplexity complexity);

    QString title() const;
    void setTitle(const QString& title);

    QString message() const;
    void setMessage(const QString& message);

    QIcon icon() const;
    void setIcon(const QIcon& icon);

    // Behavior
    int duration() const;
    void setDuration(int milliseconds);

    bool isClosable() const;
    void setClosable(bool closable);

    bool isPersistent() const;
    void setPersistent(bool persistent);

    // Elevation
    FluentNotificationElevation elevation() const;
    void setElevation(FluentNotificationElevation elevation);

    // Actions
    void addAction(const FluentNotificationAction& action);
    void addAction(const QString& text,
                   std::function<void()> callback = nullptr);
    void addAction(const QString& text, const QIcon& icon,
                   std::function<void()> callback = nullptr);
    void clearActions();

    // Progress support
    void setProgress(int value);
    void setProgressRange(int minimum, int maximum);
    void setProgressVisible(bool visible);
    int progress() const;
    bool isProgressVisible() const;

    // Quick reply support
    void setQuickReplyEnabled(bool enabled);
    void setQuickReplyPlaceholder(const QString& placeholder);
    bool isQuickReplyEnabled() const;
    QString quickReplyText() const;

    // Context menu support
    void setContextMenuEnabled(bool enabled);
    void addContextMenuAction(const QString& text,
                              std::function<void()> callback);
    void clearContextMenuActions();

    // Animation and display
    qreal opacity() const;
    void setOpacity(qreal opacity);

    void show();
    void hide();
    void close();

    // Size hints
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // Convenience factory methods (from Simple variant)
    static FluentNotification* createInfo(const QString& title,
                                          const QString& message = QString(),
                                          QWidget* parent = nullptr);
    static FluentNotification* createSuccess(const QString& title,
                                             const QString& message = QString(),
                                             QWidget* parent = nullptr);
    static FluentNotification* createWarning(const QString& title,
                                             const QString& message = QString(),
                                             QWidget* parent = nullptr);
    static FluentNotification* createError(const QString& title,
                                           const QString& message = QString(),
                                           QWidget* parent = nullptr);
    static FluentNotification* createSimple(FluentNotificationType type,
                                            const QString& title,
                                            const QString& message = QString(),
                                            QWidget* parent = nullptr);

public slots:
    void showAnimated();
    void hideAnimated();
    void startAutoHide();
    void stopAutoHide();

signals:
    void typeChanged(FluentNotificationType type);
    void complexityChanged(FluentNotificationComplexity complexity);
    void titleChanged(const QString& title);
    void messageChanged(const QString& message);
    void iconChanged(const QIcon& icon);
    void durationChanged(int duration);
    void closableChanged(bool closable);
    void persistentChanged(bool persistent);
    void opacityChanged(qreal opacity);
    void elevationChanged(FluentNotificationElevation elevation);

    void aboutToShow();
    void shown();
    void aboutToHide();
    void hidden();
    void closed();
    void actionTriggered(const QString& actionText);

    // New signals for enhanced functionality
    void progressChanged(int value);
    void quickReplySubmitted(const QString& text);
    void contextMenuRequested(const QPoint& position);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private slots:
    void onCloseButtonClicked();
    void onActionButtonClicked();
    void onAutoHideTimeout();
    void onShowAnimationFinished();
    void onHideAnimationFinished();
    void updateColors();
    void onQuickReplySubmitted();
    void onProgressChanged(int value);
    void onContextMenuActionTriggered();

private:
    void setupUI();
    void setupAnimations();
    void setupAccessibility();
    void updateLayout();
    void updateTypeIcon();
    void updateActionButtons();
    void updateAccessibleProperties();

    // Painting methods
    void paintBackground(QPainter* painter);
    void paintBorder(QPainter* painter);
    void paintShadow(QPainter* painter);

    // Utility methods
    QColor getTypeColor() const;
    QIcon getTypeIcon() const;
    QRect getContentRect() const;
    QRect getIconRect() const;
    QRect getTextRect() const;
    QRect getActionsRect() const;
    QRect getCloseButtonRect() const;

private:
    // Configuration
    FluentNotificationType m_type{FluentNotificationType::Info};
    FluentNotificationComplexity m_complexity{
        FluentNotificationComplexity::Full};
    QString m_title;
    QString m_message;
    QIcon m_customIcon;
    int m_duration{5000};  // 5 seconds default
    bool m_closable{true};
    bool m_persistent{false};
    FluentNotificationElevation m_elevation{
        FluentNotificationElevation::Medium};

    // Actions
    QList<FluentNotificationAction> m_actions;
    QList<QPushButton*> m_actionButtons;

    // UI components
    QLabel* m_iconLabel{nullptr};
    QLabel* m_titleLabel{nullptr};
    QLabel* m_messageLabel{nullptr};
    QPushButton* m_closeButton{nullptr};
    QProgressBar* m_progressBar{nullptr};
    QLineEdit* m_quickReplyInput{nullptr};
    QPushButton* m_quickReplyButton{nullptr};
    QMenu* m_contextMenu{nullptr};

    // Animation and effects
    std::unique_ptr<Animation::FluentAnimator> m_animator;
    QPropertyAnimation* m_showAnimation{nullptr};
    QPropertyAnimation* m_hideAnimation{nullptr};
    QGraphicsOpacityEffect* m_opacityEffect{nullptr};

    // Timers
    QTimer* m_autoHideTimer{nullptr};

    // Enhanced functionality state
    bool m_progressVisible{false};
    int m_progressValue{0};
    int m_progressMin{0};
    int m_progressMax{100};
    bool m_quickReplyEnabled{false};
    QString m_quickReplyPlaceholder;
    bool m_contextMenuEnabled{false};
    QList<QPair<QString, std::function<void()>>> m_contextMenuActions;

    // State
    bool m_hovered{false};
    bool m_focused{false};
    bool m_showing{false};
    bool m_hiding{false};
    qreal m_currentOpacity{1.0};

    // Layout cache
    QRect m_contentRect;
    QRect m_iconRect;
    QRect m_textRect;
    QRect m_actionsRect;
    QRect m_closeButtonRect;
    QRect m_progressRect;
    QRect m_quickReplyRect;
    bool m_layoutDirty{true};
};

// Notification manager for handling multiple notifications
class FluentNotificationManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(FluentNotificationPosition position READ position WRITE
                   setPosition NOTIFY positionChanged)
    Q_PROPERTY(FluentNotificationAnimation animation READ animation WRITE
                   setAnimation NOTIFY animationChanged)
    Q_PROPERTY(int maxVisible READ maxVisible WRITE setMaxVisible NOTIFY
                   maxVisibleChanged)
    Q_PROPERTY(int spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
    Q_PROPERTY(
        QMargins margins READ margins WRITE setMargins NOTIFY marginsChanged)

public:
    static FluentNotificationManager& instance();

    // Configuration
    FluentNotificationPosition position() const;
    void setPosition(FluentNotificationPosition position);

    FluentNotificationAnimation animation() const;
    void setAnimation(FluentNotificationAnimation animation);

    int maxVisible() const;
    void setMaxVisible(int count);

    int spacing() const;
    void setSpacing(int spacing);

    QMargins margins() const;
    void setMargins(const QMargins& margins);

    // Notification management
    FluentNotification* show(FluentNotificationType type, const QString& title,
                             const QString& message = QString(),
                             int duration = 5000);
    FluentNotification* showInfo(const QString& title,
                                 const QString& message = QString(),
                                 int duration = 5000);
    FluentNotification* showSuccess(const QString& title,
                                    const QString& message = QString(),
                                    int duration = 5000);
    FluentNotification* showWarning(const QString& title,
                                    const QString& message = QString(),
                                    int duration = 8000);
    FluentNotification* showError(const QString& title,
                                  const QString& message = QString(),
                                  int duration = 0);  // Persistent by default

    void hide(FluentNotification* notification);
    void hideAll();
    void clear();

    // Queue management
    int visibleCount() const;
    int queuedCount() const;
    QList<FluentNotification*> visibleNotifications() const;

public slots:
    void updateLayout();

signals:
    void positionChanged(FluentNotificationPosition position);
    void animationChanged(FluentNotificationAnimation animation);
    void maxVisibleChanged(int count);
    void spacingChanged(int spacing);
    void marginsChanged(const QMargins& margins);

    void notificationShown(FluentNotification* notification);
    void notificationHidden(FluentNotification* notification);

private slots:
    void onNotificationClosed();
    void processQueue();

private:
    explicit FluentNotificationManager(QObject* parent = nullptr);
    ~FluentNotificationManager() override;

    void positionNotifications();
    void animateNotification(FluentNotification* notification, bool show);
    QPoint calculateNotificationPosition(int index) const;
    QWidget* getParentWidget() const;

private:
    // Configuration
    FluentNotificationPosition m_position{FluentNotificationPosition::TopRight};
    FluentNotificationAnimation m_animation{FluentNotificationAnimation::Slide};
    int m_maxVisible{5};
    int m_spacing{8};
    QMargins m_margins{16, 16, 16, 16};

    // Notification management
    QList<FluentNotification*> m_visibleNotifications;
    QQueue<FluentNotification*> m_queuedNotifications;

    // Animation
    QTimer* m_layoutTimer{nullptr};
};

}  // namespace FluentQt::Components
