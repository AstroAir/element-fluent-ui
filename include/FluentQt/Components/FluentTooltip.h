// include/FluentQt/Components/FluentTooltip.h
#pragma once

#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QPropertyAnimation>
#include <QTimer>
#include <QWidget>
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentComponent.h"

class QVBoxLayout;

namespace FluentQt::Components {

enum class FluentTooltipPosition {
    Top,
    Bottom,
    Left,
    Right,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
    Auto  // Automatically choose best position
};

enum class FluentTooltipTheme {
    Light,
    Dark,
    Auto  // Follow system theme
};

class FluentTooltip : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(FluentTooltipPosition position READ position WRITE setPosition
                   NOTIFY positionChanged)
    Q_PROPERTY(
        FluentTooltipTheme theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(
        int showDelay READ showDelay WRITE setShowDelay NOTIFY showDelayChanged)
    Q_PROPERTY(
        int hideDelay READ hideDelay WRITE setHideDelay NOTIFY hideDelayChanged)
    Q_PROPERTY(bool persistent READ isPersistent WRITE setPersistent NOTIFY
                   persistentChanged)
    Q_PROPERTY(
        QSize maxSize READ maxSize WRITE setMaxSize NOTIFY maxSizeChanged)

public:
    explicit FluentTooltip(QWidget* parent = nullptr);
    explicit FluentTooltip(const QString& text, QWidget* parent = nullptr);
    explicit FluentTooltip(const QString& title, const QString& text,
                           QWidget* parent = nullptr);
    ~FluentTooltip() override;

    // Content
    QString text() const;
    void setText(const QString& text);

    QString title() const;
    void setTitle(const QString& title);

    // Position and appearance
    FluentTooltipPosition position() const;
    void setPosition(FluentTooltipPosition position);

    FluentTooltipTheme theme() const;
    void setTheme(FluentTooltipTheme theme);

    // Timing
    int showDelay() const;  // Delay before showing (ms)
    void setShowDelay(int delay);

    int hideDelay() const;  // Delay before hiding (ms)
    void setHideDelay(int delay);

    // Behavior
    bool isPersistent() const;  // Stays visible until explicitly hidden
    void setPersistent(bool persistent);

    QSize maxSize() const;
    void setMaxSize(const QSize& size);

    // Static convenience methods
    static void showTooltip(const QString& text, const QPoint& position,
                            QWidget* parent = nullptr);
    static void showTooltip(const QString& title, const QString& text,
                            const QPoint& position, QWidget* parent = nullptr);
    static void hideTooltip();

    // Instance methods
    void showAt(const QPoint& position);
    void showFor(QWidget* widget,
                 FluentTooltipPosition pos = FluentTooltipPosition::Auto);
    void hide();

    // Size hints
    QSize sizeHint() const override;

public slots:
    void show();

signals:
    void textChanged(const QString& text);
    void titleChanged(const QString& title);
    void positionChanged(FluentTooltipPosition position);
    void themeChanged(FluentTooltipTheme theme);
    void showDelayChanged(int delay);
    void hideDelayChanged(int delay);
    void persistentChanged(bool persistent);
    void maxSizeChanged(const QSize& size);
    void aboutToShow();
    void aboutToHide();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    bool eventFilter(QObject* object, QEvent* event) override;

private slots:
    void onShowTimer();
    void onHideTimer();
    void updateColors();
    void onAnimationFinished();

private:
    void setupUI();
    void setupAnimations();
    void setupTimers();
    void updateContent();
    void updatePosition();
    void updateTheme();
    void calculateOptimalPosition(const QPoint& targetPos,
                                  const QSize& targetSize = QSize());
    void drawTooltip(QPainter& painter);
    void drawArrow(QPainter& painter, const QRect& rect);

    QPoint calculatePosition(const QPoint& targetPos, const QSize& targetSize,
                             FluentTooltipPosition pos);
    QRect getArrowRect() const;
    QPainterPath createTooltipPath(const QRect& rect) const;

    void startShowAnimation();
    void startHideAnimation();

private:
    // Content
    QString m_text;
    QString m_title;

    // Appearance
    FluentTooltipPosition m_position{FluentTooltipPosition::Auto};
    FluentTooltipTheme m_tooltipTheme{FluentTooltipTheme::Auto};
    QSize m_maxSize{300, 200};

    // Timing
    int m_showDelay{500};  // 500ms default
    int m_hideDelay{100};  // 100ms default
    bool m_persistent{false};

    // UI components
    QLabel* m_titleLabel{nullptr};
    QLabel* m_textLabel{nullptr};
    QVBoxLayout* m_layout{nullptr};

    // Animation
    std::unique_ptr<Animation::FluentAnimator> m_animator;
    QPropertyAnimation* m_showAnimation{nullptr};
    QPropertyAnimation* m_hideAnimation{nullptr};
    QGraphicsOpacityEffect* m_opacityEffect{nullptr};

    // Timers
    QTimer* m_showTimer{nullptr};
    QTimer* m_hideTimer{nullptr};

    // State
    bool m_isShowing{false};
    bool m_isHiding{false};
    QWidget* m_targetWidget{nullptr};
    QPoint m_targetPosition;
    FluentTooltipPosition m_actualPosition{FluentTooltipPosition::Top};

    // Static instance for convenience methods
    static FluentTooltip* s_instance;

    // Arrow properties
    static constexpr int ARROW_SIZE = 8;
    static constexpr int BORDER_RADIUS = 6;
    static constexpr int PADDING = 12;
};

}  // namespace FluentQt::Components
