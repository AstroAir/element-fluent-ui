// include/FluentQt/Components/FluentNavigationView.h
#pragma once

#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPropertyAnimation>
#include <QSplitter>
#include <QStackedWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#include <functional>
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

enum class FluentNavigationDisplayMode { Auto, Expanded, Compact, Minimal };

enum class FluentNavigationSelectionFollowsFocus { Disabled, Enabled };

enum class FluentNavigationViewDisplayMode { Auto, Expanded, Compact, Minimal };

enum class FluentNavigationPaneDisplayMode {
    Auto,
    Left,
    Top,
    LeftCompact,
    LeftMinimal
};

struct FluentNavigationItem {
    QString text;
    QIcon icon;
    QString tag;
    QWidget* content{nullptr};
    QString tooltip;
    bool enabled{true};
    QVariant userData;

    FluentNavigationItem() = default;
    FluentNavigationItem(const QString& text, const QIcon& icon = QIcon(),
                         QWidget* content = nullptr)
        : text(text), icon(icon), tag(text), content(content) {}
};

class FluentNavigationView : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(FluentNavigationDisplayMode displayMode READ displayMode WRITE
                   setDisplayMode NOTIFY displayModeChanged)
    Q_PROPERTY(bool isPaneOpen READ isPaneOpen WRITE setPaneOpen NOTIFY
                   paneOpenChanged)
    Q_PROPERTY(QString header READ header WRITE setHeader NOTIFY headerChanged)
    Q_PROPERTY(bool isBackButtonVisible READ isBackButtonVisible WRITE
                   setBackButtonVisible)
    Q_PROPERTY(
        bool isSettingsVisible READ isSettingsVisible WRITE setSettingsVisible)
    Q_PROPERTY(QString paneTitle READ paneTitle WRITE setPaneTitle NOTIFY
                   paneTitleChanged)
    Q_PROPERTY(QWidget* footer READ footer WRITE setFooter NOTIFY footerChanged)
    Q_PROPERTY(QWidget* settingsItem READ settingsItem WRITE setSettingsItem
                   NOTIFY settingsItemChanged)
    Q_PROPERTY(
        bool isPaneToggleButtonVisible READ isPaneToggleButtonVisible WRITE
            setPaneToggleButtonVisible NOTIFY paneToggleButtonVisibleChanged)
    Q_PROPERTY(QWidget* contentFrame READ contentFrame WRITE setContentFrame)
    Q_PROPERTY(QWidget* autoSuggestBox READ autoSuggestBox WRITE
                   setAutoSuggestBox NOTIFY autoSuggestBoxChanged)
    Q_PROPERTY(int compactModeThreshold READ compactModeThreshold WRITE
                   setCompactModeThreshold)

public:
    explicit FluentNavigationView(QWidget* parent = nullptr);
    ~FluentNavigationView() override = default;

    // Display mode
    FluentNavigationDisplayMode displayMode() const { return m_displayMode; }
    void setDisplayMode(FluentNavigationDisplayMode mode);

    // Pane state
    bool isPaneOpen() const { return m_isPaneOpen; }
    void setPaneOpen(bool open);
    void setIsPaneOpen(bool open);

    // Pane display mode
    FluentNavigationPaneDisplayMode paneDisplayMode() const {
        return m_paneDisplayMode;
    }
    void setPaneDisplayMode(FluentNavigationPaneDisplayMode mode);

    // Header
    QString header() const { return m_header; }
    void setHeader(const QString& header);

    // Back button
    bool isBackButtonVisible() const { return m_backButtonVisible; }
    void setBackButtonVisible(bool visible);

    // Settings
    bool isSettingsVisible() const { return m_settingsVisible; }
    void setSettingsVisible(bool visible);

    // Pane title
    QString paneTitle() const { return m_paneTitle; }
    void setPaneTitle(const QString& title);

    // Footer
    QWidget* footer() const { return m_footerContent; }
    void setFooter(QWidget* footer);

    // Settings item
    QWidget* settingsItem() const { return m_settingsItem; }
    void setSettingsItem(QWidget* item);

    // Pane toggle button
    bool isPaneToggleButtonVisible() const { return m_paneToggleButtonVisible; }
    void setPaneToggleButtonVisible(bool visible);

    // Content frame
    QWidget* contentFrame() const { return m_contentFrame; }
    void setContentFrame(QWidget* frame);

    // Auto suggest box
    QWidget* autoSuggestBox() const { return m_autoSuggestBox; }
    void setAutoSuggestBox(QWidget* box);

    // Compact mode threshold
    int compactModeThreshold() const { return m_compactModeThreshold; }
    void setCompactModeThreshold(int threshold);

    // Navigation items
    void addNavigationItem(const FluentNavigationItem& item);
    void insertNavigationItem(int index, const FluentNavigationItem& item);
    void removeNavigationItem(int index);
    void clearNavigationItems();
    int navigationItemCount() const;

    // Menu item management (aliases for navigation items)
    FluentNavigationItem* addMenuItem(const QString& text,
                                      const QIcon& icon = QIcon());
    void removeMenuItem(FluentNavigationItem* item);
    void clearMenuItems();
    void invokeItem(FluentNavigationItem* item);

    // Selection
    int selectedIndex() const { return m_selectedIndex; }
    void setSelectedIndex(int index);
    FluentNavigationItem selectedItem() const;
    void setSelectedItem(FluentNavigationItem* item);

    // Content management
    QWidget* currentContent() const;
    void setContent(QWidget* content);

    // Size management
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void togglePane();
    void openPane();
    void closePane();

signals:
    void displayModeChanged(FluentNavigationDisplayMode mode);
    void paneOpenChanged(bool open);
    void headerChanged(const QString& header);
    void selectionChanged(int index);
    void selectedItemChanged(const QString& tag);
    void itemInvoked(int index);
    void itemInvoked(const QString& tag);
    void backRequested();
    void paneTitleChanged(const QString& title);
    void footerChanged(QWidget* footer);
    void settingsItemChanged(QWidget* item);
    void paneToggleButtonVisibleChanged(bool visible);
    void autoSuggestBoxChanged(QWidget* box);
    void menuItemCountChanged(int count);
    void menuItemAdded(FluentNavigationItem* item);
    void menuItemRemoved(FluentNavigationItem* item);
    void menuItemsCleared();
    void isPaneOpenChanged(bool open);
    void paneDisplayModeChanged(FluentNavigationPaneDisplayMode mode);
    void paneToggled();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onNavigationItemClicked(QListWidgetItem* item);
    void onBackButtonClicked();
    void onPaneToggleClicked();

private:
    void setupUI();
    void setupLayout();
    void setupPaneContent();
    void updateDisplayMode();
    void updatePaneState();
    void updateNavigationList();
    void updatePaneWidth();
    void updateItemVisibility();
    void animatePaneToggle();
    void animatePaneTransition();
    void updateDisplayModeFromWidth();
    void setSelectedItemTag(const QString& tag);
    void onItemClicked(const QString& tag);
    void onPaneToggleRequested();

    // Enhanced animation methods
    void animateNavigationItems(bool expanding);
    void animateNavigationItem(QWidget* item, bool expanding);
    void animatePageTransition(QWidget* fromPage, QWidget* toPage);
    void animateItemSelection(QWidget* item, bool selected);

private:
    FluentNavigationDisplayMode m_displayMode{
        FluentNavigationDisplayMode::Auto};
    bool m_isPaneOpen{true};
    QString m_header;
    bool m_backButtonVisible{false};
    bool m_settingsVisible{true};
    int m_selectedIndex{-1};
    QString m_paneTitle;
    QWidget* m_settingsItem{nullptr};
    bool m_paneToggleButtonVisible{true};
    QWidget* m_contentFrame{nullptr};
    QWidget* m_autoSuggestBox{nullptr};
    FluentNavigationPaneDisplayMode m_paneDisplayMode{
        FluentNavigationPaneDisplayMode::Auto};

    // UI Components
    QHBoxLayout* m_mainLayout;
    QSplitter* m_splitter;
    QWidget* m_paneWidget;
    QWidget* m_contentWidget;
    QLabel* m_headerLabel;
    QToolButton* m_backButton;
    QToolButton* m_paneToggleButton;
    QListWidget* m_navigationList;
    QStackedWidget* m_contentStack;
    QVBoxLayout* m_paneLayout;
    QToolButton* m_hamburgerButton;
    QWidget* m_settingsWidget;

    // Animation
    QPropertyAnimation* m_paneAnimation;

    // Data
    std::vector<FluentNavigationItem> m_navigationItems;
    QHash<QString, QWidget*> m_itemButtons;
    QHash<QString, QWidget*> m_contentPages;
    QString m_selectedItemTag;
    QWidget* m_headerContent{nullptr};
    QWidget* m_footerContent{nullptr};

    // Layout constants
    int m_compactPaneWidth{48};
    int m_expandedPaneWidth{320};
    int m_compactModeThreshold{640};
    int m_expandedModeThreshold{1008};
};

}  // namespace FluentQt::Components
