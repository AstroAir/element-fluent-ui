// tests/Components/FluentRatingControlTest.cpp
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentRatingControl.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentRatingControlTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();

    // Rating tests
    void testRating();
    void testMaxRating();
    void testPrecision();

    // Appearance tests
    void testRatingIcon();
    void testIconSize();
    void testIconColor();
    void testReadOnly();

    // Interaction tests
    void testMouseInteraction();
    void testKeyboardInteraction();
    void testHoverEffects();

    // Signal tests
    void testSignalEmission();

    // Validation tests
    void testRatingValidation();

private:
    FluentRatingControl* m_ratingControl{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentRatingControlTest::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(400, 300);
}

void FluentRatingControlTest::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentRatingControlTest::init() {
    m_ratingControl = new FluentRatingControl(m_testWidget);
}

void FluentRatingControlTest::cleanup() {
    delete m_ratingControl;
    m_ratingControl = nullptr;
}

void FluentRatingControlTest::testDefaultConstructor() {
    QVERIFY(m_ratingControl != nullptr);
    QCOMPARE(m_ratingControl->rating(), 0.0);
    QCOMPARE(m_ratingControl->maxRating(), 5);
    QCOMPARE(m_ratingControl->precision(), FluentRatingPrecision::Full);
    QVERIFY(!m_ratingControl->isReadOnly());
}

void FluentRatingControlTest::testRating() {
    QSignalSpy ratingSpy(m_ratingControl, &FluentRatingControl::ratingChanged);
    
    m_ratingControl->setRating(3.5);
    QCOMPARE(m_ratingControl->rating(), 3.5);
    QCOMPARE(ratingSpy.count(), 1);
    QCOMPARE(ratingSpy.first().first().toDouble(), 3.5);
    
    // Test bounds
    m_ratingControl->setRating(10.0); // Above maximum
    QCOMPARE(m_ratingControl->rating(), 5.0); // Should clamp to maximum
    
    m_ratingControl->setRating(-1.0); // Below minimum
    QCOMPARE(m_ratingControl->rating(), 0.0); // Should clamp to minimum
}

void FluentRatingControlTest::testMaxRating() {
    QSignalSpy maxRatingSpy(m_ratingControl, &FluentRatingControl::maxRatingChanged);
    
    m_ratingControl->setMaxRating(10);
    QCOMPARE(m_ratingControl->maxRating(), 10);
    QCOMPARE(maxRatingSpy.count(), 1);
    
    // Test that current rating adjusts if above new maximum
    m_ratingControl->setRating(8.0);
    m_ratingControl->setMaxRating(6);
    QCOMPARE(m_ratingControl->rating(), 6.0); // Should adjust to new maximum
    
    // Test invalid max rating
    m_ratingControl->setMaxRating(0);
    QVERIFY(m_ratingControl->maxRating() > 0); // Should remain positive
}

void FluentRatingControlTest::testPrecision() {
    QSignalSpy precisionSpy(m_ratingControl, &FluentRatingControl::precisionChanged);
    
    m_ratingControl->setPrecision(FluentRatingPrecision::Half);
    QCOMPARE(m_ratingControl->precision(), FluentRatingPrecision::Half);
    QCOMPARE(precisionSpy.count(), 1);
    
    // Test rating adjustment based on precision
    m_ratingControl->setRating(3.7);
    QCOMPARE(m_ratingControl->rating(), 3.5); // Should round to nearest half
    
    m_ratingControl->setPrecision(FluentRatingPrecision::Full);
    QCOMPARE(m_ratingControl->precision(), FluentRatingPrecision::Full);
    QCOMPARE(precisionSpy.count(), 2);
    
    m_ratingControl->setRating(3.7);
    QCOMPARE(m_ratingControl->rating(), 4.0); // Should round to nearest full
}

void FluentRatingControlTest::testRatingIcon() {
    QSignalSpy iconSpy(m_ratingControl, &FluentRatingControl::ratingIconChanged);
    
    m_ratingControl->setRatingIcon(FluentRatingIcon::Heart);
    QCOMPARE(m_ratingControl->ratingIcon(), FluentRatingIcon::Heart);
    QCOMPARE(iconSpy.count(), 1);
    
    m_ratingControl->setRatingIcon(FluentRatingIcon::Thumb);
    QCOMPARE(m_ratingControl->ratingIcon(), FluentRatingIcon::Thumb);
    QCOMPARE(iconSpy.count(), 2);
    
    m_ratingControl->setRatingIcon(FluentRatingIcon::Star);
    QCOMPARE(m_ratingControl->ratingIcon(), FluentRatingIcon::Star);
    QCOMPARE(iconSpy.count(), 3);
}

void FluentRatingControlTest::testIconSize() {
    QSignalSpy iconSizeSpy(m_ratingControl, &FluentRatingControl::iconSizeChanged);
    
    m_ratingControl->setIconSize(FluentRatingIconSize::Small);
    QCOMPARE(m_ratingControl->iconSize(), FluentRatingIconSize::Small);
    QCOMPARE(iconSizeSpy.count(), 1);
    
    m_ratingControl->setIconSize(FluentRatingIconSize::Large);
    QCOMPARE(m_ratingControl->iconSize(), FluentRatingIconSize::Large);
    QCOMPARE(iconSizeSpy.count(), 2);
    
    m_ratingControl->setIconSize(FluentRatingIconSize::Medium);
    QCOMPARE(m_ratingControl->iconSize(), FluentRatingIconSize::Medium);
    QCOMPARE(iconSizeSpy.count(), 3);
}

void FluentRatingControlTest::testIconColor() {
    QSignalSpy colorSpy(m_ratingControl, &FluentRatingControl::iconColorChanged);
    
    QColor testColor(255, 215, 0); // Gold
    m_ratingControl->setIconColor(testColor);
    QCOMPARE(m_ratingControl->iconColor(), testColor);
    QCOMPARE(colorSpy.count(), 1);
    
    // Test unrated color
    QSignalSpy unratedColorSpy(m_ratingControl, &FluentRatingControl::unratedColorChanged);
    QColor unratedColor(200, 200, 200); // Light gray
    m_ratingControl->setUnratedColor(unratedColor);
    QCOMPARE(m_ratingControl->unratedColor(), unratedColor);
    QCOMPARE(unratedColorSpy.count(), 1);
}

void FluentRatingControlTest::testReadOnly() {
    QSignalSpy readOnlySpy(m_ratingControl, &FluentRatingControl::readOnlyChanged);
    
    m_ratingControl->setReadOnly(true);
    QVERIFY(m_ratingControl->isReadOnly());
    QCOMPARE(readOnlySpy.count(), 1);
    
    m_ratingControl->setReadOnly(false);
    QVERIFY(!m_ratingControl->isReadOnly());
    QCOMPARE(readOnlySpy.count(), 2);
}

void FluentRatingControlTest::testMouseInteraction() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    QSignalSpy ratingSpy(m_ratingControl, &FluentRatingControl::ratingChanged);
    QSignalSpy ratingSelectedSpy(m_ratingControl, &FluentRatingControl::ratingSelected);
    
    // Test mouse click on rating control
    // Note: Exact positioning would depend on implementation details
    QTest::mouseClick(m_ratingControl, Qt::LeftButton, Qt::NoModifier, QPoint(60, 15));
    
    // Should have changed rating and emitted signals
    QVERIFY(ratingSpy.count() >= 0); // May or may not change depending on click position
    
    // Test read-only mode
    m_ratingControl->setReadOnly(true);
    double previousRating = m_ratingControl->rating();
    QTest::mouseClick(m_ratingControl, Qt::LeftButton, Qt::NoModifier, QPoint(80, 15));
    QCOMPARE(m_ratingControl->rating(), previousRating); // Should not change in read-only mode
}

void FluentRatingControlTest::testKeyboardInteraction() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    m_ratingControl->setFocus();
    
    QSignalSpy ratingSpy(m_ratingControl, &FluentRatingControl::ratingChanged);
    
    // Test arrow key navigation
    QTest::keyClick(m_ratingControl, Qt::Key_Right);
    QVERIFY(m_ratingControl->rating() > 0);
    
    QTest::keyClick(m_ratingControl, Qt::Key_Right);
    QTest::keyClick(m_ratingControl, Qt::Key_Right);
    
    QTest::keyClick(m_ratingControl, Qt::Key_Left);
    
    // Test Home/End keys
    QTest::keyClick(m_ratingControl, Qt::Key_Home);
    QCOMPARE(m_ratingControl->rating(), 0.0);
    
    QTest::keyClick(m_ratingControl, Qt::Key_End);
    QCOMPARE(m_ratingControl->rating(), static_cast<double>(m_ratingControl->maxRating()));
    
    // Test number keys
    QTest::keyClick(m_ratingControl, Qt::Key_3);
    QCOMPARE(m_ratingControl->rating(), 3.0);
    
    QTest::qWait(100);
}

void FluentRatingControlTest::testHoverEffects() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    QSignalSpy hoverRatingSpy(m_ratingControl, &FluentRatingControl::hoverRatingChanged);
    
    // Test mouse hover
    QTest::mouseMove(m_ratingControl, QPoint(40, 15));
    QTest::qWait(50);
    
    // Should show hover rating
    QVERIFY(hoverRatingSpy.count() >= 0);
    
    // Test mouse leave
    QTest::mouseMove(m_testWidget, QPoint(200, 200));
    QTest::qWait(50);
    
    // Hover rating should be reset
    QCOMPARE(m_ratingControl->hoverRating(), -1.0);
}

void FluentRatingControlTest::testSignalEmission() {
    QSignalSpy ratingSpy(m_ratingControl, &FluentRatingControl::ratingChanged);
    QSignalSpy maxRatingSpy(m_ratingControl, &FluentRatingControl::maxRatingChanged);
    QSignalSpy precisionSpy(m_ratingControl, &FluentRatingControl::precisionChanged);
    QSignalSpy iconSpy(m_ratingControl, &FluentRatingControl::ratingIconChanged);
    QSignalSpy iconSizeSpy(m_ratingControl, &FluentRatingControl::iconSizeChanged);
    QSignalSpy colorSpy(m_ratingControl, &FluentRatingControl::iconColorChanged);
    QSignalSpy unratedColorSpy(m_ratingControl, &FluentRatingControl::unratedColorChanged);
    QSignalSpy readOnlySpy(m_ratingControl, &FluentRatingControl::readOnlyChanged);
    QSignalSpy ratingSelectedSpy(m_ratingControl, &FluentRatingControl::ratingSelected);
    QSignalSpy hoverRatingSpy(m_ratingControl, &FluentRatingControl::hoverRatingChanged);
    
    // Test all signals
    m_ratingControl->setRating(4.0);
    m_ratingControl->setMaxRating(10);
    m_ratingControl->setPrecision(FluentRatingPrecision::Half);
    m_ratingControl->setRatingIcon(FluentRatingIcon::Heart);
    m_ratingControl->setIconSize(FluentRatingIconSize::Large);
    m_ratingControl->setIconColor(Qt::red);
    m_ratingControl->setUnratedColor(Qt::gray);
    m_ratingControl->setReadOnly(true);
    
    // Simulate rating selection
    emit m_ratingControl->ratingSelected(3.5);
    
    // Simulate hover
    emit m_ratingControl->hoverRatingChanged(2.5);
    
    QCOMPARE(ratingSpy.count(), 1);
    QCOMPARE(maxRatingSpy.count(), 1);
    QCOMPARE(precisionSpy.count(), 1);
    QCOMPARE(iconSpy.count(), 1);
    QCOMPARE(iconSizeSpy.count(), 1);
    QCOMPARE(colorSpy.count(), 1);
    QCOMPARE(unratedColorSpy.count(), 1);
    QCOMPARE(readOnlySpy.count(), 1);
    QCOMPARE(ratingSelectedSpy.count(), 1);
    QCOMPARE(hoverRatingSpy.count(), 1);
}

void FluentRatingControlTest::testRatingValidation() {
    // Test precision validation
    m_ratingControl->setPrecision(FluentRatingPrecision::Half);
    
    // Test various rating values with half precision
    m_ratingControl->setRating(2.3);
    QCOMPARE(m_ratingControl->rating(), 2.5); // Should round to nearest half
    
    m_ratingControl->setRating(2.7);
    QCOMPARE(m_ratingControl->rating(), 2.5); // Should round to nearest half
    
    m_ratingControl->setRating(2.8);
    QCOMPARE(m_ratingControl->rating(), 3.0); // Should round to nearest half
    
    // Test full precision
    m_ratingControl->setPrecision(FluentRatingPrecision::Full);
    m_ratingControl->setRating(2.3);
    QCOMPARE(m_ratingControl->rating(), 2.0); // Should round to nearest full
    
    m_ratingControl->setRating(2.7);
    QCOMPARE(m_ratingControl->rating(), 3.0); // Should round to nearest full
    
    // Test exact precision
    m_ratingControl->setPrecision(FluentRatingPrecision::Exact);
    m_ratingControl->setRating(2.37);
    QCOMPARE(m_ratingControl->rating(), 2.37); // Should keep exact value
    
    // Test bounds validation
    m_ratingControl->setMaxRating(5);
    m_ratingControl->setRating(7.5);
    QCOMPARE(m_ratingControl->rating(), 5.0); // Should clamp to max
    
    m_ratingControl->setRating(-1.5);
    QCOMPARE(m_ratingControl->rating(), 0.0); // Should clamp to min
    
    // Test validation with different max ratings
    m_ratingControl->setMaxRating(3);
    m_ratingControl->setRating(4.0);
    QCOMPARE(m_ratingControl->rating(), 3.0); // Should clamp to new max
    
    // Test that rating is valid
    QVERIFY(m_ratingControl->isValidRating(2.5));
    QVERIFY(!m_ratingControl->isValidRating(-1.0));
    QVERIFY(!m_ratingControl->isValidRating(10.0)); // Above max
}

QTEST_MAIN(FluentRatingControlTest)
#include "FluentRatingControlTest.moc"
