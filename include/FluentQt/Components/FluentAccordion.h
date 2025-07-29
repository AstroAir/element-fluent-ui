// include/FluentQt/Components/FluentAccordion.h
#pragma once

#include "FluentQt/Core/FluentComponent.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QGraphicsOpacityEffect>
#include <QIcon>
#include <memory>

namespace FluentQt::Components {

enum class FluentAccordionState {
    Expanded,
    Collapsed
};

class FluentAccordion : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(FluentAccordionState accordionState READ accordionState WRITE setAccordionState NOTIFY accordionStateChanged)
    Q_PROPERTY(bool collapsible READ isCollapsible WRITE setCollapsible NOTIFY collapsibleChanged)
    Q_PROPERTY(qreal contentOpacity READ contentOpacity WRITE setContentOpacity)
    Q_PROPERTY(int contentHeight READ contentHeight WRITE setContentHeight)

public:
    explicit FluentAccordion(QWidget* parent = nullptr);
    explicit FluentAccordion(const QString& title, QWidget* parent = nullptr);
    ~FluentAccordion() override;

    // Content properties
    QString title() const;
    void setTitle(const QString& title);

    QString description() const;
    void setDescription(const QString& description);

    QIcon icon() const;
    void setIcon(const QIcon& icon);

    // State properties
    FluentAccordionState accordionState() const;
    void setAccordionState(FluentAccordionState state);

    bool isCollapsible() const;
    void setCollapsible(bool collapsible);

    // Animation properties
    qreal contentOpacity() const;
    void setContentOpacity(qreal opacity);

    int contentHeight() const;
    void setContentHeight(int height);

    // Content management
    void setContentWidget(QWidget* widget);
    QWidget* contentWidget() const;

    // Size calculations
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void toggle();
    void expand();
    void collapse();

signals:
    void titleChanged(const QString& title);
    void descriptionChanged(const QString& description);
    void iconChanged(const QIcon& icon);
    void accordionStateChanged(FluentAccordionState state);
    void collapsibleChanged(bool collapsible);
    void expanded();
    void collapsed();

protected:
    // Event handling
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    
    // State management
    void updateStateStyle() override;
    void performStateTransition(Core::FluentState from, Core::FluentState to) override;

private slots:
    void onThemeChanged();
    void onAnimationFinished();

private:
    // Setup methods
    void setupUI();
    void setupAnimations();
    void updateColors();
    void updateFonts();

    // Painting methods
    void paintHeader(QPainter& painter, const QRect& rect);
    void paintIcon(QPainter& painter, const QRect& rect);
    void paintChevron(QPainter& painter, const QRect& rect);
    void paintBorder(QPainter& painter, const QRect& rect);

    // Animation methods
    void startExpandAnimation();
    void startCollapseAnimation();
    void updateContentVisibility();

    // Utility methods
    QRect headerRect() const;
    QRect contentRect() const;
    QRect iconRect() const;
    QRect chevronRect() const;
    int calculateContentHeight() const;

private:
    // Content
    QString m_title;
    QString m_description;
    QIcon m_icon;
    
    // State
    FluentAccordionState m_accordionState{FluentAccordionState::Collapsed};
    bool m_collapsible{true};
    bool m_pressed{false};
    bool m_headerHovered{false};
    
    // Layout
    QVBoxLayout* m_mainLayout{nullptr};
    QWidget* m_headerWidget{nullptr};
    QHBoxLayout* m_headerLayout{nullptr};
    QLabel* m_titleLabel{nullptr};
    QLabel* m_descriptionLabel{nullptr};
    QWidget* m_contentContainer{nullptr};
    QVBoxLayout* m_contentLayout{nullptr};
    QWidget* m_content{nullptr};
    
    // Animation
    std::unique_ptr<QParallelAnimationGroup> m_animationGroup;
    std::unique_ptr<QPropertyAnimation> m_heightAnimation;
    std::unique_ptr<QPropertyAnimation> m_opacityAnimation;
    QGraphicsOpacityEffect* m_contentOpacityEffect{nullptr};
    
    // Animation properties
    qreal m_contentOpacity{0.0};
    int m_contentHeight{0};
    int m_expandedHeight{0};
    
    // Cached values
    mutable QSize m_cachedSizeHint;
    mutable bool m_sizeHintValid{false};
};

} // namespace FluentQt::Components
