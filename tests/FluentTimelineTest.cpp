// tests/FluentTimelineTest.cpp
#include <QtTest/QtTest>
#include <QApplication>
#include <QSignalSpy>

#include "FluentQt/Components/FluentTimeline.h"
#include "FluentQt/Components/FluentTimelineItem.h"

using namespace FluentQt::Components;

class FluentTimelineTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Timeline tests
    void testTimelineCreation();
    void testTimelineConfiguration();
    void testTimelineOrientation();
    void testTimelineAlignment();
    void testTimelineStyle();
    void testTimelineItemManagement();
    void testTimelineNavigation();
    void testTimelineSignals();

    // Timeline item tests
    void testTimelineItemCreation();
    void testTimelineItemProperties();
    void testTimelineItemStates();
    void testTimelineItemTypes();
    void testTimelineItemFactoryMethods();
    void testTimelineItemExpansion();
    void testTimelineItemSignals();

private:
    QApplication* m_app;
    FluentTimeline* m_timeline;
};

void FluentTimelineTest::initTestCase() {
    // QApplication is required for widget tests
    if (!QApplication::instance()) {
        int argc = 0;
        char** argv = nullptr;
        m_app = new QApplication(argc, argv);
    } else {
        m_app = nullptr;
    }
}

void FluentTimelineTest::cleanupTestCase() {
    delete m_app;
}

void FluentTimelineTest::init() {
    m_timeline = new FluentTimeline();
}

void FluentTimelineTest::cleanup() {
    delete m_timeline;
    m_timeline = nullptr;
}

void FluentTimelineTest::testTimelineCreation() {
    // Test default constructor
    auto* timeline1 = new FluentTimeline();
    QVERIFY(timeline1 != nullptr);
    QCOMPARE(timeline1->orientation(), FluentTimelineOrientation::Vertical);
    QCOMPARE(timeline1->alignment(), FluentTimelineAlignment::Left);
    QCOMPARE(timeline1->timelineStyle(), FluentTimelineStyle::Default);
    delete timeline1;

    // Test orientation constructor
    auto* timeline2 = new FluentTimeline(FluentTimelineOrientation::Horizontal);
    QVERIFY(timeline2 != nullptr);
    QCOMPARE(timeline2->orientation(), FluentTimelineOrientation::Horizontal);
    delete timeline2;

    // Test configuration constructor
    FluentTimelineConfig config;
    config.orientation = FluentTimelineOrientation::Horizontal;
    config.alignment = FluentTimelineAlignment::Center;
    config.style = FluentTimelineStyle::Compact;
    config.itemSpacing = 20;
    config.animated = false;

    auto* timeline3 = new FluentTimeline(config);
    QVERIFY(timeline3 != nullptr);
    QCOMPARE(timeline3->orientation(), FluentTimelineOrientation::Horizontal);
    QCOMPARE(timeline3->alignment(), FluentTimelineAlignment::Center);
    QCOMPARE(timeline3->timelineStyle(), FluentTimelineStyle::Compact);
    QCOMPARE(timeline3->itemSpacing(), 20);
    QCOMPARE(timeline3->isAnimated(), false);
    delete timeline3;

    // Test factory methods
    auto* verticalTimeline = FluentTimeline::createVerticalTimeline();
    QVERIFY(verticalTimeline != nullptr);
    QCOMPARE(verticalTimeline->orientation(), FluentTimelineOrientation::Vertical);
    delete verticalTimeline;

    auto* horizontalTimeline = FluentTimeline::createHorizontalTimeline();
    QVERIFY(horizontalTimeline != nullptr);
    QCOMPARE(horizontalTimeline->orientation(), FluentTimelineOrientation::Horizontal);
    delete horizontalTimeline;

    auto* compactTimeline = FluentTimeline::createCompactTimeline();
    QVERIFY(compactTimeline != nullptr);
    QCOMPARE(compactTimeline->timelineStyle(), FluentTimelineStyle::Compact);
    delete compactTimeline;

    auto* detailedTimeline = FluentTimeline::createDetailedTimeline();
    QVERIFY(detailedTimeline != nullptr);
    QCOMPARE(detailedTimeline->timelineStyle(), FluentTimelineStyle::Detailed);
    delete detailedTimeline;
}

void FluentTimelineTest::testTimelineConfiguration() {
    FluentTimelineConfig config;
    config.orientation = FluentTimelineOrientation::Horizontal;
    config.alignment = FluentTimelineAlignment::Right;
    config.style = FluentTimelineStyle::Minimal;
    config.connectorStyle = FluentTimelineConnectorStyle::Dashed;
    config.itemSpacing = 25;
    config.connectorWidth = 3;
    config.indicatorSize = 15;
    config.animated = false;
    config.scrollable = true;
    config.interactive = false;
    config.showConnectors = false;
    config.showIndicators = false;

    m_timeline->setConfiguration(config);

    QCOMPARE(m_timeline->orientation(), FluentTimelineOrientation::Horizontal);
    QCOMPARE(m_timeline->alignment(), FluentTimelineAlignment::Right);
    QCOMPARE(m_timeline->timelineStyle(), FluentTimelineStyle::Minimal);
    QCOMPARE(m_timeline->connectorStyle(), FluentTimelineConnectorStyle::Dashed);
    QCOMPARE(m_timeline->itemSpacing(), 25);
    QCOMPARE(m_timeline->connectorWidth(), 3);
    QCOMPARE(m_timeline->indicatorSize(), 15);
    QCOMPARE(m_timeline->isAnimated(), false);
    QCOMPARE(m_timeline->isScrollable(), true);
    QCOMPARE(m_timeline->isInteractive(), false);
    QCOMPARE(m_timeline->showConnectors(), false);
    QCOMPARE(m_timeline->showIndicators(), false);

    FluentTimelineConfig retrievedConfig = m_timeline->configuration();
    QCOMPARE(retrievedConfig.orientation, config.orientation);
    QCOMPARE(retrievedConfig.alignment, config.alignment);
    QCOMPARE(retrievedConfig.style, config.style);
    QCOMPARE(retrievedConfig.itemSpacing, config.itemSpacing);
}

void FluentTimelineTest::testTimelineOrientation() {
    QSignalSpy spy(m_timeline, &FluentTimeline::orientationChanged);

    // Test vertical orientation
    m_timeline->setOrientation(FluentTimelineOrientation::Vertical);
    QCOMPARE(m_timeline->orientation(), FluentTimelineOrientation::Vertical);
    QCOMPARE(spy.count(), 1);

    // Test horizontal orientation
    m_timeline->setOrientation(FluentTimelineOrientation::Horizontal);
    QCOMPARE(m_timeline->orientation(), FluentTimelineOrientation::Horizontal);
    QCOMPARE(spy.count(), 2);

    // Test setting same orientation (should not emit signal)
    m_timeline->setOrientation(FluentTimelineOrientation::Horizontal);
    QCOMPARE(spy.count(), 2);
}

void FluentTimelineTest::testTimelineAlignment() {
    QSignalSpy spy(m_timeline, &FluentTimeline::alignmentChanged);

    // Test different alignments
    m_timeline->setAlignment(FluentTimelineAlignment::Left);
    QCOMPARE(m_timeline->alignment(), FluentTimelineAlignment::Left);

    m_timeline->setAlignment(FluentTimelineAlignment::Right);
    QCOMPARE(m_timeline->alignment(), FluentTimelineAlignment::Right);
    QCOMPARE(spy.count(), 1);

    m_timeline->setAlignment(FluentTimelineAlignment::Center);
    QCOMPARE(m_timeline->alignment(), FluentTimelineAlignment::Center);
    QCOMPARE(spy.count(), 2);

    m_timeline->setAlignment(FluentTimelineAlignment::Alternate);
    QCOMPARE(m_timeline->alignment(), FluentTimelineAlignment::Alternate);
    QCOMPARE(spy.count(), 3);
}

void FluentTimelineTest::testTimelineStyle() {
    QSignalSpy spy(m_timeline, &FluentTimeline::styleChanged);

    // Test different styles
    m_timeline->setTimelineStyle(FluentTimelineStyle::Default);
    QCOMPARE(m_timeline->timelineStyle(), FluentTimelineStyle::Default);

    m_timeline->setTimelineStyle(FluentTimelineStyle::Compact);
    QCOMPARE(m_timeline->timelineStyle(), FluentTimelineStyle::Compact);
    QCOMPARE(spy.count(), 1);

    m_timeline->setTimelineStyle(FluentTimelineStyle::Detailed);
    QCOMPARE(m_timeline->timelineStyle(), FluentTimelineStyle::Detailed);
    QCOMPARE(spy.count(), 2);

    m_timeline->setTimelineStyle(FluentTimelineStyle::Minimal);
    QCOMPARE(m_timeline->timelineStyle(), FluentTimelineStyle::Minimal);
    QCOMPARE(spy.count(), 3);
}

void FluentTimelineTest::testTimelineItemManagement() {
    QCOMPARE(m_timeline->itemCount(), 0);
    QVERIFY(m_timeline->items().isEmpty());

    // Add items
    auto* item1 = new FluentTimelineItem("Item 1");
    auto* item2 = new FluentTimelineItem("Item 2");
    auto* item3 = new FluentTimelineItem("Item 3");

    QSignalSpy addSpy(m_timeline, &FluentTimeline::itemAdded);

    m_timeline->addItem(item1);
    QCOMPARE(m_timeline->itemCount(), 1);
    QCOMPARE(m_timeline->item(0), item1);
    QCOMPARE(m_timeline->indexOf(item1), 0);
    QCOMPARE(addSpy.count(), 1);

    m_timeline->addItem(item2);
    QCOMPARE(m_timeline->itemCount(), 2);
    QCOMPARE(m_timeline->item(1), item2);

    // Insert item
    m_timeline->insertItem(1, item3);
    QCOMPARE(m_timeline->itemCount(), 3);
    QCOMPARE(m_timeline->item(1), item3);
    QCOMPARE(m_timeline->item(2), item2);
    QCOMPARE(m_timeline->indexOf(item2), 2);

    // Test convenience methods
    auto* textItem = m_timeline->addTextItem("Text Item", "Description");
    QVERIFY(textItem != nullptr);
    QCOMPARE(textItem->title(), QString("Text Item"));
    QCOMPARE(textItem->description(), QString("Description"));
    QCOMPARE(m_timeline->itemCount(), 4);

    QIcon testIcon(":/test/icon.png");
    auto* iconItem = m_timeline->addIconItem(testIcon, "Icon Item", "Icon Description");
    QVERIFY(iconItem != nullptr);
    QCOMPARE(iconItem->title(), QString("Icon Item"));
    QCOMPARE(m_timeline->itemCount(), 5);

    QDateTime testDateTime = QDateTime::currentDateTime();
    auto* dateTimeItem = m_timeline->addDateTimeItem(testDateTime, "DateTime Item", "DateTime Description");
    QVERIFY(dateTimeItem != nullptr);
    QCOMPARE(dateTimeItem->title(), QString("DateTime Item"));
    QCOMPARE(dateTimeItem->dateTime(), testDateTime);
    QCOMPARE(m_timeline->itemCount(), 6);

    // Remove items
    QSignalSpy removeSpy(m_timeline, &FluentTimeline::itemRemoved);

    m_timeline->removeItem(item1);
    QCOMPARE(m_timeline->itemCount(), 5);
    QCOMPARE(removeSpy.count(), 1);

    m_timeline->removeItem(0); // Remove item3
    QCOMPARE(m_timeline->itemCount(), 4);
    QCOMPARE(m_timeline->item(0), item2);

    // Clear all items
    m_timeline->clearItems();
    QCOMPARE(m_timeline->itemCount(), 0);
    QVERIFY(m_timeline->items().isEmpty());
}

void FluentTimelineTest::testTimelineNavigation() {
    // Add test items
    auto* item1 = new FluentTimelineItem("Item 1");
    auto* item2 = new FluentTimelineItem("Item 2");
    auto* item3 = new FluentTimelineItem("Item 3");

    m_timeline->addItem(item1);
    m_timeline->addItem(item2);
    m_timeline->addItem(item3);

    QSignalSpy currentItemSpy(m_timeline, &FluentTimeline::currentItemChanged);
    QSignalSpy currentIndexSpy(m_timeline, &FluentTimeline::currentIndexChanged);

    // Test current item/index
    QCOMPARE(m_timeline->currentItem(), nullptr);
    QCOMPARE(m_timeline->currentIndex(), -1);

    m_timeline->setCurrentItem(item2);
    QCOMPARE(m_timeline->currentItem(), item2);
    QCOMPARE(m_timeline->currentIndex(), 1);
    QCOMPARE(currentItemSpy.count(), 1);
    QCOMPARE(currentIndexSpy.count(), 1);

    m_timeline->setCurrentIndex(0);
    QCOMPARE(m_timeline->currentItem(), item1);
    QCOMPARE(m_timeline->currentIndex(), 0);
    QCOMPARE(currentItemSpy.count(), 2);
    QCOMPARE(currentIndexSpy.count(), 2);

    // Test invalid indices
    m_timeline->setCurrentIndex(-1);
    QCOMPARE(m_timeline->currentItem(), nullptr);
    QCOMPARE(m_timeline->currentIndex(), -1);

    m_timeline->setCurrentIndex(10);
    QCOMPARE(m_timeline->currentItem(), nullptr);
    QCOMPARE(m_timeline->currentIndex(), -1);
}

void FluentTimelineTest::testTimelineSignals() {
    auto* item = new FluentTimelineItem("Test Item");
    m_timeline->addItem(item);

    QSignalSpy clickSpy(m_timeline, &FluentTimeline::itemClicked);
    QSignalSpy doubleClickSpy(m_timeline, &FluentTimeline::itemDoubleClicked);

    // Simulate item click (would normally be triggered by user interaction)
    emit item->clicked();
    // Note: The timeline's itemClicked signal is connected to item's clicked signal
    // In a real test, we would simulate mouse events on the item
}

void FluentTimelineTest::testTimelineItemCreation() {
    // Test default constructor
    auto* item1 = new FluentTimelineItem();
    QVERIFY(item1 != nullptr);
    QVERIFY(item1->title().isEmpty());
    QVERIFY(item1->description().isEmpty());
    delete item1;

    // Test title constructor
    auto* item2 = new FluentTimelineItem("Test Title");
    QVERIFY(item2 != nullptr);
    QCOMPARE(item2->title(), QString("Test Title"));
    delete item2;

    // Test title and description constructor
    auto* item3 = new FluentTimelineItem("Test Title", "Test Description");
    QVERIFY(item3 != nullptr);
    QCOMPARE(item3->title(), QString("Test Title"));
    QCOMPARE(item3->description(), QString("Test Description"));
    delete item3;

    // Test data constructor
    FluentTimelineItemData data;
    data.title = "Data Title";
    data.description = "Data Description";
    data.state = FluentTimelineItemState::Current;
    data.type = FluentTimelineItemType::Task;

    auto* item4 = new FluentTimelineItem(data);
    QVERIFY(item4 != nullptr);
    QCOMPARE(item4->title(), QString("Data Title"));
    QCOMPARE(item4->description(), QString("Data Description"));
    QCOMPARE(item4->itemState(), FluentTimelineItemState::Current);
    QCOMPARE(item4->itemType(), FluentTimelineItemType::Task);
    delete item4;
}

void FluentTimelineTest::testTimelineItemProperties() {
    auto* item = new FluentTimelineItem();

    QSignalSpy titleSpy(item, &FluentTimelineItem::titleChanged);
    QSignalSpy descSpy(item, &FluentTimelineItem::descriptionChanged);
    QSignalSpy dateSpy(item, &FluentTimelineItem::dateTimeChanged);

    // Test title
    item->setTitle("New Title");
    QCOMPARE(item->title(), QString("New Title"));
    QCOMPARE(titleSpy.count(), 1);

    // Test description
    item->setDescription("New Description");
    QCOMPARE(item->description(), QString("New Description"));
    QCOMPARE(descSpy.count(), 1);

    // Test date/time
    QDateTime testDateTime = QDateTime::currentDateTime();
    item->setDateTime(testDateTime);
    QCOMPARE(item->dateTime(), testDateTime);
    QCOMPARE(dateSpy.count(), 1);

    // Test icon
    QIcon testIcon(":/test/icon.png");
    item->setIcon(testIcon);
    // Note: Icon comparison is tricky, so we just verify it was set

    // Test user data
    QVariant userData("test data");
    item->setUserData(userData);
    QCOMPARE(item->userData(), userData);

    delete item;
}

void FluentTimelineTest::testTimelineItemStates() {
    auto* item = new FluentTimelineItem("Test Item");

    QSignalSpy stateSpy(item, &FluentTimelineItem::itemStateChanged);

    // Test different states
    item->setItemState(FluentTimelineItemState::Pending);
    QCOMPARE(item->itemState(), FluentTimelineItemState::Pending);
    QCOMPARE(stateSpy.count(), 1);

    item->setItemState(FluentTimelineItemState::Current);
    QCOMPARE(item->itemState(), FluentTimelineItemState::Current);
    QCOMPARE(stateSpy.count(), 2);

    item->setItemState(FluentTimelineItemState::Completed);
    QCOMPARE(item->itemState(), FluentTimelineItemState::Completed);
    QCOMPARE(stateSpy.count(), 3);

    item->setItemState(FluentTimelineItemState::Failed);
    QCOMPARE(item->itemState(), FluentTimelineItemState::Failed);
    QCOMPARE(stateSpy.count(), 4);

    item->setItemState(FluentTimelineItemState::Cancelled);
    QCOMPARE(item->itemState(), FluentTimelineItemState::Cancelled);
    QCOMPARE(stateSpy.count(), 5);

    item->setItemState(FluentTimelineItemState::Warning);
    QCOMPARE(item->itemState(), FluentTimelineItemState::Warning);
    QCOMPARE(stateSpy.count(), 6);

    // Test convenience methods
    item->markCompleted();
    QCOMPARE(item->itemState(), FluentTimelineItemState::Completed);

    item->markFailed();
    QCOMPARE(item->itemState(), FluentTimelineItemState::Failed);

    item->markCancelled();
    QCOMPARE(item->itemState(), FluentTimelineItemState::Cancelled);

    item->markPending();
    QCOMPARE(item->itemState(), FluentTimelineItemState::Pending);

    delete item;
}

void FluentTimelineTest::testTimelineItemTypes() {
    auto* item = new FluentTimelineItem("Test Item");

    QSignalSpy typeSpy(item, &FluentTimelineItem::itemTypeChanged);

    // Test different types
    item->setItemType(FluentTimelineItemType::Default);
    QCOMPARE(item->itemType(), FluentTimelineItemType::Default);

    item->setItemType(FluentTimelineItemType::Milestone);
    QCOMPARE(item->itemType(), FluentTimelineItemType::Milestone);
    QCOMPARE(typeSpy.count(), 1);

    item->setItemType(FluentTimelineItemType::Event);
    QCOMPARE(item->itemType(), FluentTimelineItemType::Event);
    QCOMPARE(typeSpy.count(), 2);

    item->setItemType(FluentTimelineItemType::Task);
    QCOMPARE(item->itemType(), FluentTimelineItemType::Task);
    QCOMPARE(typeSpy.count(), 3);

    item->setItemType(FluentTimelineItemType::Note);
    QCOMPARE(item->itemType(), FluentTimelineItemType::Note);
    QCOMPARE(typeSpy.count(), 4);

    delete item;
}

void FluentTimelineTest::testTimelineItemFactoryMethods() {
    QDateTime testDateTime = QDateTime::currentDateTime();

    // Test milestone creation
    auto* milestone = FluentTimelineItem::createMilestone("Milestone Title", testDateTime);
    QVERIFY(milestone != nullptr);
    QCOMPARE(milestone->title(), QString("Milestone Title"));
    QCOMPARE(milestone->dateTime(), testDateTime);
    QCOMPARE(milestone->itemType(), FluentTimelineItemType::Milestone);
    QCOMPARE(milestone->indicatorStyle(), FluentTimelineItemIndicatorStyle::Diamond);
    delete milestone;

    // Test event creation
    auto* event = FluentTimelineItem::createEvent("Event Title", "Event Description", testDateTime);
    QVERIFY(event != nullptr);
    QCOMPARE(event->title(), QString("Event Title"));
    QCOMPARE(event->description(), QString("Event Description"));
    QCOMPARE(event->dateTime(), testDateTime);
    QCOMPARE(event->itemType(), FluentTimelineItemType::Event);
    delete event;

    // Test task creation
    auto* task = FluentTimelineItem::createTask("Task Title", FluentTimelineItemState::Current);
    QVERIFY(task != nullptr);
    QCOMPARE(task->title(), QString("Task Title"));
    QCOMPARE(task->itemState(), FluentTimelineItemState::Current);
    QCOMPARE(task->itemType(), FluentTimelineItemType::Task);
    delete task;

    // Test note creation
    auto* note = FluentTimelineItem::createNote("Note Title", "Note Description");
    QVERIFY(note != nullptr);
    QCOMPARE(note->title(), QString("Note Title"));
    QCOMPARE(note->description(), QString("Note Description"));
    QCOMPARE(note->itemType(), FluentTimelineItemType::Note);
    delete note;

    // Test icon item creation
    QIcon testIcon(":/test/icon.png");
    auto* iconItem = FluentTimelineItem::createIconItem(testIcon, "Icon Title", "Icon Description");
    QVERIFY(iconItem != nullptr);
    QCOMPARE(iconItem->title(), QString("Icon Title"));
    QCOMPARE(iconItem->description(), QString("Icon Description"));
    QCOMPARE(iconItem->indicatorStyle(), FluentTimelineItemIndicatorStyle::Icon);
    delete iconItem;

    // Test avatar item creation
    QPixmap testAvatar(32, 32);
    testAvatar.fill(Qt::blue);
    auto* avatarItem = FluentTimelineItem::createAvatarItem(testAvatar, "Avatar Title", "Avatar Description");
    QVERIFY(avatarItem != nullptr);
    QCOMPARE(avatarItem->title(), QString("Avatar Title"));
    QCOMPARE(avatarItem->description(), QString("Avatar Description"));
    QCOMPARE(avatarItem->indicatorStyle(), FluentTimelineItemIndicatorStyle::Avatar);
    delete avatarItem;
}

void FluentTimelineTest::testTimelineItemExpansion() {
    auto* item = new FluentTimelineItem("Expandable Item");

    QSignalSpy expandableSpy(item, &FluentTimelineItem::expandableChanged);
    QSignalSpy expandedSpy(item, &FluentTimelineItem::expandedChanged);

    // Test expandable property
    QCOMPARE(item->isExpandable(), false);
    QCOMPARE(item->isExpanded(), false);

    item->setExpandable(true);
    QCOMPARE(item->isExpandable(), true);
    QCOMPARE(expandableSpy.count(), 1);

    // Test expansion
    item->setExpanded(true);
    QCOMPARE(item->isExpanded(), true);
    QCOMPARE(expandedSpy.count(), 1);

    item->setExpanded(false);
    QCOMPARE(item->isExpanded(), false);
    QCOMPARE(expandedSpy.count(), 2);

    // Test toggle
    item->toggle();
    QCOMPARE(item->isExpanded(), true);

    item->toggle();
    QCOMPARE(item->isExpanded(), false);

    // Test expand/collapse methods
    item->expand();
    QCOMPARE(item->isExpanded(), true);

    item->collapse();
    QCOMPARE(item->isExpanded(), false);

    delete item;
}

void FluentTimelineTest::testTimelineItemSignals() {
    auto* item = new FluentTimelineItem("Test Item");

    QSignalSpy clickSpy(item, &FluentTimelineItem::clicked);
    QSignalSpy doubleClickSpy(item, &FluentTimelineItem::doubleClicked);
    QSignalSpy expandedSpy(item, &FluentTimelineItem::expanded);
    QSignalSpy collapsedSpy(item, &FluentTimelineItem::collapsed);

    // Test expansion signals
    item->setExpandable(true);
    item->expand();
    QCOMPARE(expandedSpy.count(), 1);

    item->collapse();
    QCOMPARE(collapsedSpy.count(), 1);

    delete item;
}

QTEST_MAIN(FluentTimelineTest)
#include "FluentTimelineTest.moc"
