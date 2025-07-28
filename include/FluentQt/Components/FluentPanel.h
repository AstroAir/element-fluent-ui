// include/FluentQt/Components/FluentPanel.h
#pragma once

#include "FluentQt/Core/FluentComponent.h"
#include "FluentQt/Animation/FluentAnimator.h"
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QStackedLayout>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>

namespace FluentQt::Components {

enum class FluentPanelType {
    Card,           // Elevated card with shadow
    Surface,        // Flat surface with border
    Acrylic,        // Translucent acrylic effect
    Mica,           // Mica material effect
    Sidebar,        // Navigation sidebar panel
    Content,        // Main content area
    Dialog,         // Modal dialog panel
    Flyout          // Popup flyout panel
};

enum class FluentPanelElevation {
    None,           // No elevation (0dp)
    Low,            // Subtle elevation (2dp)
    Medium,         // Standard elevation (4dp)
    High,           // Prominent elevation (8dp)
    VeryHigh        // Maximum elevation (16dp)
};

enum class FluentPanelCornerRadius {
    None,           // No rounded corners
    Small,          // 4px radius
    Medium,         // 8px radius
    Large,          // 12px radius
    ExtraLarge      // 16px radius
};

class FluentPanel : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(FluentPanelType panelType READ panelType WRITE setPanelType NOTIFY panelTypeChanged)
    Q_PROPERTY(FluentPanelElevation elevation READ elevation WRITE setElevation NOTIFY elevationChanged)
    Q_PROPERTY(FluentPanelCornerRadius cornerRadius READ cornerRadius WRITE setCornerRadius NOTIFY cornerRadiusChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
    Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor NOTIFY borderColorChanged)
    Q_PROPERTY(int borderWidth READ borderWidth WRITE setBorderWidth NOTIFY borderWidthChanged)
    Q_PROPERTY(QMargins padding READ padding WRITE setPadding NOTIFY paddingChanged)
    Q_PROPERTY(bool animated READ isAnimated WRITE setAnimated NOTIFY animatedChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(bool collapsible READ isCollapsible WRITE setCollapsible NOTIFY collapsibleChanged)
    Q_PROPERTY(bool collapsed READ isCollapsed WRITE setCollapsed NOTIFY collapsedChanged)

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

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void changeEvent(QEvent* event) override;

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

} // namespace FluentQt::Components
