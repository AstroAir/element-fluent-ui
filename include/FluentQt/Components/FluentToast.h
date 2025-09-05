// include/FluentQt/Components/FluentToast.h
#pragma once

#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QParallelAnimationGroup>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QSequentialAnimationGroup>
#include <QTimer>
#include <QVBoxLayout>
#include <QVariant>
#include <QWidget>
#include <functional>
#include <memory>
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

// Forward declarations
class FluentToastManager;
class FluentButton;

enum class FluentToastType { Info, Success, Warning, Error, Custom };

enum class FluentToastPosition {
    TopLeft,
    TopCenter,
    TopRight,
    BottomLeft,
    BottomCenter,
    BottomRight,
    Center
};

enum class FluentToastAnimation { Slide, Fade, Scale, Bounce };

struct FluentToastAction {
    QString text;
    QIcon icon;
    std::function<void()> callback;
    bool primary{false};
    bool dismissOnClick{true};

    FluentToastAction() = default;
    FluentToastAction(const QString& actionText,
                      std::function<void()> actionCallback,
                      bool isPrimary = false)
        : text(actionText),
          callback(std::move(actionCallback)),
          primary(isPrimary) {}
    FluentToastAction(const QIcon& actionIcon, const QString& actionText,
                      std::function<void()> actionCallback,
                      bool isPrimary = false)
        : text(actionText),
          icon(actionIcon),
          callback(std::move(actionCallback)),
          primary(isPrimary) {}
};

struct FluentToastConfig {
    FluentToastType type{FluentToastType::Info};
    FluentToastPosition position{FluentToastPosition::TopRight};
    FluentToastAnimation animation{FluentToastAnimation::Slide};

    int duration{5000};  // Auto-dismiss duration in ms (0 = no auto-dismiss)
    int maxWidth{400};
    int minWidth{300};
    bool closable{true};
    bool pauseOnHover{true};
    bool showProgress{false};
    bool persistent{
        false};  // If true, ignores duration and requires manual dismissal

    // Spacing and positioning (deprecated - use theme tokens instead)
    int margin{16};
    int spacing{8};

    // Animation settings
    int animationDuration{300};  // 0 = use theme default
    QEasingCurve easingCurve{QEasingCurve::OutCubic};

    // Custom styling
    QColor customBackgroundColor;
    QColor customTextColor;
    QColor customBorderColor;
    QIcon customIcon;

    bool autoCalculateColors{true};

    // Enhanced Fluent UI features
    bool useFluentMotion{true};       // Use Fluent Design motion principles
    bool respectReducedMotion{true};  // Honor system reduced motion settings
    bool enableKeyboardNavigation{true};  // Enable keyboard accessibility
    bool useSemanticColors{true};         // Use semantic color tokens
    bool autoElevation{true};  // Automatically apply elevation/shadows

    // Accessibility enhancements
    bool announceToScreenReader{true};  // Announce toast to screen readers
    bool highContrastSupport{true};     // Support high contrast themes
    QString customAriaLabel;            // Custom ARIA label override
};

class FluentToast : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(
        QString message READ message WRITE setMessage NOTIFY messageChanged)
    Q_PROPERTY(FluentToastType toastType READ toastType WRITE setToastType
                   NOTIFY typeChanged)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(
        int duration READ duration WRITE setDuration NOTIFY durationChanged)
    Q_PROPERTY(
        bool closable READ isClosable WRITE setClosable NOTIFY closableChanged)
    Q_PROPERTY(bool pauseOnHover READ pauseOnHover WRITE setPauseOnHover NOTIFY
                   pauseOnHoverChanged)
    Q_PROPERTY(bool showProgress READ showProgress WRITE setShowProgress NOTIFY
                   showProgressChanged)
    Q_PROPERTY(bool persistent READ isPersistent WRITE setPersistent NOTIFY
                   persistentChanged)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    explicit FluentToast(QWidget* parent = nullptr);
    explicit FluentToast(FluentToastType type, const QString& title,
                         QWidget* parent = nullptr);
    explicit FluentToast(FluentToastType type, const QString& title,
                         const QString& message, QWidget* parent = nullptr);
    explicit FluentToast(const FluentToastConfig& config,
                         QWidget* parent = nullptr);
    ~FluentToast() override;

    // Content properties
    QString title() const { return m_title; }
    void setTitle(const QString& title);

    QString message() const { return m_message; }
    void setMessage(const QString& message);

    FluentToastType toastType() const { return m_config.type; }
    void setToastType(FluentToastType type);

    QIcon icon() const { return m_icon; }
    void setIcon(const QIcon& icon);

    // Behavior properties
    int duration() const { return m_config.duration; }
    void setDuration(int duration);

    bool isClosable() const { return m_config.closable; }
    void setClosable(bool closable);

    bool pauseOnHover() const { return m_config.pauseOnHover; }
    void setPauseOnHover(bool pause);

    bool showProgress() const { return m_config.showProgress; }
    void setShowProgress(bool show);

    bool isPersistent() const { return m_config.persistent; }
    void setPersistent(bool persistent);

    // Configuration
    FluentToastConfig configuration() const { return m_config; }
    void setConfiguration(const FluentToastConfig& config);

    // Actions
    void addAction(const FluentToastAction& action);
    void addAction(const QString& text, std::function<void()> callback,
                   bool primary = false);
    void addAction(const QIcon& icon, const QString& text,
                   std::function<void()> callback, bool primary = false);
    void clearActions();
    QList<FluentToastAction> actions() const { return m_actions; }

    // Progress
    void setProgress(int value);
    void setProgressRange(int minimum, int maximum);
    int progress() const;
    int progressMinimum() const;
    int progressMaximum() const;

    // User data
    QVariant userData() const { return m_userData; }
    void setUserData(const QVariant& data);

    // State
    bool isShowing() const { return m_showing; }
    bool isHiding() const { return m_hiding; }
    bool isVisible() const;

    // Opacity
    qreal opacity() const { return m_opacity; }
    void setOpacity(qreal opacity);

    // Size management
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // Static factory methods
    static FluentToast* createInfo(const QString& title,
                                   const QString& message = QString(),
                                   QWidget* parent = nullptr);
    static FluentToast* createSuccess(const QString& title,
                                      const QString& message = QString(),
                                      QWidget* parent = nullptr);
    static FluentToast* createWarning(const QString& title,
                                      const QString& message = QString(),
                                      QWidget* parent = nullptr);
    static FluentToast* createError(const QString& title,
                                    const QString& message = QString(),
                                    QWidget* parent = nullptr);
    static FluentToast* createCustom(const QIcon& icon, const QString& title,
                                     const QString& message = QString(),
                                     QWidget* parent = nullptr);

public slots:
    void show();
    void hide();
    void showAnimated();
    void hideAnimated();
    void dismiss();
    void pauseTimer();
    void resumeTimer();
    void resetTimer();

signals:
    void titleChanged(const QString& title);
    void messageChanged(const QString& message);
    void typeChanged(FluentToastType type);
    void iconChanged(const QIcon& icon);
    void durationChanged(int duration);
    void closableChanged(bool closable);
    void pauseOnHoverChanged(bool pause);
    void showProgressChanged(bool show);
    void persistentChanged(bool persistent);

    void aboutToShow();
    void shown();
    void aboutToHide();
    void hidden();
    void dismissed();
    void clicked();
    void actionTriggered(const QString& actionText);
    void progressChanged(int value);
    void timerPaused();
    void timerResumed();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

    virtual void updateStateStyle() override;
    virtual void performStateTransition(Core::FluentState from,
                                        Core::FluentState to) override;

private slots:
    void onAutoHideTimer();
    void onProgressTimer();
    void onShowAnimationFinished();
    void onHideAnimationFinished();
    void onCloseButtonClicked();
    void onActionButtonClicked();
    void onThemeChanged();

private:
    void setupUI();
    void setupAnimations();
    void setupTimer();
    void updateLayout();
    void updateColors();
    void updateIcon();
    void updateProgress();
    void updateAccessibility();

    void paintBackground(QPainter& painter, const QRect& rect);
    void paintBorder(QPainter& painter, const QRect& rect);
    void paintShadow(QPainter& painter, const QRect& rect);

    QColor getBackgroundColor() const;
    QColor getTextColor() const;
    QColor getBorderColor() const;
    QColor getProgressColor() const;
    QIcon getTypeIcon() const;

    void createActionButtons();
    void updateActionButtons();

    void startShowAnimation();
    void startHideAnimation();
    void startProgressAnimation();

    QRect getContentRect() const;
    QRect getIconRect() const;
    QRect getTextRect() const;
    QRect getActionsRect() const;
    QRect getCloseButtonRect() const;
    QRect getProgressRect() const;
    QSize calculateSizeHint() const;

    // Enhanced Fluent UI compliance methods
    void setupFluentStyling();
    void updateElevation();
    void applyColorsToElements();

private:
    FluentToastConfig m_config;

    // Content
    QString m_title;
    QString m_message;
    QIcon m_icon;
    QVariant m_userData;
    QList<FluentToastAction> m_actions;

    // UI Components
    QVBoxLayout* m_mainLayout{nullptr};
    QHBoxLayout* m_contentLayout{nullptr};
    QHBoxLayout* m_actionsLayout{nullptr};
    QLabel* m_iconLabel{nullptr};
    QLabel* m_titleLabel{nullptr};
    QLabel* m_messageLabel{nullptr};
    QProgressBar* m_progressBar{nullptr};
    FluentButton* m_closeButton{nullptr};
    QList<FluentButton*> m_actionButtons;

    // Animation and effects
    std::unique_ptr<QPropertyAnimation> m_showAnimation;
    std::unique_ptr<QPropertyAnimation> m_hideAnimation;
    std::unique_ptr<QSequentialAnimationGroup> m_progressAnimation;
    std::unique_ptr<QGraphicsOpacityEffect> m_opacityEffect;
    std::unique_ptr<QGraphicsDropShadowEffect> m_shadowEffect;

    // Timers
    QTimer* m_autoHideTimer{nullptr};
    QTimer* m_progressTimer{nullptr};

    // State
    bool m_showing{false};
    bool m_hiding{false};
    bool m_timerPaused{false};
    bool m_hovered{false};
    bool m_pressed{false};
    qreal m_opacity{1.0};
    int m_remainingTime{0};
    QDateTime m_pauseTime;

    // Cached values
    QSize m_cachedSizeHint;
    bool m_sizeHintValid{false};
    QRect m_lastRect;

    // Colors (cached from theme)
    QColor m_backgroundColor;
    QColor m_textColor;
    QColor m_borderColor;
    QColor m_progressColor;

    friend class FluentToastManager;
};

}  // namespace FluentQt::Components
