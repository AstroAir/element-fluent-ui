// tests/FluentCarouselCoalesceTest.cpp
#include <QSignalSpy>
#include <QTimer>
#include <QtTest/QtTest>
#include "FluentQt/Components/FluentCarousel.h"

using namespace FluentQt::Components;

class TestableCarousel : public FluentCarousel {
    Q_OBJECT
public:
    using FluentCarousel::FluentCarousel;

    int paintCount() const { return m_paintCount; }

protected:
    void paintEvent(QPaintEvent* e) override {
        ++m_paintCount;
        FluentCarousel::paintEvent(e);
    }

private:
    int m_paintCount{0};
};

class FluentCarouselCoalesceTest : public QObject {
    Q_OBJECT
private slots:
    void coalescesRepaintsDuringTransition();
};

void FluentCarouselCoalesceTest::coalescesRepaintsDuringTransition() {
    TestableCarousel carousel;
    QWidget a, b, c;
    carousel.addItem(&a);
    carousel.addItem(&b);
    carousel.addItem(&c);

    // Ensure auto-play off to control updates
    carousel.setAutoPlayEnabled(false);

    // Start at index 0, move to 1 with animation
    const int initialPaints = carousel.paintCount();
    carousel.setCurrentIndex(1, /*animated*/ true);

    // Pump events while animation runs; we expect multiple valueChanged signals
    // but only a limited number of paintEvents thanks to coalescing (~60Hz)
    int ms = 0;
    while (ms < 350) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 5);
        QTest::qWait(5);
        ms += 5;
    }

    const int paints = carousel.paintCount() - initialPaints;
    QVERIFY(paints > 0);
    QVERIFY2(paints < 40,
             "Repaint coalescing should cap paintEvents under animation");
}

QTEST_MAIN(FluentCarouselCoalesceTest)
#include "FluentCarouselCoalesceTest.moc"
