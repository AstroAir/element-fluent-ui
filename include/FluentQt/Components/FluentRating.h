// include/FluentQt/Components/FluentRating.h
#pragma once

#include <QIcon>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QWidget>
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

enum class FluentRatingSize {
    Small,      // 16px stars
    Medium,     // 20px stars
    Large,      // 24px stars
    ExtraLarge  // 32px stars
};

enum class FluentRatingStyle {
    Stars,   // Traditional star rating
    Hearts,  // Heart-based rating
    Thumbs,  // Thumbs up/down
    Dots,    // Circular dots
    Custom   // Custom icons
};

enum class FluentRatingPrecision {
    Full,     // Only full values (1, 2, 3, etc.)
    Half,     // Half values allowed (1.5, 2.5, etc.)
    Quarter,  // Quarter values (1.25, 1.5, 1.75, etc.)
    Exact     // Any decimal value
};

enum class FluentRatingOrientation { Horizontal, Vertical };

class FluentRating : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(
        double maximum READ maximum WRITE setMaximum NOTIFY maximumChanged)
    Q_PROPERTY(
        double minimum READ minimum WRITE setMinimum NOTIFY minimumChanged)
    Q_PROPERTY(FluentRatingSize ratingSize READ ratingSize WRITE setRatingSize
                   NOTIFY ratingSizeChanged)
    Q_PROPERTY(FluentRatingStyle ratingStyle READ ratingStyle WRITE
                   setRatingStyle NOTIFY ratingStyleChanged)
    Q_PROPERTY(FluentRatingPrecision precision READ precision WRITE setPrecision
                   NOTIFY precisionChanged)
    Q_PROPERTY(FluentRatingOrientation orientation READ orientation WRITE
                   setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(
        bool readOnly READ isReadOnly WRITE setReadOnly NOTIFY readOnlyChanged)
    Q_PROPERTY(bool showTooltip READ showTooltip WRITE setShowTooltip NOTIFY
                   showTooltipChanged)
    Q_PROPERTY(bool showValue READ showValue WRITE setShowValue NOTIFY
                   showValueChanged)
    Q_PROPERTY(
        bool animated READ isAnimated WRITE setAnimated NOTIFY animatedChanged)
    Q_PROPERTY(bool allowReset READ allowReset WRITE setAllowReset NOTIFY
                   allowResetChanged)
    Q_PROPERTY(QIcon filledIcon READ filledIcon WRITE setFilledIcon NOTIFY
                   filledIconChanged)
    Q_PROPERTY(QIcon emptyIcon READ emptyIcon WRITE setEmptyIcon NOTIFY
                   emptyIconChanged)
    Q_PROPERTY(
        QIcon halfIcon READ halfIcon WRITE setHalfIcon NOTIFY halfIconChanged)
    Q_PROPERTY(QColor filledColor READ filledColor WRITE setFilledColor NOTIFY
                   filledColorChanged)
    Q_PROPERTY(QColor emptyColor READ emptyColor WRITE setEmptyColor NOTIFY
                   emptyColorChanged)

public:
    explicit FluentRating(QWidget* parent = nullptr);
    explicit FluentRating(double maximum, QWidget* parent = nullptr);
    explicit FluentRating(double value, double maximum,
                          QWidget* parent = nullptr);
    ~FluentRating() override;

    // Value properties
    double value() const;
    void setValue(double value);

    double maximum() const;
    void setMaximum(double maximum);

    double minimum() const;
    void setMinimum(double minimum);

    void setRange(double minimum, double maximum);

    // Appearance properties
    FluentRatingSize ratingSize() const;
    void setRatingSize(FluentRatingSize size);

    FluentRatingStyle ratingStyle() const;
    void setRatingStyle(FluentRatingStyle style);

    FluentRatingPrecision precision() const;
    void setPrecision(FluentRatingPrecision precision);

    FluentRatingOrientation orientation() const;
    void setOrientation(FluentRatingOrientation orientation);

    // Behavior properties
    bool isReadOnly() const;
    void setReadOnly(bool readOnly);

    bool showTooltip() const;
    void setShowTooltip(bool show);

    bool showValue() const;
    void setShowValue(bool show);

    bool isAnimated() const;
    void setAnimated(bool animated);

    bool allowReset() const;
    void setAllowReset(bool allow);

    // Icon properties
    QIcon filledIcon() const;
    void setFilledIcon(const QIcon& icon);

    QIcon emptyIcon() const;
    void setEmptyIcon(const QIcon& icon);

    QIcon halfIcon() const;
    void setHalfIcon(const QIcon& icon);

    // Color properties
    QColor filledColor() const;
    void setFilledColor(const QColor& color);

    QColor emptyColor() const;
    void setEmptyColor(const QColor& color);

    // Utility methods
    int itemCount() const;
    QSize itemSize() const;
    QRect itemRect(int index) const;
    double valueFromPosition(const QPoint& position) const;
    QString valueText() const;
    QString tooltipText(double value) const;

    // Validation
    bool isValidValue(double value) const;
    double snapToValidValue(double value) const;

    // Size hints
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // Static convenience methods
    static FluentRating* createStarRating(int maxStars = 5,
                                          QWidget* parent = nullptr);
    static FluentRating* createHeartRating(int maxHearts = 5,
                                           QWidget* parent = nullptr);
    static FluentRating* createThumbsRating(QWidget* parent = nullptr);

public slots:
    void reset();
    void animateToValue(double value);
    void highlightItem(int index);
    void clearHighlight();

signals:
    void valueChanged(double value);
    void maximumChanged(double maximum);
    void minimumChanged(double minimum);
    void ratingSizeChanged(FluentRatingSize size);
    void ratingStyleChanged(FluentRatingStyle style);
    void precisionChanged(FluentRatingPrecision precision);
    void orientationChanged(FluentRatingOrientation orientation);
    void readOnlyChanged(bool readOnly);
    void showTooltipChanged(bool show);
    void showValueChanged(bool show);
    void animatedChanged(bool animated);
    void allowResetChanged(bool allow);
    void filledIconChanged(const QIcon& icon);
    void emptyIconChanged(const QIcon& icon);
    void halfIconChanged(const QIcon& icon);
    void filledColorChanged(const QColor& color);
    void emptyColorChanged(const QColor& color);

    void hovered(double value);
    void clicked(double value);
    void ratingSelected(double value);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private slots:
    void onValueAnimationFinished();
    void onHighlightAnimationFinished();
    void updateColors();

private:
    void setupAnimations();
    void updateLayout();
    void updateSizeMetrics();
    void updateIcons();

    // Painting methods
    void paintItems(QPainter* painter);
    void paintItem(QPainter* painter, int index, const QRect& rect,
                   double fillRatio);
    void paintIcon(QPainter* painter, const QRect& rect, const QIcon& icon,
                   double opacity = 1.0);
    void paintValueText(QPainter* painter);
    void paintFocusIndicator(QPainter* painter);

    // Animation methods
    void startValueAnimation(double targetValue);
    void startHighlightAnimation(int index);
    void animateItemScale(int index);

    // Utility methods
    QRect calculateItemRect(int index) const;
    QRect calculateValueTextRect() const;
    double calculateFillRatio(int index, double value) const;
    QIcon getItemIcon(double fillRatio) const;
    QColor getItemColor(double fillRatio) const;

    void updateItemLayout();
    void updateTooltip(double value);
    void showTooltipAt(const QPoint& position, double value);
    void hideTooltip();

private:
    // Value properties
    double m_value{0.0};
    double m_maximum{5.0};
    double m_minimum{0.0};

    // Appearance properties
    FluentRatingSize m_ratingSize{FluentRatingSize::Medium};
    FluentRatingStyle m_ratingStyle{FluentRatingStyle::Stars};
    FluentRatingPrecision m_precision{FluentRatingPrecision::Full};
    FluentRatingOrientation m_orientation{FluentRatingOrientation::Horizontal};

    // Behavior properties
    bool m_readOnly{false};
    bool m_showTooltip{true};
    bool m_showValue{false};
    bool m_animated{true};
    bool m_allowReset{true};

    // Icon properties
    QIcon m_filledIcon;
    QIcon m_emptyIcon;
    QIcon m_halfIcon;

    // Color properties
    QColor m_filledColor;
    QColor m_emptyColor;

    // State
    double m_hoverValue{-1.0};
    int m_highlightedIndex{-1};
    bool m_pressed{false};

    // Size metrics (updated based on m_ratingSize)
    int m_itemSize{20};
    int m_spacing{4};
    int m_valueTextSpacing{8};

    // Layout cache
    QList<QRect> m_itemRects;
    QRect m_valueTextRect;
    bool m_layoutDirty{true};

    // Animation
    std::unique_ptr<Animation::FluentAnimator> m_animator;
    QPropertyAnimation* m_valueAnimation{nullptr};
    QPropertyAnimation* m_highlightAnimation{nullptr};
    QSequentialAnimationGroup* m_scaleAnimation{nullptr};

    // Animation properties
    Q_PROPERTY(double animatedValue READ animatedValue WRITE setAnimatedValue)
    Q_PROPERTY(
        qreal highlightOpacity READ highlightOpacity WRITE setHighlightOpacity)
    Q_PROPERTY(qreal itemScale READ itemScale WRITE setItemScale)

    double m_animatedValue{0.0};
    qreal m_highlightOpacity{0.0};
    qreal m_itemScale{1.0};

    // Property accessors for animations
    double animatedValue() const { return m_animatedValue; }
    void setAnimatedValue(double value);

    qreal highlightOpacity() const { return m_highlightOpacity; }
    void setHighlightOpacity(qreal opacity);

    qreal itemScale() const { return m_itemScale; }
    void setItemScale(qreal scale);
};

}  // namespace FluentQt::Components
