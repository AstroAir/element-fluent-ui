// include/FluentQt/Components/FluentPopover.h
#pragma once

#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <memory>
#include "FluentQt/Core/FluentComponent.h"

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

enum class FluentPopoverTrigger { Click, Hover, Focus, Manual };

class FluentPopover : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(
        QString content READ content WRITE setContent NOTIFY contentChanged)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(FluentPopoverPlacement placement READ placement WRITE
                   setPlacement NOTIFY placementChanged)
    Q_PROPERTY(FluentPopoverTrigger trigger READ trigger WRITE setTrigger NOTIFY
                   triggerChanged)
    Q_PROPERTY(
        bool visible READ isVisible WRITE setVisible NOTIFY visibilityChanged)
    Q_PROPERTY(bool showArrow READ showArrow WRITE setShowArrow NOTIFY
                   showArrowChanged)
    Q_PROPERTY(bool showCloseButton READ showCloseButton WRITE
                   setShowCloseButton NOTIFY showCloseButtonChanged)
    Q_PROPERTY(int autoHideDelay READ autoHideDelay WRITE setAutoHideDelay
                   NOTIFY autoHideDelayChanged)
    Q_PROPERTY(
        int maxWidth READ maxWidth WRITE setMaxWidth NOTIFY maxWidthChanged)
    Q_PROPERTY(
        int elevation READ elevation WRITE setElevation NOTIFY elevationChanged)
    Q_PROPERTY(bool enableBackdrop READ enableBackdrop WRITE setEnableBackdrop
                   NOTIFY enableBackdropChanged)
    Q_PROPERTY(bool enableScaleAnimation READ enableScaleAnimation WRITE
                   setEnableScaleAnimation NOTIFY enableScaleAnimationChanged)
    Q_PROPERTY(int borderWidth READ borderWidth WRITE setBorderWidth NOTIFY
                   borderWidthChanged)
    Q_PROPERTY(QColor customBackgroundColor READ customBackgroundColor WRITE
                   setCustomBackgroundColor NOTIFY customBackgroundColorChanged)
    Q_PROPERTY(QColor customBorderColor READ customBorderColor WRITE
                   setCustomBorderColor NOTIFY customBorderColorChanged)

public:
    explicit FluentPopover(QWidget* parent = nullptr);
    explicit FluentPopover(const QString& content, QWidget* parent = nullptr);
    explicit FluentPopover(const QString& title, const QString& content,
                           QWidget* parent = nullptr);
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

    // Enhanced styling properties
    int elevation() const;
    void setElevation(int elevation);

    bool enableBackdrop() const;
    void setEnableBackdrop(bool enable);

    bool enableScaleAnimation() const;
    void setEnableScaleAnimation(bool enable);

    int borderWidth() const;
    void setBorderWidth(int width);

    QColor customBackgroundColor() const;
    void setCustomBackgroundColor(const QColor& color);

    QColor customBorderColor() const;
    void setCustomBorderColor(const QColor& color);

    // Accessibility properties
    QString accessibleName() const;
    void setAccessibleName(const QString& name);

    QString accessibleDescription() const;
    void setAccessibleDescription(const QString& description);

    QString ariaLabel() const;
    void setAriaLabel(const QString& label);

    QString ariaDescribedBy() const;
    void setAriaDescribedBy(const QString& describedBy);

    // Responsive design properties
    int minWidth() const;
    void setMinWidth(int width);

    int maxHeight() const;
    void setMaxHeight(int height);

    bool adaptiveWidth() const;
    void setAdaptiveWidth(bool adaptive);

    bool adaptiveHeight() const;
    void setAdaptiveHeight(bool adaptive);

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
    void elevationChanged(int elevation);
    void enableBackdropChanged(bool enable);
    void enableScaleAnimationChanged(bool enable);
    void borderWidthChanged(int width);
    void customBackgroundColorChanged(const QColor& color);
    void customBorderColorChanged(const QColor& color);
    void accessibleNameChanged(const QString& name);
    void accessibleDescriptionChanged(const QString& description);
    void ariaLabelChanged(const QString& label);
    void ariaDescribedByChanged(const QString& describedBy);
    void minWidthChanged(int width);
    void maxHeightChanged(int height);
    void adaptiveWidthChanged(bool adaptive);
    void adaptiveHeightChanged(bool adaptive);
    void aboutToShow();
    void aboutToHide();
    void shown();
    void hidden();
    void positionChanged(const QPoint& position);
    void sizeChanged(const QSize& size);

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
    void performStateTransition(Core::FluentState from,
                                Core::FluentState to) override;

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
    void paintBackdrop(QPainter& painter);
    void paintElevationShadow(QPainter& painter, const QRect& rect);

    // Animation methods
    void startShowAnimation();
    void startHideAnimation();
    void setupScaleAnimation();
    void updateAnimationProperties();
    void updateShadowEffect();

    // Utility methods
    void updatePosition();
    void updateArrowPosition();
    QPoint calculatePosition(FluentPopoverPlacement placement) const;
    QPoint calculateArrowPosition() const;
    FluentPopoverPlacement getOptimalPlacement() const;
    void installTargetEventFilter();
    void removeTargetEventFilter();
    void updateAccessibility();
    void calculateResponsiveSize();
    QSize getAdaptiveSize() const;

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
    int m_autoHideDelay{0};  // 0 = no auto hide
    int m_maxWidth{300};
    int m_minWidth{100};
    int m_maxHeight{600};
    int m_elevation{2};
    bool m_enableBackdrop{false};
    bool m_enableScaleAnimation{true};
    int m_borderWidth{1};
    bool m_adaptiveWidth{false};
    bool m_adaptiveHeight{false};

    // Colors
    QColor m_customBackgroundColor;
    QColor m_customBorderColor;

    // Accessibility
    QString m_accessibleName;
    QString m_accessibleDescription;
    QString m_ariaLabel;
    QString m_ariaDescribedBy;

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
    std::unique_ptr<QPropertyAnimation> m_scaleAnimation;
    std::unique_ptr<QParallelAnimationGroup> m_showAnimationGroup;
    std::unique_ptr<QParallelAnimationGroup> m_hideAnimationGroup;
    QTimer* m_autoHideTimer{nullptr};

    // Cached values
    mutable QSize m_cachedSizeHint;
    mutable bool m_sizeHintValid{false};
};

}  // namespace FluentQt::Components
