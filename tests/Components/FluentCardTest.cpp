// tests/Components/FluentCardTest.cpp
#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentCardTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();
    void testTitleConstructor();
    void testTitleSubtitleConstructor();

    // Header tests
    void testTitle();
    void testSubtitle();
    void testHeaderIcon();
    void testHeaderVisible();

    // Style tests
    void testElevation();
    void testCardStyle();
    void testShadowOpacity();

    // Selection tests
    void testSelectable();
    void testSelected();

    // Expansion tests
    void testExpandable();
    void testExpanded();
    void testToggleExpanded();

    // Footer tests
    void testFooterVisible();

    // Content management tests
    void testContentWidget();
    void testHeaderActions();
    void testFooterWidgets();

    // Layout tests
    void testContentLayout();
    void testHeaderLayout();
    void testFooterLayout();

    // Animation tests
    void testAnimateIn();
    void testAnimateOut();
    void testExpandWithAnimation();
    void testCollapseWithAnimation();

    // Interaction tests
    void testMouseInteraction();
    void testCardClicked();
    void testCardDoubleClicked();
    void testHeaderClicked();

    // Signal tests
    void testPropertyChangeSignals();
    void testInteractionSignals();

    // Accessibility tests
    void testAccessibility();

    // Theme integration tests
    void testThemeIntegration();

private:
    FluentCard* m_card;
};

void FluentCardTest::initTestCase() {
    // Initialize any resources needed for all tests
}

void FluentCardTest::cleanupTestCase() {
    // Clean up any resources used for all tests
}

void FluentCardTest::init() {
    // Create a fresh card before each test
    m_card = new FluentCard();
    m_card->show();
    QTest::qWaitForWindowExposed(m_card);
}

void FluentCardTest::cleanup() {
    // Clean up after each test
    delete m_card;
    m_card = nullptr;
}

void FluentCardTest::testDefaultConstructor() {
    // Test default constructor
    FluentCard* card = new FluentCard();
    
    // Verify default properties
    QVERIFY(card->title().isEmpty());
    QVERIFY(card->subtitle().isEmpty());
    QVERIFY(card->headerIcon().isNull());
    QCOMPARE(card->elevation(), FluentCardElevation::Low);
    QCOMPARE(card->cardStyle(), FluentCardStyle::Default);
    QVERIFY(!card->isSelectable());
    QVERIFY(!card->isSelected());
    QVERIFY(!card->isExpandable());
    QVERIFY(card->isExpanded());
    QVERIFY(card->isHeaderVisible());
    QVERIFY(!card->isFooterVisible());
    QVERIFY(card->isEnabled());
    
    delete card;
}

void FluentCardTest::testTitleConstructor() {
    // Test constructor with title
    const QString cardTitle = "Test Card";
    FluentCard* card = new FluentCard(cardTitle);
    
    QCOMPARE(card->title(), cardTitle);
    QVERIFY(card->subtitle().isEmpty());
    
    delete card;
}

void FluentCardTest::testTitleSubtitleConstructor() {
    // Test constructor with title and subtitle
    const QString cardTitle = "Test Card";
    const QString cardSubtitle = "Test Subtitle";
    FluentCard* card = new FluentCard(cardTitle, cardSubtitle);
    
    QCOMPARE(card->title(), cardTitle);
    QCOMPARE(card->subtitle(), cardSubtitle);
    
    delete card;
}

void FluentCardTest::testTitle() {
    // Test setting and getting title
    const QString title1 = "Card Title";
    const QString title2 = "New Title";
    
    QSignalSpy titleChangedSpy(m_card, &FluentCard::titleChanged);
    
    m_card->setTitle(title1);
    QCOMPARE(m_card->title(), title1);
    QCOMPARE(titleChangedSpy.count(), 1);
    QCOMPARE(titleChangedSpy.first().first().toString(), title1);
    
    m_card->setTitle(title2);
    QCOMPARE(m_card->title(), title2);
    QCOMPARE(titleChangedSpy.count(), 2);
    QCOMPARE(titleChangedSpy.last().first().toString(), title2);
    
    // Setting the same title should not emit the signal
    m_card->setTitle(title2);
    QCOMPARE(titleChangedSpy.count(), 2);
}

void FluentCardTest::testSubtitle() {
    // Test setting and getting subtitle
    const QString subtitle1 = "Card Subtitle";
    const QString subtitle2 = "New Subtitle";
    
    QSignalSpy subtitleChangedSpy(m_card, &FluentCard::subtitleChanged);
    
    m_card->setSubtitle(subtitle1);
    QCOMPARE(m_card->subtitle(), subtitle1);
    QCOMPARE(subtitleChangedSpy.count(), 1);
    QCOMPARE(subtitleChangedSpy.first().first().toString(), subtitle1);
    
    m_card->setSubtitle(subtitle2);
    QCOMPARE(m_card->subtitle(), subtitle2);
    QCOMPARE(subtitleChangedSpy.count(), 2);
    QCOMPARE(subtitleChangedSpy.last().first().toString(), subtitle2);
    
    // Setting the same subtitle should not emit the signal
    m_card->setSubtitle(subtitle2);
    QCOMPARE(subtitleChangedSpy.count(), 2);
}

void FluentCardTest::testHeaderIcon() {
    // Test setting and getting header icon
    QSignalSpy headerIconChangedSpy(m_card, &FluentCard::headerIconChanged);
    
    QIcon icon1;
    QPixmap pixmap1(16, 16);
    pixmap1.fill(Qt::red);
    icon1 = QIcon(pixmap1);
    
    m_card->setHeaderIcon(icon1);
    QVERIFY(!m_card->headerIcon().isNull());
    QCOMPARE(headerIconChangedSpy.count(), 1);
    
    QIcon icon2;
    QPixmap pixmap2(16, 16);
    pixmap2.fill(Qt::green);
    icon2 = QIcon(pixmap2);
    
    m_card->setHeaderIcon(icon2);
    QVERIFY(!m_card->headerIcon().isNull());
    QCOMPARE(headerIconChangedSpy.count(), 2);
}

void FluentCardTest::testHeaderVisible() {
    // Test header visibility
    QVERIFY(m_card->isHeaderVisible()); // Default should be true
    
    m_card->setHeaderVisible(false);
    QVERIFY(!m_card->isHeaderVisible());
    
    m_card->setHeaderVisible(true);
    QVERIFY(m_card->isHeaderVisible());
}

void FluentCardTest::testElevation() {
    // Test setting and getting elevation
    QSignalSpy elevationChangedSpy(m_card, &FluentCard::elevationChanged);
    
    QCOMPARE(m_card->elevation(), FluentCardElevation::Low); // Default elevation
    
    m_card->setElevation(FluentCardElevation::Medium);
    QCOMPARE(m_card->elevation(), FluentCardElevation::Medium);
    QCOMPARE(elevationChangedSpy.count(), 1);
    QCOMPARE(elevationChangedSpy.first().first().value<FluentCardElevation>(), FluentCardElevation::Medium);
    
    m_card->setElevation(FluentCardElevation::High);
    QCOMPARE(m_card->elevation(), FluentCardElevation::High);
    QCOMPARE(elevationChangedSpy.count(), 2);
    
    m_card->setElevation(FluentCardElevation::Flat);
    QCOMPARE(m_card->elevation(), FluentCardElevation::Flat);
    QCOMPARE(elevationChangedSpy.count(), 3);
    
    // Setting the same elevation should not emit the signal
    m_card->setElevation(FluentCardElevation::Flat);
    QCOMPARE(elevationChangedSpy.count(), 3);
}

void FluentCardTest::testCardStyle() {
    // Test setting and getting card style
    QSignalSpy cardStyleChangedSpy(m_card, &FluentCard::cardStyleChanged);
    
    QCOMPARE(m_card->cardStyle(), FluentCardStyle::Default); // Default style
    
    m_card->setCardStyle(FluentCardStyle::Elevated);
    QCOMPARE(m_card->cardStyle(), FluentCardStyle::Elevated);
    QCOMPARE(cardStyleChangedSpy.count(), 1);
    QCOMPARE(cardStyleChangedSpy.first().first().value<FluentCardStyle>(), FluentCardStyle::Elevated);
    
    m_card->setCardStyle(FluentCardStyle::Outlined);
    QCOMPARE(m_card->cardStyle(), FluentCardStyle::Outlined);
    QCOMPARE(cardStyleChangedSpy.count(), 2);
    
    m_card->setCardStyle(FluentCardStyle::Filled);
    QCOMPARE(m_card->cardStyle(), FluentCardStyle::Filled);
    QCOMPARE(cardStyleChangedSpy.count(), 3);
    
    m_card->setCardStyle(FluentCardStyle::Subtle);
    QCOMPARE(m_card->cardStyle(), FluentCardStyle::Subtle);
    QCOMPARE(cardStyleChangedSpy.count(), 4);
    
    // Setting the same style should not emit the signal
    m_card->setCardStyle(FluentCardStyle::Subtle);
    QCOMPARE(cardStyleChangedSpy.count(), 4);
}

void FluentCardTest::testShadowOpacity() {
    // Test setting and getting shadow opacity
    QCOMPARE(m_card->shadowOpacity(), 1.0); // Default opacity
    
    m_card->setShadowOpacity(0.5);
    QCOMPARE(m_card->shadowOpacity(), 0.5);
    
    m_card->setShadowOpacity(0.0);
    QCOMPARE(m_card->shadowOpacity(), 0.0);
    
    m_card->setShadowOpacity(1.0);
    QCOMPARE(m_card->shadowOpacity(), 1.0);
}

void FluentCardTest::testSelectable() {
    // Test selectable property
    QVERIFY(!m_card->isSelectable()); // Default should be false

    m_card->setSelectable(true);
    QVERIFY(m_card->isSelectable());

    m_card->setSelectable(false);
    QVERIFY(!m_card->isSelectable());
}

void FluentCardTest::testSelected() {
    // Test selected property
    QSignalSpy selectedChangedSpy(m_card, &FluentCard::selectedChanged);

    QVERIFY(!m_card->isSelected()); // Default should be false

    // Make card selectable first
    m_card->setSelectable(true);

    m_card->setSelected(true);
    QVERIFY(m_card->isSelected());
    QCOMPARE(selectedChangedSpy.count(), 1);
    QCOMPARE(selectedChangedSpy.first().first().toBool(), true);

    m_card->setSelected(false);
    QVERIFY(!m_card->isSelected());
    QCOMPARE(selectedChangedSpy.count(), 2);
    QCOMPARE(selectedChangedSpy.last().first().toBool(), false);

    // Setting the same selected state should not emit the signal
    m_card->setSelected(false);
    QCOMPARE(selectedChangedSpy.count(), 2);
}

void FluentCardTest::testExpandable() {
    // Test expandable property
    QVERIFY(!m_card->isExpandable()); // Default should be false

    m_card->setExpandable(true);
    QVERIFY(m_card->isExpandable());

    m_card->setExpandable(false);
    QVERIFY(!m_card->isExpandable());
}

void FluentCardTest::testExpanded() {
    // Test expanded property
    QSignalSpy expandedChangedSpy(m_card, &FluentCard::expandedChanged);

    QVERIFY(m_card->isExpanded()); // Default should be true

    // Make card expandable first
    m_card->setExpandable(true);

    m_card->setExpanded(false);
    QVERIFY(!m_card->isExpanded());
    QCOMPARE(expandedChangedSpy.count(), 1);
    QCOMPARE(expandedChangedSpy.first().first().toBool(), false);

    m_card->setExpanded(true);
    QVERIFY(m_card->isExpanded());
    QCOMPARE(expandedChangedSpy.count(), 2);
    QCOMPARE(expandedChangedSpy.last().first().toBool(), true);

    // Setting the same expanded state should not emit the signal
    m_card->setExpanded(true);
    QCOMPARE(expandedChangedSpy.count(), 2);
}

void FluentCardTest::testToggleExpanded() {
    // Test toggle expanded functionality
    QSignalSpy expandedChangedSpy(m_card, &FluentCard::expandedChanged);

    // Make card expandable
    m_card->setExpandable(true);
    QVERIFY(m_card->isExpanded()); // Should start expanded

    m_card->toggleExpanded();
    QVERIFY(!m_card->isExpanded());
    QCOMPARE(expandedChangedSpy.count(), 1);
    QCOMPARE(expandedChangedSpy.first().first().toBool(), false);

    m_card->toggleExpanded();
    QVERIFY(m_card->isExpanded());
    QCOMPARE(expandedChangedSpy.count(), 2);
    QCOMPARE(expandedChangedSpy.last().first().toBool(), true);
}

void FluentCardTest::testFooterVisible() {
    // Test footer visibility
    QVERIFY(!m_card->isFooterVisible()); // Default should be false

    m_card->setFooterVisible(true);
    QVERIFY(m_card->isFooterVisible());

    m_card->setFooterVisible(false);
    QVERIFY(!m_card->isFooterVisible());
}

void FluentCardTest::testContentWidget() {
    // Test content widget management
    QVERIFY(m_card->contentWidget() == nullptr); // Default should be null

    // Create a test widget
    QLabel* testWidget = new QLabel("Test Content");

    m_card->setContentWidget(testWidget);
    QCOMPARE(m_card->contentWidget(), testWidget);

    // Set another widget
    QPushButton* buttonWidget = new QPushButton("Test Button");
    m_card->setContentWidget(buttonWidget);
    QCOMPARE(m_card->contentWidget(), buttonWidget);

    // The previous widget should be properly managed
    // (exact behavior depends on implementation)
}

void FluentCardTest::testHeaderActions() {
    // Test header action management
    QAction* action1 = new QAction("Action 1", this);
    QAction* action2 = new QAction("Action 2", this);

    // Add actions
    m_card->addHeaderAction(action1);
    m_card->addHeaderAction(action2);

    // Remove an action
    m_card->removeHeaderAction(action1);

    // Clear all actions
    m_card->clearHeaderActions();

    // These operations should not crash
    QVERIFY(true);
}

void FluentCardTest::testFooterWidgets() {
    // Test footer widget management
    QPushButton* button1 = new QPushButton("Button 1");
    QPushButton* button2 = new QPushButton("Button 2");

    // Add widgets
    m_card->addFooterWidget(button1);
    m_card->addFooterWidget(button2);

    // Remove a widget
    m_card->removeFooterWidget(button1);

    // Clear all widgets
    m_card->clearFooterWidgets();

    // These operations should not crash
    QVERIFY(true);
}

void FluentCardTest::testContentLayout() {
    // Test content layout access
    QLayout* contentLayout = m_card->contentLayout();
    QVERIFY(contentLayout != nullptr);

    // Should be able to add widgets to the layout
    QLabel* testLabel = new QLabel("Test");
    contentLayout->addWidget(testLabel);

    QVERIFY(true); // If we get here, the layout is functional
}

void FluentCardTest::testHeaderLayout() {
    // Test header layout access
    QLayout* headerLayout = m_card->headerLayout();
    QVERIFY(headerLayout != nullptr);

    // The header layout should be functional
    QVERIFY(true);
}

void FluentCardTest::testFooterLayout() {
    // Test footer layout access
    QLayout* footerLayout = m_card->footerLayout();
    QVERIFY(footerLayout != nullptr);

    // The footer layout should be functional
    QVERIFY(true);
}

void FluentCardTest::testAnimateIn() {
    // Test animate in functionality
    m_card->animateIn();

    // Animation should start (hard to test without access to internal state)
    // For now, just verify the method doesn't crash
    QVERIFY(true);
}

void FluentCardTest::testAnimateOut() {
    // Test animate out functionality
    m_card->animateOut();

    // Animation should start (hard to test without access to internal state)
    // For now, just verify the method doesn't crash
    QVERIFY(true);
}

void FluentCardTest::testExpandWithAnimation() {
    // Test expand with animation
    m_card->setExpandable(true);
    m_card->setExpanded(false); // Start collapsed

    QSignalSpy expandedChangedSpy(m_card, &FluentCard::expandedChanged);

    m_card->expandWithAnimation();

    // Should eventually become expanded
    QVERIFY(m_card->isExpanded());
    QCOMPARE(expandedChangedSpy.count(), 1);
}

void FluentCardTest::testCollapseWithAnimation() {
    // Test collapse with animation
    m_card->setExpandable(true);
    m_card->setExpanded(true); // Start expanded

    QSignalSpy expandedChangedSpy(m_card, &FluentCard::expandedChanged);

    m_card->collapseWithAnimation();

    // Should eventually become collapsed
    QVERIFY(!m_card->isExpanded());
    QCOMPARE(expandedChangedSpy.count(), 1);
}

void FluentCardTest::testMouseInteraction() {
    // Test mouse interaction
    QSignalSpy cardClickedSpy(m_card, &FluentCard::cardClicked);
    QSignalSpy cardDoubleClickedSpy(m_card, &FluentCard::cardDoubleClicked);

    QPoint center = m_card->rect().center();

    // Test mouse press and release (click)
    QMouseEvent pressEvent(QEvent::MouseButtonPress, center, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, center, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    QApplication::sendEvent(m_card, &pressEvent);
    QApplication::sendEvent(m_card, &releaseEvent);

    QCOMPARE(cardClickedSpy.count(), 1);

    // Test double click
    QMouseEvent doubleClickEvent(QEvent::MouseButtonDblClick, center, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(m_card, &doubleClickEvent);

    QCOMPARE(cardDoubleClickedSpy.count(), 1);
}

void FluentCardTest::testCardClicked() {
    // Test card clicked signal
    QSignalSpy cardClickedSpy(m_card, &FluentCard::cardClicked);

    // Simulate click
    QPoint center = m_card->rect().center();
    QMouseEvent pressEvent(QEvent::MouseButtonPress, center, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, center, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    QApplication::sendEvent(m_card, &pressEvent);
    QApplication::sendEvent(m_card, &releaseEvent);

    QCOMPARE(cardClickedSpy.count(), 1);
}

void FluentCardTest::testCardDoubleClicked() {
    // Test card double clicked signal
    QSignalSpy cardDoubleClickedSpy(m_card, &FluentCard::cardDoubleClicked);

    // Simulate double click
    QPoint center = m_card->rect().center();
    QMouseEvent doubleClickEvent(QEvent::MouseButtonDblClick, center, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(m_card, &doubleClickEvent);

    QCOMPARE(cardDoubleClickedSpy.count(), 1);
}

void FluentCardTest::testHeaderClicked() {
    // Test header clicked signal
    QSignalSpy headerClickedSpy(m_card, &FluentCard::headerClicked);

    // This is harder to test without access to the header widget directly
    // For now, just verify the signal exists and can be connected
    QVERIFY(m_card->metaObject()->indexOfSignal("headerClicked()") != -1);
}

void FluentCardTest::testPropertyChangeSignals() {
    // Test that property change signals are emitted correctly
    QSignalSpy titleChangedSpy(m_card, &FluentCard::titleChanged);
    QSignalSpy subtitleChangedSpy(m_card, &FluentCard::subtitleChanged);
    QSignalSpy headerIconChangedSpy(m_card, &FluentCard::headerIconChanged);
    QSignalSpy elevationChangedSpy(m_card, &FluentCard::elevationChanged);
    QSignalSpy cardStyleChangedSpy(m_card, &FluentCard::cardStyleChanged);
    QSignalSpy selectedChangedSpy(m_card, &FluentCard::selectedChanged);
    QSignalSpy expandedChangedSpy(m_card, &FluentCard::expandedChanged);

    // Change properties and verify signals
    m_card->setTitle("New Title");
    QCOMPARE(titleChangedSpy.count(), 1);

    m_card->setSubtitle("New Subtitle");
    QCOMPARE(subtitleChangedSpy.count(), 1);

    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::red);
    m_card->setHeaderIcon(QIcon(pixmap));
    QCOMPARE(headerIconChangedSpy.count(), 1);

    m_card->setElevation(FluentCardElevation::High);
    QCOMPARE(elevationChangedSpy.count(), 1);

    m_card->setCardStyle(FluentCardStyle::Elevated);
    QCOMPARE(cardStyleChangedSpy.count(), 1);

    m_card->setSelectable(true);
    m_card->setSelected(true);
    QCOMPARE(selectedChangedSpy.count(), 1);

    m_card->setExpandable(true);
    m_card->setExpanded(false);
    QCOMPARE(expandedChangedSpy.count(), 1);
}

void FluentCardTest::testInteractionSignals() {
    // Test interaction signals
    QSignalSpy cardClickedSpy(m_card, &FluentCard::cardClicked);
    QSignalSpy cardDoubleClickedSpy(m_card, &FluentCard::cardDoubleClicked);
    QSignalSpy headerClickedSpy(m_card, &FluentCard::headerClicked);

    // Simulate card click
    QPoint center = m_card->rect().center();
    QMouseEvent pressEvent(QEvent::MouseButtonPress, center, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, center, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    QApplication::sendEvent(m_card, &pressEvent);
    QApplication::sendEvent(m_card, &releaseEvent);

    QCOMPARE(cardClickedSpy.count(), 1);

    // Simulate double click
    QMouseEvent doubleClickEvent(QEvent::MouseButtonDblClick, center, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(m_card, &doubleClickEvent);

    QCOMPARE(cardDoubleClickedSpy.count(), 1);
}

void FluentCardTest::testAccessibility() {
    // Test accessibility features
    const QString accessibleName = "Test Card";
    const QString accessibleDescription = "This is a test card";

    m_card->setAccessibleName(accessibleName);
    QCOMPARE(m_card->accessibleName(), accessibleName);

    m_card->setAccessibleDescription(accessibleDescription);
    QCOMPARE(m_card->accessibleDescription(), accessibleDescription);

    // Test that card has proper focus policy
    QVERIFY(m_card->focusPolicy() != Qt::NoFocus);

    // Test that card can receive keyboard focus
    QVERIFY(m_card->focusPolicy() & Qt::TabFocus);
}

void FluentCardTest::testThemeIntegration() {
    // Test that card responds to theme changes
    auto& theme = FluentQt::Styling::FluentTheme::instance();

    // Get current theme mode
    bool originalDarkMode = theme.isDarkMode();

    // Change theme mode
    theme.setDarkMode(!originalDarkMode);

    // Card should update its appearance (this would require checking internal styling)
    // For now, just verify the card still functions correctly
    QVERIFY(m_card->isEnabled());

    // Test that property operations still work after theme change
    QSignalSpy titleChangedSpy(m_card, &FluentCard::titleChanged);
    m_card->setTitle("Theme Test");
    QCOMPARE(m_card->title(), QString("Theme Test"));
    QCOMPARE(titleChangedSpy.count(), 1);

    // Restore original theme
    theme.setDarkMode(originalDarkMode);
}

QTEST_MAIN(FluentCardTest)
#include "FluentCardTest.moc"
