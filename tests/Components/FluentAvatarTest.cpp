// tests/Components/FluentAvatarTest.cpp
#include <QSignalSpy>
#include <QtTest/QtTest>

#include "FluentQt/Components/FluentAvatar.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Core;

class FluentAvatarTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Constructor tests
    void testDefaultConstructor();

    // Content tests
    void testName();
    void testInitials();
    void testImage();
    void testIcon();

    // Type tests
    void testAvatarType();
    void testAvatarSize();
    void testShape();
    void testStatus();

    // Behavior tests
    void testClickable();
    void testAnimated();
    void testTooltip();
    void testShowStatus();

    // Utility tests
    void testGenerateInitials();
    void testGenerateBackgroundColor();
    void testValidation();
    void testSizeCalculations();

    // Static factory tests
    void testFactoryMethods();

    // Animation tests
    void testAnimations();

    // Signal tests
    void testSignalEmission();

private:
    FluentAvatar* m_avatar{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentAvatarTest::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(400, 300);
}

void FluentAvatarTest::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentAvatarTest::init() { m_avatar = new FluentAvatar(m_testWidget); }

void FluentAvatarTest::cleanup() {
    delete m_avatar;
    m_avatar = nullptr;
}

void FluentAvatarTest::testDefaultConstructor() {
    QVERIFY(m_avatar != nullptr);
    QCOMPARE(m_avatar->name(), QString());
    QCOMPARE(m_avatar->avatarType(), FluentAvatarType::Initials);
    QCOMPARE(m_avatar->avatarSize(), FluentAvatarSize::Medium);
    QCOMPARE(m_avatar->shape(), FluentAvatarShape::Circle);
    QCOMPARE(m_avatar->status(), FluentAvatarStatus::None);
}

void FluentAvatarTest::testName() {
    QSignalSpy nameSpy(m_avatar, &FluentAvatar::nameChanged);

    m_avatar->setName("John Doe");
    QCOMPARE(m_avatar->name(), "John Doe");
    QCOMPARE(nameSpy.count(), 1);
    QCOMPARE(nameSpy.first().first().toString(), "John Doe");
}

void FluentAvatarTest::testInitials() {
    QSignalSpy initialsSpy(m_avatar, &FluentAvatar::initialsChanged);

    m_avatar->setInitials("JD");
    QCOMPARE(m_avatar->initials(), "JD");
    QCOMPARE(initialsSpy.count(), 1);
}

void FluentAvatarTest::testImage() {
    QSignalSpy imageSpy(m_avatar, &FluentAvatar::imageChanged);
    QPixmap testImage(64, 64);
    testImage.fill(Qt::red);

    m_avatar->setImage(testImage);
    QCOMPARE(m_avatar->image().size(), testImage.size());
    QCOMPARE(imageSpy.count(), 1);
}

void FluentAvatarTest::testIcon() {
    QSignalSpy iconSpy(m_avatar, &FluentAvatar::iconChanged);
    QIcon testIcon(":/test/icon.png");

    m_avatar->setIcon(testIcon);
    QCOMPARE(m_avatar->icon().name(), testIcon.name());
    QCOMPARE(iconSpy.count(), 1);
}

void FluentAvatarTest::testAvatarType() {
    QSignalSpy typeSpy(m_avatar, &FluentAvatar::avatarTypeChanged);

    m_avatar->setAvatarType(FluentAvatarType::Image);
    QCOMPARE(m_avatar->avatarType(), FluentAvatarType::Image);
    QCOMPARE(typeSpy.count(), 1);

    m_avatar->setAvatarType(FluentAvatarType::Icon);
    QCOMPARE(m_avatar->avatarType(), FluentAvatarType::Icon);
    QCOMPARE(typeSpy.count(), 2);
}

void FluentAvatarTest::testAvatarSize() {
    QSignalSpy sizeSpy(m_avatar, &FluentAvatar::avatarSizeChanged);

    m_avatar->setAvatarSize(FluentAvatarSize::Large);
    QCOMPARE(m_avatar->avatarSize(), FluentAvatarSize::Large);
    QCOMPARE(sizeSpy.count(), 1);

    m_avatar->setAvatarSize(FluentAvatarSize::Small);
    QCOMPARE(m_avatar->avatarSize(), FluentAvatarSize::Small);
    QCOMPARE(sizeSpy.count(), 2);
}

void FluentAvatarTest::testShape() {
    QSignalSpy shapeSpy(m_avatar, &FluentAvatar::shapeChanged);

    m_avatar->setShape(FluentAvatarShape::Square);
    QCOMPARE(m_avatar->shape(), FluentAvatarShape::Square);
    QCOMPARE(shapeSpy.count(), 1);
}

void FluentAvatarTest::testStatus() {
    QSignalSpy statusSpy(m_avatar, &FluentAvatar::statusChanged);

    m_avatar->setStatus(FluentAvatarStatus::Busy);
    QCOMPARE(m_avatar->status(), FluentAvatarStatus::Busy);
    QCOMPARE(statusSpy.count(), 1);

    m_avatar->setStatus(FluentAvatarStatus::Away);
    QCOMPARE(m_avatar->status(), FluentAvatarStatus::Away);
    QCOMPARE(statusSpy.count(), 2);
}

void FluentAvatarTest::testClickable() {
    QSignalSpy clickableSpy(m_avatar, &FluentAvatar::clickableChanged);

    m_avatar->setClickable(true);
    QVERIFY(m_avatar->isClickable());
    QCOMPARE(clickableSpy.count(), 1);

    m_avatar->setClickable(false);
    QVERIFY(!m_avatar->isClickable());
    QCOMPARE(clickableSpy.count(), 2);
}

void FluentAvatarTest::testAnimated() {
    QSignalSpy animatedSpy(m_avatar, &FluentAvatar::animatedChanged);

    m_avatar->setAnimated(false);
    QVERIFY(!m_avatar->isAnimated());
    QCOMPARE(animatedSpy.count(), 1);
}

void FluentAvatarTest::testTooltip() {
    QSignalSpy tooltipSpy(m_avatar, &FluentAvatar::tooltipChanged);

    m_avatar->setTooltip("User Avatar");
    QCOMPARE(m_avatar->tooltip(), "User Avatar");
    QCOMPARE(tooltipSpy.count(), 1);
}

void FluentAvatarTest::testShowStatus() {
    QSignalSpy showStatusSpy(m_avatar, &FluentAvatar::showStatusChanged);

    m_avatar->setShowStatus(false);
    QVERIFY(!m_avatar->showStatus());
    QCOMPARE(showStatusSpy.count(), 1);
}

void FluentAvatarTest::testGenerateInitials() {
    QString initials = m_avatar->generateInitials("John Doe");
    QCOMPARE(initials, "JD");

    initials = m_avatar->generateInitials("Alice");
    QCOMPARE(initials, "A");

    initials = m_avatar->generateInitials("Mary Jane Watson");
    QCOMPARE(initials, "MW");  // Should take first and last

    initials = m_avatar->generateInitials("");
    QCOMPARE(initials, "");
}

void FluentAvatarTest::testGenerateBackgroundColor() {
    QColor color1 = m_avatar->generateBackgroundColor("John");
    QColor color2 = m_avatar->generateBackgroundColor("Jane");

    QVERIFY(color1.isValid());
    QVERIFY(color2.isValid());
    // Different names should generate different colors
    QVERIFY(color1 != color2);

    // Same name should generate same color
    QColor color3 = m_avatar->generateBackgroundColor("John");
    QCOMPARE(color1, color3);
}

void FluentAvatarTest::testValidation() {
    // Test image validation
    QPixmap validImage(64, 64);
    validImage.fill(Qt::blue);
    m_avatar->setImage(validImage);
    QVERIFY(m_avatar->hasValidImage());

    // Test icon validation
    QIcon validIcon(":/test/icon.png");
    m_avatar->setIcon(validIcon);
    QVERIFY(m_avatar->hasValidIcon());
}

void FluentAvatarTest::testSizeCalculations() {
    m_avatar->setAvatarSize(FluentAvatarSize::Small);
    int smallDiameter = m_avatar->avatarDiameter();

    m_avatar->setAvatarSize(FluentAvatarSize::Medium);
    int mediumDiameter = m_avatar->avatarDiameter();

    m_avatar->setAvatarSize(FluentAvatarSize::Large);
    int largeDiameter = m_avatar->avatarDiameter();

    QVERIFY(smallDiameter < mediumDiameter);
    QVERIFY(mediumDiameter < largeDiameter);

    // Test size hints
    QSize sizeHint = m_avatar->sizeHint();
    QSize minSizeHint = m_avatar->minimumSizeHint();

    QVERIFY(sizeHint.isValid());
    QVERIFY(minSizeHint.isValid());
}

void FluentAvatarTest::testFactoryMethods() {
    // Test image avatar factory
    QPixmap testImage(64, 64);
    testImage.fill(Qt::green);
    auto* imageAvatar =
        FluentAvatar::createImageAvatar(testImage, FluentAvatarSize::Large);
    QVERIFY(imageAvatar != nullptr);
    QCOMPARE(imageAvatar->avatarType(), FluentAvatarType::Image);
    QCOMPARE(imageAvatar->avatarSize(), FluentAvatarSize::Large);
    delete imageAvatar;

    // Test initials avatar factory
    auto* initialsAvatar = FluentAvatar::createInitialsAvatar("Test User");
    QVERIFY(initialsAvatar != nullptr);
    QCOMPARE(initialsAvatar->avatarType(), FluentAvatarType::Initials);
    QCOMPARE(initialsAvatar->name(), "Test User");
    delete initialsAvatar;

    // Test icon avatar factory
    QIcon testIcon(":/test/icon.png");
    auto* iconAvatar = FluentAvatar::createIconAvatar(testIcon);
    QVERIFY(iconAvatar != nullptr);
    QCOMPARE(iconAvatar->avatarType(), FluentAvatarType::Icon);
    delete iconAvatar;

    // Test placeholder avatar factory
    auto* placeholderAvatar = FluentAvatar::createPlaceholderAvatar();
    QVERIFY(placeholderAvatar != nullptr);
    delete placeholderAvatar;
}

void FluentAvatarTest::testAnimations() {
    m_testWidget->show();
    QTest::qWaitForWindowExposed(m_testWidget);

    // Test animation methods don't crash
    m_avatar->animateIn();
    m_avatar->animateOut();
    m_avatar->pulse();
    m_avatar->bounce();

    // These are visual effects, hard to test programmatically
    // but we can ensure they don't crash
    QTest::qWait(100);
}

void FluentAvatarTest::testSignalEmission() {
    QSignalSpy nameSpy(m_avatar, &FluentAvatar::nameChanged);
    QSignalSpy initialsSpy(m_avatar, &FluentAvatar::initialsChanged);
    QSignalSpy imageSpy(m_avatar, &FluentAvatar::imageChanged);
    QSignalSpy iconSpy(m_avatar, &FluentAvatar::iconChanged);
    QSignalSpy typeSpy(m_avatar, &FluentAvatar::avatarTypeChanged);
    QSignalSpy sizeSpy(m_avatar, &FluentAvatar::avatarSizeChanged);
    QSignalSpy shapeSpy(m_avatar, &FluentAvatar::shapeChanged);
    QSignalSpy statusSpy(m_avatar, &FluentAvatar::statusChanged);
    QSignalSpy clickableSpy(m_avatar, &FluentAvatar::clickableChanged);
    QSignalSpy animatedSpy(m_avatar, &FluentAvatar::animatedChanged);
    QSignalSpy tooltipSpy(m_avatar, &FluentAvatar::tooltipChanged);
    QSignalSpy showStatusSpy(m_avatar, &FluentAvatar::showStatusChanged);

    // Test all signals
    m_avatar->setName("Test");
    m_avatar->setInitials("T");
    m_avatar->setImage(QPixmap(32, 32));
    m_avatar->setIcon(QIcon());
    m_avatar->setAvatarType(FluentAvatarType::Image);
    m_avatar->setAvatarSize(FluentAvatarSize::Large);
    m_avatar->setShape(FluentAvatarShape::Square);
    m_avatar->setStatus(FluentAvatarStatus::Busy);
    m_avatar->setClickable(true);
    m_avatar->setAnimated(false);
    m_avatar->setTooltip("Test");
    m_avatar->setShowStatus(false);

    QCOMPARE(nameSpy.count(), 1);
    QCOMPARE(initialsSpy.count(), 1);
    QCOMPARE(imageSpy.count(), 1);
    QCOMPARE(iconSpy.count(), 1);
    QCOMPARE(typeSpy.count(), 1);
    QCOMPARE(sizeSpy.count(), 1);
    QCOMPARE(shapeSpy.count(), 1);
    QCOMPARE(statusSpy.count(), 1);
    QCOMPARE(clickableSpy.count(), 1);
    QCOMPARE(animatedSpy.count(), 1);
    QCOMPARE(tooltipSpy.count(), 1);
    QCOMPARE(showStatusSpy.count(), 1);
}

QTEST_MAIN(FluentAvatarTest)
#include "FluentAvatarTest.moc"
