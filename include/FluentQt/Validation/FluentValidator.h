// include/FluentQt/Validation/FluentValidator.h
#pragma once

#include <QObject>
#include <QWidget>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QRegularExpression>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QMutex>
#include <functional>
#include <memory>
#include <unordered_map>

namespace FluentQt::Validation {

// Validation result
struct FluentValidationResult {
    bool isValid{true};
    QString errorMessage;
    QString warningMessage;
    QString fieldName;
    QVariant validatedValue;
    QString validatorName;
    QStringList suggestions;
    int errorCode{0};
};

// Validation severity levels
enum class FluentValidationSeverity {
    Info,       // Informational message
    Warning,    // Warning but not blocking
    Error,      // Error that prevents submission
    Critical    // Critical error that requires immediate attention
};

// Validation trigger types
enum class FluentValidationTrigger {
    OnChange,       // Validate on every change
    OnBlur,         // Validate when field loses focus
    OnSubmit,       // Validate only on form submission
    OnDemand,       // Validate only when explicitly requested
    Debounced       // Validate after a delay (for async validation)
};

// Base validator interface
class FluentValidatorBase {
public:
    virtual ~FluentValidatorBase() = default;
    virtual FluentValidationResult validate(const QVariant& value) const = 0;
    virtual QString getName() const = 0;
    virtual QString getDescription() const = 0;
    virtual bool isAsync() const { return false; }
};

// Synchronous validator
class FluentSyncValidator : public FluentValidatorBase {
public:
    explicit FluentSyncValidator(const QString& name, 
                                std::function<FluentValidationResult(const QVariant&)> validator);
    
    FluentValidationResult validate(const QVariant& value) const override;
    QString getName() const override { return m_name; }
    QString getDescription() const override { return m_description; }
    
    void setDescription(const QString& description) { m_description = description; }

private:
    QString m_name;
    QString m_description;
    std::function<FluentValidationResult(const QVariant&)> m_validator;
};

// Asynchronous validator
class FluentAsyncValidator : public QObject, public FluentValidatorBase {
    Q_OBJECT

public:
    explicit FluentAsyncValidator(const QString& name, QObject* parent = nullptr);
    
    FluentValidationResult validate(const QVariant& value) const override;
    QString getName() const override { return m_name; }
    QString getDescription() const override { return m_description; }
    bool isAsync() const override { return true; }
    
    void setDescription(const QString& description) { m_description = description; }
    void setTimeout(int milliseconds) { m_timeout = milliseconds; }
    
    // Async validation function
    void setAsyncValidator(std::function<void(const QVariant&, std::function<void(FluentValidationResult)>)> validator);

signals:
    void validationCompleted(const FluentValidationResult& result);
    void validationTimeout();

protected:
    virtual void performAsyncValidation(const QVariant& value) = 0;

private slots:
    void onValidationTimeout();

private:
    QString m_name;
    QString m_description;
    int m_timeout{5000}; // 5 seconds default
    std::function<void(const QVariant&, std::function<void(FluentValidationResult)>)> m_asyncValidator;
    mutable QTimer* m_timeoutTimer;
};

// Built-in validators
class FluentBuiltInValidators {
public:
    // Basic validators
    static std::unique_ptr<FluentValidatorBase> required(const QString& message = "This field is required");
    static std::unique_ptr<FluentValidatorBase> minLength(int length, const QString& message = "");
    static std::unique_ptr<FluentValidatorBase> maxLength(int length, const QString& message = "");
    static std::unique_ptr<FluentValidatorBase> exactLength(int length, const QString& message = "");
    
    // Numeric validators
    static std::unique_ptr<FluentValidatorBase> minValue(double min, const QString& message = "");
    static std::unique_ptr<FluentValidatorBase> maxValue(double max, const QString& message = "");
    static std::unique_ptr<FluentValidatorBase> range(double min, double max, const QString& message = "");
    static std::unique_ptr<FluentValidatorBase> integer(const QString& message = "Must be an integer");
    static std::unique_ptr<FluentValidatorBase> decimal(int places = 2, const QString& message = "");
    
    // Pattern validators
    static std::unique_ptr<FluentValidatorBase> regex(const QRegularExpression& pattern, const QString& message = "");
    static std::unique_ptr<FluentValidatorBase> email(const QString& message = "Invalid email address");
    static std::unique_ptr<FluentValidatorBase> url(const QString& message = "Invalid URL");
    static std::unique_ptr<FluentValidatorBase> phone(const QString& message = "Invalid phone number");
    static std::unique_ptr<FluentValidatorBase> creditCard(const QString& message = "Invalid credit card number");
    
    // Date/time validators
    static std::unique_ptr<FluentValidatorBase> date(const QString& format = "yyyy-MM-dd", const QString& message = "");
    static std::unique_ptr<FluentValidatorBase> time(const QString& format = "hh:mm:ss", const QString& message = "");
    static std::unique_ptr<FluentValidatorBase> dateRange(const QDate& min, const QDate& max, const QString& message = "");
    static std::unique_ptr<FluentValidatorBase> futureDate(const QString& message = "Date must be in the future");
    static std::unique_ptr<FluentValidatorBase> pastDate(const QString& message = "Date must be in the past");
    
    // File validators
    static std::unique_ptr<FluentValidatorBase> fileSize(qint64 maxSize, const QString& message = "");
    static std::unique_ptr<FluentValidatorBase> fileExtension(const QStringList& extensions, const QString& message = "");
    static std::unique_ptr<FluentValidatorBase> imageFile(const QString& message = "Must be a valid image file");
    
    // Custom validators
    static std::unique_ptr<FluentValidatorBase> custom(std::function<FluentValidationResult(const QVariant&)> validator,
                                                      const QString& name = "Custom");
    
    // Async validators
    static std::unique_ptr<FluentAsyncValidator> asyncEmail(const QString& message = "Email address is not available");
    static std::unique_ptr<FluentAsyncValidator> asyncUsername(const QString& message = "Username is not available");
    static std::unique_ptr<FluentAsyncValidator> asyncUrl(const QString& message = "URL is not accessible");
};

// Validation rule that combines multiple validators
class FluentValidationRule {
public:
    explicit FluentValidationRule(const QString& fieldName);
    
    // Add validators
    FluentValidationRule& addValidator(std::unique_ptr<FluentValidatorBase> validator);
    FluentValidationRule& required(const QString& message = "");
    FluentValidationRule& minLength(int length, const QString& message = "");
    FluentValidationRule& maxLength(int length, const QString& message = "");
    FluentValidationRule& email(const QString& message = "");
    FluentValidationRule& regex(const QRegularExpression& pattern, const QString& message = "");
    
    // Conditional validation
    FluentValidationRule& when(std::function<bool()> condition);
    FluentValidationRule& unless(std::function<bool()> condition);
    
    // Validation execution
    QList<FluentValidationResult> validate(const QVariant& value) const;
    bool isValid(const QVariant& value) const;
    
    // Configuration
    void setStopOnFirstError(bool stop) { m_stopOnFirstError = stop; }
    void setSeverity(FluentValidationSeverity severity) { m_severity = severity; }
    void setTrigger(FluentValidationTrigger trigger) { m_trigger = trigger; }
    
    QString getFieldName() const { return m_fieldName; }
    FluentValidationSeverity getSeverity() const { return m_severity; }
    FluentValidationTrigger getTrigger() const { return m_trigger; }

private:
    QString m_fieldName;
    QList<std::unique_ptr<FluentValidatorBase>> m_validators;
    std::function<bool()> m_condition;
    bool m_stopOnFirstError{true};
    FluentValidationSeverity m_severity{FluentValidationSeverity::Error};
    FluentValidationTrigger m_trigger{FluentValidationTrigger::OnChange};
};

// Form validator that manages multiple fields
class FluentFormValidator : public QObject {
    Q_OBJECT

public:
    explicit FluentFormValidator(QObject* parent = nullptr);
    
    // Rule management
    void addRule(const QString& fieldName, const FluentValidationRule& rule);
    void removeRule(const QString& fieldName);
    FluentValidationRule& getRule(const QString& fieldName);
    bool hasRule(const QString& fieldName) const;
    
    // Field validation
    QList<FluentValidationResult> validateField(const QString& fieldName, const QVariant& value);
    bool isFieldValid(const QString& fieldName, const QVariant& value);
    
    // Form validation
    QList<FluentValidationResult> validateForm(const QVariantMap& formData);
    bool isFormValid(const QVariantMap& formData);
    
    // Cross-field validation
    void addCrossFieldRule(const QString& ruleName, 
                          const QStringList& fieldNames,
                          std::function<FluentValidationResult(const QVariantMap&)> validator);
    void removeCrossFieldRule(const QString& ruleName);
    
    // Validation triggers
    void setGlobalTrigger(FluentValidationTrigger trigger);
    void setFieldTrigger(const QString& fieldName, FluentValidationTrigger trigger);
    
    // Debounced validation
    void setDebounceDelay(int milliseconds) { m_debounceDelay = milliseconds; }
    void validateFieldDebounced(const QString& fieldName, const QVariant& value);
    
    // Error management
    QStringList getFieldErrors(const QString& fieldName) const;
    QStringList getAllErrors() const;
    void clearFieldErrors(const QString& fieldName);
    void clearAllErrors();
    
    // Configuration
    void setStopOnFirstError(bool stop) { m_stopOnFirstError = stop; }
    void setValidateOnSubmit(bool validate) { m_validateOnSubmit = validate; }

signals:
    void fieldValidated(const QString& fieldName, const QList<FluentValidationResult>& results);
    void formValidated(const QList<FluentValidationResult>& results);
    void validationStateChanged(bool isValid);
    void fieldErrorsChanged(const QString& fieldName, const QStringList& errors);

private slots:
    void onDebounceTimer();
    void onAsyncValidationCompleted(const FluentValidationResult& result);

private:
    void updateValidationState();

private:
    std::unordered_map<QString, FluentValidationRule> m_rules;
    std::unordered_map<QString, std::function<FluentValidationResult(const QVariantMap&)>> m_crossFieldRules;
    std::unordered_map<QString, QStringList> m_fieldErrors;
    
    // Debouncing
    QTimer* m_debounceTimer;
    int m_debounceDelay{300};
    QString m_pendingFieldName;
    QVariant m_pendingValue;
    
    // Configuration
    bool m_stopOnFirstError{false};
    bool m_validateOnSubmit{true};
    bool m_isFormValid{true};
    
    // Thread safety
    mutable QMutex m_rulesMutex;
    mutable QMutex m_errorsMutex;
};

// Widget integration for automatic validation
class FluentValidationIntegrator : public QObject {
    Q_OBJECT

public:
    explicit FluentValidationIntegrator(QObject* parent = nullptr);
    
    // Widget registration
    void registerWidget(const QString& fieldName, QWidget* widget, const FluentValidationRule& rule);
    void unregisterWidget(const QString& fieldName);
    void unregisterWidget(QWidget* widget);
    
    // Automatic validation setup
    void setupAutoValidation(QWidget* widget, FluentValidationTrigger trigger = FluentValidationTrigger::OnChange);
    void enableRealTimeValidation(bool enabled = true);
    
    // Visual feedback
    void setErrorStyle(const QString& styleSheet);
    void setWarningStyle(const QString& styleSheet);
    void setValidStyle(const QString& styleSheet);
    void showValidationFeedback(QWidget* widget, const FluentValidationResult& result);
    void clearValidationFeedback(QWidget* widget);
    
    // Form integration
    void setFormValidator(FluentFormValidator* validator);
    FluentFormValidator* getFormValidator() const { return m_formValidator; }

signals:
    void widgetValidated(QWidget* widget, const FluentValidationResult& result);

private slots:
    void onWidgetValueChanged();
    void onWidgetFocusLost();

private:
    void connectWidgetSignals(QWidget* widget, FluentValidationTrigger trigger);
    QVariant getWidgetValue(QWidget* widget) const;
    void applyValidationStyle(QWidget* widget, const FluentValidationResult& result);

private:
    FluentFormValidator* m_formValidator{nullptr};
    std::unordered_map<QString, QWidget*> m_registeredWidgets;
    std::unordered_map<QWidget*, QString> m_widgetToField;
    
    QString m_errorStyle;
    QString m_warningStyle;
    QString m_validStyle;
    bool m_realTimeValidation{true};
};

// Convenience macros
#define FLUENT_VALIDATE_FIELD(validator, fieldName, value) \
    validator->validateField(fieldName, value)

#define FLUENT_VALIDATE_FORM(validator, formData) \
    validator->validateForm(formData)

#define FLUENT_REQUIRED(message) \
    FluentQt::Validation::FluentBuiltInValidators::required(message)

#define FLUENT_EMAIL(message) \
    FluentQt::Validation::FluentBuiltInValidators::email(message)

#define FLUENT_MIN_LENGTH(length, message) \
    FluentQt::Validation::FluentBuiltInValidators::minLength(length, message)

} // namespace FluentQt::Validation
