// tests/Components/FluentSpinBoxTest.cpp
#include <QKeyEvent>
#include <QMouseEvent>
#include <QSignalSpy>
#include <QWheelEvent>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentSpinBox.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentSpinBoxTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();
    void testTypeConstructor();
    void testFactoryMethods();

    // Value tests
    void testValue();
    void testMinimum();
    void testMaximum();
    void testRange();
    void testSingleStep();
    void testDecimals();

    // Text properties tests
    void testPrefix();
    void testSuffix();
    void testPlaceholderText();
    void testText();
    void testCleanText();

    // Type and appearance tests
    void testSpinBoxType();
    void testSpinBoxSize();
    void testButtonLayout();

    // Behavior tests
    void testReadOnly();
    void testWrapping();
    void testAccelerated();
    void testShowButtons();
    void testAnimated();

    // Validation tests
    void testValidation();
    void testFixup();
    void testIsValid();

    // Step operations tests
    void testStepUp();
    void testStepDown();
    void testStepBy();

    // Interaction tests
    void testKeyboardInteraction();
    void testWheelInteraction();
    void testMouseInteraction();

    // Signal tests
    void testValueChangedSignal();
    void testPropertyChangeSignals();
    void testEditingSignals();

    // Utility tests
    void testSelectAll();
    void testClear();

    // Accessibility tests
    void testAccessibility();

    // Theme integration tests
    void testThemeIntegration();

private:
    FluentSpinBox* m_spinBox;
};

void FluentSpinBoxTest::initTestCase() {
    // Initialize any resources needed for all tests
}

void FluentSpinBoxTest::cleanupTestCase() {
    // Clean up any resources used for all tests
}

void FluentSpinBoxTest::init() {
    // Create a fresh spin box before each test
    m_spinBox = new FluentSpinBox();
    m_spinBox->show();
    QTest::qWaitForWindowExposed(m_spinBox);
}

void FluentSpinBoxTest::cleanup() {
    // Clean up after each test
    delete m_spinBox;
    m_spinBox = nullptr;
}

void FluentSpinBoxTest::testDefaultConstructor() {
    // Test default constructor
    FluentSpinBox* spinBox = new FluentSpinBox();

    // Verify default properties
    QCOMPARE(spinBox->value(), 0.0);
    QCOMPARE(spinBox->minimum(), 0.0);
    QCOMPARE(spinBox->maximum(), 99.99);
    QCOMPARE(spinBox->singleStep(), 1.0);
    QCOMPARE(spinBox->decimals(), 2);
    QVERIFY(spinBox->prefix().isEmpty());
    QVERIFY(spinBox->suffix().isEmpty());
    QVERIFY(spinBox->placeholderText().isEmpty());
    QCOMPARE(spinBox->spinBoxType(), FluentSpinBoxType::Double);
    QCOMPARE(spinBox->spinBoxSize(), FluentSpinBoxSize::Medium);
    QCOMPARE(spinBox->buttonLayout(), FluentSpinBoxButtonLayout::Vertical);
    QVERIFY(!spinBox->isReadOnly());
    QVERIFY(!spinBox->wrapping());
    QVERIFY(spinBox->isAccelerated());
    QVERIFY(spinBox->showButtons());
    QVERIFY(spinBox->isAnimated());
    QVERIFY(spinBox->isEnabled());

    delete spinBox;
}

void FluentSpinBoxTest::testTypeConstructor() {
    // Test constructor with type
    FluentSpinBox* integerSpinBox =
        new FluentSpinBox(FluentSpinBoxType::Integer);
    QCOMPARE(integerSpinBox->spinBoxType(), FluentSpinBoxType::Integer);
    delete integerSpinBox;

    FluentSpinBox* currencySpinBox =
        new FluentSpinBox(FluentSpinBoxType::Currency);
    QCOMPARE(currencySpinBox->spinBoxType(), FluentSpinBoxType::Currency);
    delete currencySpinBox;

    FluentSpinBox* percentageSpinBox =
        new FluentSpinBox(FluentSpinBoxType::Percentage);
    QCOMPARE(percentageSpinBox->spinBoxType(), FluentSpinBoxType::Percentage);
    delete percentageSpinBox;
}

void FluentSpinBoxTest::testFactoryMethods() {
    // Test integer spin box factory method
    FluentSpinBox* integerSpinBox = FluentSpinBox::createIntegerSpinBox(0, 100);
    QCOMPARE(integerSpinBox->spinBoxType(), FluentSpinBoxType::Integer);
    QCOMPARE(integerSpinBox->minimum(), 0.0);
    QCOMPARE(integerSpinBox->maximum(), 100.0);
    delete integerSpinBox;

    // Test double spin box factory method
    FluentSpinBox* doubleSpinBox =
        FluentSpinBox::createDoubleSpinBox(0.0, 10.0, 3);
    QCOMPARE(doubleSpinBox->spinBoxType(), FluentSpinBoxType::Double);
    QCOMPARE(doubleSpinBox->minimum(), 0.0);
    QCOMPARE(doubleSpinBox->maximum(), 10.0);
    QCOMPARE(doubleSpinBox->decimals(), 3);
    delete doubleSpinBox;

    // Test currency spin box factory method
    FluentSpinBox* currencySpinBox =
        FluentSpinBox::createCurrencySpinBox(0.0, 1000.0);
    QCOMPARE(currencySpinBox->spinBoxType(), FluentSpinBoxType::Currency);
    QCOMPARE(currencySpinBox->minimum(), 0.0);
    QCOMPARE(currencySpinBox->maximum(), 1000.0);
    QCOMPARE(currencySpinBox->decimals(), 2);
    delete currencySpinBox;

    // Test percentage spin box factory method
    FluentSpinBox* percentageSpinBox = FluentSpinBox::createPercentageSpinBox();
    QCOMPARE(percentageSpinBox->spinBoxType(), FluentSpinBoxType::Percentage);
    QCOMPARE(percentageSpinBox->minimum(), 0.0);
    QCOMPARE(percentageSpinBox->maximum(), 100.0);
    QCOMPARE(percentageSpinBox->decimals(), 1);
    QCOMPARE(percentageSpinBox->suffix(), QString("%"));
    delete percentageSpinBox;
}

void FluentSpinBoxTest::testValue() {
    // Test setting and getting value
    QSignalSpy valueChangedSpy(m_spinBox, &FluentSpinBox::valueChanged);

    QCOMPARE(m_spinBox->value(), 0.0);  // Default value

    m_spinBox->setValue(25.5);
    QCOMPARE(m_spinBox->value(), 25.5);
    QCOMPARE(valueChangedSpy.count(), 1);
    QCOMPARE(valueChangedSpy.first().first().toDouble(), 25.5);

    m_spinBox->setValue(50.75);
    QCOMPARE(m_spinBox->value(), 50.75);
    QCOMPARE(valueChangedSpy.count(), 2);
    QCOMPARE(valueChangedSpy.last().first().toDouble(), 50.75);

    // Setting the same value should not emit the signal
    m_spinBox->setValue(50.75);
    QCOMPARE(valueChangedSpy.count(), 2);

    // Test value bounds
    m_spinBox->setValue(-10.0);  // Below minimum
    QCOMPARE(m_spinBox->value(), m_spinBox->minimum());

    m_spinBox->setValue(200.0);  // Above maximum
    QCOMPARE(m_spinBox->value(), m_spinBox->maximum());
}

void FluentSpinBoxTest::testMinimum() {
    // Test setting and getting minimum
    QSignalSpy minimumChangedSpy(m_spinBox, &FluentSpinBox::minimumChanged);

    QCOMPARE(m_spinBox->minimum(), 0.0);  // Default minimum

    m_spinBox->setMinimum(-50.0);
    QCOMPARE(m_spinBox->minimum(), -50.0);
    QCOMPARE(minimumChangedSpy.count(), 1);
    QCOMPARE(minimumChangedSpy.first().first().toDouble(), -50.0);

    m_spinBox->setMinimum(10.0);
    QCOMPARE(m_spinBox->minimum(), 10.0);
    QCOMPARE(minimumChangedSpy.count(), 2);
    QCOMPARE(minimumChangedSpy.last().first().toDouble(), 10.0);

    // Setting the same minimum should not emit the signal
    m_spinBox->setMinimum(10.0);
    QCOMPARE(minimumChangedSpy.count(), 2);
}

void FluentSpinBoxTest::testMaximum() {
    // Test setting and getting maximum
    QSignalSpy maximumChangedSpy(m_spinBox, &FluentSpinBox::maximumChanged);

    QCOMPARE(m_spinBox->maximum(), 99.99);  // Default maximum

    m_spinBox->setMaximum(200.0);
    QCOMPARE(m_spinBox->maximum(), 200.0);
    QCOMPARE(maximumChangedSpy.count(), 1);
    QCOMPARE(maximumChangedSpy.first().first().toDouble(), 200.0);

    m_spinBox->setMaximum(150.0);
    QCOMPARE(m_spinBox->maximum(), 150.0);
    QCOMPARE(maximumChangedSpy.count(), 2);
    QCOMPARE(maximumChangedSpy.last().first().toDouble(), 150.0);

    // Setting the same maximum should not emit the signal
    m_spinBox->setMaximum(150.0);
    QCOMPARE(maximumChangedSpy.count(), 2);
}

void FluentSpinBoxTest::testRange() {
    // Test setting range
    QSignalSpy minimumChangedSpy(m_spinBox, &FluentSpinBox::minimumChanged);
    QSignalSpy maximumChangedSpy(m_spinBox, &FluentSpinBox::maximumChanged);

    m_spinBox->setRange(-100.0, 100.0);
    QCOMPARE(m_spinBox->minimum(), -100.0);
    QCOMPARE(m_spinBox->maximum(), 100.0);
    QCOMPARE(minimumChangedSpy.count(), 1);
    QCOMPARE(maximumChangedSpy.count(), 1);
}

void FluentSpinBoxTest::testSingleStep() {
    // Test setting and getting single step
    QSignalSpy singleStepChangedSpy(m_spinBox,
                                    &FluentSpinBox::singleStepChanged);

    QCOMPARE(m_spinBox->singleStep(), 1.0);  // Default step

    m_spinBox->setSingleStep(0.5);
    QCOMPARE(m_spinBox->singleStep(), 0.5);
    QCOMPARE(singleStepChangedSpy.count(), 1);
    QCOMPARE(singleStepChangedSpy.first().first().toDouble(), 0.5);

    m_spinBox->setSingleStep(2.0);
    QCOMPARE(m_spinBox->singleStep(), 2.0);
    QCOMPARE(singleStepChangedSpy.count(), 2);
    QCOMPARE(singleStepChangedSpy.last().first().toDouble(), 2.0);

    // Setting the same step should not emit the signal
    m_spinBox->setSingleStep(2.0);
    QCOMPARE(singleStepChangedSpy.count(), 2);

    // Test invalid step (should be ignored)
    m_spinBox->setSingleStep(0.0);
    QCOMPARE(m_spinBox->singleStep(), 2.0);  // Should remain unchanged
    QCOMPARE(singleStepChangedSpy.count(), 2);

    m_spinBox->setSingleStep(-1.0);
    QCOMPARE(m_spinBox->singleStep(), 2.0);  // Should remain unchanged
    QCOMPARE(singleStepChangedSpy.count(), 2);
}

void FluentSpinBoxTest::testDecimals() {
    // Test setting and getting decimals
    QSignalSpy decimalsChangedSpy(m_spinBox, &FluentSpinBox::decimalsChanged);

    QCOMPARE(m_spinBox->decimals(), 2);  // Default decimals

    m_spinBox->setDecimals(3);
    QCOMPARE(m_spinBox->decimals(), 3);
    QCOMPARE(decimalsChangedSpy.count(), 1);
    QCOMPARE(decimalsChangedSpy.first().first().toInt(), 3);

    m_spinBox->setDecimals(0);
    QCOMPARE(m_spinBox->decimals(), 0);
    QCOMPARE(decimalsChangedSpy.count(), 2);
    QCOMPARE(decimalsChangedSpy.last().first().toInt(), 0);

    // Setting the same decimals should not emit the signal
    m_spinBox->setDecimals(0);
    QCOMPARE(decimalsChangedSpy.count(), 2);

    // Test bounds (should be clamped to 0-10)
    m_spinBox->setDecimals(-1);
    QCOMPARE(m_spinBox->decimals(), 0);  // Should be clamped to 0

    m_spinBox->setDecimals(15);
    QCOMPARE(m_spinBox->decimals(), 10);  // Should be clamped to 10
}

void FluentSpinBoxTest::testPrefix() {
    // Test setting and getting prefix
    QSignalSpy prefixChangedSpy(m_spinBox, &FluentSpinBox::prefixChanged);

    QVERIFY(m_spinBox->prefix().isEmpty());  // Default prefix

    const QString prefix1 = "$";
    m_spinBox->setPrefix(prefix1);
    QCOMPARE(m_spinBox->prefix(), prefix1);
    QCOMPARE(prefixChangedSpy.count(), 1);
    QCOMPARE(prefixChangedSpy.first().first().toString(), prefix1);

    const QString prefix2 = "€";
    m_spinBox->setPrefix(prefix2);
    QCOMPARE(m_spinBox->prefix(), prefix2);
    QCOMPARE(prefixChangedSpy.count(), 2);
    QCOMPARE(prefixChangedSpy.last().first().toString(), prefix2);

    // Setting the same prefix should not emit the signal
    m_spinBox->setPrefix(prefix2);
    QCOMPARE(prefixChangedSpy.count(), 2);
}

void FluentSpinBoxTest::testSuffix() {
    // Test setting and getting suffix
    QSignalSpy suffixChangedSpy(m_spinBox, &FluentSpinBox::suffixChanged);

    QVERIFY(m_spinBox->suffix().isEmpty());  // Default suffix

    const QString suffix1 = "%";
    m_spinBox->setSuffix(suffix1);
    QCOMPARE(m_spinBox->suffix(), suffix1);
    QCOMPARE(suffixChangedSpy.count(), 1);
    QCOMPARE(suffixChangedSpy.first().first().toString(), suffix1);

    const QString suffix2 = " kg";
    m_spinBox->setSuffix(suffix2);
    QCOMPARE(m_spinBox->suffix(), suffix2);
    QCOMPARE(suffixChangedSpy.count(), 2);
    QCOMPARE(suffixChangedSpy.last().first().toString(), suffix2);

    // Setting the same suffix should not emit the signal
    m_spinBox->setSuffix(suffix2);
    QCOMPARE(suffixChangedSpy.count(), 2);
}

void FluentSpinBoxTest::testPlaceholderText() {
    // Test setting and getting placeholder text
    QSignalSpy placeholderTextChangedSpy(
        m_spinBox, &FluentSpinBox::placeholderTextChanged);

    QVERIFY(m_spinBox->placeholderText().isEmpty());  // Default placeholder

    const QString placeholder1 = "Enter value...";
    m_spinBox->setPlaceholderText(placeholder1);
    QCOMPARE(m_spinBox->placeholderText(), placeholder1);
    QCOMPARE(placeholderTextChangedSpy.count(), 1);
    QCOMPARE(placeholderTextChangedSpy.first().first().toString(),
             placeholder1);

    const QString placeholder2 = "Type a number";
    m_spinBox->setPlaceholderText(placeholder2);
    QCOMPARE(m_spinBox->placeholderText(), placeholder2);
    QCOMPARE(placeholderTextChangedSpy.count(), 2);
    QCOMPARE(placeholderTextChangedSpy.last().first().toString(), placeholder2);

    // Setting the same placeholder should not emit the signal
    m_spinBox->setPlaceholderText(placeholder2);
    QCOMPARE(placeholderTextChangedSpy.count(), 2);
}

void FluentSpinBoxTest::testText() {
    // Test getting text (formatted value)
    m_spinBox->setValue(25.5);
    m_spinBox->setPrefix("$");
    m_spinBox->setSuffix(" USD");

    QString text = m_spinBox->text();
    QVERIFY(text.contains("25.50"));  // Should contain the formatted value
    QVERIFY(text.contains("$"));      // Should contain prefix
    QVERIFY(text.contains("USD"));    // Should contain suffix
}

void FluentSpinBoxTest::testCleanText() {
    // Test getting clean text (value without prefix/suffix)
    m_spinBox->setValue(25.5);
    m_spinBox->setPrefix("$");
    m_spinBox->setSuffix(" USD");

    QString cleanText = m_spinBox->cleanText();
    QVERIFY(cleanText.contains("25.50"));  // Should contain the formatted value
    QVERIFY(!cleanText.contains("$"));     // Should not contain prefix
    QVERIFY(!cleanText.contains("USD"));   // Should not contain suffix
}

void FluentSpinBoxTest::testSpinBoxType() {
    // Test setting and getting spin box type
    QSignalSpy spinBoxTypeChangedSpy(m_spinBox,
                                     &FluentSpinBox::spinBoxTypeChanged);

    QCOMPARE(m_spinBox->spinBoxType(),
             FluentSpinBoxType::Double);  // Default type

    m_spinBox->setSpinBoxType(FluentSpinBoxType::Integer);
    QCOMPARE(m_spinBox->spinBoxType(), FluentSpinBoxType::Integer);
    QCOMPARE(spinBoxTypeChangedSpy.count(), 1);
    QCOMPARE(spinBoxTypeChangedSpy.first().first().value<FluentSpinBoxType>(),
             FluentSpinBoxType::Integer);

    m_spinBox->setSpinBoxType(FluentSpinBoxType::Currency);
    QCOMPARE(m_spinBox->spinBoxType(), FluentSpinBoxType::Currency);
    QCOMPARE(spinBoxTypeChangedSpy.count(), 2);

    m_spinBox->setSpinBoxType(FluentSpinBoxType::Percentage);
    QCOMPARE(m_spinBox->spinBoxType(), FluentSpinBoxType::Percentage);
    QCOMPARE(spinBoxTypeChangedSpy.count(), 3);

    // Setting the same type should not emit the signal
    m_spinBox->setSpinBoxType(FluentSpinBoxType::Percentage);
    QCOMPARE(spinBoxTypeChangedSpy.count(), 3);
}

void FluentSpinBoxTest::testSpinBoxSize() {
    // Test setting and getting spin box size
    QSignalSpy spinBoxSizeChangedSpy(m_spinBox,
                                     &FluentSpinBox::spinBoxSizeChanged);

    QCOMPARE(m_spinBox->spinBoxSize(),
             FluentSpinBoxSize::Medium);  // Default size

    m_spinBox->setSpinBoxSize(FluentSpinBoxSize::Small);
    QCOMPARE(m_spinBox->spinBoxSize(), FluentSpinBoxSize::Small);
    QCOMPARE(spinBoxSizeChangedSpy.count(), 1);
    QCOMPARE(spinBoxSizeChangedSpy.first().first().value<FluentSpinBoxSize>(),
             FluentSpinBoxSize::Small);

    m_spinBox->setSpinBoxSize(FluentSpinBoxSize::Large);
    QCOMPARE(m_spinBox->spinBoxSize(), FluentSpinBoxSize::Large);
    QCOMPARE(spinBoxSizeChangedSpy.count(), 2);

    // Setting the same size should not emit the signal
    m_spinBox->setSpinBoxSize(FluentSpinBoxSize::Large);
    QCOMPARE(spinBoxSizeChangedSpy.count(), 2);
}

void FluentSpinBoxTest::testButtonLayout() {
    // Test setting and getting button layout
    QSignalSpy buttonLayoutChangedSpy(m_spinBox,
                                      &FluentSpinBox::buttonLayoutChanged);

    QCOMPARE(m_spinBox->buttonLayout(),
             FluentSpinBoxButtonLayout::Vertical);  // Default layout

    m_spinBox->setButtonLayout(FluentSpinBoxButtonLayout::Horizontal);
    QCOMPARE(m_spinBox->buttonLayout(), FluentSpinBoxButtonLayout::Horizontal);
    QCOMPARE(buttonLayoutChangedSpy.count(), 1);
    QCOMPARE(buttonLayoutChangedSpy.first()
                 .first()
                 .value<FluentSpinBoxButtonLayout>(),
             FluentSpinBoxButtonLayout::Horizontal);

    m_spinBox->setButtonLayout(FluentSpinBoxButtonLayout::Sides);
    QCOMPARE(m_spinBox->buttonLayout(), FluentSpinBoxButtonLayout::Sides);
    QCOMPARE(buttonLayoutChangedSpy.count(), 2);

    m_spinBox->setButtonLayout(FluentSpinBoxButtonLayout::Embedded);
    QCOMPARE(m_spinBox->buttonLayout(), FluentSpinBoxButtonLayout::Embedded);
    QCOMPARE(buttonLayoutChangedSpy.count(), 3);

    // Setting the same layout should not emit the signal
    m_spinBox->setButtonLayout(FluentSpinBoxButtonLayout::Embedded);
    QCOMPARE(buttonLayoutChangedSpy.count(), 3);
}

void FluentSpinBoxTest::testReadOnly() {
    // Test read-only property
    QSignalSpy readOnlyChangedSpy(m_spinBox, &FluentSpinBox::readOnlyChanged);

    QVERIFY(!m_spinBox->isReadOnly());  // Default should be false

    m_spinBox->setReadOnly(true);
    QVERIFY(m_spinBox->isReadOnly());
    QCOMPARE(readOnlyChangedSpy.count(), 1);
    QCOMPARE(readOnlyChangedSpy.first().first().toBool(), true);

    m_spinBox->setReadOnly(false);
    QVERIFY(!m_spinBox->isReadOnly());
    QCOMPARE(readOnlyChangedSpy.count(), 2);
    QCOMPARE(readOnlyChangedSpy.last().first().toBool(), false);

    // Setting the same read-only state should not emit the signal
    m_spinBox->setReadOnly(false);
    QCOMPARE(readOnlyChangedSpy.count(), 2);
}

void FluentSpinBoxTest::testWrapping() {
    // Test wrapping property
    QSignalSpy wrappingChangedSpy(m_spinBox, &FluentSpinBox::wrappingChanged);

    QVERIFY(!m_spinBox->wrapping());  // Default should be false

    m_spinBox->setWrapping(true);
    QVERIFY(m_spinBox->wrapping());
    QCOMPARE(wrappingChangedSpy.count(), 1);
    QCOMPARE(wrappingChangedSpy.first().first().toBool(), true);

    m_spinBox->setWrapping(false);
    QVERIFY(!m_spinBox->wrapping());
    QCOMPARE(wrappingChangedSpy.count(), 2);
    QCOMPARE(wrappingChangedSpy.last().first().toBool(), false);

    // Setting the same wrapping state should not emit the signal
    m_spinBox->setWrapping(false);
    QCOMPARE(wrappingChangedSpy.count(), 2);
}

void FluentSpinBoxTest::testAccelerated() {
    // Test accelerated property
    QSignalSpy acceleratedChangedSpy(m_spinBox,
                                     &FluentSpinBox::acceleratedChanged);

    QVERIFY(m_spinBox->isAccelerated());  // Default should be true

    m_spinBox->setAccelerated(false);
    QVERIFY(!m_spinBox->isAccelerated());
    QCOMPARE(acceleratedChangedSpy.count(), 1);
    QCOMPARE(acceleratedChangedSpy.first().first().toBool(), false);

    m_spinBox->setAccelerated(true);
    QVERIFY(m_spinBox->isAccelerated());
    QCOMPARE(acceleratedChangedSpy.count(), 2);
    QCOMPARE(acceleratedChangedSpy.last().first().toBool(), true);

    // Setting the same accelerated state should not emit the signal
    m_spinBox->setAccelerated(true);
    QCOMPARE(acceleratedChangedSpy.count(), 2);
}

void FluentSpinBoxTest::testShowButtons() {
    // Test show buttons property
    QSignalSpy showButtonsChangedSpy(m_spinBox,
                                     &FluentSpinBox::showButtonsChanged);

    QVERIFY(m_spinBox->showButtons());  // Default should be true

    m_spinBox->setShowButtons(false);
    QVERIFY(!m_spinBox->showButtons());
    QCOMPARE(showButtonsChangedSpy.count(), 1);
    QCOMPARE(showButtonsChangedSpy.first().first().toBool(), false);

    m_spinBox->setShowButtons(true);
    QVERIFY(m_spinBox->showButtons());
    QCOMPARE(showButtonsChangedSpy.count(), 2);
    QCOMPARE(showButtonsChangedSpy.last().first().toBool(), true);

    // Setting the same show buttons state should not emit the signal
    m_spinBox->setShowButtons(true);
    QCOMPARE(showButtonsChangedSpy.count(), 2);
}

void FluentSpinBoxTest::testAnimated() {
    // Test animated property
    QSignalSpy animatedChangedSpy(m_spinBox, &FluentSpinBox::animatedChanged);

    QVERIFY(m_spinBox->isAnimated());  // Default should be true

    m_spinBox->setAnimated(false);
    QVERIFY(!m_spinBox->isAnimated());
    QCOMPARE(animatedChangedSpy.count(), 1);
    QCOMPARE(animatedChangedSpy.first().first().toBool(), false);

    m_spinBox->setAnimated(true);
    QVERIFY(m_spinBox->isAnimated());
    QCOMPARE(animatedChangedSpy.count(), 2);
    QCOMPARE(animatedChangedSpy.last().first().toBool(), true);

    // Setting the same animated state should not emit the signal
    m_spinBox->setAnimated(true);
    QCOMPARE(animatedChangedSpy.count(), 2);
}

void FluentSpinBoxTest::testValidation() {
    // Test validation functionality
    QString input;
    int pos = 0;

    // Test valid input
    input = "25.5";
    pos = input.length();
    QCOMPARE(m_spinBox->validate(input, pos), QValidator::Acceptable);

    // Test invalid input
    input = "abc";
    pos = input.length();
    QCOMPARE(m_spinBox->validate(input, pos), QValidator::Invalid);

    // Test intermediate input
    input = "25.";
    pos = input.length();
    QValidator::State state = m_spinBox->validate(input, pos);
    QVERIFY(state == QValidator::Intermediate ||
            state == QValidator::Acceptable);
}

void FluentSpinBoxTest::testFixup() {
    // Test fixup functionality
    QString input = "abc123def";
    m_spinBox->fixup(input);

    // After fixup, the input should be a valid number or empty
    bool ok;
    input.toDouble(&ok);
    QVERIFY(ok || input.isEmpty());
}

void FluentSpinBoxTest::testIsValid() {
    // Test isValid functionality
    QVERIFY(m_spinBox->isValid());  // Should be valid by default

    // Set a valid value
    m_spinBox->setValue(25.5);
    QVERIFY(m_spinBox->isValid());

    // The validity depends on internal state, which is hard to test directly
    // For now, just verify the method doesn't crash
}

void FluentSpinBoxTest::testStepUp() {
    // Test step up functionality
    QSignalSpy valueChangedSpy(m_spinBox, &FluentSpinBox::valueChanged);

    double initialValue = m_spinBox->value();
    double step = m_spinBox->singleStep();

    m_spinBox->stepUp();

    QCOMPARE(m_spinBox->value(), initialValue + step);
    QCOMPARE(valueChangedSpy.count(), 1);

    // Test step up when read-only (should not change)
    m_spinBox->setReadOnly(true);
    valueChangedSpy.clear();
    double readOnlyValue = m_spinBox->value();

    m_spinBox->stepUp();
    QCOMPARE(m_spinBox->value(), readOnlyValue);
    QCOMPARE(valueChangedSpy.count(), 0);

    m_spinBox->setReadOnly(false);
}

void FluentSpinBoxTest::testStepDown() {
    // Test step down functionality
    QSignalSpy valueChangedSpy(m_spinBox, &FluentSpinBox::valueChanged);

    m_spinBox->setValue(10.0);  // Set a value above minimum
    double initialValue = m_spinBox->value();
    double step = m_spinBox->singleStep();

    m_spinBox->stepDown();

    QCOMPARE(m_spinBox->value(), initialValue - step);
    QCOMPARE(valueChangedSpy.count(), 2);  // One for setValue, one for stepDown

    // Test step down when read-only (should not change)
    m_spinBox->setReadOnly(true);
    valueChangedSpy.clear();
    double readOnlyValue = m_spinBox->value();

    m_spinBox->stepDown();
    QCOMPARE(m_spinBox->value(), readOnlyValue);
    QCOMPARE(valueChangedSpy.count(), 0);

    m_spinBox->setReadOnly(false);
}

void FluentSpinBoxTest::testStepBy() {
    // Test step by functionality
    QSignalSpy valueChangedSpy(m_spinBox, &FluentSpinBox::valueChanged);

    m_spinBox->setValue(10.0);
    double initialValue = m_spinBox->value();
    double step = m_spinBox->singleStep();

    // Step by positive amount
    m_spinBox->stepBy(3);
    QCOMPARE(m_spinBox->value(), initialValue + (3 * step));

    // Step by negative amount
    valueChangedSpy.clear();
    initialValue = m_spinBox->value();
    m_spinBox->stepBy(-2);
    QCOMPARE(m_spinBox->value(), initialValue - (2 * step));
    QCOMPARE(valueChangedSpy.count(), 1);
}

void FluentSpinBoxTest::testKeyboardInteraction() {
    // Test keyboard interaction
    QSignalSpy valueChangedSpy(m_spinBox, &FluentSpinBox::valueChanged);

    m_spinBox->setValue(10.0);
    m_spinBox->setFocus();
    QVERIFY(m_spinBox->hasFocus());

    double initialValue = m_spinBox->value();
    double step = m_spinBox->singleStep();

    // Test Up arrow key
    QKeyEvent upKey(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier);
    QApplication::sendEvent(m_spinBox, &upKey);
    QCOMPARE(m_spinBox->value(), initialValue + step);

    // Test Down arrow key
    valueChangedSpy.clear();
    initialValue = m_spinBox->value();
    QKeyEvent downKey(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
    QApplication::sendEvent(m_spinBox, &downKey);
    QCOMPARE(m_spinBox->value(), initialValue - step);
    QCOMPARE(valueChangedSpy.count(), 1);

    // Test Page Up key (should step by 10)
    valueChangedSpy.clear();
    initialValue = m_spinBox->value();
    QKeyEvent pageUpKey(QEvent::KeyPress, Qt::Key_PageUp, Qt::NoModifier);
    QApplication::sendEvent(m_spinBox, &pageUpKey);
    QCOMPARE(m_spinBox->value(), initialValue + (10 * step));
    QCOMPARE(valueChangedSpy.count(), 1);

    // Test Page Down key (should step by -10)
    valueChangedSpy.clear();
    initialValue = m_spinBox->value();
    QKeyEvent pageDownKey(QEvent::KeyPress, Qt::Key_PageDown, Qt::NoModifier);
    QApplication::sendEvent(m_spinBox, &pageDownKey);
    QCOMPARE(m_spinBox->value(), initialValue - (10 * step));
    QCOMPARE(valueChangedSpy.count(), 1);

    // Test Home key (should go to minimum)
    valueChangedSpy.clear();
    QKeyEvent homeKey(QEvent::KeyPress, Qt::Key_Home, Qt::NoModifier);
    QApplication::sendEvent(m_spinBox, &homeKey);
    QCOMPARE(m_spinBox->value(), m_spinBox->minimum());
    QCOMPARE(valueChangedSpy.count(), 1);

    // Test End key (should go to maximum)
    valueChangedSpy.clear();
    QKeyEvent endKey(QEvent::KeyPress, Qt::Key_End, Qt::NoModifier);
    QApplication::sendEvent(m_spinBox, &endKey);
    QCOMPARE(m_spinBox->value(), m_spinBox->maximum());
    QCOMPARE(valueChangedSpy.count(), 1);
}

void FluentSpinBoxTest::testWheelInteraction() {
    // Test wheel interaction
    QSignalSpy valueChangedSpy(m_spinBox, &FluentSpinBox::valueChanged);

    m_spinBox->setValue(10.0);
    m_spinBox->setFocus();

    double initialValue = m_spinBox->value();
    double step = m_spinBox->singleStep();

    // Test wheel up (should increase value)
    QWheelEvent wheelUpEvent(QPointF(10, 10), QPointF(10, 10), QPoint(0, 0),
                             QPoint(0, 120), Qt::NoButton, Qt::NoModifier,
                             Qt::NoScrollPhase, false);
    QApplication::sendEvent(m_spinBox, &wheelUpEvent);
    QCOMPARE(m_spinBox->value(), initialValue + step);
    QCOMPARE(valueChangedSpy.count(), 1);

    // Test wheel down (should decrease value)
    valueChangedSpy.clear();
    initialValue = m_spinBox->value();
    QWheelEvent wheelDownEvent(QPointF(10, 10), QPointF(10, 10), QPoint(0, 0),
                               QPoint(0, -120), Qt::NoButton, Qt::NoModifier,
                               Qt::NoScrollPhase, false);
    QApplication::sendEvent(m_spinBox, &wheelDownEvent);
    QCOMPARE(m_spinBox->value(), initialValue - step);
    QCOMPARE(valueChangedSpy.count(), 1);

    // Test wheel when not focused (should be ignored)
    m_spinBox->clearFocus();
    valueChangedSpy.clear();
    initialValue = m_spinBox->value();
    QApplication::sendEvent(m_spinBox, &wheelUpEvent);
    QCOMPARE(m_spinBox->value(), initialValue);  // Should not change
    QCOMPARE(valueChangedSpy.count(), 0);
}

void FluentSpinBoxTest::testMouseInteraction() {
    // Test mouse interaction with buttons
    // This is harder to test without access to internal button rectangles
    // For now, just verify that mouse events don't crash the component

    QPoint center = m_spinBox->rect().center();
    QMouseEvent pressEvent(QEvent::MouseButtonPress, center, Qt::LeftButton,
                           Qt::LeftButton, Qt::NoModifier);
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, center, Qt::LeftButton,
                             Qt::LeftButton, Qt::NoModifier);

    // These should not crash
    QApplication::sendEvent(m_spinBox, &pressEvent);
    QApplication::sendEvent(m_spinBox, &releaseEvent);

    QVERIFY(true);  // If we get here, the events were handled without crashing
}

void FluentSpinBoxTest::testValueChangedSignal() {
    // Test value changed signal
    QSignalSpy valueChangedSpy(m_spinBox, &FluentSpinBox::valueChanged);

    m_spinBox->setValue(25.5);
    QCOMPARE(valueChangedSpy.count(), 1);
    QCOMPARE(valueChangedSpy.first().first().toDouble(), 25.5);

    m_spinBox->setValue(50.0);
    QCOMPARE(valueChangedSpy.count(), 2);
    QCOMPARE(valueChangedSpy.last().first().toDouble(), 50.0);

    // Setting the same value should not emit the signal
    m_spinBox->setValue(50.0);
    QCOMPARE(valueChangedSpy.count(), 2);
}

void FluentSpinBoxTest::testPropertyChangeSignals() {
    // Test that property change signals are emitted correctly
    QSignalSpy minimumChangedSpy(m_spinBox, &FluentSpinBox::minimumChanged);
    QSignalSpy maximumChangedSpy(m_spinBox, &FluentSpinBox::maximumChanged);
    QSignalSpy singleStepChangedSpy(m_spinBox,
                                    &FluentSpinBox::singleStepChanged);
    QSignalSpy decimalsChangedSpy(m_spinBox, &FluentSpinBox::decimalsChanged);
    QSignalSpy prefixChangedSpy(m_spinBox, &FluentSpinBox::prefixChanged);
    QSignalSpy suffixChangedSpy(m_spinBox, &FluentSpinBox::suffixChanged);
    QSignalSpy placeholderTextChangedSpy(
        m_spinBox, &FluentSpinBox::placeholderTextChanged);
    QSignalSpy spinBoxTypeChangedSpy(m_spinBox,
                                     &FluentSpinBox::spinBoxTypeChanged);
    QSignalSpy spinBoxSizeChangedSpy(m_spinBox,
                                     &FluentSpinBox::spinBoxSizeChanged);
    QSignalSpy buttonLayoutChangedSpy(m_spinBox,
                                      &FluentSpinBox::buttonLayoutChanged);
    QSignalSpy readOnlyChangedSpy(m_spinBox, &FluentSpinBox::readOnlyChanged);
    QSignalSpy wrappingChangedSpy(m_spinBox, &FluentSpinBox::wrappingChanged);
    QSignalSpy acceleratedChangedSpy(m_spinBox,
                                     &FluentSpinBox::acceleratedChanged);
    QSignalSpy showButtonsChangedSpy(m_spinBox,
                                     &FluentSpinBox::showButtonsChanged);
    QSignalSpy animatedChangedSpy(m_spinBox, &FluentSpinBox::animatedChanged);

    // Change properties and verify signals
    m_spinBox->setMinimum(-10.0);
    QCOMPARE(minimumChangedSpy.count(), 1);

    m_spinBox->setMaximum(200.0);
    QCOMPARE(maximumChangedSpy.count(), 1);

    m_spinBox->setSingleStep(0.5);
    QCOMPARE(singleStepChangedSpy.count(), 1);

    m_spinBox->setDecimals(3);
    QCOMPARE(decimalsChangedSpy.count(), 1);

    m_spinBox->setPrefix("$");
    QCOMPARE(prefixChangedSpy.count(), 1);

    m_spinBox->setSuffix(" USD");
    QCOMPARE(suffixChangedSpy.count(), 1);

    m_spinBox->setPlaceholderText("Enter amount");
    QCOMPARE(placeholderTextChangedSpy.count(), 1);

    m_spinBox->setSpinBoxType(FluentSpinBoxType::Currency);
    QCOMPARE(spinBoxTypeChangedSpy.count(), 1);

    m_spinBox->setSpinBoxSize(FluentSpinBoxSize::Large);
    QCOMPARE(spinBoxSizeChangedSpy.count(), 1);

    m_spinBox->setButtonLayout(FluentSpinBoxButtonLayout::Horizontal);
    QCOMPARE(buttonLayoutChangedSpy.count(), 1);

    m_spinBox->setReadOnly(true);
    QCOMPARE(readOnlyChangedSpy.count(), 1);

    m_spinBox->setWrapping(true);
    QCOMPARE(wrappingChangedSpy.count(), 1);

    m_spinBox->setAccelerated(false);
    QCOMPARE(acceleratedChangedSpy.count(), 1);

    m_spinBox->setShowButtons(false);
    QCOMPARE(showButtonsChangedSpy.count(), 1);

    m_spinBox->setAnimated(false);
    QCOMPARE(animatedChangedSpy.count(), 1);
}

void FluentSpinBoxTest::testEditingSignals() {
    // Test editing signals
    QSignalSpy editingFinishedSpy(m_spinBox, &FluentSpinBox::editingFinished);
    QSignalSpy textChangedSpy(m_spinBox, &FluentSpinBox::textChanged);

    // Access the internal line edit to simulate text changes
    QLineEdit* lineEdit = m_spinBox->lineEdit();
    QVERIFY(lineEdit != nullptr);

    // Simulate text change
    lineEdit->setText("25.5");
    // The textChanged signal should be emitted (if connected properly)

    // Simulate editing finished
    lineEdit->editingFinished();
    // The editingFinished signal should be emitted (if connected properly)

    // Note: The actual signal emission depends on internal connections
    // For now, just verify the methods don't crash
    QVERIFY(true);
}

void FluentSpinBoxTest::testSelectAll() {
    // Test select all functionality
    m_spinBox->setValue(25.5);

    QLineEdit* lineEdit = m_spinBox->lineEdit();
    QVERIFY(lineEdit != nullptr);

    m_spinBox->selectAll();

    // Verify that all text is selected
    QVERIFY(lineEdit->hasSelectedText());
    QCOMPARE(lineEdit->selectedText(), lineEdit->text());
}

void FluentSpinBoxTest::testClear() {
    // Test clear functionality
    QSignalSpy valueChangedSpy(m_spinBox, &FluentSpinBox::valueChanged);

    m_spinBox->setValue(25.5);
    QCOMPARE(m_spinBox->value(), 25.5);

    m_spinBox->clear();
    QCOMPARE(m_spinBox->value(), m_spinBox->minimum());
    QCOMPARE(valueChangedSpy.count(), 2);  // One for setValue, one for clear
}

void FluentSpinBoxTest::testAccessibility() {
    // Test accessibility features
    const QString accessibleName = "Amount Spin Box";
    const QString accessibleDescription = "Enter a numeric amount";

    m_spinBox->setAccessibleName(accessibleName);
    QCOMPARE(m_spinBox->accessibleName(), accessibleName);

    m_spinBox->setAccessibleDescription(accessibleDescription);
    QCOMPARE(m_spinBox->accessibleDescription(), accessibleDescription);

    // Test that spin box has proper focus policy
    QVERIFY(m_spinBox->focusPolicy() != Qt::NoFocus);

    // Test that spin box accepts keyboard input
    QVERIFY(m_spinBox->focusPolicy() & Qt::TabFocus);

    // Test that internal line edit is accessible
    QLineEdit* lineEdit = m_spinBox->lineEdit();
    QVERIFY(lineEdit != nullptr);
    QVERIFY(lineEdit->focusPolicy() != Qt::NoFocus);
}

void FluentSpinBoxTest::testThemeIntegration() {
    // Test that spin box responds to theme changes
    auto& theme = FluentQt::Styling::FluentTheme::instance();

    // Get current theme mode
    bool originalDarkMode = theme.isDarkMode();

    // Change theme mode
    theme.setDarkMode(!originalDarkMode);

    // Spin box should update its appearance (this would require checking
    // internal styling) For now, just verify the spin box still functions
    // correctly
    QVERIFY(m_spinBox->isEnabled());

    // Test that value operations still work after theme change
    QSignalSpy valueChangedSpy(m_spinBox, &FluentSpinBox::valueChanged);
    m_spinBox->setValue(42.0);
    QCOMPARE(m_spinBox->value(), 42.0);
    QCOMPARE(valueChangedSpy.count(), 1);

    // Restore original theme
    theme.setDarkMode(originalDarkMode);
}

QTEST_MAIN(FluentSpinBoxTest)
#include "FluentSpinBoxTest.moc"
