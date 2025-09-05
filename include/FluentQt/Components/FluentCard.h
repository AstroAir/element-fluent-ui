// include/FluentQt/Components/FluentCard.h
#pragma once

#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QScrollArea>
#include <QToolButton>
#include <QVBoxLayout>
#include <memory>
#include <vector>
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

enum class FluentCardElevation {
    Flat = 0,
    Low = 2,
    Medium = 8,
    High = 16,
    VeryHigh = 32
};

enum class FluentCardSize {
    Small,      // < 200px width
    Medium,     // 200-400px width
    Large,      // 400-600px width
    ExtraLarge  // > 600px width
};

enum class FluentCardStyle { Default, Outlined, Elevated, Filled, Subtle };

class FluentCardHeader;
class FluentCardContent;
class FluentCardFooter;

class FluentCard : public Core::FluentComponent {
    Q_OBJECT
    Q_ENUM(FluentCardElevation)
    Q_ENUM(FluentCardStyle)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(
        QString subtitle READ subtitle WRITE setSubtitle NOTIFY subtitleChanged)
    Q_PROPERTY(QIcon headerIcon READ headerIcon WRITE setHeaderIcon NOTIFY
                   headerIconChanged)
    Q_PROPERTY(FluentCardElevation elevation READ elevation WRITE setElevation
                   NOTIFY elevationChanged)
    Q_PROPERTY(FluentCardStyle cardStyle READ cardStyle WRITE setCardStyle
                   NOTIFY cardStyleChanged)
    Q_PROPERTY(bool selectable READ isSelectable WRITE setSelectable)
    Q_PROPERTY(
        bool selected READ isSelected WRITE setSelected NOTIFY selectedChanged)
    Q_PROPERTY(bool expandable READ isExpandable WRITE setExpandable)
    Q_PROPERTY(
        bool expanded READ isExpanded WRITE setExpanded NOTIFY expandedChanged)
    Q_PROPERTY(bool headerVisible READ isHeaderVisible WRITE setHeaderVisible)
    Q_PROPERTY(bool footerVisible READ isFooterVisible WRITE setFooterVisible)
    Q_PROPERTY(qreal shadowOpacity READ shadowOpacity WRITE setShadowOpacity)
    Q_PROPERTY(qreal expansionProgress READ expansionProgress WRITE
                   setExpansionProgress)
    Q_PROPERTY(
        QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)

public:
    explicit FluentCard(QWidget* parent = nullptr);
    explicit FluentCard(const QString& title, QWidget* parent = nullptr);
    explicit FluentCard(const QString& title, const QString& subtitle,
                        QWidget* parent = nullptr);
    ~FluentCard() override;

    // Header properties
    QString title() const;
    void setTitle(const QString& title);

    QString subtitle() const;
    void setSubtitle(const QString& subtitle);

    QIcon headerIcon() const;
    void setHeaderIcon(const QIcon& icon);

    bool isHeaderVisible() const { return m_headerVisible; }
    void setHeaderVisible(bool visible);

    // Style properties
    FluentCardElevation elevation() const noexcept { return m_elevation; }
    void setElevation(FluentCardElevation elevation);

    FluentCardStyle cardStyle() const noexcept { return m_cardStyle; }
    void setCardStyle(FluentCardStyle style);

    qreal shadowOpacity() const noexcept { return m_shadowOpacity; }
    void setShadowOpacity(qreal opacity);

    qreal expansionProgress() const noexcept { return m_expansionProgress; }
    void setExpansionProgress(qreal progress);

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor& color);

    // Selection properties
    bool isSelectable() const noexcept { return m_selectable; }
    void setSelectable(bool selectable);

    bool isSelected() const noexcept { return m_selected; }
    void setSelected(bool selected);

    // Expansion properties
    bool isExpandable() const noexcept { return m_expandable; }
    void setExpandable(bool expandable);

    bool isExpanded() const noexcept { return m_expanded; }
    void setExpanded(bool expanded);

    // Footer properties
    bool isFooterVisible() const { return m_footerVisible; }
    void setFooterVisible(bool visible);

    // Content management
    void setContentWidget(QWidget* widget);
    QWidget* contentWidget() const;

    void addHeaderAction(QAction* action);
    void removeHeaderAction(QAction* action);
    void clearHeaderActions();

    void addFooterWidget(QWidget* widget);
    void removeFooterWidget(QWidget* widget);
    void clearFooterWidgets();

    // Layout access
    QLayout* contentLayout() const;
    QLayout* headerLayout() const;
    QLayout* footerLayout() const;

    // Size management
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // Animation
    void animateIn();
    void animateOut();

    // Micro-interactions
    void addHoverEffect();
    void addPressEffect();
    void addFocusEffect();

public slots:
    void toggleExpanded();
    void expandWithAnimation();
    void collapseWithAnimation();

signals:
    void titleChanged(const QString& title);
    void subtitleChanged(const QString& subtitle);
    void headerIconChanged(const QIcon& icon);
    void elevationChanged(FluentCardElevation elevation);
    void cardStyleChanged(FluentCardStyle style);
    void selectedChanged(bool selected);
    void expandedChanged(bool expanded);
    void cardClicked();
    void cardDoubleClicked();
    void headerClicked();

protected:
    // Event handling
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;

    // State management
    void updateStateStyle() override;
    void performStateTransition(Core::FluentState from,
                                Core::FluentState to) override;

private slots:
    void onExpansionAnimationValueChanged(const QVariant& value);
    void onExpansionAnimationFinished();
    void onElevationAnimationFinished();
    void onThemeChanged();

private:
    // Setup methods
    void setupLayout();
    void setupHeader();
    void setupContent();
    void setupFooter();
    void setupAnimations();

    // Painting methods
    void paintBackground(QPainter& painter, const QRect& rect);
    void paintBorder(QPainter& painter, const QRect& rect);
    void paintSelection(QPainter& painter, const QRect& rect);
    void paintShadow(QPainter& painter, const QRect& rect);

    // Style methods
    QColor getBackgroundColor() const;
    QColor getBorderColor() const;
    QColor getSelectionColor() const;
    QPen getBorderPen() const;
    int getBorderWidth() const;

    // Corner radius methods
    int getFluentCornerRadius() const;
    int getFluentCornerRadius(const QSize& size) const;

    // Convenience methods for enhanced developer experience
    void setCardContent(QWidget* content);
    void setCardHeader(const QString& title,
                       const QString& subtitle = QString());
    void setCardFooter(QWidget* footer);
    void setCardIcon(const QIcon& icon);
    void setCardActions(const QList<QAction*>& actions);

    // Smart defaults based on FluentUI guidelines
    void applyFluentDefaults();
    void setFluentSize(FluentCardSize size);

    // Enhanced state management
    bool isInteractive() const;
    void setInteractive(bool interactive);
    bool hasContent() const;

    // Enhanced accessibility
    void setupAccessibility();
    void updateAccessibilityInfo();

    // Shadow methods
    void updateShadowEffect();
    void animateElevation(FluentCardElevation from, FluentCardElevation to);

    // Layout methods
    void updateHeaderVisibility();
    void updateFooterVisibility();
    void updateContentGeometry();

    // Animation methods
    void startExpansionAnimation(bool expanding);
    void updateExpansionState();

    // Utility methods
    int getHeaderHeight() const;
    int getFooterHeight() const;
    QRect getContentRect() const;

private:
    // Components
    std::unique_ptr<FluentCardHeader> m_header;
    std::unique_ptr<FluentCardContent> m_content;
    std::unique_ptr<FluentCardFooter> m_footer;

    // Layout
    QVBoxLayout* m_mainLayout;

    // Properties
    FluentCardElevation m_elevation{FluentCardElevation::Low};
    FluentCardStyle m_cardStyle{FluentCardStyle::Default};
    bool m_selectable{false};
    bool m_selected{false};
    bool m_expandable{false};
    bool m_expanded{true};
    bool m_headerVisible{true};
    bool m_footerVisible{false};

    // Animation properties
    qreal m_shadowOpacity{1.0};
    qreal m_expansionProgress{1.0};
    int m_collapsedHeight{0};
    int m_expandedHeight{0};
    QColor m_backgroundColor;

    // Animations
    std::unique_ptr<QPropertyAnimation> m_expansionAnimation;
    std::unique_ptr<QPropertyAnimation> m_elevationAnimation;
    std::unique_ptr<QGraphicsDropShadowEffect> m_shadowEffect;
    QList<QPropertyAnimation*> m_currentAnimations;

    // State
    bool m_pressed{false};
    QPoint m_pressPosition;
};

// Helper classes for card sections
class FluentCardHeader : public QWidget {
    Q_OBJECT

public:
    explicit FluentCardHeader(QWidget* parent = nullptr);

    void setTitle(const QString& title);
    QString title() const;

    void setSubtitle(const QString& subtitle);
    QString subtitle() const;

    void setIcon(const QIcon& icon);
    QIcon icon() const;

    void addAction(QAction* action);
    void removeAction(QAction* action);
    void clearActions();

    QSize sizeHint() const override;

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void setupLayout();
    void updateActions();

private:
    QHBoxLayout* m_layout;
    QLabel* m_iconLabel;
    QLabel* m_titleLabel;
    QLabel* m_subtitleLabel;
    QHBoxLayout* m_actionsLayout;

    std::vector<QToolButton*> m_actionButtons;
    bool m_pressed{false};
};

class FluentCardContent : public QScrollArea {
    Q_OBJECT

public:
    explicit FluentCardContent(QWidget* parent = nullptr);

    void setContentWidget(QWidget* widget);
    QWidget* contentWidget() const { return m_contentWidget; }

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    QWidget* m_contentWidget{nullptr};
};

class FluentCardFooter : public QWidget {
    Q_OBJECT

public:
    explicit FluentCardFooter(QWidget* parent = nullptr);

    void addWidget(QWidget* widget);
    void removeWidget(QWidget* widget);
    void clearWidgets();

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void setupLayout();

private:
    QHBoxLayout* m_layout;
};

}  // namespace FluentQt::Components
