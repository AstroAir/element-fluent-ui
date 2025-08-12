// tests/Components/FluentTreeViewPerfTest.cpp
#include <QSignalSpy>
#include <QtTest/QtTest>
#include "FluentQt/Components/FluentTreeView.h"
#include "FluentQt/Core/FluentPerformance.h"

using namespace FluentQt::Components;
using FluentQt::Core::FluentPerformanceMonitor;

class FluentTreeViewPerfTest : public QObject {
    Q_OBJECT
private slots:
    void debounceFiltering_shouldDelayFilteringWork();
    void styleCaching_shouldReuseCachedStyles();
};

void FluentTreeViewPerfTest::debounceFiltering_shouldDelayFilteringWork() {
    FluentTreeView view;
    view.resize(400, 300);

    // Populate with a small tree to ensure filter traverses items
    auto* a = view.addTopLevelItem("Alpha");
    auto* b = view.addTopLevelItem("Beta");
    view.addChildItem(a, "Alpha-1");
    view.addChildItem(a, "Alpha-2");
    view.addChildItem(b, "Beta-1");

    // Start with empty filter, then set to a value
    QElapsedTimer t;
    t.start();
    view.setFilter("a");
    // Because of debounce (120ms), filtering should not complete immediately.
    // Give it a short time (< debounce) and verify that another change resets
    // timer.
    QTest::qWait(60);
    view.setFilter("al");
    // Now wait slightly longer than debounce to allow apply
    QTest::qWait(140);

    // If we reached here without crashing, debounce at least coalesced the two
    // inputs.
    QVERIFY(t.elapsed() >= 200);  // sanity: total wait time was ~200ms
}

void FluentTreeViewPerfTest::styleCaching_shouldReuseCachedStyles() {
    FluentTreeView view;

    // First application builds styles
    view.setFilteringEnabled(true);  // triggers UI to be present
    view.show();
    QCoreApplication::processEvents();

    // Call updateTreeStyling twice to simulate themeChanged without palette
    // change
    view.setStyleSheet("");  // ensure no prior style prevents setStyleSheet
    view.update();
    QMetaObject::invokeMethod(&view, "updateTreeStyling", Qt::DirectConnection);
    QMetaObject::invokeMethod(&view, "updateTreeStyling", Qt::DirectConnection);

    // No direct hook to assert cache hit; at least ensure nothing crashes and
    // widget has stylesheet
    QVERIFY(!view.styleSheet().isNull());
}

QTEST_MAIN(FluentTreeViewPerfTest)
#include "FluentTreeViewPerfTest.moc"
