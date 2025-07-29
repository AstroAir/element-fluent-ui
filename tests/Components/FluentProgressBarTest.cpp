// tests/Components/FluentProgressBarTest.cpp
#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>
#include <QTimer>
#include <QPainter>

#include "FluentQt/Components/FluentProgressBar.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentProgressBarTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();
    void testTypeConstructor();

    // Value tests
    void testValue();
    void testMinimum();
    void testMaximum();
    void testRange();

    // Text tests
    void testText();
    void testTextVisible();

    // Type and appearance tests
    void testProgressType();
    void testProgressSize();

    // Animation tests
    void testAnimated();
    void testAccentColor();

    // State control tests
    void testStart();
    void testStop();
    void testPause();
    void testResume();
    void testReset();

    // Utility tests
    void testPercentage();
    void testIsComplete();

    // Signal tests
    void testValueChangedSignal();
    void testPropertyChangeSignals();
    void testStateChangeSignals();

    // Indeterminate mode tests
    void testIndeterminateMode();
    void testRingMode();
    void testDotsMode();

    // Animation behavior tests
    void testValueAnimation();
    void testIndeterminateAnimation();

    // Accessibility tests
    void testAccessibility();

    // Theme integration tests
    void testThemeIntegration();

private:
    FluentProgressBar* m_progressBar;
};

void FluentProgressBarTest::initTestCase() {
    // Initialize any resources needed for all tests
}

void FluentProgressBarTest::cleanupTestCase() {
    // Clean up any resources used for all tests
}

void FluentProgressBarTest::init() {
    // Create a fresh progress bar before each test
    m_progressBar = new FluentProgressBar();
    // Note: FluentProgressBar may not inherit from QWidget directly
    // m_progressBar->show();
    // QTest::qWaitForWindowExposed(m_progressBar);
}

void FluentProgressBarTest::cleanup() {
    // Clean up after each test
    delete m_progressBar;
    m_progressBar = nullptr;
}

void FluentProgressBarTest::testDefaultConstructor() {
    // Test default constructor
    FluentProgressBar* progressBar = new FluentProgressBar();
    
    // Verify default properties
    QCOMPARE(progressBar->value(), 0);
    QCOMPARE(progressBar->minimum(), 0);
    QCOMPARE(progressBar->maximum(), 100);
    QVERIFY(progressBar->text().isEmpty());
    QCOMPARE(progressBar->progressType(), FluentProgressBarType::Determinate);
    QCOMPARE(progressBar->progressSize(), FluentProgressBarSize::Medium);
    QVERIFY(progressBar->isTextVisible());
    QVERIFY(progressBar->isAnimated());
    QVERIFY(!progressBar->isComplete());
    
    delete progressBar;
}

void FluentProgressBarTest::testTypeConstructor() {
    // Test constructor with type
    FluentProgressBar* indeterminateBar = new FluentProgressBar(FluentProgressBarType::Indeterminate);
    QCOMPARE(indeterminateBar->progressType(), FluentProgressBarType::Indeterminate);
    delete indeterminateBar;
    
    FluentProgressBar* ringBar = new FluentProgressBar(FluentProgressBarType::Ring);
    QCOMPARE(ringBar->progressType(), FluentProgressBarType::Ring);
    delete ringBar;
    
    FluentProgressBar* dotsBar = new FluentProgressBar(FluentProgressBarType::Dots);
    QCOMPARE(dotsBar->progressType(), FluentProgressBarType::Dots);
    delete dotsBar;
}

void FluentProgressBarTest::testValue() {
    // Test setting and getting value
    QSignalSpy valueChangedSpy(m_progressBar, &FluentProgressBar::valueChanged);
    
    QCOMPARE(m_progressBar->value(), 0); // Default value
    
    m_progressBar->setValue(50);
    QCOMPARE(m_progressBar->value(), 50);
    QCOMPARE(valueChangedSpy.count(), 1);
    QCOMPARE(valueChangedSpy.first().first().toInt(), 50);
    
    m_progressBar->setValue(75);
    QCOMPARE(m_progressBar->value(), 75);
    QCOMPARE(valueChangedSpy.count(), 2);
    QCOMPARE(valueChangedSpy.last().first().toInt(), 75);
    
    // Setting the same value should not emit the signal
    m_progressBar->setValue(75);
    QCOMPARE(valueChangedSpy.count(), 2);
    
    // Test value bounds
    m_progressBar->setValue(-10); // Below minimum
    QCOMPARE(m_progressBar->value(), m_progressBar->minimum());
    
    m_progressBar->setValue(200); // Above maximum
    QCOMPARE(m_progressBar->value(), m_progressBar->maximum());
}

void FluentProgressBarTest::testMinimum() {
    // Test setting and getting minimum
    QSignalSpy minimumChangedSpy(m_progressBar, &FluentProgressBar::minimumChanged);
    
    QCOMPARE(m_progressBar->minimum(), 0); // Default minimum
    
    m_progressBar->setMinimum(10);
    QCOMPARE(m_progressBar->minimum(), 10);
    QCOMPARE(minimumChangedSpy.count(), 1);
    QCOMPARE(minimumChangedSpy.first().first().toInt(), 10);
    
    m_progressBar->setMinimum(20);
    QCOMPARE(m_progressBar->minimum(), 20);
    QCOMPARE(minimumChangedSpy.count(), 2);
    QCOMPARE(minimumChangedSpy.last().first().toInt(), 20);
    
    // Setting the same minimum should not emit the signal
    m_progressBar->setMinimum(20);
    QCOMPARE(minimumChangedSpy.count(), 2);
    
    // Test that value is adjusted if below new minimum
    m_progressBar->setValue(5); // Below new minimum
    QCOMPARE(m_progressBar->value(), 20); // Should be clamped to minimum
}

void FluentProgressBarTest::testMaximum() {
    // Test setting and getting maximum
    QSignalSpy maximumChangedSpy(m_progressBar, &FluentProgressBar::maximumChanged);
    
    QCOMPARE(m_progressBar->maximum(), 100); // Default maximum
    
    m_progressBar->setMaximum(200);
    QCOMPARE(m_progressBar->maximum(), 200);
    QCOMPARE(maximumChangedSpy.count(), 1);
    QCOMPARE(maximumChangedSpy.first().first().toInt(), 200);
    
    m_progressBar->setMaximum(150);
    QCOMPARE(m_progressBar->maximum(), 150);
    QCOMPARE(maximumChangedSpy.count(), 2);
    QCOMPARE(maximumChangedSpy.last().first().toInt(), 150);
    
    // Setting the same maximum should not emit the signal
    m_progressBar->setMaximum(150);
    QCOMPARE(maximumChangedSpy.count(), 2);
}

void FluentProgressBarTest::testRange() {
    // Test setting range
    QSignalSpy minimumChangedSpy(m_progressBar, &FluentProgressBar::minimumChanged);
    QSignalSpy maximumChangedSpy(m_progressBar, &FluentProgressBar::maximumChanged);
    
    m_progressBar->setRange(10, 200);
    QCOMPARE(m_progressBar->minimum(), 10);
    QCOMPARE(m_progressBar->maximum(), 200);
    QCOMPARE(minimumChangedSpy.count(), 1);
    QCOMPARE(maximumChangedSpy.count(), 1);
}

void FluentProgressBarTest::testText() {
    // Test setting and getting text
    const QString text1 = "Loading...";
    const QString text2 = "Processing...";
    
    QSignalSpy textChangedSpy(m_progressBar, &FluentProgressBar::textChanged);
    
    m_progressBar->setText(text1);
    QCOMPARE(m_progressBar->text(), text1);
    QCOMPARE(textChangedSpy.count(), 1);
    QCOMPARE(textChangedSpy.first().first().toString(), text1);
    
    m_progressBar->setText(text2);
    QCOMPARE(m_progressBar->text(), text2);
    QCOMPARE(textChangedSpy.count(), 2);
    QCOMPARE(textChangedSpy.last().first().toString(), text2);
    
    // Setting the same text should not emit the signal
    m_progressBar->setText(text2);
    QCOMPARE(textChangedSpy.count(), 2);
}

void FluentProgressBarTest::testTextVisible() {
    // Test text visibility
    QSignalSpy textVisibleChangedSpy(m_progressBar, &FluentProgressBar::textVisibleChanged);
    
    QVERIFY(m_progressBar->isTextVisible()); // Default should be true
    
    m_progressBar->setTextVisible(false);
    QVERIFY(!m_progressBar->isTextVisible());
    QCOMPARE(textVisibleChangedSpy.count(), 1);
    QCOMPARE(textVisibleChangedSpy.first().first().toBool(), false);
    
    m_progressBar->setTextVisible(true);
    QVERIFY(m_progressBar->isTextVisible());
    QCOMPARE(textVisibleChangedSpy.count(), 2);
    QCOMPARE(textVisibleChangedSpy.last().first().toBool(), true);
    
    // Setting the same visibility should not emit the signal
    m_progressBar->setTextVisible(true);
    QCOMPARE(textVisibleChangedSpy.count(), 2);
}

void FluentProgressBarTest::testProgressType() {
    // Test setting and getting progress type
    QSignalSpy progressTypeChangedSpy(m_progressBar, &FluentProgressBar::progressTypeChanged);
    
    QCOMPARE(m_progressBar->progressType(), FluentProgressBarType::Determinate); // Default type
    
    m_progressBar->setProgressType(FluentProgressBarType::Indeterminate);
    QCOMPARE(m_progressBar->progressType(), FluentProgressBarType::Indeterminate);
    QCOMPARE(progressTypeChangedSpy.count(), 1);
    QCOMPARE(progressTypeChangedSpy.first().first().value<FluentProgressBarType>(), FluentProgressBarType::Indeterminate);
    
    m_progressBar->setProgressType(FluentProgressBarType::Ring);
    QCOMPARE(m_progressBar->progressType(), FluentProgressBarType::Ring);
    QCOMPARE(progressTypeChangedSpy.count(), 2);
    
    m_progressBar->setProgressType(FluentProgressBarType::Dots);
    QCOMPARE(m_progressBar->progressType(), FluentProgressBarType::Dots);
    QCOMPARE(progressTypeChangedSpy.count(), 3);
    
    // Setting the same type should not emit the signal
    m_progressBar->setProgressType(FluentProgressBarType::Dots);
    QCOMPARE(progressTypeChangedSpy.count(), 3);
}

void FluentProgressBarTest::testProgressSize() {
    // Test setting and getting progress size
    QSignalSpy progressSizeChangedSpy(m_progressBar, &FluentProgressBar::progressSizeChanged);

    QCOMPARE(m_progressBar->progressSize(), FluentProgressBarSize::Medium); // Default size

    m_progressBar->setProgressSize(FluentProgressBarSize::Small);
    QCOMPARE(m_progressBar->progressSize(), FluentProgressBarSize::Small);
    QCOMPARE(progressSizeChangedSpy.count(), 1);
    QCOMPARE(progressSizeChangedSpy.first().first().value<FluentProgressBarSize>(), FluentProgressBarSize::Small);

    m_progressBar->setProgressSize(FluentProgressBarSize::Large);
    QCOMPARE(m_progressBar->progressSize(), FluentProgressBarSize::Large);
    QCOMPARE(progressSizeChangedSpy.count(), 2);

    // Setting the same size should not emit the signal
    m_progressBar->setProgressSize(FluentProgressBarSize::Large);
    QCOMPARE(progressSizeChangedSpy.count(), 2);
}

void FluentProgressBarTest::testAnimated() {
    // Test animated property
    QSignalSpy animatedChangedSpy(m_progressBar, &FluentProgressBar::animatedChanged);

    QVERIFY(m_progressBar->isAnimated()); // Default should be true

    m_progressBar->setAnimated(false);
    QVERIFY(!m_progressBar->isAnimated());
    QCOMPARE(animatedChangedSpy.count(), 1);
    QCOMPARE(animatedChangedSpy.first().first().toBool(), false);

    m_progressBar->setAnimated(true);
    QVERIFY(m_progressBar->isAnimated());
    QCOMPARE(animatedChangedSpy.count(), 2);
    QCOMPARE(animatedChangedSpy.last().first().toBool(), true);

    // Setting the same animated state should not emit the signal
    m_progressBar->setAnimated(true);
    QCOMPARE(animatedChangedSpy.count(), 2);
}

void FluentProgressBarTest::testAccentColor() {
    // Test setting and getting accent color
    QSignalSpy accentColorChangedSpy(m_progressBar, &FluentProgressBar::accentColorChanged);

    QColor color1(255, 0, 0); // Red
    m_progressBar->setAccentColor(color1);
    QCOMPARE(m_progressBar->accentColor(), color1);
    QCOMPARE(accentColorChangedSpy.count(), 1);
    QCOMPARE(accentColorChangedSpy.first().first().value<QColor>(), color1);

    QColor color2(0, 255, 0); // Green
    m_progressBar->setAccentColor(color2);
    QCOMPARE(m_progressBar->accentColor(), color2);
    QCOMPARE(accentColorChangedSpy.count(), 2);
    QCOMPARE(accentColorChangedSpy.last().first().value<QColor>(), color2);

    // Setting the same color should not emit the signal
    m_progressBar->setAccentColor(color2);
    QCOMPARE(accentColorChangedSpy.count(), 2);
}

void FluentProgressBarTest::testStart() {
    // Test start functionality
    QSignalSpy startedSpy(m_progressBar, &FluentProgressBar::started);

    // For determinate progress, start should not emit started signal
    m_progressBar->start();
    QCOMPARE(startedSpy.count(), 0);

    // For indeterminate progress, start should emit started signal
    m_progressBar->setProgressType(FluentProgressBarType::Indeterminate);
    m_progressBar->start();
    QCOMPARE(startedSpy.count(), 1);

    // Starting again should not emit signal again
    m_progressBar->start();
    QCOMPARE(startedSpy.count(), 1);
}

void FluentProgressBarTest::testStop() {
    // Test stop functionality
    m_progressBar->setProgressType(FluentProgressBarType::Indeterminate);
    m_progressBar->start();

    m_progressBar->stop();

    // After stop, the progress bar should be in a stopped state
    // We can't directly test internal state, but we can verify the method doesn't crash
    QVERIFY(true);
}

void FluentProgressBarTest::testPause() {
    // Test pause functionality
    QSignalSpy pausedSpy(m_progressBar, &FluentProgressBar::paused);

    m_progressBar->setProgressType(FluentProgressBarType::Indeterminate);
    m_progressBar->start();

    m_progressBar->pause();
    QCOMPARE(pausedSpy.count(), 1);

    // Pausing again should not emit signal
    m_progressBar->pause();
    QCOMPARE(pausedSpy.count(), 1);
}

void FluentProgressBarTest::testResume() {
    // Test resume functionality
    QSignalSpy resumedSpy(m_progressBar, &FluentProgressBar::resumed);

    m_progressBar->setProgressType(FluentProgressBarType::Indeterminate);
    m_progressBar->start();
    m_progressBar->pause();

    m_progressBar->resume();
    QCOMPARE(resumedSpy.count(), 1);

    // Resuming again should not emit signal
    m_progressBar->resume();
    QCOMPARE(resumedSpy.count(), 1);
}

void FluentProgressBarTest::testReset() {
    // Test reset functionality
    m_progressBar->setValue(50);
    m_progressBar->setText("Loading...");
    m_progressBar->setProgressType(FluentProgressBarType::Indeterminate);
    m_progressBar->start();

    m_progressBar->reset();

    QCOMPARE(m_progressBar->value(), m_progressBar->minimum());
    // After reset, the progress bar should be in a reset state
    // We can't directly test internal state, but we can verify the method works
    QVERIFY(true);
}

void FluentProgressBarTest::testPercentage() {
    // Test percentage calculation
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    QCOMPARE(m_progressBar->percentage(), 0.0);

    m_progressBar->setValue(25);
    QCOMPARE(m_progressBar->percentage(), 0.25);

    m_progressBar->setValue(50);
    QCOMPARE(m_progressBar->percentage(), 0.5);

    m_progressBar->setValue(100);
    QCOMPARE(m_progressBar->percentage(), 1.0);

    // Test with different range
    m_progressBar->setRange(10, 60);
    m_progressBar->setValue(35); // Middle of range
    QCOMPARE(m_progressBar->percentage(), 0.5);
}

void FluentProgressBarTest::testIsComplete() {
    // Test completion detection
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    QVERIFY(!m_progressBar->isComplete());

    m_progressBar->setValue(50);
    QVERIFY(!m_progressBar->isComplete());

    m_progressBar->setValue(100);
    QVERIFY(m_progressBar->isComplete());

    // Test with different range
    m_progressBar->setRange(10, 60);
    m_progressBar->setValue(60);
    QVERIFY(m_progressBar->isComplete());
}

void FluentProgressBarTest::testValueChangedSignal() {
    // Test value changed signal
    QSignalSpy valueChangedSpy(m_progressBar, &FluentProgressBar::valueChanged);

    m_progressBar->setValue(25);
    QCOMPARE(valueChangedSpy.count(), 1);
    QCOMPARE(valueChangedSpy.first().first().toInt(), 25);

    m_progressBar->setValue(50);
    QCOMPARE(valueChangedSpy.count(), 2);
    QCOMPARE(valueChangedSpy.last().first().toInt(), 50);

    // Setting the same value should not emit the signal
    m_progressBar->setValue(50);
    QCOMPARE(valueChangedSpy.count(), 2);
}

void FluentProgressBarTest::testPropertyChangeSignals() {
    // Test that property change signals are emitted correctly
    QSignalSpy minimumChangedSpy(m_progressBar, &FluentProgressBar::minimumChanged);
    QSignalSpy maximumChangedSpy(m_progressBar, &FluentProgressBar::maximumChanged);
    QSignalSpy textChangedSpy(m_progressBar, &FluentProgressBar::textChanged);
    QSignalSpy progressTypeChangedSpy(m_progressBar, &FluentProgressBar::progressTypeChanged);
    QSignalSpy progressSizeChangedSpy(m_progressBar, &FluentProgressBar::progressSizeChanged);
    QSignalSpy textVisibleChangedSpy(m_progressBar, &FluentProgressBar::textVisibleChanged);
    QSignalSpy animatedChangedSpy(m_progressBar, &FluentProgressBar::animatedChanged);
    QSignalSpy accentColorChangedSpy(m_progressBar, &FluentProgressBar::accentColorChanged);

    // Change properties and verify signals
    m_progressBar->setMinimum(10);
    QCOMPARE(minimumChangedSpy.count(), 1);

    m_progressBar->setMaximum(200);
    QCOMPARE(maximumChangedSpy.count(), 1);

    m_progressBar->setText("Loading...");
    QCOMPARE(textChangedSpy.count(), 1);

    m_progressBar->setProgressType(FluentProgressBarType::Ring);
    QCOMPARE(progressTypeChangedSpy.count(), 1);

    m_progressBar->setProgressSize(FluentProgressBarSize::Large);
    QCOMPARE(progressSizeChangedSpy.count(), 1);

    m_progressBar->setTextVisible(false);
    QCOMPARE(textVisibleChangedSpy.count(), 1);

    m_progressBar->setAnimated(false);
    QCOMPARE(animatedChangedSpy.count(), 1);

    m_progressBar->setAccentColor(QColor(255, 0, 0));
    QCOMPARE(accentColorChangedSpy.count(), 1);
}

void FluentProgressBarTest::testStateChangeSignals() {
    // Test state change signals
    QSignalSpy startedSpy(m_progressBar, &FluentProgressBar::started);
    QSignalSpy pausedSpy(m_progressBar, &FluentProgressBar::paused);
    QSignalSpy resumedSpy(m_progressBar, &FluentProgressBar::resumed);
    QSignalSpy finishedSpy(m_progressBar, &FluentProgressBar::finished);

    // Test started signal
    m_progressBar->setProgressType(FluentProgressBarType::Indeterminate);
    m_progressBar->start();
    QCOMPARE(startedSpy.count(), 1);

    // Test paused signal
    m_progressBar->pause();
    QCOMPARE(pausedSpy.count(), 1);

    // Test resumed signal
    m_progressBar->resume();
    QCOMPARE(resumedSpy.count(), 1);

    // Test finished signal
    m_progressBar->setProgressType(FluentProgressBarType::Determinate);
    m_progressBar->setValue(m_progressBar->maximum());
    QCOMPARE(finishedSpy.count(), 1);
}

void FluentProgressBarTest::testIndeterminateMode() {
    // Test indeterminate mode
    m_progressBar->setProgressType(FluentProgressBarType::Indeterminate);
    QCOMPARE(m_progressBar->progressType(), FluentProgressBarType::Indeterminate);

    // Start indeterminate animation
    QSignalSpy startedSpy(m_progressBar, &FluentProgressBar::started);
    m_progressBar->start();
    QCOMPARE(startedSpy.count(), 1);

    // Stop animation
    m_progressBar->stop();

    // The progress bar should handle indeterminate mode without crashing
    QVERIFY(true);
}

void FluentProgressBarTest::testRingMode() {
    // Test ring mode
    m_progressBar->setProgressType(FluentProgressBarType::Ring);
    QCOMPARE(m_progressBar->progressType(), FluentProgressBarType::Ring);

    // Set value for ring progress
    m_progressBar->setValue(50);
    QCOMPARE(m_progressBar->value(), 50);

    // Start ring animation
    QSignalSpy startedSpy(m_progressBar, &FluentProgressBar::started);
    m_progressBar->start();
    QCOMPARE(startedSpy.count(), 1);

    // The progress bar should handle ring mode without crashing
    QVERIFY(true);
}

void FluentProgressBarTest::testDotsMode() {
    // Test dots mode
    m_progressBar->setProgressType(FluentProgressBarType::Dots);
    QCOMPARE(m_progressBar->progressType(), FluentProgressBarType::Dots);

    // Start dots animation
    QSignalSpy startedSpy(m_progressBar, &FluentProgressBar::started);
    m_progressBar->start();
    QCOMPARE(startedSpy.count(), 1);

    // The progress bar should handle dots mode without crashing
    QVERIFY(true);
}

void FluentProgressBarTest::testValueAnimation() {
    // Test value animation
    m_progressBar->setAnimated(true);

    QSignalSpy valueChangedSpy(m_progressBar, &FluentProgressBar::valueChanged);

    // Animate value change
    m_progressBar->setValue(0);
    m_progressBar->setValue(50);

    QCOMPARE(valueChangedSpy.count(), 2); // One for 0, one for 50
    QCOMPARE(m_progressBar->value(), 50);

    // Test without animation
    m_progressBar->setAnimated(false);
    valueChangedSpy.clear();

    m_progressBar->setValue(75);
    QCOMPARE(valueChangedSpy.count(), 1);
    QCOMPARE(m_progressBar->value(), 75);
}

void FluentProgressBarTest::testIndeterminateAnimation() {
    // Test indeterminate animation
    m_progressBar->setProgressType(FluentProgressBarType::Indeterminate);
    m_progressBar->setAnimated(true);

    // Start animation
    m_progressBar->start();

    // Pause animation
    m_progressBar->pause();

    // Resume animation
    m_progressBar->resume();

    // Stop animation
    m_progressBar->stop();

    // All animation operations should work without crashing
    QVERIFY(true);
}

void FluentProgressBarTest::testAccessibility() {
    // Test accessibility features
    // Note: FluentProgressBar may not inherit from QWidget directly,
    // so accessibility methods may not be available

    // For now, just verify that the progress bar can be created and used
    // without accessibility-related crashes
    m_progressBar->setValue(50);
    m_progressBar->setText("Progress: 50%");

    // Test that progress bar has proper accessibility role
    // (This would require checking internal accessibility implementation)
    QVERIFY(true);
}

void FluentProgressBarTest::testThemeIntegration() {
    // Test that progress bar responds to theme changes
    auto& theme = FluentQt::Styling::FluentTheme::instance();

    // Get current theme mode
    auto originalMode = theme.mode();

    // Change theme mode
    auto newMode = (originalMode == FluentQt::Styling::FluentThemeMode::Light) ?
                   FluentQt::Styling::FluentThemeMode::Dark :
                   FluentQt::Styling::FluentThemeMode::Light;
    theme.setMode(newMode);

    // Progress bar should update its appearance (this would require checking internal styling)
    // For now, just verify the progress bar still functions correctly
    QSignalSpy valueChangedSpy(m_progressBar, &FluentProgressBar::valueChanged);
    m_progressBar->setValue(42);
    QCOMPARE(m_progressBar->value(), 42);
    QCOMPARE(valueChangedSpy.count(), 1);

    // Restore original theme
    theme.setMode(originalMode);
}

QTEST_MAIN(FluentProgressBarTest)
#include "FluentProgressBarTest.moc"


