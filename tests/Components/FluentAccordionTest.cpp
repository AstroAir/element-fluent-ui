// tests/Components/FluentAccordionTest.cpp
#include <QKeyEvent>
#include <QMouseEvent>
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentAccordion.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentAccordionTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();
    void testTitleConstructor();

    // Content tests
    void testTitle();
    void testDescription();
    void testIcon();
    void testContentWidget();

    // State tests
    void testAccordionState();
    void testCollapsible();
    void testToggle();
    void testExpand();
    void testCollapse();

    // Animation tests
    void testContentOpacity();
    void testContentHeight();

    // Interaction tests
    void testMouseInteraction();
    void testKeyboardInteraction();

    // Signal tests
    void testSignalEmission();

    // Size tests
    void testSizeHints();

private:
    FluentAccordion* m_accordion{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentAccordionTest::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(400, 300);
}

void FluentAccordionTest::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentAccordionTest::init() {
    m_accordion = new FluentAccordion(m_testWidget);
}

void FluentAccordionTest::cleanup() {
    delete m_accordion;
    m_accordion = nullptr;
}

void FluentAccordionTest::testDefaultConstructor() {
    QVERIFY(m_accordion != nullptr);
    QCOMPARE(m_accordion->title(), QString());
    QCOMPARE(m_accordion->description(), QString());
    QCOMPARE(m_accordion->accordionState(), FluentAccordionState::Collapsed);
    QVERIFY(m_accordion->isCollapsible());
}

void FluentAccordionTest::testTitleConstructor() {
    auto* accordion = new FluentAccordion("Test Title", m_testWidget);
    QCOMPARE(accordion->title(), "Test Title");
    delete accordion;
}

void FluentAccordionTest::testTitle() {
    QSignalSpy titleSpy(m_accordion, &FluentAccordion::titleChanged);
    
    m_accordion->setTitle("New Title");
    QCOMPARE(m_accordion->title(), "New Title");
    QCOMPARE(titleSpy.count(), 1);
    QCOMPARE(titleSpy.first().first().toString(), "New Title");
}

void FluentAccordionTest::testDescription() {
    QSignalSpy descSpy(m_accordion, &FluentAccordion::descriptionChanged);
    
    m_accordion->setDescription("Test Description");
    QCOMPARE(m_accordion->description(), "Test Description");
    QCOMPARE(descSpy.count(), 1);
}

void FluentAccordionTest::testIcon() {
    QSignalSpy iconSpy(m_accordion, &FluentAccordion::iconChanged);
    QIcon testIcon(":/test/icon.png");
    
    m_accordion->setIcon(testIcon);
    QCOMPARE(m_accordion->icon().name(), testIcon.name());
    QCOMPARE(iconSpy.count(), 1);
}

void FluentAccordionTest::testContentWidget() {
    auto* content = new QWidget();
    m_accordion->setContentWidget(content);
    QCOMPARE(m_accordion->contentWidget(), content);
}

void FluentAccordionTest::testAccordionState() {
    QSignalSpy stateSpy(m_accordion, &FluentAccordion::accordionStateChanged);
    
    m_accordion->setAccordionState(FluentAccordionState::Expanded);
    QCOMPARE(m_accordion->accordionState(), FluentAccordionState::Expanded);
    QCOMPARE(stateSpy.count(), 1);
}

void FluentAccordionTest::testCollapsible() {
    QSignalSpy collapsibleSpy(m_accordion, &FluentAccordion::collapsibleChanged);
    
    m_accordion->setCollapsible(false);
    QVERIFY(!m_accordion->isCollapsible());
    QCOMPARE(collapsibleSpy.count(), 1);
}

void FluentAccordionTest::testToggle() {
    QSignalSpy expandedSpy(m_accordion, &FluentAccordion::expanded);
    QSignalSpy collapsedSpy(m_accordion, &FluentAccordion::collapsed);
    
    // Start collapsed, toggle should expand
    m_accordion->toggle();
    QCOMPARE(m_accordion->accordionState(), FluentAccordionState::Expanded);
    QCOMPARE(expandedSpy.count(), 1);
    
    // Toggle again should collapse
    m_accordion->toggle();
    QCOMPARE(m_accordion->accordionState(), FluentAccordionState::Collapsed);
    QCOMPARE(collapsedSpy.count(), 1);
}

void FluentAccordionTest::testExpand() {
    QSignalSpy expandedSpy(m_accordion, &FluentAccordion::expanded);
    
    m_accordion->expand();
    QCOMPARE(m_accordion->accordionState(), FluentAccordionState::Expanded);
    QCOMPARE(expandedSpy.count(), 1);
}

void FluentAccordionTest::testCollapse() {
    QSignalSpy collapsedSpy(m_accordion, &FluentAccordion::collapsed);
    
    m_accordion->setAccordionState(FluentAccordionState::Expanded);
    m_accordion->collapse();
    QCOMPARE(m_accordion->accordionState(), FluentAccordionState::Collapsed);
    QCOMPARE(collapsedSpy.count(), 1);
}

void FluentAccordionTest::testContentOpacity() {
    m_accordion->setContentOpacity(0.5);
    QCOMPARE(m_accordion->contentOpacity(), 0.5);
}

void FluentAccordionTest::testContentHeight() {
    m_accordion->setContentHeight(200);
    QCOMPARE(m_accordion->contentHeight(), 200);
}

void FluentAccordionTest::testMouseInteraction() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    QSignalSpy expandedSpy(m_accordion, &FluentAccordion::expanded);
    
    // Click on header should toggle
    QTest::mouseClick(m_accordion, Qt::LeftButton);
    QCOMPARE(expandedSpy.count(), 1);
}

void FluentAccordionTest::testKeyboardInteraction() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    m_accordion->setFocus();
    
    QSignalSpy expandedSpy(m_accordion, &FluentAccordion::expanded);
    
    // Space or Enter should toggle
    QTest::keyClick(m_accordion, Qt::Key_Space);
    QCOMPARE(expandedSpy.count(), 1);
}

void FluentAccordionTest::testSignalEmission() {
    QSignalSpy titleSpy(m_accordion, &FluentAccordion::titleChanged);
    QSignalSpy descSpy(m_accordion, &FluentAccordion::descriptionChanged);
    QSignalSpy iconSpy(m_accordion, &FluentAccordion::iconChanged);
    QSignalSpy stateSpy(m_accordion, &FluentAccordion::accordionStateChanged);
    QSignalSpy collapsibleSpy(m_accordion, &FluentAccordion::collapsibleChanged);
    QSignalSpy expandedSpy(m_accordion, &FluentAccordion::expanded);
    QSignalSpy collapsedSpy(m_accordion, &FluentAccordion::collapsed);
    
    // Test all signals are emitted correctly
    m_accordion->setTitle("Test");
    m_accordion->setDescription("Test Desc");
    m_accordion->setIcon(QIcon());
    m_accordion->setAccordionState(FluentAccordionState::Expanded);
    m_accordion->setCollapsible(false);
    m_accordion->expand();
    m_accordion->collapse();
    
    QCOMPARE(titleSpy.count(), 1);
    QCOMPARE(descSpy.count(), 1);
    QCOMPARE(iconSpy.count(), 1);
    QCOMPARE(stateSpy.count(), 2); // One from setAccordionState, one from collapse
    QCOMPARE(collapsibleSpy.count(), 1);
    QCOMPARE(expandedSpy.count(), 1);
    QCOMPARE(collapsedSpy.count(), 1);
}

void FluentAccordionTest::testSizeHints() {
    QSize sizeHint = m_accordion->sizeHint();
    QSize minSizeHint = m_accordion->minimumSizeHint();
    
    QVERIFY(sizeHint.isValid());
    QVERIFY(minSizeHint.isValid());
    QVERIFY(sizeHint.width() >= minSizeHint.width());
    QVERIFY(sizeHint.height() >= minSizeHint.height());
}

QTEST_MAIN(FluentAccordionTest)
#include "FluentAccordionTest.moc"
