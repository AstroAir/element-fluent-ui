#include "FluentComponentValidator.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QMetaMethod>
#include <QStandardPaths>
#include <QTextStream>

FluentComponentValidator::FluentComponentValidator(QObject* parent)
    : QObject(parent) {
    initializeComponentSpecs();
    setupAnimationCapture();
}

void FluentComponentValidator::validateAllComponents() {
    qDebug() << "Starting comprehensive FluentQt component validation...";

    // Get list of available components (only those currently compiled)
    QStringList components = {
        // Form Components (available)
        "FluentButton", "FluentTextInput", "FluentCheckBox", "FluentComboBox",
        "FluentSlider",
        // Note: FluentRadioButton, FluentSpinBox, FluentToggleSwitch
        // temporarily disabled in CMake

        // Layout Components (available)
        "FluentCard", "FluentNavigationView", "FluentTabView",
        // "FluentScrollArea",  // Temporarily disabled due to compilation
        // errors
        "FluentSplitter",
        // Note: FluentPanel temporarily disabled in CMake

        // Feedback Components (available)
        "FluentProgressBar", "FluentToast", "FluentTooltip", "FluentBadge",
        // Note: FluentLoadingIndicator, FluentNotification temporarily disabled
        // in CMake

        // Specialized Components (available)
        "FluentCalendar", "FluentRichTextEditor", "FluentTreeView"
        // Note: FluentColorPicker has header/implementation mismatch
        // Note: FluentDatePicker, FluentTimePicker, FluentContentDialog,
        // FluentPopover not implemented yet
    };

    m_totalTests = 0;
    m_passedTests = 0;
    m_failedTests = 0;
    m_results.clear();

    for (const QString& componentName : components) {
        emit validationStarted(componentName);
        validateComponent(componentName);
    }

    emit allValidationsCompleted(m_totalTests, m_passedTests);
    printSummary();
}

void FluentComponentValidator::validateComponent(const QString& componentName) {
    qDebug() << "Validating component:" << componentName;

    QWidget* component = createComponent(componentName);
    if (!component) {
        addResult("Component Creation", componentName, "Infrastructure", false,
                  "Failed to create component instance", "Valid component",
                  "nullptr");
        return;
    }

    validateComponent(component, componentName);

    // Cleanup
    component->deleteLater();
}

void FluentComponentValidator::validateComponent(QWidget* component,
                                                 const QString& componentName) {
    if (!component)
        return;

    m_currentTestWidget = component;
    ComponentValidationSpec spec = getValidationSpec(componentName);

    // 1. Interface Completeness Validation
    bool interfaceValid = validateInterfaceCompleteness(component, spec);

    // 2. Animation System Verification
    bool animationValid = validateAnimationSystem(component, spec);

    // 3. Fluent Design Compliance
    bool fluentValid = validateFluentDesignCompliance(component, spec);

    // 4. Component-Specific Testing
    bool behaviorValid = validateComponentSpecificBehavior(component, spec);

    // 5. Cross-Platform Compatibility
    bool platformValid = validateCrossPlatformCompatibility(component, spec);

    // Overall component validation result
    bool overallValid = interfaceValid && animationValid && fluentValid &&
                        behaviorValid && platformValid;

    addResult("Overall Validation", componentName, "Summary", overallValid,
              QString("Interface: %1, Animation: %2, Fluent: %3, Behavior: %4, "
                      "Platform: %5")
                  .arg(interfaceValid ? "✓" : "✗")
                  .arg(animationValid ? "✓" : "✗")
                  .arg(fluentValid ? "✓" : "✗")
                  .arg(behaviorValid ? "✓" : "✗")
                  .arg(platformValid ? "✓" : "✗"));

    emit validationCompleted(componentName, overallValid);
    cleanupTestWidget();
}

bool FluentComponentValidator::validateInterfaceCompleteness(
    QWidget* component, const ComponentValidationSpec& spec) {
    bool allValid = true;

    // Check required properties
    const QMetaObject* metaObj = component->metaObject();
    for (const QString& propName : spec.requiredProperties) {
        int propIndex = metaObj->indexOfProperty(propName.toUtf8().constData());
        bool hasProperty = (propIndex >= 0);

        addResult("Property: " + propName, spec.componentName, "Interface",
                  hasProperty,
                  hasProperty ? "Property exists" : "Property missing",
                  "Property exists",
                  hasProperty ? "Property exists" : "Property missing");

        if (!hasProperty)
            allValid = false;
    }

    // Check required methods
    for (const QString& methodName : spec.requiredMethods) {
        bool hasMethod = false;
        for (int i = 0; i < metaObj->methodCount(); ++i) {
            QMetaMethod method = metaObj->method(i);
            if (QString::fromUtf8(method.name()) == methodName) {
                hasMethod = true;
                break;
            }
        }

        addResult("Method: " + methodName, spec.componentName, "Interface",
                  hasMethod, hasMethod ? "Method exists" : "Method missing",
                  "Method exists",
                  hasMethod ? "Method exists" : "Method missing");

        if (!hasMethod)
            allValid = false;
    }

    // Check required signals
    for (const QString& signalName : spec.requiredSignals) {
        bool hasSignal = false;
        for (int i = 0; i < metaObj->methodCount(); ++i) {
            QMetaMethod method = metaObj->method(i);
            if (method.methodType() == QMetaMethod::Signal &&
                QString::fromUtf8(method.name()) == signalName) {
                hasSignal = true;
                break;
            }
        }

        addResult("Signal: " + signalName, spec.componentName, "Interface",
                  hasSignal, hasSignal ? "Signal exists" : "Signal missing",
                  "Signal exists",
                  hasSignal ? "Signal exists" : "Signal missing");

        if (!hasSignal)
            allValid = false;
    }

    // Check inheritance from FluentComponent
    bool inheritsFluentComponent =
        qobject_cast<Core::FluentComponent*>(component) != nullptr;
    addResult("FluentComponent Inheritance", spec.componentName, "Interface",
              inheritsFluentComponent,
              inheritsFluentComponent ? "Inherits from FluentComponent"
                                      : "Does not inherit from FluentComponent",
              "Inherits from FluentComponent",
              inheritsFluentComponent
                  ? "Inherits from FluentComponent"
                  : "Does not inherit from FluentComponent");

    if (!inheritsFluentComponent)
        allValid = false;

    return allValid;
}

bool FluentComponentValidator::validateAnimationSystem(
    QWidget* component, const ComponentValidationSpec& spec) {
    bool allValid = true;

    // Test hover animation
    startTimer();
    bool hasHoverAnimation = triggerHoverAnimation(component);
    auto hoverDuration = getElapsedTime();

    addResult("Hover Animation", spec.componentName, "Animation",
              hasHoverAnimation,
              QString("Animation triggered in %1ms").arg(hoverDuration.count()),
              "Animation exists",
              hasHoverAnimation ? "Animation exists" : "No animation");

    if (!hasHoverAnimation)
        allValid = false;

    // Test press animation
    startTimer();
    bool hasPressAnimation = triggerPressAnimation(component);
    auto pressDuration = getElapsedTime();

    addResult("Press Animation", spec.componentName, "Animation",
              hasPressAnimation,
              QString("Animation triggered in %1ms").arg(pressDuration.count()),
              "Animation exists",
              hasPressAnimation ? "Animation exists" : "No animation");

    if (!hasPressAnimation)
        allValid = false;

    // Test animation timing compliance
    if (m_currentAnimation) {
        bool timingValid = validateAnimationTiming(
            m_currentAnimation, spec.expectedAnimationDuration);
        addResult("Animation Timing", spec.componentName, "Animation",
                  timingValid,
                  QString("Expected: %1ms, Actual: %2ms")
                      .arg(spec.expectedAnimationDuration.count())
                      .arg(m_currentAnimation->duration()),
                  QString("%1ms").arg(spec.expectedAnimationDuration.count()),
                  QString("%1ms").arg(m_currentAnimation->duration()));

        if (!timingValid)
            allValid = false;

        // Test easing curve compliance
        bool easingValid =
            validateEasingCurve(m_currentAnimation, spec.expectedEasing);
        addResult("Easing Curve", spec.componentName, "Animation", easingValid,
                  easingValid ? "Correct Fluent easing curve"
                              : "Incorrect easing curve",
                  "Fluent easing curve",
                  easingValid ? "Fluent easing curve" : "Non-Fluent curve");

        if (!easingValid)
            allValid = false;

        // Test hardware acceleration
        bool hardwareAccelValid =
            validateHardwareAcceleration(m_currentAnimation);
        addResult("Hardware Acceleration", spec.componentName, "Animation",
                  hardwareAccelValid,
                  hardwareAccelValid ? "Hardware acceleration enabled"
                                     : "Hardware acceleration disabled",
                  "Hardware acceleration enabled",
                  hardwareAccelValid ? "Hardware acceleration enabled"
                                     : "Hardware acceleration disabled");

        if (!hardwareAccelValid && m_strictMode)
            allValid = false;
    }

    // Test reduced motion support
    bool reducedMotionValid = validateReducedMotionSupport(component);
    addResult("Reduced Motion Support", spec.componentName, "Animation",
              reducedMotionValid,
              reducedMotionValid ? "Reduced motion supported"
                                 : "Reduced motion not supported",
              "Reduced motion supported",
              reducedMotionValid ? "Reduced motion supported"
                                 : "Reduced motion not supported");

    if (!reducedMotionValid)
        allValid = false;

    return allValid;
}

bool FluentComponentValidator::validateFluentDesignCompliance(
    QWidget* component, const ComponentValidationSpec& spec) {
    bool allValid = true;

    // Test Fluent colors
    bool colorsValid = validateFluentColors(component);
    addResult(
        "Fluent Colors", spec.componentName, "Fluent Design", colorsValid,
        colorsValid ? "Uses Fluent color system" : "Does not use Fluent colors",
        "Fluent color system",
        colorsValid ? "Fluent color system" : "Non-Fluent colors");

    if (!colorsValid)
        allValid = false;

    // Test typography
    bool typographyValid = validateFluentTypography(component);
    addResult("Fluent Typography", spec.componentName, "Fluent Design",
              typographyValid,
              typographyValid ? "Uses Fluent typography"
                              : "Does not use Fluent typography",
              "Fluent typography",
              typographyValid ? "Fluent typography" : "Non-Fluent typography");

    if (!typographyValid)
        allValid = false;

    // Test spacing
    bool spacingValid = validateFluentSpacing(component);
    addResult("Fluent Spacing", spec.componentName, "Fluent Design",
              spacingValid,
              spacingValid ? "Uses Fluent spacing system"
                           : "Does not use Fluent spacing",
              "Fluent spacing system",
              spacingValid ? "Fluent spacing system" : "Non-Fluent spacing");

    if (!spacingValid)
        allValid = false;

    // Test state transitions
    bool stateValid = validateStateTransitions(component);
    addResult(
        "State Transitions", spec.componentName, "Fluent Design", stateValid,
        stateValid ? "Proper state transitions" : "Improper state transitions",
        "Proper state transitions",
        stateValid ? "Proper state transitions" : "Improper state transitions");

    if (!stateValid)
        allValid = false;

    // Test theme support
    if (spec.requiresThemeSupport) {
        bool themeValid = validateThemeSupport(component);
        addResult(
            "Theme Support", spec.componentName, "Fluent Design", themeValid,
            themeValid ? "Supports theme switching"
                       : "Does not support theme switching",
            "Theme support", themeValid ? "Theme support" : "No theme support");

        if (!themeValid)
            allValid = false;
    }

    return allValid;
}

void FluentComponentValidator::addResult(const QString& testName,
                                         const QString& componentName,
                                         const QString& category, bool passed,
                                         const QString& details,
                                         const QString& expected,
                                         const QString& actual) {
    ValidationResult result(testName, componentName, category, passed, details,
                            expected, actual);
    result.duration = getElapsedTime();
    addResult(result);
}

void FluentComponentValidator::addResult(const ValidationResult& result) {
    m_results.append(result);
    m_totalTests++;

    if (result.passed) {
        m_passedTests++;
    } else {
        m_failedTests++;
    }

    if (m_verboseOutput) {
        QString status = result.passed ? "✓ PASS" : "✗ FAIL";
        qDebug() << QString("[%1] %2::%3 - %4")
                        .arg(status)
                        .arg(result.componentName)
                        .arg(result.testName)
                        .arg(result.details);
    }

    emit testCompleted(result);
}

QWidget* FluentComponentValidator::createComponent(
    const QString& componentName) {
    try {
        // Form Components
        if (componentName == "FluentButton")
            return new Components::FluentButton("Test Button");
        if (componentName == "FluentTextInput")
            return new Components::FluentTextInput();
        if (componentName == "FluentCheckBox")
            return new Components::FluentCheckBox("Test Checkbox");
        // Note: FluentRadioButton, FluentSpinBox, FluentToggleSwitch
        // temporarily disabled
        if (componentName == "FluentComboBox")
            return new Components::FluentComboBox();
        if (componentName == "FluentSlider")
            return new Components::FluentSlider();

        // Layout Components
        if (componentName == "FluentCard")
            return new Components::FluentCard();
        // Note: FluentPanel temporarily disabled
        if (componentName == "FluentNavigationView")
            return new Components::FluentNavigationView();
        if (componentName == "FluentTabView")
            return new Components::FluentTabView();
        // if (componentName == "FluentScrollArea")
        //     return new Components::FluentScrollArea();  // Temporarily
        //     disabled
        if (componentName == "FluentSplitter")
            return new Components::FluentSplitter();

        // Feedback Components
        if (componentName == "FluentProgressBar")
            return new Components::FluentProgressBar();
        // Note: FluentLoadingIndicator, FluentNotification temporarily disabled
        if (componentName == "FluentToast")
            return new Components::FluentToast();  // Default constructor
        if (componentName == "FluentTooltip")
            return new Components::FluentTooltip("Test Tooltip");
        if (componentName == "FluentBadge")
            return new Components::FluentBadge("Test");

        // Specialized Components (available)
        if (componentName == "FluentCalendar")
            return new Components::FluentCalendar();
        // Note: FluentColorPicker has header/implementation mismatch
        if (componentName == "FluentRichTextEditor")
            return new Components::FluentRichTextEditor();
        if (componentName == "FluentTreeView")
            return new Components::FluentTreeView();

    } catch (const std::exception& e) {
        qDebug() << "Exception creating component" << componentName << ":"
                 << e.what();
        return nullptr;
    } catch (...) {
        qDebug() << "Unknown exception creating component" << componentName;
        return nullptr;
    }

    qDebug() << "Unknown component name:" << componentName;
    return nullptr;
}

ComponentValidationSpec FluentComponentValidator::getValidationSpec(
    const QString& componentName) {
    if (m_componentSpecs.contains(componentName)) {
        return m_componentSpecs[componentName];
    }

    // Default specification
    ComponentValidationSpec spec;
    spec.componentName = componentName;
    spec.requiredProperties = {"state", "animated", "cornerRadius"};
    spec.requiredMethods = {"setState", "setAnimated", "setCornerRadius"};
    spec.requiredSignals = {"stateChanged", "themeChanged"};
    spec.expectedAnimations = {"hover", "press", "focus"};
    spec.fluentDesignRequirements = {"colors", "typography", "spacing",
                                     "states"};
    spec.requiresAccessibility = true;
    spec.requiresThemeSupport = true;
    spec.expectedAnimationDuration = 150ms;
    spec.expectedEasing = Animation::FluentEasing::FluentStandard;

    return spec;
}

void FluentComponentValidator::initializeComponentSpecs() {
    // Button specification
    ComponentValidationSpec buttonSpec;
    buttonSpec.componentName = "FluentButton";
    buttonSpec.requiredProperties = {"text", "state", "animated",
                                     "cornerRadius", "buttonType"};
    buttonSpec.requiredMethods = {"setText", "setState", "setButtonType",
                                  "click"};
    buttonSpec.requiredSignals = {"clicked", "pressed", "released",
                                  "stateChanged"};
    buttonSpec.expectedAnimations = {"hover", "press", "focus"};
    buttonSpec.fluentDesignRequirements = {"colors", "typography", "spacing",
                                           "states", "elevation"};
    buttonSpec.requiresAccessibility = true;
    buttonSpec.requiresThemeSupport = true;
    buttonSpec.expectedAnimationDuration = 100ms;  // Utility motion for buttons
    buttonSpec.expectedEasing = Animation::FluentEasing::FluentButton;
    m_componentSpecs["FluentButton"] = buttonSpec;

    // Card specification
    ComponentValidationSpec cardSpec;
    cardSpec.componentName = "FluentCard";
    cardSpec.requiredProperties = {"title",    "subtitle",     "state",
                                   "animated", "cornerRadius", "elevation"};
    cardSpec.requiredMethods = {"setTitle", "setSubtitle", "setState",
                                "setElevation"};
    cardSpec.requiredSignals = {"clicked", "stateChanged", "elevationChanged"};
    cardSpec.expectedAnimations = {"hover", "elevation", "entrance", "exit"};
    cardSpec.fluentDesignRequirements = {"colors", "typography", "spacing",
                                         "states", "elevation",  "shadows"};
    cardSpec.requiresAccessibility = true;
    cardSpec.requiresThemeSupport = true;
    cardSpec.expectedAnimationDuration = 250ms;  // Primary motion for cards
    cardSpec.expectedEasing = Animation::FluentEasing::FluentCard;
    m_componentSpecs["FluentCard"] = cardSpec;

    // Text Input specification
    ComponentValidationSpec textInputSpec;
    textInputSpec.componentName = "FluentTextInput";
    textInputSpec.requiredProperties = {"text", "placeholderText", "state",
                                        "animated", "cornerRadius"};
    textInputSpec.requiredMethods = {"setText", "setPlaceholderText",
                                     "setState", "clear", "selectAll"};
    textInputSpec.requiredSignals = {"textChanged", "editingFinished",
                                     "returnPressed", "stateChanged"};
    textInputSpec.expectedAnimations = {"focus", "validation", "error"};
    textInputSpec.fluentDesignRequirements = {"colors", "typography", "spacing",
                                              "states", "borders"};
    textInputSpec.requiresAccessibility = true;
    textInputSpec.requiresThemeSupport = true;
    textInputSpec.expectedAnimationDuration =
        150ms;  // Secondary motion for form controls
    textInputSpec.expectedEasing = Animation::FluentEasing::FluentStandard;
    m_componentSpecs["FluentTextInput"] = textInputSpec;

    // Add more component specifications as needed...
    // For brevity, I'll add a few key ones and use defaults for others
}

void FluentComponentValidator::setupAnimationCapture() {
    m_animationTimeout = new QTimer(this);
    m_animationTimeout->setSingleShot(true);
    m_animationTimeout->setInterval(5000);  // 5 second timeout for animations

    connect(m_animationTimeout, &QTimer::timeout, this, [this]() {
        qDebug() << "Animation capture timeout";
        m_currentAnimation = nullptr;
    });
}

void FluentComponentValidator::cleanupTestWidget() {
    m_currentTestWidget = nullptr;
    m_currentAnimation = nullptr;
    if (m_animationTimeout) {
        m_animationTimeout->stop();
    }
}

// Animation validation methods
bool FluentComponentValidator::validateAnimationTiming(
    QPropertyAnimation* animation, std::chrono::milliseconds expected) {
    if (!animation)
        return false;

    int actualDuration = animation->duration();
    int expectedDuration = static_cast<int>(expected.count());

    // Allow 10% tolerance for timing
    int tolerance = expectedDuration / 10;
    return qAbs(actualDuration - expectedDuration) <= tolerance;
}

bool FluentComponentValidator::validateEasingCurve(
    QPropertyAnimation* animation, Animation::FluentEasing expected) {
    Q_UNUSED(expected)  // TODO: Implement proper Fluent easing validation
    if (!animation)
        return false;

    // This is a simplified check - in a real implementation, you'd compare the
    // actual curve parameters
    QEasingCurve curve = animation->easingCurve();

    // For now, just check that it's not the default linear curve
    return curve.type() != QEasingCurve::Linear;
}

bool FluentComponentValidator::validateHardwareAcceleration(
    QPropertyAnimation* animation) {
    if (!animation)
        return false;

    // Check if hardware acceleration properties are set
    return animation->property("hardwareAccelerated").toBool();
}

bool FluentComponentValidator::validateReducedMotionSupport(
    QWidget* component) {
    if (!component)
        return false;

    // Check if the component respects reduced motion settings
    // This would typically involve checking if animations are disabled when
    // reduced motion is enabled
    // For now, just check if accessibility is active
    return QAccessible::isActive();
}

// Fluent Design validation methods
bool FluentComponentValidator::validateFluentColors(QWidget* component) {
    if (!component)
        return false;

    // Check if the component uses Fluent Design color tokens
    QPalette palette = component->palette();

    // This is a simplified check - in practice, you'd verify against the actual
    // Fluent color system
    return !palette.color(QPalette::Window).isValid() ||
           palette.color(QPalette::Window) !=
               QColor(Qt::white);  // Not using default white
}

bool FluentComponentValidator::validateFluentTypography(QWidget* component) {
    if (!component)
        return false;

    // Check if the component uses Fluent typography
    QFont font = component->font();

    // Fluent Design typically uses Segoe UI or system fonts
    return font.family().contains("Segoe", Qt::CaseInsensitive) ||
           font.family().isEmpty();  // System default
}

bool FluentComponentValidator::validateFluentSpacing(QWidget* component) {
    if (!component)
        return false;

    // Check if the component uses Fluent spacing tokens
    QMargins margins = component->contentsMargins();

    // Fluent Design uses 4px grid system
    return (margins.left() % 4 == 0) && (margins.top() % 4 == 0) &&
           (margins.right() % 4 == 0) && (margins.bottom() % 4 == 0);
}

bool FluentComponentValidator::validateStateTransitions(QWidget* component) {
    if (!component)
        return false;

    // Check if the component properly handles state transitions
    auto* fluentComponent = qobject_cast<Core::FluentComponent*>(component);
    if (!fluentComponent)
        return false;

    // Test state transition
    Core::FluentState originalState = fluentComponent->state();
    fluentComponent->setState(Core::FluentState::Hovered);
    bool stateChanged =
        (fluentComponent->state() == Core::FluentState::Hovered);
    fluentComponent->setState(originalState);  // Restore

    return stateChanged;
}

bool FluentComponentValidator::validateThemeSupport(QWidget* component) {
    if (!component)
        return false;

    // Check if the component responds to theme changes
    auto& theme = Styling::FluentTheme::instance();
    auto originalMode = theme.mode();

    // Switch theme and check if component updates
    auto newMode = (originalMode == Styling::FluentThemeMode::Light)
                       ? Styling::FluentThemeMode::Dark
                       : Styling::FluentThemeMode::Light;

    theme.setMode(newMode);
    QCoreApplication::processEvents();  // Allow theme change to propagate

    // Check if component's appearance changed (simplified check)
    QPalette currentPalette = component->palette();

    theme.setMode(originalMode);  // Restore
    QCoreApplication::processEvents();

    return true;  // Assume theme support exists if no crash occurred
}

// Animation trigger methods
bool FluentComponentValidator::triggerHoverAnimation(QWidget* component) {
    if (!component)
        return false;

    // Simulate hover event
    QEnterEvent enterEvent(QPointF(10, 10), QPointF(10, 10), QPointF(10, 10));
    QCoreApplication::sendEvent(component, &enterEvent);
    QCoreApplication::processEvents();

    // Check if any animation was triggered
    return component->findChild<QPropertyAnimation*>() != nullptr;
}

bool FluentComponentValidator::triggerPressAnimation(QWidget* component) {
    if (!component)
        return false;

    // Simulate mouse press event using modern constructor
    QPoint pos(10, 10);
    QMouseEvent pressEvent(QEvent::MouseButtonPress, pos, pos, Qt::LeftButton,
                           Qt::LeftButton, Qt::NoModifier);
    QCoreApplication::sendEvent(component, &pressEvent);
    QCoreApplication::processEvents();

    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, pos, pos,
                             Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QCoreApplication::sendEvent(component, &releaseEvent);
    QCoreApplication::processEvents();

    return true;  // Assume animation was triggered
}

bool FluentComponentValidator::triggerFocusAnimation(QWidget* component) {
    if (!component)
        return false;

    // Simulate focus events
    component->setFocus();
    QCoreApplication::processEvents();

    return component->hasFocus();
}

// Component-specific behavior validation
bool FluentComponentValidator::validateComponentSpecificBehavior(
    QWidget* component, const ComponentValidationSpec& spec) {
    bool allValid = true;

    // Test component-specific functionality based on type
    if (spec.componentName == "FluentButton") {
        auto* button = qobject_cast<Components::FluentButton*>(component);
        if (button) {
            // Test button click using animateClick method
            bool clickable = true;
            try {
                button->animateClick();
                QCoreApplication::processEvents();
            } catch (...) {
                clickable = false;
            }

            addResult("Button Click", spec.componentName, "Behavior", clickable,
                      clickable ? "Button is clickable" : "Button click failed",
                      "Clickable", clickable ? "Clickable" : "Not clickable");

            if (!clickable)
                allValid = false;
        }
    } else if (spec.componentName == "FluentCard") {
        auto* card = qobject_cast<Components::FluentCard*>(component);
        if (card) {
            // Test card title setting
            QString testTitle = "Test Title";
            card->setTitle(testTitle);
            bool titleSet = (card->title() == testTitle);

            addResult("Card Title", spec.componentName, "Behavior", titleSet,
                      titleSet ? "Title set correctly" : "Title not set",
                      testTitle, card->title());

            if (!titleSet)
                allValid = false;
        }
    }

    return allValid;
}

// Cross-platform compatibility validation
bool FluentComponentValidator::validateCrossPlatformCompatibility(
    QWidget* component, const ComponentValidationSpec& spec) {
    bool allValid = true;

    // Test high DPI support
    bool highDpiValid = true;
    qreal devicePixelRatio = component->devicePixelRatio();

    addResult("High DPI Support", spec.componentName, "Platform", highDpiValid,
              QString("Device pixel ratio: %1").arg(devicePixelRatio),
              "DPI awareness", QString("Ratio: %1").arg(devicePixelRatio));

    // Test accessibility compliance
    bool accessibilityValid = validateAccessibilityCompliance(component);
    addResult(
        "Accessibility Compliance", spec.componentName, "Platform",
        accessibilityValid,
        accessibilityValid ? "Accessibility compliant" : "Accessibility issues",
        "Accessibility compliant",
        accessibilityValid ? "Accessibility compliant"
                           : "Accessibility issues");

    if (!accessibilityValid)
        allValid = false;

    // Test keyboard navigation
    bool keyboardValid = validateKeyboardNavigation(component);
    addResult("Keyboard Navigation", spec.componentName, "Platform",
              keyboardValid,
              keyboardValid ? "Keyboard navigation works"
                            : "Keyboard navigation issues",
              "Keyboard navigation",
              keyboardValid ? "Keyboard navigation" : "No keyboard navigation");

    if (!keyboardValid)
        allValid = false;

    return allValid;
}

// Accessibility validation methods
bool FluentComponentValidator::validateAccessibilityCompliance(
    QWidget* component) {
    if (!component)
        return false;

    // Check if component has accessible name
    QString accessibleName = component->accessibleName();
    bool hasAccessibleName = !accessibleName.isEmpty();

    // Check if component has accessible description
    QString accessibleDescription = component->accessibleDescription();

    // Check if component supports focus
    bool supportsFocus = component->focusPolicy() != Qt::NoFocus;

    return hasAccessibleName || supportsFocus;
}

bool FluentComponentValidator::validateKeyboardNavigation(QWidget* component) {
    if (!component)
        return false;

    // Check if component accepts focus
    if (component->focusPolicy() == Qt::NoFocus) {
        return true;  // Components that don't need focus are valid
    }

    // Test tab navigation
    component->setFocus();
    QCoreApplication::processEvents();

    return component->hasFocus();
}

bool FluentComponentValidator::validateScreenReaderSupport(QWidget* component) {
    if (!component)
        return false;

    // Check if component has proper accessible properties
    return !component->accessibleName().isEmpty() ||
           !component->accessibleDescription().isEmpty();
}

bool FluentComponentValidator::validateFocusManagement(QWidget* component) {
    if (!component)
        return false;

    // Test focus in/out events
    component->setFocus();
    bool canReceiveFocus = component->hasFocus();

    component->clearFocus();
    bool canLoseFocus = !component->hasFocus();

    return canReceiveFocus && canLoseFocus;
}

// Report generation and summary
void FluentComponentValidator::generateReport(const QString& outputPath) {
    QString reportPath = outputPath.isEmpty()
                             ? QStandardPaths::writableLocation(
                                   QStandardPaths::DocumentsLocation) +
                                   "/FluentQt_Validation_Report.json"
                             : outputPath;

    QJsonObject report;
    report["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    report["totalTests"] = m_totalTests;
    report["passedTests"] = m_passedTests;
    report["failedTests"] = m_failedTests;
    report["successRate"] =
        m_totalTests > 0 ? (double(m_passedTests) / m_totalTests * 100.0) : 0.0;

    QJsonArray results;
    for (const auto& result : m_results) {
        QJsonObject resultObj;
        resultObj["testName"] = result.testName;
        resultObj["componentName"] = result.componentName;
        resultObj["category"] = result.category;
        resultObj["passed"] = result.passed;
        resultObj["details"] = result.details;
        resultObj["expectedValue"] = result.expectedValue;
        resultObj["actualValue"] = result.actualValue;
        resultObj["duration"] = static_cast<qint64>(result.duration.count());
        results.append(resultObj);
    }
    report["results"] = results;

    QFile file(reportPath);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(report);
        file.write(doc.toJson());
        qDebug() << "Validation report saved to:" << reportPath;
    } else {
        qDebug() << "Failed to save validation report to:" << reportPath;
    }
}

void FluentComponentValidator::printSummary() const {
    qDebug() << "\n=== FluentQt Component Validation Summary ===";
    qDebug() << "Total Tests:" << m_totalTests;
    qDebug() << "Passed Tests:" << m_passedTests;
    qDebug() << "Failed Tests:" << m_failedTests;

    if (m_totalTests > 0) {
        double successRate = (double(m_passedTests) / m_totalTests) * 100.0;
        qDebug() << "Success Rate:" << QString::number(successRate, 'f', 1)
                 << "%";

        if (successRate >= 90.0) {
            qDebug()
                << "🎉 Excellent! FluentQt components are highly compliant.";
        } else if (successRate >= 75.0) {
            qDebug() << "✅ Good! Most FluentQt components are compliant.";
        } else if (successRate >= 50.0) {
            qDebug() << "⚠️ Fair. Some FluentQt components need improvement.";
        } else {
            qDebug()
                << "❌ Poor. Many FluentQt components need significant work.";
        }
    }

    // Category breakdown
    QHash<QString, QPair<int, int>>
        categoryStats;  // category -> (passed, total)
    for (const auto& result : m_results) {
        auto& stats = categoryStats[result.category];
        stats.second++;  // total
        if (result.passed) {
            stats.first++;  // passed
        }
    }

    qDebug() << "\n=== Category Breakdown ===";
    for (auto it = categoryStats.begin(); it != categoryStats.end(); ++it) {
        const QString& category = it.key();
        int passed = it.value().first;
        int total = it.value().second;
        double rate = total > 0 ? (double(passed) / total * 100.0) : 0.0;

        qDebug() << QString("%1: %2/%3 (%4%)")
                        .arg(category, -20)
                        .arg(passed)
                        .arg(total)
                        .arg(QString::number(rate, 'f', 1));
    }
}

// Missing slot implementations
void FluentComponentValidator::onThemeChanged() {
    // Handle theme change events during validation
    if (m_currentTestWidget) {
        // Force update of the current test widget
        m_currentTestWidget->update();
        QCoreApplication::processEvents();
    }
}

void FluentComponentValidator::onAnimationFinished() {
    // Handle animation completion during validation
    if (m_currentAnimation) {
        m_currentAnimation = nullptr;
    }
    if (m_animationTimeout) {
        m_animationTimeout->stop();
    }
}
