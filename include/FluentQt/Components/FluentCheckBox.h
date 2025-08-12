// include/FluentQt/Components/FluentCheckBox.h
#pragma once

#include <QCheckBox>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

enum class FluentCheckState { Unchecked, PartiallyChecked, Checked };

class FluentCheckBox : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(FluentCheckState checkState READ checkState WRITE setCheckState
                   NOTIFY checkStateChanged)
    Q_PROPERTY(
        bool tristate READ isTristate WRITE setTristate NOTIFY tristateChanged)
    Q_PROPERTY(bool autoExclusive READ autoExclusive WRITE setAutoExclusive
                   NOTIFY autoExclusiveChanged)
    Q_PROPERTY(
        QSize iconSize READ iconSize WRITE setIconSize NOTIFY iconSizeChanged)

public:
    explicit FluentCheckBox(QWidget* parent = nullptr);
    explicit FluentCheckBox(const QString& text, QWidget* parent = nullptr);
    ~FluentCheckBox() override;

    // Text properties
    QString text() const;
    void setText(const QString& text);

    // Check state
    FluentCheckState checkState() const;
    void setCheckState(FluentCheckState state);

    bool isChecked() const;
    void setChecked(bool checked);

    // Tristate support
    bool isTristate() const;
    void setTristate(bool tristate);

    // Auto exclusive (radio button behavior)
    bool autoExclusive() const;
    void setAutoExclusive(bool autoExclusive);

    // Icon size
    QSize iconSize() const;
    void setIconSize(const QSize& size);

    // Size hint
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void toggle();
    void click();

signals:
    void textChanged(const QString& text);
    void checkStateChanged(FluentCheckState state);
    void tristateChanged(bool tristate);
    void autoExclusiveChanged(bool autoExclusive);
    void iconSizeChanged(const QSize& size);
    void toggled(bool checked);
    void clicked(bool checked);
    void stateChanged(int state);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void changeEvent(QEvent* event) override;

private slots:
    void updateAnimations();

private:
    void setupAnimations();
    void updateColors();
    void updateFonts();
    void animateStateChange();
    void animateCheckChange();
    void drawCheckBox(QPainter& painter, const QRect& rect) const;
    void drawCheckMark(QPainter& painter, const QRect& rect) const;
    void drawPartialMark(QPainter& painter, const QRect& rect) const;
    QRect checkBoxRect() const;
    QRect textRect() const;

    void nextCheckState();

private:
    QString m_text;
    FluentCheckState m_checkState{FluentCheckState::Unchecked};
    bool m_tristate{false};
    bool m_autoExclusive{false};
    QSize m_iconSize{20, 20};
    bool m_pressed{false};

    // Animation
    std::unique_ptr<Animation::FluentAnimator> m_animator;
    QPropertyAnimation* m_checkAnimation{nullptr};
    QPropertyAnimation* m_stateAnimation{nullptr};
    QGraphicsOpacityEffect* m_checkOpacityEffect{nullptr};

    qreal m_checkProgress{0.0};  // 0.0 = unchecked, 1.0 = checked
};

}  // namespace FluentQt::Components
