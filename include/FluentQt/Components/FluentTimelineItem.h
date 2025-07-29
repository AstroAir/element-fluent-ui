// include/FluentQt/Components/FluentTimelineItem.h
#pragma once

#include "FluentQt/Core/FluentComponent.h"
#include <QLabel>
#include <QIcon>
#include <QDateTime>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QVBoxLayout>
#include <QPixmap>
#include <QVariant>
#include <memory>

namespace FluentQt::Components {

enum class FluentTimelineItemState {
    Pending,
    Current,
    Completed,
    Failed,
    Cancelled,
    Warning
};

enum class FluentTimelineItemType {
    Default,
    Milestone,
    Event,
    Task,
    Note,
    Custom
};

enum class FluentTimelineItemSize {
    Small,
    Medium,
    Large
};

enum class FluentTimelineItemIndicatorStyle {
    Circle,
    Square,
    Diamond,
    Icon,
    Avatar,
    Custom
};

struct FluentTimelineItemData {
    QString title;
    QString description;
    QString subtitle;
    QDateTime dateTime;
    QIcon icon;
    QPixmap avatar;
    QVariant userData;
    
    FluentTimelineItemState state{FluentTimelineItemState::Pending};
    FluentTimelineItemType type{FluentTimelineItemType::Default};
    FluentTimelineItemSize size{FluentTimelineItemSize::Medium};
    FluentTimelineItemIndicatorStyle indicatorStyle{FluentTimelineItemIndicatorStyle::Circle};
    
    bool expandable{false};
    bool expanded{false};
    bool interactive{true};
    bool showDateTime{true};
    bool showIndicator{true};
    
    QColor customColor;
    QString customText;
    QIcon customIcon;
};

class FluentTimelineItem : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString subtitle READ subtitle WRITE setSubtitle NOTIFY subtitleChanged)
    Q_PROPERTY(QDateTime dateTime READ dateTime WRITE setDateTime NOTIFY dateTimeChanged)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(QPixmap avatar READ avatar WRITE setAvatar NOTIFY avatarChanged)
    Q_PROPERTY(FluentTimelineItemState itemState READ itemState WRITE setItemState NOTIFY itemStateChanged)
    Q_PROPERTY(FluentTimelineItemType itemType READ itemType WRITE setItemType NOTIFY itemTypeChanged)
    Q_PROPERTY(FluentTimelineItemSize itemSize READ itemSize WRITE setItemSize NOTIFY itemSizeChanged)
    Q_PROPERTY(FluentTimelineItemIndicatorStyle indicatorStyle READ indicatorStyle WRITE setIndicatorStyle NOTIFY indicatorStyleChanged)
    Q_PROPERTY(bool expandable READ isExpandable WRITE setExpandable NOTIFY expandableChanged)
    Q_PROPERTY(bool expanded READ isExpanded WRITE setExpanded NOTIFY expandedChanged)
    Q_PROPERTY(bool interactive READ isInteractive WRITE setInteractive NOTIFY interactiveChanged)
    Q_PROPERTY(bool showDateTime READ showDateTime WRITE setShowDateTime NOTIFY showDateTimeChanged)
    Q_PROPERTY(bool showIndicator READ showIndicator WRITE setShowIndicator NOTIFY showIndicatorChanged)

public:
    explicit FluentTimelineItem(QWidget* parent = nullptr);
    explicit FluentTimelineItem(const QString& title, QWidget* parent = nullptr);
    explicit FluentTimelineItem(const QString& title, const QString& description, QWidget* parent = nullptr);
    explicit FluentTimelineItem(const FluentTimelineItemData& data, QWidget* parent = nullptr);
    ~FluentTimelineItem() override;

    // Content properties
    QString title() const { return m_data.title; }
    void setTitle(const QString& title);

    QString description() const { return m_data.description; }
    void setDescription(const QString& description);

    QString subtitle() const { return m_data.subtitle; }
    void setSubtitle(const QString& subtitle);

    QDateTime dateTime() const { return m_data.dateTime; }
    void setDateTime(const QDateTime& dateTime);

    QIcon icon() const { return m_data.icon; }
    void setIcon(const QIcon& icon);

    QPixmap avatar() const { return m_data.avatar; }
    void setAvatar(const QPixmap& avatar);

    QVariant userData() const { return m_data.userData; }
    void setUserData(const QVariant& data);

    // State and type properties
    FluentTimelineItemState itemState() const { return m_data.state; }
    void setItemState(FluentTimelineItemState state);

    FluentTimelineItemType itemType() const { return m_data.type; }
    void setItemType(FluentTimelineItemType type);

    FluentTimelineItemSize itemSize() const { return m_data.size; }
    void setItemSize(FluentTimelineItemSize size);

    FluentTimelineItemIndicatorStyle indicatorStyle() const { return m_data.indicatorStyle; }
    void setIndicatorStyle(FluentTimelineItemIndicatorStyle style);

    // Behavior properties
    bool isExpandable() const { return m_data.expandable; }
    void setExpandable(bool expandable);

    bool isExpanded() const { return m_data.expanded; }
    void setExpanded(bool expanded);

    bool isInteractive() const { return m_data.interactive; }
    void setInteractive(bool interactive);

    bool showDateTime() const { return m_data.showDateTime; }
    void setShowDateTime(bool show);

    bool showIndicator() const { return m_data.showIndicator; }
    void setShowIndicator(bool show);

    // Custom styling
    QColor customColor() const { return m_data.customColor; }
    void setCustomColor(const QColor& color);

    QString customText() const { return m_data.customText; }
    void setCustomText(const QString& text);

    QIcon customIcon() const { return m_data.customIcon; }
    void setCustomIcon(const QIcon& icon);

    // Data management
    FluentTimelineItemData data() const { return m_data; }
    void setData(const FluentTimelineItemData& data);

    // Content widgets
    void setContentWidget(QWidget* widget);
    QWidget* contentWidget() const { return m_contentWidget; }

    void setExpandedWidget(QWidget* widget);
    QWidget* expandedWidget() const { return m_expandedWidget; }

    // Size and layout
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // Animation control
    void animateExpansion(bool expanded);
    void animateStateChange(FluentTimelineItemState newState);

    // Static factory methods
    static FluentTimelineItem* createMilestone(const QString& title, const QDateTime& dateTime = QDateTime::currentDateTime(), QWidget* parent = nullptr);
    static FluentTimelineItem* createEvent(const QString& title, const QString& description, const QDateTime& dateTime = QDateTime::currentDateTime(), QWidget* parent = nullptr);
    static FluentTimelineItem* createTask(const QString& title, FluentTimelineItemState state = FluentTimelineItemState::Pending, QWidget* parent = nullptr);
    static FluentTimelineItem* createNote(const QString& title, const QString& description, QWidget* parent = nullptr);
    static FluentTimelineItem* createIconItem(const QIcon& icon, const QString& title, const QString& description = QString(), QWidget* parent = nullptr);
    static FluentTimelineItem* createAvatarItem(const QPixmap& avatar, const QString& title, const QString& description = QString(), QWidget* parent = nullptr);

public slots:
    void toggle();
    void expand();
    void collapse();
    void markCompleted();
    void markFailed();
    void markCancelled();
    void markPending();

signals:
    void titleChanged(const QString& title);
    void descriptionChanged(const QString& description);
    void subtitleChanged(const QString& subtitle);
    void dateTimeChanged(const QDateTime& dateTime);
    void iconChanged(const QIcon& icon);
    void avatarChanged(const QPixmap& avatar);
    void itemStateChanged(FluentTimelineItemState state);
    void itemTypeChanged(FluentTimelineItemType type);
    void itemSizeChanged(FluentTimelineItemSize size);
    void indicatorStyleChanged(FluentTimelineItemIndicatorStyle style);
    void expandableChanged(bool expandable);
    void expandedChanged(bool expanded);
    void interactiveChanged(bool interactive);
    void showDateTimeChanged(bool show);
    void showIndicatorChanged(bool show);
    
    void clicked();
    void doubleClicked();
    void expanded();
    void collapsed();
    void stateChanged(FluentTimelineItemState state);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    virtual void updateStateStyle() override;
    virtual void performStateTransition(Core::FluentState from, Core::FluentState to) override;

private slots:
    void onThemeChanged();
    void onExpansionAnimationFinished();
    void onStateAnimationFinished();

private:
    void setupLayout();
    void setupAnimations();
    void updateLayout();
    void updateColors();
    void updateAccessibility();
    
    void paintIndicator(QPainter& painter, const QRect& rect);
    void paintContent(QPainter& painter, const QRect& rect);
    void paintBackground(QPainter& painter, const QRect& rect);
    void paintBorder(QPainter& painter, const QRect& rect);
    
    QRect getIndicatorRect() const;
    QRect getContentRect() const;
    QRect getTitleRect() const;
    QRect getDescriptionRect() const;
    QRect getDateTimeRect() const;
    
    QColor getStateColor() const;
    QColor getBackgroundColor() const;
    QColor getBorderColor() const;
    QColor getTextColor() const;
    QColor getSecondaryTextColor() const;
    
    int getIndicatorSize() const;
    QFont getTitleFont() const;
    QFont getDescriptionFont() const;
    QFont getDateTimeFont() const;
    QSize calculateSizeHint() const;

private:
    FluentTimelineItemData m_data;
    
    // Layout and widgets
    QVBoxLayout* m_mainLayout{nullptr};
    QWidget* m_contentWidget{nullptr};
    QWidget* m_expandedWidget{nullptr};
    
    // Animation
    std::unique_ptr<QPropertyAnimation> m_expansionAnimation;
    std::unique_ptr<QPropertyAnimation> m_stateAnimation;
    std::unique_ptr<QGraphicsOpacityEffect> m_opacityEffect;
    
    // State
    bool m_hovered{false};
    bool m_pressed{false};
    qreal m_expansionProgress{0.0};
    qreal m_stateProgress{1.0};
    
    // Cached values
    QSize m_cachedSizeHint;
    bool m_sizeHintValid{false};
    QRect m_lastRect;
};

} // namespace FluentQt::Components
