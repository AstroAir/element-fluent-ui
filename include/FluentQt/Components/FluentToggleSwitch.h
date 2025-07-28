// include/FluentQt/Components/FluentToggleSwitch.h
#pragma once

#include "FluentQt/Core/FluentComponent.h"
#include "FluentQt/Animation/FluentAnimator.h"
#include <QWidget>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QIcon>

namespace FluentQt::Components {

enum class FluentToggleSwitchSize {
    Small,      // 32x16px
    Medium,     // 40x20px  
    Large       // 48x24px
};

enum class FluentToggleSwitchLabelPosition {
    Left,
    Right,
    Above,
    Below,
    None
};

class FluentToggleSwitch : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(bool checked READ isChecked WRITE setChecked NOTIFY checkedChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString onText READ onText WRITE setOnText NOTIFY onTextChanged)
    Q_PROPERTY(QString offText READ offText WRITE setOffText NOTIFY offTextChanged)
    Q_PROPERTY(QIcon onIcon READ onIcon WRITE setOnIcon NOTIFY onIconChanged)
    Q_PROPERTY(QIcon offIcon READ offIcon WRITE setOffIcon NOTIFY offIconChanged)
    Q_PROPERTY(FluentToggleSwitchSize size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(FluentToggleSwitchLabelPosition labelPosition READ labelPosition WRITE setLabelPosition NOTIFY labelPositionChanged)
    Q_PROPERTY(bool animated READ isAnimated WRITE setAnimated NOTIFY animatedChanged)
    Q_PROPERTY(bool showIcons READ showIcons WRITE setShowIcons NOTIFY showIconsChanged)
    Q_PROPERTY(bool showStateText READ showStateText WRITE setShowStateText NOTIFY showStateTextChanged)
    Q_PROPERTY(int animationDuration READ animationDuration WRITE setAnimationDuration NOTIFY animationDurationChanged)

public:
    explicit FluentToggleSwitch(QWidget* parent = nullptr);
    explicit FluentToggleSwitch(const QString& text, QWidget* parent = nullptr);
    explicit FluentToggleSwitch(const QString& text, bool checked, QWidget* parent = nullptr);
    ~FluentToggleSwitch() override;

    // Checked state
    bool isChecked() const;
    void setChecked(bool checked);
    void toggle();

    // Text properties
    QString text() const;
    void setText(const QString& text);
    
    QString onText() const;
    void setOnText(const QString& text);
    
    QString offText() const;
    void setOffText(const QString& text);

    // Icon properties
    QIcon onIcon() const;
    void setOnIcon(const QIcon& icon);
    
    QIcon offIcon() const;
    void setOffIcon(const QIcon& icon);
    
    bool showIcons() const;
    void setShowIcons(bool show);

    // Size and layout
    FluentToggleSwitchSize size() const;
    void setSize(FluentToggleSwitchSize size);
    
    FluentToggleSwitchLabelPosition labelPosition() const;
    void setLabelPosition(FluentToggleSwitchLabelPosition position);
    
    bool showStateText() const;
    void setShowStateText(bool show);

    // Animation
    bool isAnimated() const;
    void setAnimated(bool animated);
    
    int animationDuration() const;
    void setAnimationDuration(int duration);

    // Utility methods
    QSize switchSize() const;
    QSize thumbSize() const;
    QRect switchRect() const;
    QRect thumbRect() const;
    QRect labelRect() const;
    QRect stateTextRect() const;
    
    QString currentStateText() const;
    QIcon currentIcon() const;

    // Size hints
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void animateToggle();

signals:
    void checkedChanged(bool checked);
    void textChanged(const QString& text);
    void onTextChanged(const QString& text);
    void offTextChanged(const QString& text);
    void onIconChanged(const QIcon& icon);
    void offIconChanged(const QIcon& icon);
    void sizeChanged(FluentToggleSwitchSize size);
    void labelPositionChanged(FluentToggleSwitchLabelPosition position);
    void animatedChanged(bool animated);
    void showIconsChanged(bool show);
    void showStateTextChanged(bool show);
    void animationDurationChanged(int duration);
    
    void toggled(bool checked);
    void clicked();

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

private slots:
    void onToggleAnimationFinished();
    void onThumbPositionChanged();
    void updateColors();

private:
    void setupAnimations();
    void updateLayout();
    void updateThumbPosition();
    
    // Painting methods
    void paintSwitch(QPainter* painter);
    void paintThumb(QPainter* painter);
    void paintLabel(QPainter* painter);
    void paintStateText(QPainter* painter);
    void paintIcon(QPainter* painter);
    void paintFocusIndicator(QPainter* painter);
    
    // Animation methods
    void startToggleAnimation();
    void animateThumbPosition();
    void animateColors();
    void animateScale();
    
    // Utility methods
    QRect calculateSwitchRect() const;
    QRect calculateThumbRect() const;
    QRect calculateLabelRect() const;
    QRect calculateStateTextRect() const;
    QRect calculateIconRect() const;
    
    QPoint getThumbPosition(bool checked) const;
    QColor getSwitchColor() const;
    QColor getThumbColor() const;
    QColor getTrackColor() const;
    
    void updateSizeMetrics();
    void layoutComponents();
    
    bool hitTestSwitch(const QPoint& position) const;

private:
    // State
    bool m_checked{false};
    bool m_pressed{false};
    bool m_hovered{false};
    
    // Text properties
    QString m_text;
    QString m_onText{"On"};
    QString m_offText{"Off"};
    
    // Icon properties
    QIcon m_onIcon;
    QIcon m_offIcon;
    bool m_showIcons{false};
    
    // Layout properties
    FluentToggleSwitchSize m_size{FluentToggleSwitchSize::Medium};
    FluentToggleSwitchLabelPosition m_labelPosition{FluentToggleSwitchLabelPosition::Right};
    bool m_showStateText{false};
    
    // Animation properties
    bool m_animated{true};
    int m_animationDuration{200};
    
    // Size metrics (updated based on m_size)
    int m_switchWidth{40};
    int m_switchHeight{20};
    int m_thumbSize{16};
    int m_padding{2};
    
    // Layout cache
    QRect m_switchRect;
    QRect m_thumbRect;
    QRect m_labelRect;
    QRect m_stateTextRect;
    QRect m_iconRect;
    bool m_layoutDirty{true};
    
    // Animation
    std::unique_ptr<Animation::FluentAnimator> m_animator;
    QParallelAnimationGroup* m_toggleAnimation{nullptr};
    QPropertyAnimation* m_thumbAnimation{nullptr};
    QPropertyAnimation* m_colorAnimation{nullptr};
    QPropertyAnimation* m_scaleAnimation{nullptr};
    
    // Animation properties
    Q_PROPERTY(qreal thumbPosition READ thumbPosition WRITE setThumbPosition)
    Q_PROPERTY(QColor switchColor READ switchColor WRITE setSwitchColor)
    Q_PROPERTY(qreal scale READ scale WRITE setScale)
    
    qreal m_thumbPosition{0.0};     // 0.0 = off position, 1.0 = on position
    QColor m_switchColor;
    qreal m_scale{1.0};
    
    // Property accessors for animations
    qreal thumbPosition() const { return m_thumbPosition; }
    void setThumbPosition(qreal position);
    
    QColor switchColor() const { return m_switchColor; }
    void setSwitchColor(const QColor& color);
    
    qreal scale() const { return m_scale; }
    void setScale(qreal scale);
};

} // namespace FluentQt::Components
