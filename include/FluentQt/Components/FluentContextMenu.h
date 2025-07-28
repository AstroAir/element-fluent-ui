// include/FluentQt/Components/FluentContextMenu.h
#pragma once

#include "FluentQt/Core/FluentComponent.h"
#include "FluentQt/Animation/FluentAnimator.h"
#include <QMenu>
#include <QAction>
#include <QWidgetAction>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QVBoxLayout>
#include <QIcon>

namespace FluentQt::Components {

// Fluent Design System spacing constants
namespace FluentSpacing {
    constexpr int XSmall = 4;
    constexpr int Small = 8;
    constexpr int Medium = 12;
    constexpr int Large = 16;
    constexpr int XLarge = 20;
    constexpr int XXLarge = 24;
}

// Fluent Design System typography constants
namespace FluentTypography {
    constexpr int CaptionFontSize = 12;
    constexpr int BodyFontSize = 14;
    constexpr int SubtitleFontSize = 16;
    constexpr int TitleFontSize = 20;
    constexpr int LargeTitleFontSize = 28;
}

enum class FluentMenuSeparatorType {
    Line,
    Space,
    Header
};

enum class FluentMenuItemType {
    Action,
    Toggle,
    Radio,
    Separator,
    Submenu
};

enum class FluentMenuItemState {
    Normal,
    Hovered,
    Pressed,
    Disabled,
    Focused
};

class FluentMenuSeparator : public QWidget {
    Q_OBJECT

public:
    explicit FluentMenuSeparator(FluentMenuSeparatorType type = FluentMenuSeparatorType::Line, 
                                QWidget* parent = nullptr);
    
    void setText(const QString& text);  // For header type
    void setSeparatorType(FluentMenuSeparatorType type);

protected:
    void paintEvent(QPaintEvent* event) override;
    QSize sizeHint() const override;

private:
    FluentMenuSeparatorType m_type;
    QString m_text;
};

class FluentMenuItem : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal hoverOpacity READ hoverOpacity WRITE setHoverOpacity)
    Q_PROPERTY(QColor currentBackgroundColor READ currentBackgroundColor WRITE setCurrentBackgroundColor)
    Q_PROPERTY(qreal focusOpacity READ focusOpacity WRITE setFocusOpacity)

public:
    explicit FluentMenuItem(QAction* action, QWidget* parent = nullptr);
    explicit FluentMenuItem(FluentMenuItemType type, QWidget* parent = nullptr);

    QAction* action() const { return m_action; }
    void setAction(QAction* action);

    // Basic properties
    void setIcon(const QIcon& icon);
    void setText(const QString& text);
    void setShortcut(const QKeySequence& shortcut);
    void setSubMenu(QMenu* menu);

    // Checkable/Toggle functionality
    bool isCheckable() const;
    void setCheckable(bool checkable);

    bool isChecked() const;
    void setChecked(bool checked);

    bool isEnabled() const;
    void setEnabled(bool enabled);

    // Menu item type and state
    FluentMenuItemType itemType() const { return m_itemType; }
    void setItemType(FluentMenuItemType type);

    FluentMenuItemState itemState() const { return m_itemState; }
    void setItemState(FluentMenuItemState state);

    // Radio group functionality
    void setRadioGroup(const QString& groupName);
    QString radioGroup() const { return m_radioGroup; }

    // Animation properties
    void setHoverAnimationEnabled(bool enabled);
    bool isHoverAnimationEnabled() const { return m_hoverAnimationEnabled; }

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    QSize sizeHint() const override;

signals:
    void triggered();
    void hovered();

private:
    void updateFromAction();
    void setupHoverAnimations();
    void startHoverAnimation(bool hovered);
    void paintMenuItem(QPainter& painter, const QRect& rect);
    void paintIcon(QPainter& painter, const QRect& rect);
    void paintText(QPainter& painter, const QRect& rect);
    void paintShortcut(QPainter& painter, const QRect& rect);
    void paintArrow(QPainter& painter, const QRect& rect);
    void paintCheckmark(QPainter& painter, const QRect& rect);

    // Property accessors for animations
    qreal hoverOpacity() const { return m_hoverOpacity; }
    void setHoverOpacity(qreal opacity);

    QColor currentBackgroundColor() const { return m_currentBackgroundColor; }
    void setCurrentBackgroundColor(const QColor& color);

    qreal focusOpacity() const { return m_focusOpacity; }
    void setFocusOpacity(qreal opacity);

    // Focus indicator methods
    void setShowFocusIndicator(bool show);
    bool showFocusIndicator() const { return m_showFocusIndicator; }
    void startFocusAnimation(bool focused);

private:
    QAction* m_action;
    FluentMenuItemType m_itemType{FluentMenuItemType::Action};
    FluentMenuItemState m_itemState{FluentMenuItemState::Normal};
    QString m_radioGroup;
    bool m_hovered{false};
    bool m_pressed{false};
    bool m_hoverAnimationEnabled{true};

    // Animation properties
    std::unique_ptr<QPropertyAnimation> m_hoverAnimation;
    std::unique_ptr<QPropertyAnimation> m_backgroundAnimation;
    std::unique_ptr<QPropertyAnimation> m_focusAnimation;
    QColor m_currentBackgroundColor;
    qreal m_hoverOpacity{0.0};
    qreal m_focusOpacity{0.0};
    bool m_showFocusIndicator{true};
};

class FluentContextMenu : public QMenu {
    Q_OBJECT

public:
    explicit FluentContextMenu(QWidget* parent = nullptr);
    
    // Action management
    QAction* addAction(const QString& text);
    QAction* addAction(const QIcon& icon, const QString& text);
    QAction* addAction(const QString& text, const QObject* receiver, const char* member);
    QAction* addAction(const QIcon& icon, const QString& text, const QObject* receiver, const char* member);
    
    template<typename Func>
    QAction* addAction(const QString& text, const Func& slot) {
        auto* action = new QAction(text, this);
        connect(action, &QAction::triggered, slot);
        QMenu::addAction(action);
        return action;
    }

    template<typename Func>
    QAction* addAction(const QIcon& icon, const QString& text, const Func& slot) {
        auto* action = new QAction(icon, text, this);
        connect(action, &QAction::triggered, slot);
        QMenu::addAction(action);
        return action;
    }
    
    QAction* insertAction(QAction* before, QAction* action);
    void removeAction(QAction* action);
    
    // Fluent-specific actions
    FluentMenuItem* addFluentAction(const QString& text, const QIcon& icon = QIcon());
    FluentMenuItem* addFluentAction(const QString& text, const QIcon& icon, const std::function<void()>& callback);

    // Toggle and radio items
    FluentMenuItem* addToggleAction(const QString& text, const QIcon& icon = QIcon(), bool checked = false);
    FluentMenuItem* addRadioAction(const QString& text, const QString& group, const QIcon& icon = QIcon(), bool checked = false);

    // Checkable actions
    FluentMenuItem* addCheckableAction(const QString& text, const QIcon& icon = QIcon(), bool checked = false);

    // Separators
    QAction* addSeparator();
    QAction* addSeparator(const QString& text);  // Header separator
    QAction* insertSeparator(QAction* before);

    // Submenus
    FluentContextMenu* addMenu(const QString& title);
    FluentContextMenu* addMenu(const QIcon& icon, const QString& title);
    FluentContextMenu* addFluentSubmenu(const QString& title, const QIcon& icon = QIcon());
    FluentContextMenu* insertMenu(QAction* before, FluentContextMenu* menu);

    // Enhanced submenu positioning
    void setSubmenuDelay(int milliseconds);
    int submenuDelay() const { return m_submenuDelay; }

    void setSubmenuOverlap(int pixels);
    int submenuOverlap() const { return m_submenuOverlap; }

    // Cascading menu management
    void showSubmenuAt(FluentContextMenu* submenu, const QPoint& position);
    void hideAllSubmenus();
    QList<FluentContextMenu*> activeSubmenus() const;
    
    // Widget actions
    QWidgetAction* addWidget(QWidget* widget);
    QWidgetAction* insertWidget(QAction* before, QWidget* widget);
    
    // Menu operations
    void popup(const QPoint& pos);
    void exec(const QPoint& pos);
    void hide();
    
    // Properties
    QList<QAction*> actions() const;
    void clear();
    bool isEmpty() const;
    
    // Appearance
    void setMinimumWidth(int width);
    void setMaximumWidth(int width);
    void setTitle(const QString& title);
    QString title() const;
    
    // Animation
    void setAnimationEnabled(bool enabled);
    bool isAnimationEnabled() const;
    void setAnimationDuration(int duration);
    int animationDuration() const { return m_animationDuration; }

    // Enhanced animation types
    void setEntranceAnimation(Animation::FluentAnimationType type);
    Animation::FluentAnimationType entranceAnimation() const { return m_entranceAnimationType; }

    void setExitAnimation(Animation::FluentAnimationType type);
    Animation::FluentAnimationType exitAnimation() const { return m_exitAnimationType; }

    void setHoverAnimationEnabled(bool enabled);
    bool isHoverAnimationEnabled() const { return m_hoverAnimationEnabled; }

    // Shadow
    void setShadowEnabled(bool enabled);
    bool isShadowEnabled() const { return m_shadowEnabled; }

    // Rounded corners
    void setRoundedCorners(bool rounded);
    bool hasRoundedCorners() const { return m_roundedCorners; }

signals:
    void aboutToShow();
    void aboutToHide();
    void triggered(QAction* action);
    void hovered(QAction* action);

protected:
    void paintEvent(QPaintEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void leaveEvent(QEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void onActionTriggered();
    void onActionHovered();
    void onSubMenuAboutToShow();
    void onAnimationFinished();

private:
    void setupUI();
    void setupAnimation();
    void setupShadow();
    void updateTheme();
    void setupLayout();
    void setupShadowEffect();
    void updateMenuItems();
    void showSubMenu(FluentContextMenu* subMenu, const QPoint& pos);
    void hideSubMenu();
    void selectAction(QAction* action);
    void activateAction(QAction* action);
    void animateShow();
    void animateHide();

    // Submenu positioning
    QPoint calculateSubmenuPosition(FluentContextMenu* submenu, const QPoint& triggerPos);
    
    QAction* actionAt(const QPoint& pos) const;
    QRect actionGeometry(QAction* action) const;
    void updateActionRects();
    
    void paintMenuBackground(QPainter& painter, const QRect& rect);
    void paintMenuBorder(QPainter& painter, const QRect& rect);
    void paintMenuTitle(QPainter& painter, const QRect& rect);

private:
    QVBoxLayout* m_layout;
    QString m_title;
    QList<QAction*> m_actions;
    QMap<QAction*, FluentMenuItem*> m_menuItems;
    QMap<QAction*, QRect> m_actionRects;
    
    QAction* m_selectedAction{nullptr};
    FluentContextMenu* m_activeSubMenu{nullptr};
    QList<FluentContextMenu*> m_activeSubmenus;

    // Submenu configuration
    int m_submenuDelay{300};  // milliseconds
    int m_submenuOverlap{4};  // pixels

    bool m_animationEnabled{true};
    int m_animationDuration{200};
    Animation::FluentAnimationType m_entranceAnimationType{Animation::FluentAnimationType::Fade};
    Animation::FluentAnimationType m_exitAnimationType{Animation::FluentAnimationType::Fade};
    bool m_hoverAnimationEnabled{true};
    bool m_shadowEnabled{true};
    bool m_roundedCorners{true};
    std::unique_ptr<QPropertyAnimation> m_showAnimation;
    QPropertyAnimation* m_fadeAnimation;
    std::unique_ptr<QPropertyAnimation> m_hideAnimation;
    std::unique_ptr<QGraphicsDropShadowEffect> m_shadowEffect;

    int m_minimumWidth{120};
    int m_maximumWidth{300};
};



} // namespace FluentQt::Components