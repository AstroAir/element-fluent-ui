#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QPixmap>
#include <QWidget>
#include "FluentQt/Components/FluentImageView.h"

using namespace FluentQt::Components;

class FluentImageViewTestSimple : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Basic functionality tests
    void testDefaultConstructor();
    void testPixmapConstructor();
    void testSetPixmap();
    void testSetSource();
    void testScaleMode();
    void testAlignment();
    void testZoomFunctionality();
    void testRotation();
    void testSignals();

private:
    FluentImageView* m_imageView{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentImageViewTestSimple::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(400, 300);
}

void FluentImageViewTestSimple::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentImageViewTestSimple::init() {
    m_imageView = new FluentImageView(m_testWidget);
}

void FluentImageViewTestSimple::cleanup() {
    delete m_imageView;
    m_imageView = nullptr;
}

void FluentImageViewTestSimple::testDefaultConstructor() {
    QVERIFY(m_imageView != nullptr);
    QVERIFY(m_imageView->pixmap().isNull());
    QCOMPARE(m_imageView->scaleMode(), FluentImageScaleMode::Fit);
    QCOMPARE(m_imageView->alignment(), FluentImageAlignment::Center);
    QVERIFY(m_imageView->isZoomEnabled());
    QCOMPARE(m_imageView->zoomFactor(), 1.0);
}

void FluentImageViewTestSimple::testPixmapConstructor() {
    QPixmap testPixmap(100, 100);
    testPixmap.fill(Qt::red);
    
    auto* imageView = new FluentImageView(testPixmap, m_testWidget);
    QVERIFY(!imageView->pixmap().isNull());
    QCOMPARE(imageView->pixmap().size(), testPixmap.size());
    delete imageView;
}

void FluentImageViewTestSimple::testSetPixmap() {
    QSignalSpy pixmapSpy(m_imageView, &FluentImageView::pixmapChanged);
    
    QPixmap testPixmap(150, 100);
    testPixmap.fill(Qt::blue);
    
    m_imageView->setPixmap(testPixmap);
    QCOMPARE(m_imageView->pixmap().size(), testPixmap.size());
    QCOMPARE(pixmapSpy.count(), 1);
    
    // Test null pixmap
    m_imageView->setPixmap(QPixmap());
    QVERIFY(m_imageView->pixmap().isNull());
    QCOMPARE(pixmapSpy.count(), 2);
}

void FluentImageViewTestSimple::testSetSource() {
    QSignalSpy sourceSpy(m_imageView, &FluentImageView::sourceChanged);
    
    QString testSource = "test_image.png";
    m_imageView->setSource(testSource);
    QCOMPARE(m_imageView->source(), testSource);
    QCOMPARE(sourceSpy.count(), 1);
    
    // Test empty source
    m_imageView->setSource("");
    QCOMPARE(m_imageView->source(), QString(""));
    QCOMPARE(sourceSpy.count(), 2);
}

void FluentImageViewTestSimple::testScaleMode() {
    QSignalSpy scaleModeSpy(m_imageView, &FluentImageView::scaleModeChanged);

    m_imageView->setScaleMode(FluentImageScaleMode::None);
    QCOMPARE(m_imageView->scaleMode(), FluentImageScaleMode::None);
    QCOMPARE(scaleModeSpy.count(), 1);

    m_imageView->setScaleMode(FluentImageScaleMode::Fill);
    QCOMPARE(m_imageView->scaleMode(), FluentImageScaleMode::Fill);
    QCOMPARE(scaleModeSpy.count(), 2);

    m_imageView->setScaleMode(FluentImageScaleMode::Stretch);
    QCOMPARE(m_imageView->scaleMode(), FluentImageScaleMode::Stretch);
    QCOMPARE(scaleModeSpy.count(), 3);
}

void FluentImageViewTestSimple::testAlignment() {
    QSignalSpy alignmentSpy(m_imageView, &FluentImageView::alignmentChanged);

    m_imageView->setAlignment(FluentImageAlignment::TopLeft);
    QCOMPARE(m_imageView->alignment(), FluentImageAlignment::TopLeft);
    QCOMPARE(alignmentSpy.count(), 1);

    m_imageView->setAlignment(FluentImageAlignment::BottomRight);
    QCOMPARE(m_imageView->alignment(), FluentImageAlignment::BottomRight);
    QCOMPARE(alignmentSpy.count(), 2);
}

void FluentImageViewTestSimple::testZoomFunctionality() {
    QSignalSpy zoomEnabledSpy(m_imageView, &FluentImageView::zoomEnabledChanged);
    QSignalSpy zoomFactorSpy(m_imageView, &FluentImageView::zoomFactorChanged);

    // Test zoom enabled/disabled
    m_imageView->setZoomEnabled(false);
    QVERIFY(!m_imageView->isZoomEnabled());
    QCOMPARE(zoomEnabledSpy.count(), 1);

    m_imageView->setZoomEnabled(true);
    QVERIFY(m_imageView->isZoomEnabled());
    QCOMPARE(zoomEnabledSpy.count(), 2);

    // Test zoom factor
    m_imageView->setZoomFactor(2.0);
    QCOMPARE(m_imageView->zoomFactor(), 2.0);
    QCOMPARE(zoomFactorSpy.count(), 1);

    // Test zoom methods
    qreal initialZoom = m_imageView->zoomFactor();
    m_imageView->zoomIn();
    QVERIFY(m_imageView->zoomFactor() > initialZoom);

    qreal currentZoom = m_imageView->zoomFactor();
    m_imageView->zoomOut();
    QVERIFY(m_imageView->zoomFactor() < currentZoom);

    m_imageView->resetZoom();
    QCOMPARE(m_imageView->zoomFactor(), 1.0);
}

void FluentImageViewTestSimple::testRotation() {
    // Test rotation methods (they exist but don't expose angle)
    m_imageView->rotateLeft();
    m_imageView->rotateRight();
    
    // Test flip methods
    m_imageView->flipHorizontal();
    m_imageView->flipVertical();
    
    // These should not crash and should be callable
    QVERIFY(true);
}

void FluentImageViewTestSimple::testSignals() {
    QSignalSpy pixmapSpy(m_imageView, &FluentImageView::pixmapChanged);
    QSignalSpy sourceSpy(m_imageView, &FluentImageView::sourceChanged);
    QSignalSpy scaleModeSpy(m_imageView, &FluentImageView::scaleModeChanged);
    QSignalSpy alignmentSpy(m_imageView, &FluentImageView::alignmentChanged);
    QSignalSpy zoomEnabledSpy(m_imageView, &FluentImageView::zoomEnabledChanged);
    QSignalSpy zoomFactorSpy(m_imageView, &FluentImageView::zoomFactorChanged);

    // Trigger various changes
    QPixmap testPixmap(50, 50);
    testPixmap.fill(Qt::yellow);
    m_imageView->setPixmap(testPixmap);
    m_imageView->setSource("test.png");
    m_imageView->setScaleMode(FluentImageScaleMode::Fill);
    m_imageView->setAlignment(FluentImageAlignment::TopLeft);
    m_imageView->setZoomEnabled(false);
    m_imageView->setZoomFactor(1.5);

    // Verify signals were emitted
    QCOMPARE(pixmapSpy.count(), 1);
    QCOMPARE(sourceSpy.count(), 1);
    QCOMPARE(scaleModeSpy.count(), 1);
    QCOMPARE(alignmentSpy.count(), 1);
    QCOMPARE(zoomEnabledSpy.count(), 1);
    QCOMPARE(zoomFactorSpy.count(), 1);
}

QTEST_MAIN(FluentImageViewTestSimple)
#include "FluentImageViewTestSimple.moc"
