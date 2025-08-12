// include/FluentQt/Components/FluentTabView.h
#pragma once

#include <QBoxLayout>
#include <QHBoxLayout>
#include <QMenu>
#include <QPropertyAnimation>
#include <QScrollArea>
#include <QStackedWidget>
#include <QTabBar>
#include <QTabWidget>
#include <QVBoxLayout>
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

enum class FluentTabPosition { Top, Bottom, Left, Right };

enum class FluentTabStyle { Standard, Pill, Underline, Compact };

enum class FluentTabCloseMode { Never, Always, OnHover, OnSelection };

struct FluentTabData {
    QString text;
    QString tooltip;
    QIcon icon;
    QWidget* content{nullptr};
    bool closeable{true};
    bool pinned{false};
    QVariantMap userData;
};

class FluentTabButton : public FluentButton {
    Q_OBJECT

public:
    explicit FluentTabButton(const FluentTabData& data,
                             QWidget* parent = nullptr);

    void setTabData(const FluentTabData& data);
    FluentTabData tabData() const { return m_tabData; }

    void setSelected(bool selected);
    bool isSelected() const { return m_selected; }

    void setPinned(bool pinned);
    bool isPinned() const { return m_tabData.pinned; }

protected:
    void paintEvent(QPaintEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

signals:
    void closeRequested();
    void pinRequested(bool pinned);
    void contextMenuRequested(const QPoint& pos);

private:
    void updateButtonAppearance();
    void setupContextMenu();

private:
    FluentTabData m_tabData;
    bool m_selected{false};
    QMenu* m_contextMenu{nullptr};
};

class FluentTabView : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(FluentTabPosition tabPosition READ tabPosition WRITE
                   setTabPosition NOTIFY tabPositionChanged)
    Q_PROPERTY(FluentTabStyle tabStyle READ tabStyle WRITE setTabStyle NOTIFY
                   tabStyleChanged)
    Q_PROPERTY(FluentTabCloseMode closeMode READ closeMode WRITE setCloseMode)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY
                   currentChanged)
    Q_PROPERTY(bool tabsClosable READ tabsClosable WRITE setTabsClosable)
    Q_PROPERTY(bool tabsMovable READ tabsMovable WRITE setTabsMovable)
    Q_PROPERTY(bool scrollable READ scrollable WRITE setScrollable)
    Q_PROPERTY(bool showAddButton READ showAddButton WRITE setShowAddButton)

public:
    explicit FluentTabView(QWidget* parent = nullptr);

    // Position and style
    FluentTabPosition tabPosition() const noexcept { return m_tabPosition; }
    void setTabPosition(FluentTabPosition position);

    FluentTabStyle tabStyle() const noexcept { return m_tabStyle; }
    void setTabStyle(FluentTabStyle style);

    FluentTabCloseMode closeMode() const noexcept { return m_closeMode; }
    void setCloseMode(FluentTabCloseMode mode);

    // Current tab
    int currentIndex() const noexcept { return m_currentIndex; }
    void setCurrentIndex(int index);

    QWidget* currentWidget() const;
    void setCurrentWidget(QWidget* widget);

    // Tab management
    bool tabsClosable() const noexcept { return m_tabsClosable; }
    void setTabsClosable(bool closable);

    bool tabsMovable() const noexcept { return m_tabsMovable; }
    void setTabsMovable(bool movable);

    bool scrollable() const noexcept { return m_scrollable; }
    void setScrollable(bool scrollable);

    bool showAddButton() const noexcept { return m_showAddButton; }
    void setShowAddButton(bool show);

    // Tab operations
    int addTab(const QString& text, QWidget* widget = nullptr);
    int addTab(const QIcon& icon, const QString& text,
               QWidget* widget = nullptr);
    int addTab(const FluentTabData& data);

    int insertTab(int index, const QString& text, QWidget* widget = nullptr);
    int insertTab(int index, const FluentTabData& data);

    void removeTab(int index);
    void moveTab(int from, int to);

    // Tab properties
    int count() const;
    QString tabText(int index) const;
    void setTabText(int index, const QString& text);

    QIcon tabIcon(int index) const;
    void setTabIcon(int index, const QIcon& icon);

    QString tabToolTip(int index) const;
    void setTabToolTip(int index, const QString& tooltip);

    bool isTabClosable(int index) const;
    void setTabClosable(int index, bool closable);

    bool isTabPinned(int index) const;
    void setTabPinned(int index, bool pinned);

    QWidget* widget(int index) const;
    void setWidget(int index, QWidget* widget);

    // Tab finding
    int indexOf(QWidget* widget) const;
    int findTab(const QString& text) const;

    // Batch operations
    void closeAllTabs();
    void closeOtherTabs(int index);
    void closeTabsToTheRight(int index);
    void closeTabsToTheLeft(int index);
    void closeUnpinnedTabs();

    // Session management
    QVariantMap saveSession() const;
    void restoreSession(const QVariantMap& session);

signals:
    void tabPositionChanged(FluentTabPosition position);
    void tabStyleChanged(FluentTabStyle style);
    void currentChanged(int index);
    void tabCloseRequested(int index);
    void tabMoved(int from, int to);
    void tabBarClicked(int index);
    void tabBarDoubleClicked(int index);
    void addTabRequested();
    void tabPinned(int index, bool pinned);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private slots:
    void onTabButtonClicked();
    void onTabCloseRequested();
    void onTabPinRequested(bool pinned);
    void onAddButtonClicked();
    void updateTabLayout();

private:
    void setupTabBar();
    void setupContentArea();
    void updateTabButton(int index);
    void updateTabPositions();
    void scrollToTab(int index);
    void animateTabTransition(int from, int to);
    FluentTabButton* createTabButton(const FluentTabData& data);

private:
    QBoxLayout* m_mainLayout;
    QWidget* m_tabBar;
    QScrollArea* m_tabScrollArea;
    QHBoxLayout* m_tabBarLayout;
    QStackedWidget* m_contentStack;
    FluentButton* m_addButton;

    FluentTabPosition m_tabPosition{FluentTabPosition::Top};
    FluentTabStyle m_tabStyle{FluentTabStyle::Standard};
    FluentTabCloseMode m_closeMode{FluentTabCloseMode::OnHover};

    int m_currentIndex{-1};
    bool m_tabsClosable{true};
    bool m_tabsMovable{true};
    bool m_scrollable{true};
    bool m_showAddButton{false};

    std::vector<FluentTabData> m_tabData;
    std::vector<FluentTabButton*> m_tabButtons;

    std::unique_ptr<QPropertyAnimation> m_scrollAnimation;
    std::unique_ptr<QPropertyAnimation> m_transitionAnimation;
};

}  // namespace FluentQt::Components
