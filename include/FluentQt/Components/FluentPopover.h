// include/FluentQt/Components/FluentPopover.h
#pragma once

#include "FluentQt/Core/FluentComponent.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QGraphicsDropShadowEffect>
#include <QTimer>
#include <QIcon>
#include <memory>

namespace FluentQt::Components {

enum class FluentPopoverPlacement {
    Top,
    TopStart,
    TopEnd,
    Bottom,
    BottomStart,
    BottomEnd,
    Left,
    LeftStart,
    LeftEnd,
    Right,
    RightStart,
    RightEnd,
    Auto
};

enum class FluentPopoverTrigger {
    Click,
    Hover,
    Focus,
    Manual
};

class FluentPopover : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString content READ content WRITE setContent NOTIFY contentChanged)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(FluentPopoverPlacement placement READ placement WRITE setPlacement NOTIFY placementChanged)
    Q_PROPERTY(FluentPopoverTrigger trigger READ trigger WRITE setTrigger NOTIFY triggerChanged)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibilityChanged)
    Q_PROPERTY(bool showArrow READ showArrow WRITE setShowArrow NOTIFY showArrowChanged)
    Q_PROPERTY(bool showCloseButton READ showCloseButton WRITE setShowCloseButton NOTIFY showCloseButtonChanged)
    Q_PROPERTY(int autoHideDelay READ autoHideDelay WRITE setAutoHideDelay NOTIFY autoHideDelayChanged)
    Q_PROPERTY(int maxWidth READ maxWidth WRITE setMaxWidth NOTIFY maxWidthChanged)

public:
    explicit FluentPopover(QWidget* parent = nullptr);
    explicit FluentPopover(const QString& content, QWidget* parent = nullptr);
    explicit FluentPopover(const QString& title, const QString& content, QWidget* parent = nullptr);
    ~FluentPopover() override;

    // Content properties
    QString title() const;
    void setTitle(const QString& title);

    QString content() const;
    void setContent(const QString& content);

    QIcon icon() const;
    void setIcon(const QIcon& icon);

    // Placement properties
    FluentPopoverPlacement placement() const;
    void setPlacement(FluentPopoverPlacement placement);

    FluentPopoverTrigger trigger() const;
    void setTrigger(FluentPopoverTrigger trigger);

    // Appearance properties
    bool showArrow() const;
    void setShowArrow(bool show);

    bool showCloseButton() const;
    void setShowCloseButton(bool show);

    int autoHideDelay() const;
    void setAutoHideDelay(int delay);

    int maxWidth() const;
    void setMaxWidth(int width);

    // Content management
    void setContentWidget(QWidget* widget);
    QWidget* contentWidget() const;

    // Target management
    void setTarget(QWidget* target);
    QWidget* target() const;

    // Size calculations
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void show();
    void hide();
    void toggle();

signals:
    void titleChanged(const QString& title);
    void contentChanged(const QString& content);
    void iconChanged(const QIcon& icon);
    void placementChanged(FluentPopoverPlacement placement);
    void triggerChanged(FluentPopoverTrigger trigger);
    void visibilityChanged(bool visible);
    void showArrowChanged(bool show);
    void showCloseButtonChanged(bool show);
    void autoHideDelayChanged(int delay);
    void maxWidthChanged(int width);
    void aboutToShow();
    void aboutToHide();
    void shown();
    void hidden();

protected:
    // Event handling
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    bool eventFilter(QObject* object, QEvent* event) override;
    
    // State management
    void updateStateStyle() override;
    void performStateTransition(Core::FluentState from, Core::FluentState to) override;

private slots:
    void onCloseButtonClicked();
    void onAutoHideTimer();
    void onThemeChanged();
    void onShowAnimationFinished();
    void onHideAnimationFinished();

private:
    // Setup methods
    void setupUI();
    void setupAnimations();
    void updateColors();
    void updateFonts();

    // Painting methods
    void paintBackground(QPainter& painter, const QRect& rect);
    void paintBorder(QPainter& painter, const QRect& rect);
    void paintArrow(QPainter& painter);

    // Animation methods
    void startShowAnimation();
    void startHideAnimation();

    // Utility methods
    void updatePosition();
    void updateArrowPosition();
    QPoint calculatePosition(FluentPopoverPlacement placement) const;
    QPoint calculateArrowPosition() const;
    FluentPopoverPlacement getOptimalPlacement() const;
    void installTargetEventFilter();
    void removeTargetEventFilter();

private:
    // Content
    QString m_title;
    QString m_content;
    QIcon m_icon;
    
    // Placement
    FluentPopoverPlacement m_placement{FluentPopoverPlacement::Auto};
    FluentPopoverTrigger m_trigger{FluentPopoverTrigger::Click};
    FluentPopoverPlacement m_actualPlacement{FluentPopoverPlacement::Top};
    
    // Appearance
    bool m_showArrow{true};
    bool m_showCloseButton{false};
    int m_autoHideDelay{0}; // 0 = no auto hide
    int m_maxWidth{300};
    
    // State
    bool m_isVisible{false};
    QPoint m_arrowPosition;
    
    // Target
    QWidget* m_target{nullptr};
    
    // UI components
    QVBoxLayout* m_mainLayout{nullptr};
    QHBoxLayout* m_headerLayout{nullptr};
    QLabel* m_iconLabel{nullptr};
    QLabel* m_titleLabel{nullptr};
    QPushButton* m_closeButton{nullptr};
    QLabel* m_contentLabel{nullptr};
    QWidget* m_contentWidget{nullptr};
    
    // Effects and animations
    QGraphicsOpacityEffect* m_opacityEffect{nullptr};
    QGraphicsDropShadowEffect* m_shadowEffect{nullptr};
    std::unique_ptr<QPropertyAnimation> m_showAnimation;
    std::unique_ptr<QPropertyAnimation> m_hideAnimation;
    QTimer* m_autoHideTimer{nullptr};
    
    // Cached values
    mutable QSize m_cachedSizeHint;
    mutable bool m_sizeHintValid{false};
};

} // namespace FluentQt::Components
