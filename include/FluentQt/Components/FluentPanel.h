// include/FluentQt/Components/FluentPanel.h
#pragma once

#include <QAccessible>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMap>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QStackedLayout>
#include <QVBoxLayout>
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

enum class FluentPanelType {
    Card,     // Elevated card with shadow
    Surface,  // Flat surface with border
    Acrylic,  // Translucent acrylic effect
    Mica,     // Mica material effect
    Sidebar,  // Navigation sidebar panel
    Content,  // Main content area
    Dialog,   // Modal dialog panel
    Flyout    // Popup flyout panel
};

enum class FluentPanelElevation {
    None,     // No elevation (0dp)
    Low,      // Subtle elevation (2dp)
    Medium,   // Standard elevation (4dp)
    High,     // Prominent elevation (8dp)
    VeryHigh  // Maximum elevation (16dp)
};

enum class FluentPanelCornerRadius {
    None,       // No rounded corners
    Small,      // 4px radius
    Medium,     // 8px radius
    Large,      // 12px radius
    ExtraLarge  // 16px radius
};

class FluentPanel : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(FluentPanelType panelType READ panelType WRITE setPanelType
                   NOTIFY panelTypeChanged)
    Q_PROPERTY(FluentPanelElevation elevation READ elevation WRITE setElevation
                   NOTIFY elevationChanged)
    Q_PROPERTY(FluentPanelCornerRadius cornerRadius READ cornerRadius WRITE
                   setCornerRadius NOTIFY cornerRadiusChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE
                   setBackgroundColor NOTIFY backgroundColorChanged)
    Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor NOTIFY
                   borderColorChanged)
    Q_PROPERTY(int borderWidth READ borderWidth WRITE setBorderWidth NOTIFY
                   borderWidthChanged)
    Q_PROPERTY(
        QMargins padding READ padding WRITE setPadding NOTIFY paddingChanged)
    Q_PROPERTY(
        bool animated READ isAnimated WRITE setAnimated NOTIFY animatedChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(bool collapsible READ isCollapsible WRITE setCollapsible NOTIFY
                   collapsibleChanged)
    Q_PROPERTY(bool collapsed READ isCollapsed WRITE setCollapsed NOTIFY
                   collapsedChanged)

    // Accessibility properties
    Q_PROPERTY(QString accessibleName READ accessibleName WRITE
                   setAccessibleName NOTIFY accessibleNameChanged)
    Q_PROPERTY(QString accessibleDescription READ accessibleDescription WRITE
                   setAccessibleDescription NOTIFY accessibleDescriptionChanged)
    Q_PROPERTY(QString accessibleRole READ accessibleRole WRITE
                   setAccessibleRole NOTIFY accessibleRoleChanged)
    Q_PROPERTY(bool keyboardNavigationEnabled READ isKeyboardNavigationEnabled
                   WRITE setKeyboardNavigationEnabled NOTIFY
                       keyboardNavigationEnabledChanged)
    Q_PROPERTY(bool screenReaderSupport READ hasScreenReaderSupport WRITE
                   setScreenReaderSupport NOTIFY screenReaderSupportChanged)
    Q_PROPERTY(
        int tabOrder READ tabOrder WRITE setTabOrder NOTIFY tabOrderChanged)

    // Responsive design properties
    Q_PROPERTY(bool responsiveEnabled READ isResponsiveEnabled WRITE
                   setResponsiveEnabled NOTIFY responsiveEnabledChanged)
    Q_PROPERTY(QSize minimumContentSize READ minimumContentSize WRITE
                   setMinimumContentSize NOTIFY minimumContentSizeChanged)
    Q_PROPERTY(QSize maximumContentSize READ maximumContentSize WRITE
                   setMaximumContentSize NOTIFY maximumContentSizeChanged)
    Q_PROPERTY(bool adaptiveLayout READ hasAdaptiveLayout WRITE
                   setAdaptiveLayout NOTIFY adaptiveLayoutChanged)
    Q_PROPERTY(bool touchOptimized READ isTouchOptimized WRITE setTouchOptimized
                   NOTIFY touchOptimizedChanged)
    Q_PROPERTY(int breakpointWidth READ breakpointWidth WRITE setBreakpointWidth
                   NOTIFY breakpointWidthChanged)

    // Enhanced visual state properties
    Q_PROPERTY(bool focusIndicatorVisible READ isFocusIndicatorVisible WRITE
                   setFocusIndicatorVisible NOTIFY focusIndicatorVisibleChanged)
    Q_PROPERTY(QColor focusIndicatorColor READ focusIndicatorColor WRITE
                   setFocusIndicatorColor NOTIFY focusIndicatorColorChanged)
    Q_PROPERTY(int focusIndicatorWidth READ focusIndicatorWidth WRITE
                   setFocusIndicatorWidth NOTIFY focusIndicatorWidthChanged)
    Q_PROPERTY(bool hoverEffectsEnabled READ areHoverEffectsEnabled WRITE
                   setHoverEffectsEnabled NOTIFY hoverEffectsEnabledChanged)
    Q_PROPERTY(
        bool pressAnimationsEnabled READ arePressAnimationsEnabled WRITE
            setPressAnimationsEnabled NOTIFY pressAnimationsEnabledChanged)
    Q_PROPERTY(qreal hoverOpacity READ hoverOpacity WRITE setHoverOpacity NOTIFY
                   hoverOpacityChanged)
    Q_PROPERTY(qreal pressedScale READ pressedScale WRITE setPressedScale NOTIFY
                   pressedScaleChanged)

    // Keyboard navigation properties
    Q_PROPERTY(
        bool keyboardShortcutsEnabled READ areKeyboardShortcutsEnabled WRITE
            setKeyboardShortcutsEnabled NOTIFY keyboardShortcutsEnabledChanged)
    Q_PROPERTY(QString expandShortcut READ expandShortcut WRITE
                   setExpandShortcut NOTIFY expandShortcutChanged)
    Q_PROPERTY(QString collapseShortcut READ collapseShortcut WRITE
                   setCollapseShortcut NOTIFY collapseShortcutChanged)
    Q_PROPERTY(bool cycleFocusEnabled READ isCycleFocusEnabled WRITE
                   setCycleFocusEnabled NOTIFY cycleFocusEnabledChanged)

public:
    explicit FluentPanel(QWidget* parent = nullptr);
    explicit FluentPanel(FluentPanelType type, QWidget* parent = nullptr);
    explicit FluentPanel(const QString& title, QWidget* parent = nullptr);
    ~FluentPanel() override;

    // Panel type and appearance
    FluentPanelType panelType() const;
    void setPanelType(FluentPanelType type);

    FluentPanelElevation elevation() const;
    void setElevation(FluentPanelElevation elevation);

    FluentPanelCornerRadius cornerRadius() const;
    void setCornerRadius(FluentPanelCornerRadius radius);

    // Colors and styling
    QColor backgroundColor() const;
    void setBackgroundColor(const QColor& color);

    QColor borderColor() const;
    void setBorderColor(const QColor& color);

    int borderWidth() const;
    void setBorderWidth(int width);

    QMargins padding() const;
    void setPadding(const QMargins& padding);
    void setPadding(int padding);
    void setPadding(int left, int top, int right, int bottom);

    // Animation
    bool isAnimated() const;
    void setAnimated(bool animated);

    // Title and collapsing
    QString title() const;
    void setTitle(const QString& title);

    bool isCollapsible() const;
    void setCollapsible(bool collapsible);

    bool isCollapsed() const;
    void setCollapsed(bool collapsed);

    // Accessibility methods
    QString accessibleName() const;
    void setAccessibleName(const QString& name);

    QString accessibleDescription() const;
    void setAccessibleDescription(const QString& description);

    QString accessibleRole() const;
    void setAccessibleRole(const QString& role);

    bool isKeyboardNavigationEnabled() const;
    void setKeyboardNavigationEnabled(bool enabled);

    bool hasScreenReaderSupport() const;
    void setScreenReaderSupport(bool enabled);

    int tabOrder() const;
    void setTabOrder(int order);

    // Keyboard navigation methods
    void setFocusToContent();
    void setFocusToTitle();
    bool handleKeyboardNavigation(QKeyEvent* event);

    // Responsive design methods
    bool isResponsiveEnabled() const;
    void setResponsiveEnabled(bool enabled);

    QSize minimumContentSize() const;
    void setMinimumContentSize(const QSize& size);

    QSize maximumContentSize() const;
    void setMaximumContentSize(const QSize& size);

    bool hasAdaptiveLayout() const;
    void setAdaptiveLayout(bool adaptive);

    bool isTouchOptimized() const;
    void setTouchOptimized(bool optimized);

    int breakpointWidth() const;
    void setBreakpointWidth(int width);

    // Responsive behavior methods
    void updateResponsiveLayout();
    bool isCompactMode() const;
    void setCompactMode(bool compact);

    // Enhanced visual state methods
    bool isFocusIndicatorVisible() const;
    void setFocusIndicatorVisible(bool visible);

    QColor focusIndicatorColor() const;
    void setFocusIndicatorColor(const QColor& color);

    int focusIndicatorWidth() const;
    void setFocusIndicatorWidth(int width);

    bool areHoverEffectsEnabled() const;
    void setHoverEffectsEnabled(bool enabled);

    bool arePressAnimationsEnabled() const;
    void setPressAnimationsEnabled(bool enabled);

    qreal hoverOpacity() const;
    void setHoverOpacity(qreal opacity);

    qreal pressedScale() const;
    void setPressedScale(qreal scale);

    // Visual effect methods
    void triggerHoverEffect();
    void triggerPressEffect();
    void triggerFocusEffect();
    void resetVisualEffects();

    // Enhanced keyboard navigation methods
    bool areKeyboardShortcutsEnabled() const;
    void setKeyboardShortcutsEnabled(bool enabled);

    QString expandShortcut() const;
    void setExpandShortcut(const QString& shortcut);

    QString collapseShortcut() const;
    void setCollapseShortcut(const QString& shortcut);

    bool isCycleFocusEnabled() const;
    void setCycleFocusEnabled(bool enabled);

    // Advanced keyboard navigation
    void registerKeyboardShortcut(const QString& key, const QString& action);
    void unregisterKeyboardShortcut(const QString& key);
    bool handleKeyboardShortcut(const QString& key);
    void cycleFocus(bool forward = true);
    QWidget* getNextFocusableWidget(bool forward = true) const;

    // Layout management
    void setLayout(QLayout* layout);
    QLayout* layout() const;

    // Convenience layout creators
    QVBoxLayout* createVerticalLayout();
    QHBoxLayout* createHorizontalLayout();
    QGridLayout* createGridLayout();
    QStackedLayout* createStackedLayout();

    // Content area access
    QWidget* contentWidget() const;
    void setContentWidget(QWidget* widget);

    // Size hints
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void expand();
    void collapse();
    void toggleCollapsed();
    void animateIn();
    void animateOut();

signals:
    void panelTypeChanged(FluentPanelType type);
    void elevationChanged(FluentPanelElevation elevation);
    void cornerRadiusChanged(FluentPanelCornerRadius radius);
    void backgroundColorChanged(const QColor& color);
    void borderColorChanged(const QColor& color);
    void borderWidthChanged(int width);
    void paddingChanged(const QMargins& padding);
    void animatedChanged(bool animated);
    void titleChanged(const QString& title);
    void collapsibleChanged(bool collapsible);
    void collapsedChanged(bool collapsed);
    void expandRequested();
    void collapseRequested();
    void clicked();

    // Accessibility signals
    void accessibleNameChanged(const QString& name);
    void accessibleDescriptionChanged(const QString& description);
    void accessibleRoleChanged(const QString& role);
    void keyboardNavigationEnabledChanged(bool enabled);
    void screenReaderSupportChanged(bool enabled);
    void tabOrderChanged(int order);
    void focusChanged(bool hasFocus);
    void keyboardNavigationRequested(const QString& direction);

    // Responsive design signals
    void responsiveEnabledChanged(bool enabled);
    void minimumContentSizeChanged(const QSize& size);
    void maximumContentSizeChanged(const QSize& size);
    void adaptiveLayoutChanged(bool adaptive);
    void touchOptimizedChanged(bool optimized);
    void breakpointWidthChanged(int width);
    void compactModeChanged(bool compact);

    // Enhanced visual state signals
    void focusIndicatorVisibleChanged(bool visible);
    void focusIndicatorColorChanged(const QColor& color);
    void focusIndicatorWidthChanged(int width);
    void hoverEffectsEnabledChanged(bool enabled);
    void pressAnimationsEnabledChanged(bool enabled);
    void hoverOpacityChanged(qreal opacity);
    void pressedScaleChanged(qreal scale);

    // Keyboard navigation signals
    void keyboardShortcutsEnabledChanged(bool enabled);
    void expandShortcutChanged(const QString& shortcut);
    void collapseShortcutChanged(const QString& shortcut);
    void cycleFocusEnabledChanged(bool enabled);
    void keyboardShortcutTriggered(const QString& action);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void changeEvent(QEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void updateColors();
    void onCollapseAnimationFinished();
    void onExpandAnimationFinished();

private:
    void setupUI();
    void setupAnimations();
    void updateShadowEffect();
    void updatePanelStyle();
    void updateTitleVisibility();

    void drawPanel(QPainter& painter);
    void drawBackground(QPainter& painter, const QRect& rect);
    void drawBorder(QPainter& painter, const QRect& rect);
    void drawTitle(QPainter& painter, const QRect& rect);

    QRect getTitleRect() const;
    QRect getContentRect() const;
    int getTitleHeight() const;
    int getCornerRadiusPixels() const;
    QColor getBackgroundColor() const;
    QColor getBorderColor() const;

    void startCollapseAnimation();
    void startExpandAnimation();

    // Enhanced functionality helpers
    void updateAccessibility();
    void updateResponsiveState();
    void updateVisualEffects();
    void updateKeyboardShortcuts();
    void drawFocusIndicator(QPainter& painter, const QRect& rect);
    void applyHoverEffect();
    void applyPressEffect();
    void resetToNormalState();
    bool isInCompactBreakpoint() const;
    void setupKeyboardShortcuts();
    void handleAccessibilityEvent(QAccessibleEvent* event);

private:
    FluentPanelType m_panelType{FluentPanelType::Card};
    FluentPanelElevation m_elevation{FluentPanelElevation::Medium};
    FluentPanelCornerRadius m_cornerRadius{FluentPanelCornerRadius::Medium};

    QColor m_backgroundColor;
    QColor m_borderColor;
    int m_borderWidth{1};
    QMargins m_padding{16, 16, 16, 16};
    bool m_animated{true};

    QString m_title;
    bool m_collapsible{false};
    bool m_collapsed{false};

    // Accessibility members
    QString m_accessibleName;
    QString m_accessibleDescription;
    QString m_accessibleRole{"panel"};
    bool m_keyboardNavigationEnabled{true};
    bool m_screenReaderSupport{true};
    int m_tabOrder{0};

    // Responsive design members
    bool m_responsiveEnabled{true};
    QSize m_minimumContentSize{200, 100};
    QSize m_maximumContentSize{QWIDGETSIZE_MAX, QWIDGETSIZE_MAX};
    bool m_adaptiveLayout{true};
    bool m_touchOptimized{false};
    int m_breakpointWidth{768};
    bool m_compactMode{false};

    // Enhanced visual state members
    bool m_focusIndicatorVisible{true};
    QColor m_focusIndicatorColor;
    int m_focusIndicatorWidth{2};
    bool m_hoverEffectsEnabled{true};
    bool m_pressAnimationsEnabled{true};
    qreal m_hoverOpacity{0.8};
    qreal m_pressedScale{0.98};

    // Keyboard navigation members
    bool m_keyboardShortcutsEnabled{true};
    QString m_expandShortcut{"Space"};
    QString m_collapseShortcut{"Space"};
    bool m_cycleFocusEnabled{true};
    QMap<QString, QString> m_keyboardShortcuts;

    // UI components
    QWidget* m_contentWidget{nullptr};
    QVBoxLayout* m_mainLayout{nullptr};
    QLabel* m_titleLabel{nullptr};
    QPushButton* m_collapseButton{nullptr};

    // Animation
    std::unique_ptr<Animation::FluentAnimator> m_animator;
    QPropertyAnimation* m_collapseAnimation{nullptr};
    QPropertyAnimation* m_fadeAnimation{nullptr};
    QGraphicsDropShadowEffect* m_shadowEffect{nullptr};

    // State
    bool m_isPressed{false};
    bool m_isHovered{false};
    int m_expandedHeight{0};
    int m_collapsedHeight{0};

    // Constants
    static constexpr int TITLE_HEIGHT = 40;
    static constexpr int COLLAPSE_BUTTON_SIZE = 24;
    static constexpr int ANIMATION_DURATION = 250;
};

}  // namespace FluentQt::Components
