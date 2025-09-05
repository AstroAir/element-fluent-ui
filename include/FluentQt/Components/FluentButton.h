// include/FluentQt/Components/FluentButton.h
#pragma once

#include <QAccessible>
#include <QFontMetrics>
#include <QGraphicsEffect>
#include <QIcon>
#include <QKeySequence>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QSequentialAnimationGroup>
#include <QTimer>
#include <memory>
#include <optional>
#include "FluentQt/Core/FluentComponent.h"
#include "FluentQt/Styling/FluentDesignTokenUtils.h"

namespace FluentQt::Components {

// Dirty region tracking for paint optimization
enum class FluentButtonDirtyRegion {
    None = 0x00,
    Background = 0x01,
    Border = 0x02,
    Content = 0x04,
    All = Background | Border | Content
};
Q_DECLARE_FLAGS(FluentButtonDirtyRegions, FluentButtonDirtyRegion)
Q_DECLARE_OPERATORS_FOR_FLAGS(FluentQt::Components::FluentButtonDirtyRegions)

enum class FluentButtonStyle {
    Default,
    Primary,
    Accent,
    Subtle,
    Outline,
    Hyperlink,
    Text,
    Toggle,
    Icon,
    Split
};

enum class FluentButtonSize { Small, Medium, Large, ExtraLarge };

enum class FluentIconPosition { Left, Right, Top, Bottom };

class FluentButton : public Core::FluentComponent {
    Q_OBJECT
    Q_ENUM(FluentButtonStyle)
    Q_ENUM(FluentButtonSize)
    Q_ENUM(FluentIconPosition)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(FluentButtonStyle buttonStyle READ buttonStyle WRITE
                   setButtonStyle NOTIFY styleChanged)
    Q_PROPERTY(FluentButtonSize buttonSize READ buttonSize WRITE setButtonSize
                   NOTIFY sizeChanged)
    Q_PROPERTY(FluentIconPosition iconPosition READ iconPosition WRITE
                   setIconPosition NOTIFY iconPositionChanged)
    Q_PROPERTY(
        bool loading READ isLoading WRITE setLoading NOTIFY loadingChanged)
    Q_PROPERTY(bool checkable READ isCheckable WRITE setCheckable)
    Q_PROPERTY(
        bool checked READ isChecked WRITE setChecked NOTIFY checkedChanged)

    // Animation properties
    Q_PROPERTY(qreal backgroundOpacity READ backgroundOpacity WRITE
                   setBackgroundOpacity)
    Q_PROPERTY(
        qreal loadingRotation READ loadingRotation WRITE setLoadingRotation)
    Q_PROPERTY(qreal revealProgress READ revealProgress WRITE setRevealProgress)
    Q_PROPERTY(bool flat READ isFlat WRITE setFlat)

public:
    explicit FluentButton(QWidget* parent = nullptr);
    explicit FluentButton(const QString& text, QWidget* parent = nullptr);
    explicit FluentButton(const QIcon& icon, const QString& text,
                          QWidget* parent = nullptr);
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

    // Shortcut key support
    QKeySequence shortcut() const noexcept { return m_shortcut; }
    void setShortcut(const QKeySequence& shortcut);
    void setShortcut(QKeySequence::StandardKey key);

    // Enhanced accessibility
    QString accessibleName() const;
    void setAccessibleName(const QString& name);
    QString accessibleDescription() const;
    void setAccessibleDescription(const QString& description);
    QAccessible::Role accessibleRole() const;
    void setAccessibleRole(QAccessible::Role role);

    // Animation properties
    qreal backgroundOpacity() const noexcept { return m_backgroundOpacity; }
    void setBackgroundOpacity(qreal opacity);

    qreal loadingRotation() const noexcept { return m_loadingRotation; }
    void setLoadingRotation(qreal rotation);

    qreal revealProgress() const noexcept { return m_revealProgress; }
    void setRevealProgress(qreal progress);

    // Design token integration
    bool useDesignTokens() const noexcept { return m_useDesignTokens; }
    void setUseDesignTokens(bool use);
    void refreshFromDesignTokens();

    // Token-based styling methods
    QColor getTokenColor(const QString& tokenName) const;
    QFont getTokenFont(const QString& tokenName) const;
    int getTokenSize(const QString& tokenName) const;
    int getTokenSpacing(const QString& tokenName) const;

    // Size calculations
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // Convenience static methods
    static FluentButton* createPrimaryButton(const QString& text,
                                             QWidget* parent = nullptr);
    static FluentButton* createAccentButton(const QString& text,
                                            QWidget* parent = nullptr);
    static FluentButton* createSubtleButton(const QString& text,
                                            QWidget* parent = nullptr);
    static FluentButton* createTextButton(const QString& text,
                                          QWidget* parent = nullptr);
    static FluentButton* createOutlineButton(const QString& text,
                                             QWidget* parent = nullptr);
    static FluentButton* createIconButton(const QIcon& icon,
                                          QWidget* parent = nullptr);
    static FluentButton* createToggleButton(const QString& text,
                                            QWidget* parent = nullptr);
    static FluentButton* createSplitButton(const QString& text,
                                           const QIcon& dropdownIcon = QIcon(),
                                           QWidget* parent = nullptr);

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
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void changeEvent(QEvent* event) override;

    // State management
    void updateStateStyle() override;
    void performStateTransition(Core::FluentState from,
                                Core::FluentState to) override;

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
    void startStateTransitionAnimation(Core::FluentState from,
                                       Core::FluentState to);
    void startLoadingAnimation();
    void stopLoadingAnimation();
    void startRevealAnimation(const QPoint& center);
    void createBackgroundAnimation();

    // Utility methods
    void updateGeometry();
    void updateAccessibility();
    bool isPressed() const { return m_pressed; }

    // Cache management methods
    void invalidateCache(FluentButtonDirtyRegions regions = {}) const;
    void updateCacheIfNeeded() const;
    QString generateStyleKey() const;

    // Internal animation methods
    void animateClickVisual();

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

    // Shortcut and accessibility
    QKeySequence m_shortcut;
    QString m_accessibleName;
    QString m_accessibleDescription;
    QAccessible::Role m_accessibleRole{QAccessible::Button};

    // Design token integration
    bool m_useDesignTokens{true};

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

    // Paint caching for performance optimization
    mutable QPixmap m_cachedBackground;
    mutable QPixmap m_cachedBorder;
    mutable QPixmap m_cachedContent;
    mutable QRect m_cachedRect;
    mutable QString m_cachedStyleKey;
    mutable bool m_backgroundCacheValid{false};
    mutable bool m_borderCacheValid{false};
    mutable bool m_contentCacheValid{false};

    // Dirty region tracking
    mutable FluentButtonDirtyRegions m_dirtyRegions{
        FluentButtonDirtyRegion::All};
};

}  // namespace FluentQt::Components
