// tests/Components/FluentCheckBoxTestSimple.cpp
#include <QtTest/QtTest>
#include "FluentQt/Components/FluentCheckBox.h"

using namespace FluentQt::Components;

class FluentCheckBoxTestSimple : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Basic functionality tests (no signals)
    void testDefaultConstructor();
    void testTextConstructor();
    void testBasicProperties();
    void testCheckState();
    void testTristate();
    void testIconSize();
    void testAutoExclusive();
    void testCheckProgress();

private:
    FluentCheckBox* m_checkBox{nullptr};
    QWidget* m_testWidget{nullptr};
};

void FluentCheckBoxTestSimple::initTestCase() {
    m_testWidget = new QWidget();
    m_testWidget->resize(400, 300);
}

void FluentCheckBoxTestSimple::cleanupTestCase() {
    delete m_testWidget;
    m_testWidget = nullptr;
}

void FluentCheckBoxTestSimple::init() {
    m_checkBox = new FluentCheckBox(m_testWidget);
}

void FluentCheckBoxTestSimple::cleanup() {
    delete m_checkBox;
    m_checkBox = nullptr;
}

void FluentCheckBoxTestSimple::testDefaultConstructor() {
    QVERIFY(m_checkBox != nullptr);
    QCOMPARE(m_checkBox->text(), QString());
    QCOMPARE(m_checkBox->checkState(), FluentCheckState::Unchecked);
    QVERIFY(!m_checkBox->isChecked());
    QVERIFY(!m_checkBox->isTristate());
    QVERIFY(m_checkBox->isEnabled());
}

void FluentCheckBoxTestSimple::testTextConstructor() {
    auto* checkBox = new FluentCheckBox("Test CheckBox", m_testWidget);
    QCOMPARE(checkBox->text(), "Test CheckBox");
    delete checkBox;
}

void FluentCheckBoxTestSimple::testBasicProperties() {
    // Test text property
    m_checkBox->setText("New Text");
    QCOMPARE(m_checkBox->text(), "New Text");

    // Test empty text
    m_checkBox->setText("");
    QCOMPARE(m_checkBox->text(), "");

    // Test enabled/disabled
    m_checkBox->setEnabled(false);
    QVERIFY(!m_checkBox->isEnabled());

    m_checkBox->setEnabled(true);
    QVERIFY(m_checkBox->isEnabled());
}

void FluentCheckBoxTestSimple::testCheckState() {
    // Test setting checked state
    m_checkBox->setCheckState(FluentCheckState::Checked);
    QCOMPARE(m_checkBox->checkState(), FluentCheckState::Checked);
    QVERIFY(m_checkBox->isChecked());

    m_checkBox->setCheckState(FluentCheckState::PartiallyChecked);
    QCOMPARE(m_checkBox->checkState(), FluentCheckState::PartiallyChecked);
    QVERIFY(!m_checkBox->isChecked());

    m_checkBox->setCheckState(FluentCheckState::Unchecked);
    QCOMPARE(m_checkBox->checkState(), FluentCheckState::Unchecked);
    QVERIFY(!m_checkBox->isChecked());
}

void FluentCheckBoxTestSimple::testTristate() {
    // Test tristate functionality
    m_checkBox->setTristate(true);
    QVERIFY(m_checkBox->isTristate());

    m_checkBox->setTristate(false);
    QVERIFY(!m_checkBox->isTristate());

    // Test that tristate allows PartiallyChecked
    m_checkBox->setTristate(true);
    m_checkBox->setCheckState(FluentCheckState::PartiallyChecked);
    QCOMPARE(m_checkBox->checkState(), FluentCheckState::PartiallyChecked);
}

void FluentCheckBoxTestSimple::testIconSize() {
    // Test icon size property
    QSize testSize(24, 24);
    m_checkBox->setIconSize(testSize);
    QCOMPARE(m_checkBox->iconSize(), testSize);

    QSize smallSize(16, 16);
    m_checkBox->setIconSize(smallSize);
    QCOMPARE(m_checkBox->iconSize(), smallSize);
}

void FluentCheckBoxTestSimple::testAutoExclusive() {
    // Test auto exclusive property
    m_checkBox->setAutoExclusive(true);
    QVERIFY(m_checkBox->autoExclusive());

    m_checkBox->setAutoExclusive(false);
    QVERIFY(!m_checkBox->autoExclusive());
}

void FluentCheckBoxTestSimple::testCheckProgress() {
    // Test check progress property (for animations)
    m_checkBox->setCheckProgress(0.5);
    QCOMPARE(m_checkBox->checkProgress(), 0.5);

    m_checkBox->setCheckProgress(1.0);
    QCOMPARE(m_checkBox->checkProgress(), 1.0);

    m_checkBox->setCheckProgress(0.0);
    QCOMPARE(m_checkBox->checkProgress(), 0.0);
}

QTEST_MAIN(FluentCheckBoxTestSimple)
#include "FluentCheckBoxTestSimple.moc"
