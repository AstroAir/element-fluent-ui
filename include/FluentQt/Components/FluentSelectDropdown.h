// include/FluentQt/Components/FluentSelectDropdown.h
#pragma once

#include <QAbstractItemModel>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QPoint>
#include <QPropertyAnimation>
#include <QRect>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>
#include "FluentQt/Components/FluentSelectItem.h"
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

// Forward declarations
class FluentSelect;
class FluentSelectModel;
class FluentSelectItemDelegate;

enum class FluentDropdownState { Hidden, Showing, Visible, Hiding };

class FluentSelectDropdown : public QWidget {
    Q_OBJECT
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    explicit FluentSelectDropdown(FluentSelect* parent);
    ~FluentSelectDropdown() override;

    // Visibility and animation
    void showAnimated();
    void hideAnimated();
    void setVisible(bool visible) override;

    bool isAnimating() const {
        return m_state == FluentDropdownState::Showing ||
               m_state == FluentDropdownState::Hiding;
    }
    FluentDropdownState state() const { return m_state; }

    // Geometry and positioning
    void updateGeometry();
    void updatePosition();
    QSize calculateSize() const;
    QRect calculateGeometry() const;

    // Model and view
    void setModel(QAbstractItemModel* model);
    QAbstractItemModel* model() const;

    void setSelectionModel(QItemSelectionModel* selectionModel);
    QItemSelectionModel* selectionModel() const;

    QListView* listView() const { return m_listView; }
    FluentSelectItemDelegate* itemDelegate() const { return m_itemDelegate; }

    // Search functionality
    void setSearchVisible(bool visible);
    bool isSearchVisible() const { return m_searchVisible; }

    void setSearchText(const QString& text);
    QString searchText() const;

    void focusSearch();
    void clearSearch();

    // Configuration
    void setMaxVisibleItems(int max);
    int maxVisibleItems() const { return m_maxVisibleItems; }

    void setMaxHeight(int height);
    int maxHeight() const { return m_maxHeight; }

    void setMinWidth(int width);
    int minWidth() const { return m_minWidth; }

    void setMaxWidth(int width);
    int maxWidth() const { return m_maxWidth; }

    // Item navigation
    void selectNextItem();
    void selectPreviousItem();
    void selectFirstItem();
    void selectLastItem();
    void activateCurrentItem();

    // Utility
    int itemHeight() const;
    int visibleItemCount() const;
    QModelIndex currentIndex() const;
    void setCurrentIndex(const QModelIndex& index);

    // Opacity for animations
    qreal opacity() const;
    void setOpacity(qreal opacity);

signals:
    void itemActivated(const QModelIndex& index);
    void itemClicked(const QModelIndex& index);
    void selectionChanged();
    void searchTextChanged(const QString& text);
    void aboutToShow();
    void shown();
    void aboutToHide();
    void hidden();

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    bool eventFilter(QObject* object, QEvent* event) override;

private slots:
    void onItemActivated(const QModelIndex& index);
    void onItemClicked(const QModelIndex& index);
    void onSelectionChanged();
    void onSearchTextChanged(const QString& text);
    void onShowAnimationFinished();
    void onHideAnimationFinished();
    void onThemeChanged();

private:
    void setupUI();
    void setupAnimations();
    void setupConnections();

    void updateColors();
    void updateShadow();
    void updateScrollBars();

    void paintBackground(QPainter& painter, const QRect& rect);
    void paintBorder(QPainter& painter, const QRect& rect);

    QColor getBackgroundColor() const;
    QColor getBorderColor() const;
    QColor getShadowColor() const;

    void ensureCurrentItemVisible();
    void scrollToItem(const QModelIndex& index);

    QRect getDropdownRect() const;
    QPoint getOptimalPosition() const;
    bool shouldDropUp() const;

    void startShowAnimation();
    void startHideAnimation();

private:
    FluentSelect* m_select;
    FluentDropdownState m_state{FluentDropdownState::Hidden};

    // UI Components
    QVBoxLayout* m_mainLayout{nullptr};
    QLineEdit* m_searchEdit{nullptr};
    QListView* m_listView{nullptr};
    QScrollArea* m_scrollArea{nullptr};
    QLabel* m_emptyLabel{nullptr};

    // Delegate
    FluentSelectItemDelegate* m_itemDelegate{nullptr};

    // Animation and effects
    std::unique_ptr<QPropertyAnimation> m_showAnimation;
    std::unique_ptr<QPropertyAnimation> m_hideAnimation;
    std::unique_ptr<QGraphicsOpacityEffect> m_opacityEffect;
    std::unique_ptr<QGraphicsDropShadowEffect> m_shadowEffect;

    // Configuration
    int m_maxVisibleItems{10};
    int m_maxHeight{300};
    int m_minWidth{0};
    int m_maxWidth{0};
    bool m_searchVisible{false};

    // State
    qreal m_opacity{1.0};
    QRect m_targetGeometry;
    QPoint m_targetPosition;

    // Colors (cached from theme)
    QColor m_backgroundColor;
    QColor m_borderColor;
    QColor m_shadowColor;
};

}  // namespace FluentQt::Components
