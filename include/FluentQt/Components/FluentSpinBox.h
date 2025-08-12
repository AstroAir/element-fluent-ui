// include/FluentQt/Components/FluentSpinBox.h
#pragma once

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPropertyAnimation>
#include <QTimer>
#include <QVBoxLayout>
#include <QValidator>
#include <QWidget>
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

enum class FluentSpinBoxType { Integer, Double, Currency, Percentage };

enum class FluentSpinBoxSize { Small, Medium, Large };

enum class FluentSpinBoxButtonLayout {
    Vertical,    // Buttons stacked vertically on the right
    Horizontal,  // Buttons side by side on the right
    Sides,       // Decrease on left, increase on right
    Embedded     // Buttons embedded in the input field
};

class FluentSpinBox : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(
        double minimum READ minimum WRITE setMinimum NOTIFY minimumChanged)
    Q_PROPERTY(
        double maximum READ maximum WRITE setMaximum NOTIFY maximumChanged)
    Q_PROPERTY(double singleStep READ singleStep WRITE setSingleStep NOTIFY
                   singleStepChanged)
    Q_PROPERTY(
        int decimals READ decimals WRITE setDecimals NOTIFY decimalsChanged)
    Q_PROPERTY(QString prefix READ prefix WRITE setPrefix NOTIFY prefixChanged)
    Q_PROPERTY(QString suffix READ suffix WRITE setSuffix NOTIFY suffixChanged)
    Q_PROPERTY(QString placeholderText READ placeholderText WRITE
                   setPlaceholderText NOTIFY placeholderTextChanged)
    Q_PROPERTY(FluentSpinBoxType spinBoxType READ spinBoxType WRITE
                   setSpinBoxType NOTIFY spinBoxTypeChanged)
    Q_PROPERTY(FluentSpinBoxSize spinBoxSize READ spinBoxSize WRITE
                   setSpinBoxSize NOTIFY spinBoxSizeChanged)
    Q_PROPERTY(FluentSpinBoxButtonLayout buttonLayout READ buttonLayout WRITE
                   setButtonLayout NOTIFY buttonLayoutChanged)
    Q_PROPERTY(
        bool readOnly READ isReadOnly WRITE setReadOnly NOTIFY readOnlyChanged)
    Q_PROPERTY(
        bool wrapping READ wrapping WRITE setWrapping NOTIFY wrappingChanged)
    Q_PROPERTY(bool accelerated READ isAccelerated WRITE setAccelerated NOTIFY
                   acceleratedChanged)
    Q_PROPERTY(bool showButtons READ showButtons WRITE setShowButtons NOTIFY
                   showButtonsChanged)
    Q_PROPERTY(
        bool animated READ isAnimated WRITE setAnimated NOTIFY animatedChanged)

public:
    explicit FluentSpinBox(QWidget* parent = nullptr);
    explicit FluentSpinBox(FluentSpinBoxType type, QWidget* parent = nullptr);
    ~FluentSpinBox() override;

    // Value properties
    double value() const;
    void setValue(double value);

    double minimum() const;
    void setMinimum(double minimum);

    double maximum() const;
    void setMaximum(double maximum);

    void setRange(double minimum, double maximum);

    double singleStep() const;
    void setSingleStep(double step);

    int decimals() const;
    void setDecimals(int decimals);

    // Text properties
    QString prefix() const;
    void setPrefix(const QString& prefix);

    QString suffix() const;
    void setSuffix(const QString& suffix);

    QString placeholderText() const;
    void setPlaceholderText(const QString& text);

    QString text() const;
    QString cleanText() const;

    // Type and appearance
    FluentSpinBoxType spinBoxType() const;
    void setSpinBoxType(FluentSpinBoxType type);

    FluentSpinBoxSize spinBoxSize() const;
    void setSpinBoxSize(FluentSpinBoxSize size);

    FluentSpinBoxButtonLayout buttonLayout() const;
    void setButtonLayout(FluentSpinBoxButtonLayout layout);

    // Behavior properties
    bool isReadOnly() const;
    void setReadOnly(bool readOnly);

    bool wrapping() const;
    void setWrapping(bool wrapping);

    bool isAccelerated() const;
    void setAccelerated(bool accelerated);

    bool showButtons() const;
    void setShowButtons(bool show);

    // Animation
    bool isAnimated() const;
    void setAnimated(bool animated);

    // Validation
    QValidator::State validate(QString& input, int& pos) const;
    void fixup(QString& input) const;

    // Utility methods
    bool isValid() const;
    void selectAll();
    void clear();

    // Size hints
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // Access to internal components
    QLineEdit* lineEdit() const { return m_lineEdit; }
    FluentButton* upButton() const { return m_upButton; }
    FluentButton* downButton() const { return m_downButton; }

    // Convenience factories (expected by tests)
    static FluentSpinBox* createIntegerSpinBox(int minimum, int maximum,
                                               QWidget* parent = nullptr);
    static FluentSpinBox* createDoubleSpinBox(double minimum, double maximum,
                                              int decimals,
                                              QWidget* parent = nullptr);
    static FluentSpinBox* createCurrencySpinBox(double minimum, double maximum,
                                                QWidget* parent = nullptr);
    static FluentSpinBox* createPercentageSpinBox(QWidget* parent = nullptr);

public slots:
    void stepUp();
    void stepDown();
    void stepBy(int steps);
    void animateValue(double targetValue);

signals:
    void valueChanged(double value);
    void minimumChanged(double minimum);
    void maximumChanged(double maximum);
    void singleStepChanged(double step);
    void decimalsChanged(int decimals);
    void prefixChanged(const QString& prefix);
    void suffixChanged(const QString& suffix);
    void placeholderTextChanged(const QString& text);
    void spinBoxTypeChanged(FluentSpinBoxType type);
    void spinBoxSizeChanged(FluentSpinBoxSize size);
    void buttonLayoutChanged(FluentSpinBoxButtonLayout layout);
    void readOnlyChanged(bool readOnly);
    void wrappingChanged(bool wrapping);
    void acceleratedChanged(bool accelerated);
    void showButtonsChanged(bool show);
    void animatedChanged(bool animated);

    void editingFinished();
    void textChanged(const QString& text);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void changeEvent(QEvent* event) override;

private slots:
    void onLineEditTextChanged(const QString& text);
    void onLineEditEditingFinished();
    void onUpButtonPressed();
    void onDownButtonPressed();
    void onUpButtonReleased();
    void onDownButtonReleased();
    void onRepeatTimer();
    void onAccelerationTimer();
    void onValueAnimationFinished();
    void updateButtonStates();

private:
    void setupLayout();
    void setupLineEdit();
    void setupButtons();
    void setupAnimations();
    void setupValidation();
    void updateLayout();
    void updateButtonLayout();
    void updateSizeMetrics();

    // Value handling
    double boundValue(double value) const;
    double parseValue(const QString& text) const;
    QString formatValue(double value) const;
    void updateDisplayText();
    void commitValue();

    // Button handling
    void startRepeatAction(bool increment);
    void stopRepeatAction();
    void performStep(bool increment);

    // Animation methods
    void startValueAnimation(double targetValue);
    void updateValueAnimation();

    // Utility methods
    int getStepMultiplier() const;
    QString getFormattedText(double value) const;
    void updateAccessibility();

private:
    // Core components
    QLineEdit* m_lineEdit{nullptr};
    FluentButton* m_upButton{nullptr};
    FluentButton* m_downButton{nullptr};
    QHBoxLayout* m_mainLayout{nullptr};
    QVBoxLayout* m_buttonsLayout{nullptr};

    // Value properties
    double m_value{0.0};
    double m_minimum{0.0};
    double m_maximum{99.99};
    double m_singleStep{1.0};
    int m_decimals{2};
    QString m_prefix;
    QString m_suffix;
    QString m_placeholderText;

    // Type and appearance
    FluentSpinBoxType m_spinBoxType{FluentSpinBoxType::Double};
    FluentSpinBoxSize m_spinBoxSize{FluentSpinBoxSize::Medium};
    FluentSpinBoxButtonLayout m_buttonLayout{
        FluentSpinBoxButtonLayout::Vertical};

    // Behavior properties
    bool m_readOnly{false};
    bool m_wrapping{false};
    bool m_accelerated{true};
    bool m_showButtons{true};
    bool m_animated{true};

    // State
    bool m_isValid{true};
    bool m_pendingValueUpdate{false};
    double m_pendingValue{0.0};

    // Repeat and acceleration
    QTimer* m_repeatTimer{nullptr};
    QTimer* m_accelerationTimer{nullptr};
    bool m_repeatIncrement{true};
    int m_accelerationLevel{0};
    int m_repeatCount{0};

    // Animation
    std::unique_ptr<Animation::FluentAnimator> m_animator;
    QPropertyAnimation* m_valueAnimation{nullptr};

    // Validation
    QValidator* m_validator{nullptr};

    // Size metrics
    int m_buttonSize{24};
    int m_spacing{2};
};

}  // namespace FluentQt::Components
