// include/FluentQt/Components/FluentButton.h
#pragma once

#include "FluentQt/Core/FluentComponent.h"
#include <QPushButton>
#include <QIcon>
#include <QFontMetrics>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QTimer>
#include <QGraphicsEffect>
#include <memory>
#include <optional>

namespace FluentQt::Components {

enum class FluentButtonStyle {
    Default,
    Primary,
    Accent,
    Subtle,
    Outline,
    Hyperlink,
    Toggle
};

enum class FluentButtonSize {
    Small,
    Medium,
    Large,
    ExtraLarge
};

enum class FluentIconPosition {
    Left,
    Right,
    Top,
    Bottom
};

class FluentButton : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(FluentButtonStyle buttonStyle READ buttonStyle WRITE setButtonStyle NOTIFY styleChanged)
    Q_PROPERTY(FluentButtonSize buttonSize READ buttonSize WRITE setButtonSize NOTIFY sizeChanged)
    Q_PROPERTY(FluentIconPosition iconPosition READ iconPosition WRITE setIconPosition NOTIFY iconPositionChanged)
    Q_PROPERTY(bool loading READ isLoading WRITE setLoading NOTIFY loadingChanged)
    Q_PROPERTY(bool checkable READ isCheckable WRITE setCheckable)
    Q_PROPERTY(bool checked READ isChecked WRITE setChecked NOTIFY checkedChanged)
    Q_PROPERTY(bool flat READ isFlat WRITE setFlat)
    Q_PROPERTY(qreal backgroundOpacity READ backgroundOpacity WRITE setBackgroundOpacity)

public:
    explicit FluentButton(QWidget* parent = nullptr);
    explicit FluentButton(const QString& text, QWidget* parent = nullptr);
    explicit FluentButton(const QIcon& icon, const QString& text, QWidget* parent = nullptr);
    ~FluentButton() override;

    // Content properties
    QString text() const noexcept { return m_text; }
    void setText(const QString& text);

    QIcon icon() const noexcept { return m_icon; }
    void setIcon(const QIcon& icon);

    FluentIconPosition iconPosition() const noexcept { return m_iconPosition; }
    void setIconPosition(FluentIconPosition position);

    // Style properties
    FluentButtonStyle buttonStyle() const noexcept { return m_buttonStyle; }
    void setButtonStyle(FluentButtonStyle style);

    FluentButtonSize buttonSize() const noexcept { return m_buttonSize; }
    void setButtonSize(FluentButtonSize size);

    bool isFlat() const noexcept { return m_flat; }
    void setFlat(bool flat);

    // State properties
    bool isLoading() const noexcept { return m_loading; }
    void setLoading(bool loading);

    bool isCheckable() const noexcept { return m_checkable; }
    void setCheckable(bool checkable);

    bool isChecked() const noexcept { return m_checked; }
    void setChecked(bool checked);

    // Animation properties
    qreal backgroundOpacity() const noexcept { return m_backgroundOpacity; }
    void setBackgroundOpacity(qreal opacity);

    // Size calculations
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // Convenience static methods
    static FluentButton* createPrimaryButton(const QString& text, QWidget* parent = nullptr);
    static FluentButton* createAccentButton(const QString& text, QWidget* parent = nullptr);
    static FluentButton* createIconButton(const QIcon& icon, QWidget* parent = nullptr);

public slots:
    void animateClick();
    void toggle();

signals:
    void textChanged(const QString& text);
    void iconChanged(const QIcon& icon);
    void styleChanged(FluentButtonStyle style);
    void sizeChanged(FluentButtonSize size);
    void iconPositionChanged(FluentIconPosition position);
    void loadingChanged(bool loading);
    void checkedChanged(bool checked);
    void clicked(bool checked = false);
    void pressed();
    void released();
    void toggled(bool checked);

protected:
    // Event handling
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void changeEvent(QEvent* event) override;
    
    // State management
    void updateStateStyle() override;
    void performStateTransition(Core::FluentState from, Core::FluentState to) override;

private slots:
    void onLoadingAnimationValueChanged(const QVariant& value);
    void onBackgroundAnimationFinished();
    void onThemeChanged();

private:
    // Painting methods
    void paintBackground(QPainter& painter, const QRect& rect);
    void paintBorder(QPainter& painter, const QRect& rect);
    void paintContent(QPainter& painter, const QRect& rect);
    void paintIcon(QPainter& painter, const QRect& iconRect);
    void paintText(QPainter& painter, const QRect& textRect);
    void paintLoadingIndicator(QPainter& painter, const QRect& rect);
    void paintFocusRing(QPainter& painter, const QRect& rect);
    void paintRevealEffect(QPainter& painter, const QRect& rect);

    // Calculation methods
    QRect calculateContentRect() const;
    QRect calculateIconRect() const;
    QRect calculateTextRect() const;
    QSize calculateIconSize() const;
    QSize calculateTextSize() const;

    // Style methods
    QColor getBackgroundColor() const;
    QColor getTextColor() const;
    QColor getBorderColor() const;
    QColor getFocusColor() const;
    QPen getBorderPen() const;
    QFont getFont() const;
    
    int getHorizontalPadding() const;
    int getVerticalPadding() const;
    int getIconTextSpacing() const;
    int getBorderWidth() const;

    // Animation methods
    void startStateTransitionAnimation(Core::FluentState from, Core::FluentState to);
    void startLoadingAnimation();
    void stopLoadingAnimation();
    void startRevealAnimation(const QPoint& center);
    void createBackgroundAnimation();

    // Utility methods
    void updateGeometry();
    void updateAccessibility();
    bool isPressed() const { return m_pressed; }

private:
    // Content
    QString m_text;
    QIcon m_icon;
    FluentIconPosition m_iconPosition{FluentIconPosition::Left};

    // Style
    FluentButtonStyle m_buttonStyle{FluentButtonStyle::Default};
    FluentButtonSize m_buttonSize{FluentButtonSize::Medium};
    bool m_flat{false};

    // State
    bool m_loading{false};
    bool m_checkable{false};
    bool m_checked{false};
    bool m_pressed{false};
    bool m_spacePressedOnButton{false};

    // Animation properties
    qreal m_backgroundOpacity{1.0};
    qreal m_loadingRotation{0.0};
    qreal m_revealProgress{0.0};
    QPoint m_revealCenter;

    // Animations
    std::unique_ptr<QPropertyAnimation> m_backgroundAnimation;
    std::unique_ptr<QPropertyAnimation> m_loadingAnimation;
    std::unique_ptr<QSequentialAnimationGroup> m_revealAnimation;
    std::unique_ptr<QTimer> m_clickTimer;

    // Cached values for performance
    mutable QSize m_cachedSizeHint;
    mutable bool m_sizeHintValid{false};
};

} // namespace FluentQt::Components