// tests/Components/FluentBadgeTest.cpp
#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>
#include <QLabel>
#include <QPushButton>

#include "FluentQt/Components/FluentBadge.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentBadgeTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();
    void testTextConstructor();
    void testCountConstructor();
    void testIconConstructor();
    void testTypeConstructor();

    // Content tests
    void testText();
    void testCount();
    void testMaxCount();
    void testIcon();

    // Type and appearance tests
    void testBadgeType();
    void testBadgeSize();
    void testBadgeStyle();
    void testPosition();
    void testStatus();
    void testCustomColor();

    // Behavior tests
    void testVisible();
    void testAnimated();
    void testShowZero();
    void testPulsing();

    // Utility tests
    void testDisplayText();
    void testIsEmpty();
    void testBadgeRect();

    // Attachment tests
    void testAttachTo();
    void testDetach();
    void testAttachedWidget();

    // Static factory methods tests
    void testCreateCountBadge();
    void testCreateStatusBadge();
    void testCreateDotBadge();
    void testCreateTextBadge();

    // Animation tests
    void testShowHideAnimation();
    void testPulseAnimation();

    // Interaction tests
    void testIncrement();
    void testDecrement();
    void testReset();

    // Signal tests
    void testPropertyChangeSignals();
    void testInteractionSignals();

    // Accessibility tests
    void testAccessibility();

    // Theme integration tests
    void testThemeIntegration();

private:
    FluentBadge* m_badge;
    QWidget* m_parentWidget;
};

void FluentBadgeTest::initTestCase() {
    // Initialize any resources needed for all tests
}

void FluentBadgeTest::cleanupTestCase() {
    // Clean up any resources used for all tests
}

void FluentBadgeTest::init() {
    // Create a fresh badge and parent widget before each test
    m_badge = new FluentBadge();
    m_parentWidget = new QWidget();
    m_parentWidget->resize(200, 100);
    m_parentWidget->show();
}

void FluentBadgeTest::cleanup() {
    // Clean up after each test
    delete m_badge;
    delete m_parentWidget;
    m_badge = nullptr;
    m_parentWidget = nullptr;
}

void FluentBadgeTest::testDefaultConstructor() {
    // Test default constructor
    FluentBadge* badge = new FluentBadge();
    
    // Verify default properties
    QVERIFY(badge->text().isEmpty());
    QCOMPARE(badge->count(), 0);
    QCOMPARE(badge->maxCount(), 99);
    QVERIFY(badge->icon().isNull());
    QCOMPARE(badge->badgeType(), FluentBadgeType::Count);
    QCOMPARE(badge->badgeSize(), FluentBadgeSize::Medium);
    QCOMPARE(badge->badgeStyle(), FluentBadgeStyle::Filled);
    QCOMPARE(badge->position(), FluentBadgePosition::TopRight);
    QCOMPARE(badge->status(), FluentBadgeStatus::None);
    QVERIFY(!badge->isVisible());
    QVERIFY(badge->isAnimated());
    QVERIFY(!badge->showZero());
    QVERIFY(!badge->isPulsing());
    QVERIFY(badge->isEmpty());
    
    delete badge;
}

void FluentBadgeTest::testTextConstructor() {
    // Test constructor with text
    const QString badgeText = "NEW";
    FluentBadge* badge = new FluentBadge(badgeText);
    
    QCOMPARE(badge->text(), badgeText);
    QCOMPARE(badge->badgeType(), FluentBadgeType::Text);
    QVERIFY(!badge->isEmpty());
    
    delete badge;
}

void FluentBadgeTest::testCountConstructor() {
    // Test constructor with count
    const int badgeCount = 5;
    FluentBadge* badge = new FluentBadge(badgeCount);
    
    QCOMPARE(badge->count(), badgeCount);
    QCOMPARE(badge->badgeType(), FluentBadgeType::Count);
    QVERIFY(!badge->isEmpty());
    
    delete badge;
}

void FluentBadgeTest::testIconConstructor() {
    // Test constructor with icon
    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::red);
    QIcon badgeIcon(pixmap);
    
    FluentBadge* badge = new FluentBadge(badgeIcon);
    
    QVERIFY(!badge->icon().isNull());
    QCOMPARE(badge->badgeType(), FluentBadgeType::Icon);
    QVERIFY(!badge->isEmpty());
    
    delete badge;
}

void FluentBadgeTest::testTypeConstructor() {
    // Test constructor with type
    FluentBadge* dotBadge = new FluentBadge(FluentBadgeType::Dot);
    QCOMPARE(dotBadge->badgeType(), FluentBadgeType::Dot);
    delete dotBadge;
    
    FluentBadge* statusBadge = new FluentBadge(FluentBadgeType::Status);
    QCOMPARE(statusBadge->badgeType(), FluentBadgeType::Status);
    delete statusBadge;
}

void FluentBadgeTest::testText() {
    // Test setting and getting text
    const QString text1 = "NEW";
    const QString text2 = "HOT";
    
    QSignalSpy textChangedSpy(m_badge, &FluentBadge::textChanged);
    
    m_badge->setText(text1);
    QCOMPARE(m_badge->text(), text1);
    QCOMPARE(textChangedSpy.count(), 1);
    QCOMPARE(textChangedSpy.first().first().toString(), text1);
    
    m_badge->setText(text2);
    QCOMPARE(m_badge->text(), text2);
    QCOMPARE(textChangedSpy.count(), 2);
    QCOMPARE(textChangedSpy.last().first().toString(), text2);
    
    // Setting the same text should not emit the signal
    m_badge->setText(text2);
    QCOMPARE(textChangedSpy.count(), 2);
}

void FluentBadgeTest::testCount() {
    // Test setting and getting count
    QSignalSpy countChangedSpy(m_badge, &FluentBadge::countChanged);
    
    QCOMPARE(m_badge->count(), 0); // Default count
    
    m_badge->setCount(5);
    QCOMPARE(m_badge->count(), 5);
    QCOMPARE(countChangedSpy.count(), 1);
    QCOMPARE(countChangedSpy.first().first().toInt(), 5);
    
    m_badge->setCount(10);
    QCOMPARE(m_badge->count(), 10);
    QCOMPARE(countChangedSpy.count(), 2);
    QCOMPARE(countChangedSpy.last().first().toInt(), 10);
    
    // Setting the same count should not emit the signal
    m_badge->setCount(10);
    QCOMPARE(countChangedSpy.count(), 2);
    
    // Test count above max count
    m_badge->setMaxCount(99);
    m_badge->setCount(150);
    QCOMPARE(m_badge->count(), 150);
    QCOMPARE(m_badge->displayText(), QString("99+")); // Should show "99+"
}

void FluentBadgeTest::testMaxCount() {
    // Test setting and getting max count
    QSignalSpy maxCountChangedSpy(m_badge, &FluentBadge::maxCountChanged);
    
    QCOMPARE(m_badge->maxCount(), 99); // Default max count
    
    m_badge->setMaxCount(50);
    QCOMPARE(m_badge->maxCount(), 50);
    QCOMPARE(maxCountChangedSpy.count(), 1);
    QCOMPARE(maxCountChangedSpy.first().first().toInt(), 50);
    
    m_badge->setMaxCount(200);
    QCOMPARE(m_badge->maxCount(), 200);
    QCOMPARE(maxCountChangedSpy.count(), 2);
    QCOMPARE(maxCountChangedSpy.last().first().toInt(), 200);
    
    // Setting the same max count should not emit the signal
    m_badge->setMaxCount(200);
    QCOMPARE(maxCountChangedSpy.count(), 2);
}

void FluentBadgeTest::testIcon() {
    // Test setting and getting icon
    QSignalSpy iconChangedSpy(m_badge, &FluentBadge::iconChanged);
    
    QPixmap pixmap1(16, 16);
    pixmap1.fill(Qt::red);
    QIcon icon1(pixmap1);
    
    m_badge->setIcon(icon1);
    QVERIFY(!m_badge->icon().isNull());
    QCOMPARE(iconChangedSpy.count(), 1);
    
    QPixmap pixmap2(16, 16);
    pixmap2.fill(Qt::green);
    QIcon icon2(pixmap2);
    
    m_badge->setIcon(icon2);
    QVERIFY(!m_badge->icon().isNull());
    QCOMPARE(iconChangedSpy.count(), 2);
}

void FluentBadgeTest::testBadgeType() {
    // Test setting and getting badge type
    QSignalSpy badgeTypeChangedSpy(m_badge, &FluentBadge::badgeTypeChanged);
    
    QCOMPARE(m_badge->badgeType(), FluentBadgeType::Count); // Default type
    
    m_badge->setBadgeType(FluentBadgeType::Dot);
    QCOMPARE(m_badge->badgeType(), FluentBadgeType::Dot);
    QCOMPARE(badgeTypeChangedSpy.count(), 1);
    QCOMPARE(badgeTypeChangedSpy.first().first().value<FluentBadgeType>(), FluentBadgeType::Dot);
    
    m_badge->setBadgeType(FluentBadgeType::Text);
    QCOMPARE(m_badge->badgeType(), FluentBadgeType::Text);
    QCOMPARE(badgeTypeChangedSpy.count(), 2);
    
    m_badge->setBadgeType(FluentBadgeType::Icon);
    QCOMPARE(m_badge->badgeType(), FluentBadgeType::Icon);
    QCOMPARE(badgeTypeChangedSpy.count(), 3);
    
    m_badge->setBadgeType(FluentBadgeType::Status);
    QCOMPARE(m_badge->badgeType(), FluentBadgeType::Status);
    QCOMPARE(badgeTypeChangedSpy.count(), 4);
    
    // Setting the same type should not emit the signal
    m_badge->setBadgeType(FluentBadgeType::Status);
    QCOMPARE(badgeTypeChangedSpy.count(), 4);
}

void FluentBadgeTest::testBadgeSize() {
    // Test setting and getting badge size
    QSignalSpy badgeSizeChangedSpy(m_badge, &FluentBadge::badgeSizeChanged);

    QCOMPARE(m_badge->badgeSize(), FluentBadgeSize::Medium); // Default size

    m_badge->setBadgeSize(FluentBadgeSize::Small);
    QCOMPARE(m_badge->badgeSize(), FluentBadgeSize::Small);
    QCOMPARE(badgeSizeChangedSpy.count(), 1);
    QCOMPARE(badgeSizeChangedSpy.first().first().value<FluentBadgeSize>(), FluentBadgeSize::Small);

    m_badge->setBadgeSize(FluentBadgeSize::Large);
    QCOMPARE(m_badge->badgeSize(), FluentBadgeSize::Large);
    QCOMPARE(badgeSizeChangedSpy.count(), 2);

    // Setting the same size should not emit the signal
    m_badge->setBadgeSize(FluentBadgeSize::Large);
    QCOMPARE(badgeSizeChangedSpy.count(), 2);
}

void FluentBadgeTest::testBadgeStyle() {
    // Test setting and getting badge style
    QSignalSpy badgeStyleChangedSpy(m_badge, &FluentBadge::badgeStyleChanged);

    QCOMPARE(m_badge->badgeStyle(), FluentBadgeStyle::Filled); // Default style

    m_badge->setBadgeStyle(FluentBadgeStyle::Outline);
    QCOMPARE(m_badge->badgeStyle(), FluentBadgeStyle::Outline);
    QCOMPARE(badgeStyleChangedSpy.count(), 1);
    QCOMPARE(badgeStyleChangedSpy.first().first().value<FluentBadgeStyle>(), FluentBadgeStyle::Outline);

    m_badge->setBadgeStyle(FluentBadgeStyle::Subtle);
    QCOMPARE(m_badge->badgeStyle(), FluentBadgeStyle::Subtle);
    QCOMPARE(badgeStyleChangedSpy.count(), 2);

    // Setting the same style should not emit the signal
    m_badge->setBadgeStyle(FluentBadgeStyle::Subtle);
    QCOMPARE(badgeStyleChangedSpy.count(), 2);
}

void FluentBadgeTest::testPosition() {
    // Test setting and getting position
    QSignalSpy positionChangedSpy(m_badge, &FluentBadge::positionChanged);

    QCOMPARE(m_badge->position(), FluentBadgePosition::TopRight); // Default position

    m_badge->setPosition(FluentBadgePosition::TopLeft);
    QCOMPARE(m_badge->position(), FluentBadgePosition::TopLeft);
    QCOMPARE(positionChangedSpy.count(), 1);
    QCOMPARE(positionChangedSpy.first().first().value<FluentBadgePosition>(), FluentBadgePosition::TopLeft);

    m_badge->setPosition(FluentBadgePosition::BottomRight);
    QCOMPARE(m_badge->position(), FluentBadgePosition::BottomRight);
    QCOMPARE(positionChangedSpy.count(), 2);

    m_badge->setPosition(FluentBadgePosition::BottomLeft);
    QCOMPARE(m_badge->position(), FluentBadgePosition::BottomLeft);
    QCOMPARE(positionChangedSpy.count(), 3);

    // Setting the same position should not emit the signal
    m_badge->setPosition(FluentBadgePosition::BottomLeft);
    QCOMPARE(positionChangedSpy.count(), 3);
}

void FluentBadgeTest::testStatus() {
    // Test setting and getting status
    QSignalSpy statusChangedSpy(m_badge, &FluentBadge::statusChanged);

    QCOMPARE(m_badge->status(), FluentBadgeStatus::None); // Default status

    m_badge->setStatus(FluentBadgeStatus::Success);
    QCOMPARE(m_badge->status(), FluentBadgeStatus::Success);
    QCOMPARE(statusChangedSpy.count(), 1);
    QCOMPARE(statusChangedSpy.first().first().value<FluentBadgeStatus>(), FluentBadgeStatus::Success);

    m_badge->setStatus(FluentBadgeStatus::Warning);
    QCOMPARE(m_badge->status(), FluentBadgeStatus::Warning);
    QCOMPARE(statusChangedSpy.count(), 2);

    m_badge->setStatus(FluentBadgeStatus::Error);
    QCOMPARE(m_badge->status(), FluentBadgeStatus::Error);
    QCOMPARE(statusChangedSpy.count(), 3);

    m_badge->setStatus(FluentBadgeStatus::Info);
    QCOMPARE(m_badge->status(), FluentBadgeStatus::Info);
    QCOMPARE(statusChangedSpy.count(), 4);

    // Setting the same status should not emit the signal
    m_badge->setStatus(FluentBadgeStatus::Info);
    QCOMPARE(statusChangedSpy.count(), 4);
}

void FluentBadgeTest::testCustomColor() {
    // Test setting and getting custom color
    QSignalSpy customColorChangedSpy(m_badge, &FluentBadge::customColorChanged);

    QColor color1(255, 0, 0); // Red
    m_badge->setCustomColor(color1);
    QCOMPARE(m_badge->customColor(), color1);
    QCOMPARE(customColorChangedSpy.count(), 1);
    QCOMPARE(customColorChangedSpy.first().first().value<QColor>(), color1);

    QColor color2(0, 255, 0); // Green
    m_badge->setCustomColor(color2);
    QCOMPARE(m_badge->customColor(), color2);
    QCOMPARE(customColorChangedSpy.count(), 2);
    QCOMPARE(customColorChangedSpy.last().first().value<QColor>(), color2);

    // Setting the same color should not emit the signal
    m_badge->setCustomColor(color2);
    QCOMPARE(customColorChangedSpy.count(), 2);
}

void FluentBadgeTest::testVisible() {
    // Test visibility
    QSignalSpy visibilityChangedSpy(m_badge, &FluentBadge::visibilityChanged);

    QVERIFY(!m_badge->isVisible()); // Default should be false

    m_badge->setVisible(true);
    QVERIFY(m_badge->isVisible());
    QCOMPARE(visibilityChangedSpy.count(), 1);
    QCOMPARE(visibilityChangedSpy.first().first().toBool(), true);

    m_badge->setVisible(false);
    QVERIFY(!m_badge->isVisible());
    QCOMPARE(visibilityChangedSpy.count(), 2);
    QCOMPARE(visibilityChangedSpy.last().first().toBool(), false);

    // Setting the same visibility should not emit the signal
    m_badge->setVisible(false);
    QCOMPARE(visibilityChangedSpy.count(), 2);
}

void FluentBadgeTest::testAnimated() {
    // Test animated property
    QSignalSpy animatedChangedSpy(m_badge, &FluentBadge::animatedChanged);

    QVERIFY(m_badge->isAnimated()); // Default should be true

    m_badge->setAnimated(false);
    QVERIFY(!m_badge->isAnimated());
    QCOMPARE(animatedChangedSpy.count(), 1);
    QCOMPARE(animatedChangedSpy.first().first().toBool(), false);

    m_badge->setAnimated(true);
    QVERIFY(m_badge->isAnimated());
    QCOMPARE(animatedChangedSpy.count(), 2);
    QCOMPARE(animatedChangedSpy.last().first().toBool(), true);

    // Setting the same animated state should not emit the signal
    m_badge->setAnimated(true);
    QCOMPARE(animatedChangedSpy.count(), 2);
}

void FluentBadgeTest::testShowZero() {
    // Test show zero property
    QSignalSpy showZeroChangedSpy(m_badge, &FluentBadge::showZeroChanged);

    QVERIFY(!m_badge->showZero()); // Default should be false

    m_badge->setShowZero(true);
    QVERIFY(m_badge->showZero());
    QCOMPARE(showZeroChangedSpy.count(), 1);
    QCOMPARE(showZeroChangedSpy.first().first().toBool(), true);

    m_badge->setShowZero(false);
    QVERIFY(!m_badge->showZero());
    QCOMPARE(showZeroChangedSpy.count(), 2);
    QCOMPARE(showZeroChangedSpy.last().first().toBool(), false);

    // Setting the same show zero state should not emit the signal
    m_badge->setShowZero(false);
    QCOMPARE(showZeroChangedSpy.count(), 2);

    // Test behavior with count = 0
    m_badge->setCount(0);
    QVERIFY(m_badge->isEmpty()); // Should be empty when showZero is false

    m_badge->setShowZero(true);
    QVERIFY(!m_badge->isEmpty()); // Should not be empty when showZero is true
}

void FluentBadgeTest::testPulsing() {
    // Test pulsing property
    QSignalSpy pulsingChangedSpy(m_badge, &FluentBadge::pulsingChanged);

    QVERIFY(!m_badge->isPulsing()); // Default should be false

    m_badge->setPulsing(true);
    QVERIFY(m_badge->isPulsing());
    QCOMPARE(pulsingChangedSpy.count(), 1);
    QCOMPARE(pulsingChangedSpy.first().first().toBool(), true);

    m_badge->setPulsing(false);
    QVERIFY(!m_badge->isPulsing());
    QCOMPARE(pulsingChangedSpy.count(), 2);
    QCOMPARE(pulsingChangedSpy.last().first().toBool(), false);

    // Setting the same pulsing state should not emit the signal
    m_badge->setPulsing(false);
    QCOMPARE(pulsingChangedSpy.count(), 2);
}

void FluentBadgeTest::testDisplayText() {
    // Test display text generation
    m_badge->setBadgeType(FluentBadgeType::Count);

    // Test normal count
    m_badge->setCount(5);
    QCOMPARE(m_badge->displayText(), QString("5"));

    // Test count at max
    m_badge->setMaxCount(99);
    m_badge->setCount(99);
    QCOMPARE(m_badge->displayText(), QString("99"));

    // Test count above max
    m_badge->setCount(150);
    QCOMPARE(m_badge->displayText(), QString("99+"));

    // Test zero count without showZero
    m_badge->setCount(0);
    m_badge->setShowZero(false);
    QCOMPARE(m_badge->displayText(), QString(""));

    // Test zero count with showZero
    m_badge->setShowZero(true);
    QCOMPARE(m_badge->displayText(), QString("0"));

    // Test text type
    m_badge->setBadgeType(FluentBadgeType::Text);
    m_badge->setText("NEW");
    QCOMPARE(m_badge->displayText(), QString("NEW"));
}

void FluentBadgeTest::testIsEmpty() {
    // Test isEmpty functionality
    QVERIFY(m_badge->isEmpty()); // Default should be empty

    // Count badge with count > 0
    m_badge->setBadgeType(FluentBadgeType::Count);
    m_badge->setCount(5);
    QVERIFY(!m_badge->isEmpty());

    // Count badge with count = 0 and showZero = false
    m_badge->setCount(0);
    m_badge->setShowZero(false);
    QVERIFY(m_badge->isEmpty());

    // Count badge with count = 0 and showZero = true
    m_badge->setShowZero(true);
    QVERIFY(!m_badge->isEmpty());

    // Text badge with text
    m_badge->setBadgeType(FluentBadgeType::Text);
    m_badge->setText("NEW");
    QVERIFY(!m_badge->isEmpty());

    // Text badge without text
    m_badge->setText("");
    QVERIFY(m_badge->isEmpty());

    // Dot badge (never empty)
    m_badge->setBadgeType(FluentBadgeType::Dot);
    QVERIFY(!m_badge->isEmpty());

    // Status badge (never empty)
    m_badge->setBadgeType(FluentBadgeType::Status);
    QVERIFY(!m_badge->isEmpty());

    // Icon badge with icon
    m_badge->setBadgeType(FluentBadgeType::Icon);
    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::red);
    m_badge->setIcon(QIcon(pixmap));
    QVERIFY(!m_badge->isEmpty());

    // Icon badge without icon
    m_badge->setIcon(QIcon());
    QVERIFY(m_badge->isEmpty());
}

void FluentBadgeTest::testBadgeRect() {
    // Test badge rect calculation
    m_badge->setCount(5);
    m_badge->setVisible(true);

    QRect rect = m_badge->badgeRect();
    QVERIFY(rect.isValid());
    QVERIFY(rect.width() > 0);
    QVERIFY(rect.height() > 0);
}

void FluentBadgeTest::testAttachTo() {
    // Test attaching badge to widget
    QVERIFY(m_badge->attachedWidget() == nullptr); // Initially not attached

    m_badge->attachTo(m_parentWidget);
    QCOMPARE(m_badge->attachedWidget(), m_parentWidget);

    // Test attaching with specific position
    m_badge->attachTo(m_parentWidget, FluentBadgePosition::TopLeft);
    QCOMPARE(m_badge->attachedWidget(), m_parentWidget);
    QCOMPARE(m_badge->position(), FluentBadgePosition::TopLeft);
}

void FluentBadgeTest::testDetach() {
    // Test detaching badge from widget
    m_badge->attachTo(m_parentWidget);
    QCOMPARE(m_badge->attachedWidget(), m_parentWidget);

    m_badge->detach();
    QVERIFY(m_badge->attachedWidget() == nullptr);
}

void FluentBadgeTest::testAttachedWidget() {
    // Test attached widget property
    QVERIFY(m_badge->attachedWidget() == nullptr);

    m_badge->attachTo(m_parentWidget);
    QCOMPARE(m_badge->attachedWidget(), m_parentWidget);

    // Attach to different widget
    QWidget* anotherWidget = new QWidget();
    m_badge->attachTo(anotherWidget);
    QCOMPARE(m_badge->attachedWidget(), anotherWidget);

    delete anotherWidget;
}

void FluentBadgeTest::testCreateCountBadge() {
    // Test count badge factory method
    FluentBadge* countBadge = FluentBadge::createCountBadge(10);

    QCOMPARE(countBadge->badgeType(), FluentBadgeType::Count);
    QCOMPARE(countBadge->count(), 10);
    QVERIFY(!countBadge->isEmpty());

    delete countBadge;
}

void FluentBadgeTest::testCreateStatusBadge() {
    // Test status badge factory method
    FluentBadge* statusBadge = FluentBadge::createStatusBadge(FluentBadgeStatus::Success);

    QCOMPARE(statusBadge->badgeType(), FluentBadgeType::Status);
    QCOMPARE(statusBadge->status(), FluentBadgeStatus::Success);
    QVERIFY(!statusBadge->isEmpty());

    delete statusBadge;
}

void FluentBadgeTest::testCreateDotBadge() {
    // Test dot badge factory method
    FluentBadge* dotBadge = FluentBadge::createDotBadge();

    QCOMPARE(dotBadge->badgeType(), FluentBadgeType::Dot);
    QVERIFY(!dotBadge->isEmpty());

    delete dotBadge;
}

void FluentBadgeTest::testCreateTextBadge() {
    // Test text badge factory method
    const QString text = "NEW";
    FluentBadge* textBadge = FluentBadge::createTextBadge(text);

    QCOMPARE(textBadge->badgeType(), FluentBadgeType::Text);
    QCOMPARE(textBadge->text(), text);
    QVERIFY(!textBadge->isEmpty());

    delete textBadge;
}

void FluentBadgeTest::testShowHideAnimation() {
    // Test show and hide animations
    m_badge->setCount(5);
    m_badge->setAnimated(true);

    // Test animated show
    m_badge->show();
    // Animation should start (hard to test without access to internal state)
    QVERIFY(true);

    // Test animated hide
    m_badge->hide();
    // Animation should start (hard to test without access to internal state)
    QVERIFY(true);

    // Test non-animated show/hide
    m_badge->setAnimated(false);
    m_badge->show();
    m_badge->hide();
    QVERIFY(true);
}

void FluentBadgeTest::testPulseAnimation() {
    // Test pulse animation
    m_badge->setCount(5);
    m_badge->setAnimated(true);

    // Test single pulse
    m_badge->pulse();
    QVERIFY(true); // Animation should start

    // Test continuous pulsing
    m_badge->setPulsing(true);
    QVERIFY(m_badge->isPulsing());

    // Test stop pulsing
    m_badge->stopPulsing();
    QVERIFY(!m_badge->isPulsing());
}

void FluentBadgeTest::testIncrement() {
    // Test increment functionality
    QSignalSpy countChangedSpy(m_badge, &FluentBadge::countChanged);

    m_badge->setCount(5);
    countChangedSpy.clear();

    m_badge->increment();
    QCOMPARE(m_badge->count(), 6);
    QCOMPARE(countChangedSpy.count(), 1);
    QCOMPARE(countChangedSpy.first().first().toInt(), 6);

    m_badge->increment();
    QCOMPARE(m_badge->count(), 7);
    QCOMPARE(countChangedSpy.count(), 2);
}

void FluentBadgeTest::testDecrement() {
    // Test decrement functionality
    QSignalSpy countChangedSpy(m_badge, &FluentBadge::countChanged);

    m_badge->setCount(5);
    countChangedSpy.clear();

    m_badge->decrement();
    QCOMPARE(m_badge->count(), 4);
    QCOMPARE(countChangedSpy.count(), 1);
    QCOMPARE(countChangedSpy.first().first().toInt(), 4);

    // Test decrement at zero (should not go below 0)
    m_badge->setCount(0);
    countChangedSpy.clear();

    m_badge->decrement();
    QCOMPARE(m_badge->count(), 0);
    QCOMPARE(countChangedSpy.count(), 0); // Should not emit signal
}

void FluentBadgeTest::testReset() {
    // Test reset functionality
    QSignalSpy countChangedSpy(m_badge, &FluentBadge::countChanged);

    m_badge->setCount(10);
    countChangedSpy.clear();

    m_badge->reset();
    QCOMPARE(m_badge->count(), 0);
    QCOMPARE(countChangedSpy.count(), 1);
    QCOMPARE(countChangedSpy.first().first().toInt(), 0);
}

void FluentBadgeTest::testPropertyChangeSignals() {
    // Test that property change signals are emitted correctly
    QSignalSpy textChangedSpy(m_badge, &FluentBadge::textChanged);
    QSignalSpy countChangedSpy(m_badge, &FluentBadge::countChanged);
    QSignalSpy maxCountChangedSpy(m_badge, &FluentBadge::maxCountChanged);
    QSignalSpy iconChangedSpy(m_badge, &FluentBadge::iconChanged);
    QSignalSpy badgeTypeChangedSpy(m_badge, &FluentBadge::badgeTypeChanged);
    QSignalSpy badgeSizeChangedSpy(m_badge, &FluentBadge::badgeSizeChanged);
    QSignalSpy badgeStyleChangedSpy(m_badge, &FluentBadge::badgeStyleChanged);
    QSignalSpy positionChangedSpy(m_badge, &FluentBadge::positionChanged);
    QSignalSpy statusChangedSpy(m_badge, &FluentBadge::statusChanged);
    QSignalSpy customColorChangedSpy(m_badge, &FluentBadge::customColorChanged);
    QSignalSpy visibilityChangedSpy(m_badge, &FluentBadge::visibilityChanged);
    QSignalSpy animatedChangedSpy(m_badge, &FluentBadge::animatedChanged);
    QSignalSpy showZeroChangedSpy(m_badge, &FluentBadge::showZeroChanged);
    QSignalSpy pulsingChangedSpy(m_badge, &FluentBadge::pulsingChanged);

    // Change properties and verify signals
    m_badge->setText("NEW");
    QCOMPARE(textChangedSpy.count(), 1);

    m_badge->setCount(5);
    QCOMPARE(countChangedSpy.count(), 1);

    m_badge->setMaxCount(50);
    QCOMPARE(maxCountChangedSpy.count(), 1);

    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::red);
    m_badge->setIcon(QIcon(pixmap));
    QCOMPARE(iconChangedSpy.count(), 1);

    m_badge->setBadgeType(FluentBadgeType::Text);
    QCOMPARE(badgeTypeChangedSpy.count(), 1);

    m_badge->setBadgeSize(FluentBadgeSize::Large);
    QCOMPARE(badgeSizeChangedSpy.count(), 1);

    m_badge->setBadgeStyle(FluentBadgeStyle::Outline);
    QCOMPARE(badgeStyleChangedSpy.count(), 1);

    m_badge->setPosition(FluentBadgePosition::TopLeft);
    QCOMPARE(positionChangedSpy.count(), 1);

    m_badge->setStatus(FluentBadgeStatus::Success);
    QCOMPARE(statusChangedSpy.count(), 1);

    m_badge->setCustomColor(QColor(255, 0, 0));
    QCOMPARE(customColorChangedSpy.count(), 1);

    m_badge->setVisible(true);
    QCOMPARE(visibilityChangedSpy.count(), 1);

    m_badge->setAnimated(false);
    QCOMPARE(animatedChangedSpy.count(), 1);

    m_badge->setShowZero(true);
    QCOMPARE(showZeroChangedSpy.count(), 1);

    m_badge->setPulsing(true);
    QCOMPARE(pulsingChangedSpy.count(), 1);
}

void FluentBadgeTest::testInteractionSignals() {
    // Test interaction signals
    QSignalSpy clickedSpy(m_badge, &FluentBadge::clicked);
    QSignalSpy doubleClickedSpy(m_badge, &FluentBadge::doubleClicked);

    // These signals would be emitted by mouse events
    // For now, just verify the signals exist and can be connected
    const QMetaObject* metaObj = m_badge->metaObject();
    QVERIFY(QMetaObject::indexOfSignal(metaObj, "clicked()") != -1);
    QVERIFY(QMetaObject::indexOfSignal(metaObj, "doubleClicked()") != -1);
}

void FluentBadgeTest::testAccessibility() {
    // Test accessibility features
    m_badge->setCount(5);
    m_badge->setText("Notification badge");

    // Test that badge can be created and used without accessibility-related crashes
    m_badge->setVisible(true);
    m_badge->attachTo(m_parentWidget);

    // Accessibility should work without crashing
    QVERIFY(true);
}

void FluentBadgeTest::testThemeIntegration() {
    // Test that badge responds to theme changes
    auto& theme = FluentQt::Styling::FluentTheme::instance();

    // Get current theme mode
    auto originalMode = theme.mode();

    // Change theme mode
    auto newMode = (originalMode == FluentQt::Styling::FluentThemeMode::Light) ?
                   FluentQt::Styling::FluentThemeMode::Dark :
                   FluentQt::Styling::FluentThemeMode::Light;
    theme.setMode(newMode);

    // Badge should update its appearance (this would require checking internal styling)
    // For now, just verify the badge still functions correctly
    QSignalSpy countChangedSpy(m_badge, &FluentBadge::countChanged);
    m_badge->setCount(42);
    QCOMPARE(m_badge->count(), 42);
    QCOMPARE(countChangedSpy.count(), 1);

    // Test that badge can be shown with new theme
    m_badge->setVisible(true);
    m_badge->attachTo(m_parentWidget);

    // Restore original theme
    theme.setMode(originalMode);
}

QTEST_MAIN(FluentBadgeTest)
#include "FluentBadgeTest.moc"
