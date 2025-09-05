// include/FluentQt/Components/FluentRadioButton.h
#pragma once

#include <QAbstractButton>
#include <QButtonGroup>
#include <QEnterEvent>
#include <QGraphicsOpacityEffect>
#include <QIcon>
#include <QMutex>
#include <QPropertyAnimation>
#include <memory>
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentState.h"

namespace FluentQt::Components {

enum class FluentRadioButtonSize {
    Small,   // 16x16px
    Medium,  // 20x20px
    Large    // 24x24px
};

enum class FluentRadioButtonLabelPosition { Left, Right, Above, Below };

enum class FluentRadioButtonComplexity {
    Simple,  // Lightweight mode with basic functionality (no animations)
    Full     // Full-featured mode with animations and advanced features
};

class FluentRadioButton : public QAbstractButton {
    Q_OBJECT
    Q_PROPERTY(
        bool checked READ isChecked WRITE setChecked NOTIFY checkedChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(
        FluentRadioButtonSize size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(FluentRadioButtonComplexity complexity READ complexity WRITE
                   setComplexity NOTIFY complexityChanged)
    Q_PROPERTY(FluentRadioButtonLabelPosition labelPosition READ labelPosition
                   WRITE setLabelPosition NOTIFY labelPositionChanged)
    Q_PROPERTY(bool autoExclusive READ autoExclusive WRITE setAutoExclusive
                   NOTIFY autoExclusiveChanged)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(
        bool showIcon READ showIcon WRITE setShowIcon NOTIFY showIconChanged)
    Q_PROPERTY(
        bool animated READ isAnimated WRITE setAnimated NOTIFY animatedChanged)
    Q_PROPERTY(int animationDuration READ animationDuration WRITE
                   setAnimationDuration NOTIFY animationDurationChanged)

    // Animation properties for QPropertyAnimation
    Q_PROPERTY(qreal radioScale READ radioScale WRITE setRadioScale)
    Q_PROPERTY(QColor radioColor READ radioColor WRITE setRadioColor)
    Q_PROPERTY(
        qreal indicatorOpacity READ indicatorOpacity WRITE setIndicatorOpacity)

public:
    explicit FluentRadioButton(QWidget* parent = nullptr);
    explicit FluentRadioButton(const QString& text, QWidget* parent = nullptr);
    explicit FluentRadioButton(const QString& text, const QString& value,
                               QWidget* parent = nullptr);
    ~FluentRadioButton() override;

    // Checked state
    bool isChecked() const;
    void setChecked(bool checked);
    void toggle();

    // Text properties
    QString text() const;
    void setText(const QString& text);

    QString value() const;
    void setValue(const QString& value);

    // Icon properties
    QIcon icon() const;
    void setIcon(const QIcon& icon);

    bool showIcon() const;
    void setShowIcon(bool show);

    // Size and layout
    FluentRadioButtonSize size() const;
    void setSize(FluentRadioButtonSize size);

    // Complexity mode
    FluentRadioButtonComplexity complexity() const;
    void setComplexity(FluentRadioButtonComplexity complexity);

    FluentRadioButtonLabelPosition labelPosition() const;
    void setLabelPosition(FluentRadioButtonLabelPosition position);

    // Group behavior
    bool autoExclusive() const;
    void setAutoExclusive(bool autoExclusive);

    QButtonGroup* buttonGroup() const;
    void setButtonGroup(QButtonGroup* group);

    // Animation
    bool isAnimated() const;
    void setAnimated(bool animated);

    int animationDuration() const;
    void setAnimationDuration(int duration);

    // Utility methods
    QSize radioSize() const;
    QSize iconSize() const;
    QRect radioRect() const;
    QRect iconRect() const;
    QRect labelRect() const;

    // Size hints
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // Static convenience methods
    static QButtonGroup* createButtonGroup(const QStringList& options,
                                           QWidget* parent = nullptr);
    static QList<FluentRadioButton*> createRadioGroup(
        const QStringList& options, QWidget* parent = nullptr);

    // Factory methods for simple radio buttons (from Simple variant)
    static FluentRadioButton* createSimple(const QString& text = QString(),
                                           QWidget* parent = nullptr);
    static FluentRadioButton* createSimple(const QString& text,
                                           const QString& value,
                                           QWidget* parent = nullptr);

public slots:
    void animateCheck();
    void click();

signals:
    void checkedChanged(bool checked);
    void textChanged(const QString& text);
    void valueChanged(const QString& value);
    void sizeChanged(FluentRadioButtonSize size);
    void complexityChanged(FluentRadioButtonComplexity complexity);
    void labelPositionChanged(FluentRadioButtonLabelPosition position);
    void autoExclusiveChanged(bool autoExclusive);
    void iconChanged(const QIcon& icon);
    void showIconChanged(bool show);
    void animatedChanged(bool animated);
    void animationDurationChanged(int duration);

    void toggled(bool checked);
    void clicked();
    void pressed();
    void released();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;

private slots:
    void onCheckAnimationFinished();
    void onRadioPositionChanged();
    void updateColors();
    void onButtonGroupToggled(QAbstractButton* button, bool checked);

private:
    void setupAnimations();
    void setupButtonGroup();
    void setupAccessibility();
    void updateLayout();
    void updateRadioPosition();

    // Painting methods
    void paintRadio(QPainter* painter);
    void paintRadioIndicator(QPainter* painter);
    void paintRadioButton(
        QPainter* painter);  // Legacy method name for compatibility
    void paintIcon(QPainter* painter);
    void paintLabel(QPainter* painter);
    void paintText(QPainter* painter);  // Legacy method name for compatibility
    void paintFocusIndicator(QPainter* painter);
    void paintHoverEffect(QPainter* painter);

    // Animation methods
    void startCheckAnimation();
    void animateRadioScale();
    void animateColors();
    void animateIndicatorOpacity();
    void startPressAnimation();
    void startReleaseAnimation();
    void startHoverAnimation(bool hover);

    // Utility methods
    QRect calculateRadioRect() const;
    QRect calculateIconRect() const;
    QRect calculateLabelRect() const;

    QColor getRadioColor() const;
    QColor getIndicatorColor() const;
    QColor getTextColor() const;
    QColor getBorderColor() const;
    QColor getHoverColor() const;
    QColor getFocusColor() const;

    void updateSizeMetrics();
    void layoutComponents();

    bool hitTestRadio(const QPoint& position) const;
    void updateAccessibility();

    // State management for FluentUI compliance (override from FluentComponent)
    // Core::FluentState state() const; // Inherited from FluentComponent

    // Button-specific functionality (since we no longer inherit from
    // QAbstractButton)
    bool isDown() const { return m_pressed; }
    void setDown(bool down);

    // FluentComponent-style state management (manual implementation)
    void updateStateStyle();
    void performStateTransition(Core::FluentState from, Core::FluentState to);

private:
    // State
    bool m_checked{false};
    bool m_pressed{false};
    bool m_hovered{false};
    FluentRadioButtonComplexity m_complexity{FluentRadioButtonComplexity::Full};

    // Content properties
    QString m_text;
    QString m_value;
    QIcon m_icon;
    bool m_showIcon{false};

    // Layout properties
    FluentRadioButtonSize m_size{FluentRadioButtonSize::Medium};
    FluentRadioButtonLabelPosition m_labelPosition{
        FluentRadioButtonLabelPosition::Right};

    // Group behavior
    bool m_autoExclusive{true};
    QButtonGroup* m_buttonGroup{nullptr};
    bool m_ownButtonGroup{false};  // Whether we created the button group

    // Animation properties
    bool m_animated{true};
    int m_animationDuration{200};

    // Size metrics (updated based on m_size)
    int m_radioSize{20};
    int m_indicatorSize{8};
    int m_iconSize{16};
    int m_spacing{8};

    // Layout cache
    QRect m_radioRect;
    QRect m_iconRect;
    QRect m_labelRect;
    bool m_layoutDirty{true};

    // Animation
    std::unique_ptr<Animation::FluentAnimator> m_animator;
    QPropertyAnimation* m_checkAnimation{nullptr};
    QPropertyAnimation* m_scaleAnimation{nullptr};
    QPropertyAnimation* m_colorAnimation{nullptr};
    QPropertyAnimation* m_indicatorAnimation{nullptr};

    // Animation properties
    qreal m_radioScale{1.0};
    QColor m_radioColor;
    qreal m_indicatorOpacity{0.0};  // 0.0 = hidden, 1.0 = fully visible

    // Color properties for FluentUI styling
    QColor m_borderColor;
    QColor m_backgroundColor;
    QColor m_textColor;
    QColor m_indicatorColor;

    // Cached theme colors for performance optimization
    mutable QColor m_cachedAccentColor;
    mutable QColor m_cachedHoverColor;
    mutable QColor m_cachedFocusColor;
    mutable bool m_colorsCacheValid{false};

    // Thread safety
    mutable QMutex m_mutex;

    // Property accessors for animations
    qreal radioScale() const;
    void setRadioScale(qreal scale);

    QColor radioColor() const;
    void setRadioColor(const QColor& color);

    qreal indicatorOpacity() const;
    void setIndicatorOpacity(qreal opacity);
};

}  // namespace FluentQt::Components
