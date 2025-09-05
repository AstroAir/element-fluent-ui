#pragma once

#include <QColor>
#include <QIcon>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QString>
#include <QToolButton>
#include <QWidget>
#include "FluentSidebar.h"

namespace FluentQt::Components {

/**
 * @brief Enhanced sidebar item widget with full Fluent UI styling
 * 
 * This widget implements a single sidebar item with proper Fluent UI design
 * system compliance, including design tokens, accessibility, and state management.
 */
class FluentSidebarItemWidget : public QToolButton {
    Q_OBJECT

public:
    explicit FluentSidebarItemWidget(const FluentSidebarItem& item,
                                     QWidget* parent = nullptr);
    ~FluentSidebarItemWidget() override = default;

    // Item management
    void updateItem(const FluentSidebarItem& item);
    const FluentSidebarItem& item() const { return m_item; }

    // Display mode
    void setCompactMode(bool compact);
    bool isCompactMode() const { return m_compactMode; }

    // State management
    void setState(FluentSidebarItemState state);
    FluentSidebarItemState state() const { return m_currentState; }

    // Badge management
    void setBadge(FluentSidebarBadgeType type, int count = 0, const QString& text = QString());
    void setBadgeColor(const QColor& color);
    void clearBadge();

    // Hierarchy support
    void setIndentLevel(int level);
    int indentLevel() const { return m_indentLevel; }
    void setExpandable(bool expandable);
    void setExpanded(bool expanded);

    // Focus and accessibility
    void setFocusIndicatorVisible(bool visible);
    bool isFocusIndicatorVisible() const { return m_focusIndicatorVisible; }

    // Design tokens
    void refreshDesignTokens();
    void setUseFluentDesignTokens(bool use);

signals:
    void itemClicked(const QString& id);
    void itemHovered(const QString& id);
    void itemFocused(const QString& id);
    void itemStateChanged(const QString& id, FluentSidebarItemState state);
    void itemExpansionToggled(const QString& id, bool expanded);

protected:
    // Event handling
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

    // Size and layout
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

private:
    void setupWidget();
    void updateLayout();
    void updateAccessibility();
    void updateTooltip();

    // Painting helpers
    void paintBackground(QPainter* painter, const QRect& rect);
    void paintSelectionIndicator(QPainter* painter, const QRect& rect);
    void paintContent(QPainter* painter, const QRect& rect);
    void paintIcon(QPainter* painter, const QRect& iconRect);
    void paintText(QPainter* painter, const QRect& textRect);
    void paintBadge(QPainter* painter, const QRect& rect);
    void paintFocusIndicator(QPainter* painter, const QRect& rect);
    void paintExpandIndicator(QPainter* painter, const QRect& rect);

    // Design token helpers
    QColor getStateColor(const QString& colorName) const;
    QFont getStateFont(const QString& fontName) const;
    int getStateSpacing(const QString& spacingName) const;
    int getBorderRadius() const;

    // Layout calculations
    QRect getIconRect(const QRect& contentRect) const;
    QRect getTextRect(const QRect& contentRect) const;
    QRect getBadgeRect(const QRect& contentRect) const;
    QRect getExpandIndicatorRect(const QRect& contentRect) const;

    // State management
    void transitionToState(FluentSidebarItemState newState);
    void updateStateColors();

private:
    FluentSidebarItem m_item;
    FluentSidebarItemState m_currentState = FluentSidebarItemState::Normal;
    bool m_compactMode = false;
    bool m_focusIndicatorVisible = true;
    bool m_useFluentDesignTokens = true;
    int m_indentLevel = 0;

    // Badge properties
    FluentSidebarBadgeType m_badgeType = FluentSidebarBadgeType::None;
    int m_badgeCount = 0;
    QString m_badgeText;
    QColor m_badgeColor;

    // Visual state
    bool m_isPressed = false;
    bool m_isHovered = false;
    bool m_hasFocus = false;

    // Design token cache
    mutable QHash<QString, QColor> m_colorCache;
    mutable QHash<QString, QFont> m_fontCache;
    mutable QHash<QString, int> m_spacingCache;
    mutable bool m_cacheValid = false;

    // Animation support (for future enhancement)
    QPropertyAnimation* m_stateAnimation = nullptr;
};

}  // namespace FluentQt::Components
