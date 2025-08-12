// include/FluentQt/Components/FluentSeparator.h
#pragma once

#include <QHBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QVBoxLayout>
#include <QWidget>
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

enum class FluentSeparatorType {
    Line,      // Simple line separator
    Text,      // Separator with text label
    Icon,      // Separator with icon
    Gradient,  // Gradient line separator
    Dashed,    // Dashed line separator
    Dotted,    // Dotted line separator
    Double     // Double line separator
};

enum class FluentSeparatorOrientation { Horizontal, Vertical };

enum class FluentSeparatorAlignment {
    Left,    // Text/icon aligned to left (or top for vertical)
    Center,  // Text/icon centered
    Right    // Text/icon aligned to right (or bottom for vertical)
};

enum class FluentSeparatorStyle {
    Subtle,     // Light, minimal appearance
    Standard,   // Normal appearance
    Prominent,  // Bold, emphasized appearance
    Accent      // Uses accent color
};

class FluentSeparator : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(FluentSeparatorType separatorType READ separatorType WRITE
                   setSeparatorType NOTIFY separatorTypeChanged)
    Q_PROPERTY(FluentSeparatorOrientation orientation READ orientation WRITE
                   setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(FluentSeparatorAlignment alignment READ alignment WRITE
                   setAlignment NOTIFY alignmentChanged)
    Q_PROPERTY(FluentSeparatorStyle separatorStyle READ separatorStyle WRITE
                   setSeparatorStyle NOTIFY separatorStyleChanged)
    Q_PROPERTY(QColor lineColor READ lineColor WRITE setLineColor NOTIFY
                   lineColorChanged)
    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor NOTIFY
                   textColorChanged)
    Q_PROPERTY(
        int lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged)
    Q_PROPERTY(int spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
    Q_PROPERTY(int margins READ margins WRITE setMargins NOTIFY marginsChanged)
    Q_PROPERTY(
        bool animated READ isAnimated WRITE setAnimated NOTIFY animatedChanged)
    Q_PROPERTY(
        bool visible READ isVisible WRITE setVisible NOTIFY visibilityChanged)

public:
    explicit FluentSeparator(QWidget* parent = nullptr);
    explicit FluentSeparator(FluentSeparatorOrientation orientation,
                             QWidget* parent = nullptr);
    explicit FluentSeparator(const QString& text, QWidget* parent = nullptr);
    explicit FluentSeparator(const QIcon& icon, QWidget* parent = nullptr);
    explicit FluentSeparator(FluentSeparatorType type,
                             QWidget* parent = nullptr);
    ~FluentSeparator() override;

    // Content properties
    QString text() const;
    void setText(const QString& text);

    QIcon icon() const;
    void setIcon(const QIcon& icon);

    // Type and appearance
    FluentSeparatorType separatorType() const;
    void setSeparatorType(FluentSeparatorType type);

    FluentSeparatorOrientation orientation() const;
    void setOrientation(FluentSeparatorOrientation orientation);

    FluentSeparatorAlignment alignment() const;
    void setAlignment(FluentSeparatorAlignment alignment);

    FluentSeparatorStyle separatorStyle() const;
    void setSeparatorStyle(FluentSeparatorStyle style);

    // Color properties
    QColor lineColor() const;
    void setLineColor(const QColor& color);

    QColor textColor() const;
    void setTextColor(const QColor& color);

    // Size properties
    int lineWidth() const;
    void setLineWidth(int width);

    int spacing() const;
    void setSpacing(int spacing);

    int margins() const;
    void setMargins(int margins);
    void setMargins(int left, int top, int right, int bottom);

    // Behavior properties
    bool isAnimated() const;
    void setAnimated(bool animated);

    bool isVisible() const;
    void setVisible(bool visible) override;

    // Utility methods
    QSize contentSize() const;
    QRect lineRect() const;
    QRect textRect() const;
    QRect iconRect() const;

    bool hasContent() const;
    bool isHorizontal() const;
    bool isVertical() const;

    // Size hints
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // Static convenience methods
    static FluentSeparator* createHorizontalLine(QWidget* parent = nullptr);
    static FluentSeparator* createVerticalLine(QWidget* parent = nullptr);
    static FluentSeparator* createTextSeparator(const QString& text,
                                                QWidget* parent = nullptr);
    static FluentSeparator* createIconSeparator(const QIcon& icon,
                                                QWidget* parent = nullptr);
    static FluentSeparator* createGradientSeparator(
        FluentSeparatorOrientation orientation =
            FluentSeparatorOrientation::Horizontal,
        QWidget* parent = nullptr);

public slots:
    void show() override;
    void hide() override;
    void animateIn();
    void animateOut();
    void fadeIn();
    void fadeOut();

signals:
    void textChanged(const QString& text);
    void iconChanged(const QIcon& icon);
    void separatorTypeChanged(FluentSeparatorType type);
    void orientationChanged(FluentSeparatorOrientation orientation);
    void alignmentChanged(FluentSeparatorAlignment alignment);
    void separatorStyleChanged(FluentSeparatorStyle style);
    void lineColorChanged(const QColor& color);
    void textColorChanged(const QColor& color);
    void lineWidthChanged(int width);
    void spacingChanged(int spacing);
    void marginsChanged(int margins);
    void animatedChanged(bool animated);
    void visibilityChanged(bool visible);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;

private slots:
    void onShowAnimationFinished();
    void onHideAnimationFinished();
    void updateColors();

private:
    void setupLayout();
    void setupAnimations();
    void updateLayout();
    void updateSizeMetrics();
    void updateGeometry();

    // Painting methods
    void paintLine(QPainter* painter);
    void paintText(QPainter* painter);
    void paintIcon(QPainter* painter);
    void paintGradientLine(QPainter* painter);
    void paintDashedLine(QPainter* painter);
    void paintDottedLine(QPainter* painter);
    void paintDoubleLine(QPainter* painter);

    // Animation methods
    void startShowAnimation();
    void startHideAnimation();
    void startFadeAnimation(bool fadeIn);
    void stopAllAnimations();

    // Utility methods
    QRect calculateLineRect() const;
    QRect calculateTextRect() const;
    QRect calculateIconRect() const;
    QRect calculateContentRect() const;

    QColor getLineColor() const;
    QColor getTextColor() const;
    QFont getTextFont() const;
    QPen getLinePen() const;

    void layoutHorizontalSeparator();
    void layoutVerticalSeparator();
    void updateContentLayout();

    QSize calculateTextSize() const;
    QSize calculateIconSize() const;
    QSize calculateLineSize() const;

private:
    // Content properties
    QString m_text;
    QIcon m_icon;

    // Type and appearance
    FluentSeparatorType m_separatorType{FluentSeparatorType::Line};
    FluentSeparatorOrientation m_orientation{
        FluentSeparatorOrientation::Horizontal};
    FluentSeparatorAlignment m_alignment{FluentSeparatorAlignment::Center};
    FluentSeparatorStyle m_separatorStyle{FluentSeparatorStyle::Standard};

    // Color properties
    QColor m_lineColor;
    QColor m_textColor;

    // Size properties
    int m_lineWidth{1};
    int m_spacing{8};
    int m_leftMargin{0};
    int m_topMargin{0};
    int m_rightMargin{0};
    int m_bottomMargin{0};

    // Behavior properties
    bool m_animated{true};

    // State
    bool m_hasCustomLineColor{false};
    bool m_hasCustomTextColor{false};

    // Layout cache
    QRect m_lineRect;
    QRect m_textRect;
    QRect m_iconRect;
    QRect m_contentRect;
    bool m_layoutDirty{true};

    // Size metrics
    int m_defaultLineWidth{1};
    int m_defaultSpacing{8};
    int m_iconSize{16};
    int m_textPadding{4};

    // Animation
    std::unique_ptr<Animation::FluentAnimator> m_animator;
    QPropertyAnimation* m_showAnimation{nullptr};
    QPropertyAnimation* m_hideAnimation{nullptr};
    QPropertyAnimation* m_fadeAnimation{nullptr};

    // Animation properties
    Q_PROPERTY(
        qreal separatorOpacity READ separatorOpacity WRITE setSeparatorOpacity)
    Q_PROPERTY(qreal separatorScale READ separatorScale WRITE setSeparatorScale)
    Q_PROPERTY(
        int animatedLineWidth READ animatedLineWidth WRITE setAnimatedLineWidth)

    qreal m_separatorOpacity{1.0};
    qreal m_separatorScale{1.0};
    int m_animatedLineWidth{1};

    // Property accessors for animations
    qreal separatorOpacity() const { return m_separatorOpacity; }
    void setSeparatorOpacity(qreal opacity);

    qreal separatorScale() const { return m_separatorScale; }
    void setSeparatorScale(qreal scale);

    int animatedLineWidth() const { return m_animatedLineWidth; }
    void setAnimatedLineWidth(int width);
};

}  // namespace FluentQt::Components
