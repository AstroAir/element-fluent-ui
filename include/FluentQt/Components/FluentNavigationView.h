// include/FluentQt/Components/FluentNavigationView.h
#pragma once

#include "FluentQt/Core/FluentComponent.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLabel>
#include <QToolButton>
#include <QPropertyAnimation>
#include <QIcon>
#include <QSplitter>
#include <functional>

namespace FluentQt::Components {

enum class FluentNavigationDisplayMode {
    Auto,
    Expanded,
    Compact,
    Minimal
};

enum class FluentNavigationSelectionFollowsFocus {
    Disabled,
    Enabled
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
    FluentNavigationItem(const QString& text, const QIcon& icon = QIcon(), QWidget* content = nullptr)
        : text(text), icon(icon), content(content), tag(text) {}
};

class FluentNavigationView : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(FluentNavigationDisplayMode displayMode READ displayMode WRITE setDisplayMode NOTIFY displayModeChanged)
    Q_PROPERTY(bool isPaneOpen READ isPaneOpen WRITE setPaneOpen NOTIFY paneOpenChanged)
    Q_PROPERTY(QString header READ header WRITE setHeader NOTIFY headerChanged)
    Q_PROPERTY(bool isBackButtonVisible READ isBackButtonVisible WRITE setBackButtonVisible)
    Q_PROPERTY(bool isSettingsVisible READ isSettingsVisible WRITE setSettingsVisible)

public:
    explicit FluentNavigationView(QWidget* parent = nullptr);
    ~FluentNavigationView() override = default;

    // Display mode
    FluentNavigationDisplayMode displayMode() const { return m_displayMode; }
    void setDisplayMode(FluentNavigationDisplayMode mode);

    // Pane state
    bool isPaneOpen() const { return m_isPaneOpen; }
    void setPaneOpen(bool open);

    // Header
    QString header() const { return m_header; }
    void setHeader(const QString& header);

    // Back button
    bool isBackButtonVisible() const { return m_backButtonVisible; }
    void setBackButtonVisible(bool visible);

    // Settings
    bool isSettingsVisible() const { return m_settingsVisible; }
    void setSettingsVisible(bool visible);

    // Navigation items
    void addNavigationItem(const FluentNavigationItem& item);
    void insertNavigationItem(int index, const FluentNavigationItem& item);
    void removeNavigationItem(int index);
    void clearNavigationItems();
    int navigationItemCount() const;

    // Selection
    int selectedIndex() const { return m_selectedIndex; }
    void setSelectedIndex(int index);
    FluentNavigationItem selectedItem() const;

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
    void setIsPaneOpen(bool open);

    // Enhanced animation methods
    void animateNavigationItems(bool expanding);
    void animateNavigationItem(QWidget* item, bool expanding);
    void animatePageTransition(QWidget* fromPage, QWidget* toPage);
    void animateItemSelection(QWidget* item, bool selected);

private:
    FluentNavigationDisplayMode m_displayMode{FluentNavigationDisplayMode::Auto};
    bool m_isPaneOpen{true};
    QString m_header;
    bool m_backButtonVisible{false};
    bool m_settingsVisible{true};
    int m_selectedIndex{-1};

    // UI Components
    QHBoxLayout* m_mainLayout;
    QSplitter* m_splitter;
    QWidget* m_paneWidget;
    QVBoxLayout* m_paneLayout;
    QWidget* m_contentWidget;
    QLabel* m_headerLabel;
    QToolButton* m_backButton;
    QToolButton* m_paneToggleButton;
    QToolButton* m_hamburgerButton;
    QListWidget* m_navigationList;
    QStackedWidget* m_contentStack;
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

} // namespace FluentQt::Components
