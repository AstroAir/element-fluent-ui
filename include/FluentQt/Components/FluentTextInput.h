// include/FluentQt/Components/FluentTextInput.h
#pragma once

#include "FluentQt/Core/FluentComponent.h"
#include "FluentQt/Animation/FluentAnimator.h"
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QTimer>

namespace FluentQt::Components {

enum class FluentTextInputType {
    Text,
    Password,
    Email,
    Number,
    Search,
    Url,
    Tel
};

enum class FluentTextInputState {
    Normal,
    Focused,
    Error,
    Success,
    Disabled
};

class FluentTextInput : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString placeholderText READ placeholderText WRITE setPlaceholderText NOTIFY placeholderTextChanged)
    Q_PROPERTY(QString labelText READ labelText WRITE setLabelText NOTIFY labelTextChanged)
    Q_PROPERTY(QString helperText READ helperText WRITE setHelperText NOTIFY helperTextChanged)
    Q_PROPERTY(QString errorText READ errorText WRITE setErrorText NOTIFY errorTextChanged)
    Q_PROPERTY(FluentTextInputType inputType READ inputType WRITE setInputType NOTIFY inputTypeChanged)
    Q_PROPERTY(FluentTextInputState inputState READ inputState WRITE setInputState NOTIFY inputStateChanged)
    Q_PROPERTY(bool required READ isRequired WRITE setRequired NOTIFY requiredChanged)
    Q_PROPERTY(bool clearButtonVisible READ isClearButtonVisible WRITE setClearButtonVisible NOTIFY clearButtonVisibleChanged)
    Q_PROPERTY(int maxLength READ maxLength WRITE setMaxLength NOTIFY maxLengthChanged)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly NOTIFY readOnlyChanged)

public:
    explicit FluentTextInput(QWidget* parent = nullptr);
    ~FluentTextInput() override;

    // Text properties
    QString text() const;
    void setText(const QString& text);
    
    QString placeholderText() const;
    void setPlaceholderText(const QString& text);
    
    QString labelText() const;
    void setLabelText(const QString& text);
    
    QString helperText() const;
    void setHelperText(const QString& text);
    
    QString errorText() const;
    void setErrorText(const QString& text);

    // Input type and state
    FluentTextInputType inputType() const;
    void setInputType(FluentTextInputType type);
    
    FluentTextInputState inputState() const;
    void setInputState(FluentTextInputState state);

    // Properties
    bool isRequired() const;
    void setRequired(bool required);
    
    bool isClearButtonVisible() const;
    void setClearButtonVisible(bool visible);
    
    int maxLength() const;
    void setMaxLength(int length);
    
    bool isReadOnly() const;
    void setReadOnly(bool readOnly);

    // Validation
    bool isValid() const;
    void validate();
    void clearValidation();

    // Focus management
    void setFocus();
    void clearFocus();

    // Selection
    void selectAll();
    void selectText(int start, int length);
    QString selectedText() const;

public slots:
    void clear();
    void undo();
    void redo();
    void copy();
    void cut();
    void paste();

signals:
    void textChanged(const QString& text);
    void textEdited(const QString& text);
    void placeholderTextChanged(const QString& text);
    void labelTextChanged(const QString& text);
    void helperTextChanged(const QString& text);
    void errorTextChanged(const QString& text);
    void inputTypeChanged(FluentTextInputType type);
    void inputStateChanged(FluentTextInputState state);
    void requiredChanged(bool required);
    void clearButtonVisibleChanged(bool visible);
    void maxLengthChanged(int length);
    void readOnlyChanged(bool readOnly);
    void validationChanged(bool isValid);
    void returnPressed();
    void editingFinished();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void changeEvent(QEvent* event) override;

private slots:
    void onTextChanged(const QString& text);
    void onTextEdited(const QString& text);
    void onReturnPressed();
    void onEditingFinished();
    void onClearButtonClicked();
    void updateAnimations();

private:
    void setupUI();
    void setupAnimations();
    void setupConnections();
    void updateInputType();
    void updateInputState();
    void updateColors();
    void updateFonts();
    void updateGeometry();
    void updateClearButton();
    void animateStateChange();
    void animateFocusChange(bool focused);
    void animateValidationChange();
    
    // Validation helpers
    bool validateEmail(const QString& text) const;
    bool validateUrl(const QString& text) const;
    bool validateNumber(const QString& text) const;
    bool validateRequired(const QString& text) const;

private:
    // UI components
    QVBoxLayout* m_mainLayout{nullptr};
    QHBoxLayout* m_inputLayout{nullptr};
    QLabel* m_label{nullptr};
    QLineEdit* m_lineEdit{nullptr};
    QPushButton* m_clearButton{nullptr};
    QLabel* m_helperLabel{nullptr};
    QLabel* m_errorLabel{nullptr};
    
    // Properties
    FluentTextInputType m_inputType{FluentTextInputType::Text};
    FluentTextInputState m_inputState{FluentTextInputState::Normal};
    bool m_required{false};
    bool m_clearButtonVisible{true};
    bool m_isValid{true};
    
    // Animation
    std::unique_ptr<Animation::FluentAnimator> m_animator;
    QPropertyAnimation* m_focusAnimation{nullptr};
    QPropertyAnimation* m_stateAnimation{nullptr};
    QPropertyAnimation* m_validationAnimation{nullptr};
    QGraphicsOpacityEffect* m_helperOpacityEffect{nullptr};
    QGraphicsOpacityEffect* m_errorOpacityEffect{nullptr};
    
    // Timing
    QTimer* m_validationTimer{nullptr};
};

} // namespace FluentQt::Components
