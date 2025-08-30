// tests/Components/FluentSearchBoxTest.cpp
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentSearchBox.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentSearchBoxTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();

    // Text tests
    void testSearchText();
    void testPlaceholderText();
    void testClearText();

    // Search functionality tests
    void testSearch();
    void testSearchDelay();
    void testAutoSearch();

    // Suggestions tests
    void testSuggestions();
    void testSuggestionSelection();
    void testSuggestionFiltering();

    // Appearance tests
    void testSearchIcon();
    void testClearButton();
    void testSearchButton();

    // History tests
    void testSearchHistory();
    void testHistoryLimit();

    // Signal tests
    void testSignalEmission();

    // Interaction tests
    void testKeyboardInteraction();
    void testMouseInteraction();

private:
    FluentSearchBox* m_searchBox{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentSearchBoxTest::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(400, 300);
}

void FluentSearchBoxTest::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentSearchBoxTest::init() {
    m_searchBox = new FluentSearchBox(m_testWidget);
}

void FluentSearchBoxTest::cleanup() {
    delete m_searchBox;
    m_searchBox = nullptr;
}

void FluentSearchBoxTest::testDefaultConstructor() {
    QVERIFY(m_searchBox != nullptr);
    QCOMPARE(m_searchBox->searchText(), QString());
    QCOMPARE(m_searchBox->placeholderText(), "Search");
    QVERIFY(m_searchBox->autoSearch());
    QCOMPARE(m_searchBox->searchDelay(), 300);
    QVERIFY(m_searchBox->isClearButtonVisible());
}

void FluentSearchBoxTest::testSearchText() {
    QSignalSpy searchTextSpy(m_searchBox, &FluentSearchBox::searchTextChanged);
    
    m_searchBox->setSearchText("test query");
    QCOMPARE(m_searchBox->searchText(), "test query");
    QCOMPARE(searchTextSpy.count(), 1);
    QCOMPARE(searchTextSpy.first().first().toString(), "test query");
    
    // Test empty text
    m_searchBox->setSearchText("");
    QCOMPARE(m_searchBox->searchText(), "");
    QCOMPARE(searchTextSpy.count(), 2);
}

void FluentSearchBoxTest::testPlaceholderText() {
    QSignalSpy placeholderSpy(m_searchBox, &FluentSearchBox::placeholderTextChanged);
    
    m_searchBox->setPlaceholderText("Enter search term...");
    QCOMPARE(m_searchBox->placeholderText(), "Enter search term...");
    QCOMPARE(placeholderSpy.count(), 1);
    
    // Test empty placeholder
    m_searchBox->setPlaceholderText("");
    QCOMPARE(m_searchBox->placeholderText(), "");
    QCOMPARE(placeholderSpy.count(), 2);
}

void FluentSearchBoxTest::testClearText() {
    m_searchBox->setSearchText("some text");
    
    QSignalSpy searchTextSpy(m_searchBox, &FluentSearchBox::searchTextChanged);
    QSignalSpy clearedSpy(m_searchBox, &FluentSearchBox::searchCleared);
    
    m_searchBox->clear();
    QCOMPARE(m_searchBox->searchText(), "");
    QCOMPARE(searchTextSpy.count(), 1);
    QCOMPARE(clearedSpy.count(), 1);
}

void FluentSearchBoxTest::testSearch() {
    QSignalSpy searchSpy(m_searchBox, &FluentSearchBox::searchRequested);
    QSignalSpy searchPerformedSpy(m_searchBox, &FluentSearchBox::searchPerformed);
    
    m_searchBox->setSearchText("test query");
    m_searchBox->performSearch();
    
    QCOMPARE(searchSpy.count(), 1);
    QCOMPARE(searchSpy.first().first().toString(), "test query");
    QCOMPARE(searchPerformedSpy.count(), 1);
    
    // Test empty search
    m_searchBox->setSearchText("");
    m_searchBox->performSearch();
    QCOMPARE(searchSpy.count(), 1); // Should not emit for empty search
}

void FluentSearchBoxTest::testSearchDelay() {
    QSignalSpy searchDelaySpy(m_searchBox, &FluentSearchBox::searchDelayChanged);
    
    m_searchBox->setSearchDelay(500);
    QCOMPARE(m_searchBox->searchDelay(), 500);
    QCOMPARE(searchDelaySpy.count(), 1);
    
    // Test invalid delay
    m_searchBox->setSearchDelay(-100);
    QVERIFY(m_searchBox->searchDelay() >= 0);
}

void FluentSearchBoxTest::testAutoSearch() {
    QSignalSpy autoSearchSpy(m_searchBox, &FluentSearchBox::autoSearchChanged);
    
    m_searchBox->setAutoSearch(false);
    QVERIFY(!m_searchBox->autoSearch());
    QCOMPARE(autoSearchSpy.count(), 1);
    
    m_searchBox->setAutoSearch(true);
    QVERIFY(m_searchBox->autoSearch());
    QCOMPARE(autoSearchSpy.count(), 2);
}

void FluentSearchBoxTest::testSuggestions() {
    QSignalSpy suggestionsSpy(m_searchBox, &FluentSearchBox::suggestionsChanged);
    
    QStringList suggestions = {"apple", "application", "apply", "approach"};
    m_searchBox->setSuggestions(suggestions);
    QCOMPARE(m_searchBox->suggestions(), suggestions);
    QCOMPARE(suggestionsSpy.count(), 1);
    
    // Test adding individual suggestion
    m_searchBox->addSuggestion("approve");
    QVERIFY(m_searchBox->suggestions().contains("approve"));
    QCOMPARE(suggestionsSpy.count(), 2);
    
    // Test removing suggestion
    m_searchBox->removeSuggestion("apple");
    QVERIFY(!m_searchBox->suggestions().contains("apple"));
    QCOMPARE(suggestionsSpy.count(), 3);
    
    // Test clearing suggestions
    m_searchBox->clearSuggestions();
    QVERIFY(m_searchBox->suggestions().isEmpty());
    QCOMPARE(suggestionsSpy.count(), 4);
}

void FluentSearchBoxTest::testSuggestionSelection() {
    QStringList suggestions = {"apple", "application", "apply"};
    m_searchBox->setSuggestions(suggestions);
    
    QSignalSpy suggestionSelectedSpy(m_searchBox, &FluentSearchBox::suggestionSelected);
    
    m_searchBox->selectSuggestion("application");
    QCOMPARE(m_searchBox->searchText(), "application");
    QCOMPARE(suggestionSelectedSpy.count(), 1);
    QCOMPARE(suggestionSelectedSpy.first().first().toString(), "application");
}

void FluentSearchBoxTest::testSuggestionFiltering() {
    QStringList suggestions = {"apple", "application", "apply", "banana", "cherry"};
    m_searchBox->setSuggestions(suggestions);
    
    // Test filtering suggestions based on search text
    m_searchBox->setSearchText("app");
    QStringList filteredSuggestions = m_searchBox->filteredSuggestions();
    
    QVERIFY(filteredSuggestions.contains("apple"));
    QVERIFY(filteredSuggestions.contains("application"));
    QVERIFY(filteredSuggestions.contains("apply"));
    QVERIFY(!filteredSuggestions.contains("banana"));
    QVERIFY(!filteredSuggestions.contains("cherry"));
    
    // Test case insensitive filtering
    m_searchBox->setSearchText("APP");
    filteredSuggestions = m_searchBox->filteredSuggestions();
    QVERIFY(filteredSuggestions.contains("apple"));
}

void FluentSearchBoxTest::testSearchIcon() {
    QSignalSpy searchIconSpy(m_searchBox, &FluentSearchBox::searchIconChanged);
    
    QIcon customIcon(":/icons/search.png");
    m_searchBox->setSearchIcon(customIcon);
    QCOMPARE(m_searchBox->searchIcon().name(), customIcon.name());
    QCOMPARE(searchIconSpy.count(), 1);
    
    // Test null icon
    m_searchBox->setSearchIcon(QIcon());
    QVERIFY(m_searchBox->searchIcon().isNull());
    QCOMPARE(searchIconSpy.count(), 2);
}

void FluentSearchBoxTest::testClearButton() {
    QSignalSpy clearButtonSpy(m_searchBox, &FluentSearchBox::clearButtonVisibleChanged);
    
    m_searchBox->setClearButtonVisible(false);
    QVERIFY(!m_searchBox->isClearButtonVisible());
    QCOMPARE(clearButtonSpy.count(), 1);
    
    m_searchBox->setClearButtonVisible(true);
    QVERIFY(m_searchBox->isClearButtonVisible());
    QCOMPARE(clearButtonSpy.count(), 2);
}

void FluentSearchBoxTest::testSearchButton() {
    QSignalSpy searchButtonSpy(m_searchBox, &FluentSearchBox::searchButtonVisibleChanged);
    
    m_searchBox->setSearchButtonVisible(true);
    QVERIFY(m_searchBox->isSearchButtonVisible());
    QCOMPARE(searchButtonSpy.count(), 1);
    
    m_searchBox->setSearchButtonVisible(false);
    QVERIFY(!m_searchBox->isSearchButtonVisible());
    QCOMPARE(searchButtonSpy.count(), 2);
}

void FluentSearchBoxTest::testSearchHistory() {
    QSignalSpy historySpy(m_searchBox, &FluentSearchBox::searchHistoryChanged);
    
    // Perform searches to build history
    m_searchBox->setSearchText("first search");
    m_searchBox->performSearch();
    
    m_searchBox->setSearchText("second search");
    m_searchBox->performSearch();
    
    m_searchBox->setSearchText("third search");
    m_searchBox->performSearch();
    
    QStringList history = m_searchBox->searchHistory();
    QVERIFY(history.contains("first search"));
    QVERIFY(history.contains("second search"));
    QVERIFY(history.contains("third search"));
    QVERIFY(historySpy.count() >= 3);
    
    // Test clearing history
    m_searchBox->clearSearchHistory();
    QVERIFY(m_searchBox->searchHistory().isEmpty());
}

void FluentSearchBoxTest::testHistoryLimit() {
    QSignalSpy historyLimitSpy(m_searchBox, &FluentSearchBox::historyLimitChanged);
    
    m_searchBox->setHistoryLimit(3);
    QCOMPARE(m_searchBox->historyLimit(), 3);
    QCOMPARE(historyLimitSpy.count(), 1);
    
    // Add more searches than the limit
    for (int i = 1; i <= 5; ++i) {
        m_searchBox->setSearchText(QString("search %1").arg(i));
        m_searchBox->performSearch();
    }
    
    // History should be limited to 3 items
    QVERIFY(m_searchBox->searchHistory().size() <= 3);
    
    // Test invalid limit
    m_searchBox->setHistoryLimit(-1);
    QVERIFY(m_searchBox->historyLimit() >= 0);
}

void FluentSearchBoxTest::testSignalEmission() {
    QSignalSpy searchTextSpy(m_searchBox, &FluentSearchBox::searchTextChanged);
    QSignalSpy placeholderSpy(m_searchBox, &FluentSearchBox::placeholderTextChanged);
    QSignalSpy searchSpy(m_searchBox, &FluentSearchBox::searchRequested);
    QSignalSpy searchPerformedSpy(m_searchBox, &FluentSearchBox::searchPerformed);
    QSignalSpy clearedSpy(m_searchBox, &FluentSearchBox::searchCleared);
    QSignalSpy suggestionsSpy(m_searchBox, &FluentSearchBox::suggestionsChanged);
    QSignalSpy suggestionSelectedSpy(m_searchBox, &FluentSearchBox::suggestionSelected);
    QSignalSpy autoSearchSpy(m_searchBox, &FluentSearchBox::autoSearchChanged);
    QSignalSpy searchDelaySpy(m_searchBox, &FluentSearchBox::searchDelayChanged);
    QSignalSpy historySpy(m_searchBox, &FluentSearchBox::searchHistoryChanged);
    
    // Test all signals
    m_searchBox->setSearchText("test");
    m_searchBox->setPlaceholderText("Search here...");
    m_searchBox->performSearch();
    m_searchBox->clear();
    m_searchBox->setSuggestions({"suggestion1", "suggestion2"});
    m_searchBox->selectSuggestion("suggestion1");
    m_searchBox->setAutoSearch(false);
    m_searchBox->setSearchDelay(500);
    
    QCOMPARE(searchTextSpy.count(), 2); // setText, clear
    QCOMPARE(placeholderSpy.count(), 1);
    QCOMPARE(searchSpy.count(), 1);
    QCOMPARE(searchPerformedSpy.count(), 1);
    QCOMPARE(clearedSpy.count(), 1);
    QCOMPARE(suggestionsSpy.count(), 1);
    QCOMPARE(suggestionSelectedSpy.count(), 1);
    QCOMPARE(autoSearchSpy.count(), 1);
    QCOMPARE(searchDelaySpy.count(), 1);
    QVERIFY(historySpy.count() >= 1);
}

void FluentSearchBoxTest::testKeyboardInteraction() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    m_searchBox->setFocus();
    
    QSignalSpy searchSpy(m_searchBox, &FluentSearchBox::searchRequested);
    QSignalSpy textSpy(m_searchBox, &FluentSearchBox::searchTextChanged);
    
    // Test typing
    QTest::keyClicks(m_searchBox, "test query");
    QCOMPARE(m_searchBox->searchText(), "test query");
    QVERIFY(textSpy.count() >= 1);
    
    // Test Enter key
    QTest::keyClick(m_searchBox, Qt::Key_Return);
    QCOMPARE(searchSpy.count(), 1);
    
    // Test Escape key to clear
    QTest::keyClick(m_searchBox, Qt::Key_Escape);
    QCOMPARE(m_searchBox->searchText(), "");
    
    // Test arrow keys for suggestion navigation (if suggestions are visible)
    m_searchBox->setSuggestions({"apple", "application", "apply"});
    m_searchBox->setSearchText("app");
    
    QTest::keyClick(m_searchBox, Qt::Key_Down);
    QTest::keyClick(m_searchBox, Qt::Key_Up);
    QTest::keyClick(m_searchBox, Qt::Key_Return);
}

void FluentSearchBoxTest::testMouseInteraction() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);
    
    m_searchBox->setSearchText("test text");
    
    QSignalSpy clearedSpy(m_searchBox, &FluentSearchBox::searchCleared);
    QSignalSpy searchSpy(m_searchBox, &FluentSearchBox::searchRequested);
    
    // Test clicking clear button (if visible and accessible)
    if (m_searchBox->isClearButtonVisible()) {
        // Simulate clear button click
        m_searchBox->clear();
        QCOMPARE(clearedSpy.count(), 1);
    }
    
    // Test clicking search button (if visible and accessible)
    if (m_searchBox->isSearchButtonVisible()) {
        // Simulate search button click
        m_searchBox->performSearch();
        QCOMPARE(searchSpy.count(), 1);
    }
    
    // Test mouse click to focus
    QTest::mouseClick(m_searchBox, Qt::LeftButton);
    QVERIFY(m_searchBox->hasFocus());
    
    // Test double-click to select all
    QTest::mouseDClick(m_searchBox, Qt::LeftButton);
    QVERIFY(m_searchBox->hasSelectedText());
}

QTEST_MAIN(FluentSearchBoxTest)
#include "FluentSearchBoxTest.moc"
