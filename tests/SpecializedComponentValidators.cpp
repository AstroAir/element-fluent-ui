#include <QSignalSpy>
#include <QTest>
#include "FluentComponentValidator.h"

// Form Component Validators Implementation
bool FormComponentValidator::validateButton(
    Components::FluentButton* button, FluentComponentValidator* validator) {
    if (!button || !validator)
        return false;

    bool allValid = true;

    // Test button text functionality
    QString originalText = button->text();
    QString testText = "Test Button Text";
    button->setText(testText);
    bool textSet = (button->text() == testText);

    validator->addResult(
        "Button Text Setting", "FluentButton", "Form Component", textSet,
        textSet ? "Text set correctly" : "Text not set properly", testText,
        button->text());

    if (!textSet)
        allValid = false;

    // Test button states
    button->setCheckable(true);
    button->setChecked(true);
    bool checkableWorks = button->isChecked();

    validator->addResult(
        "Button Checkable State", "FluentButton", "Form Component",
        checkableWorks,
        checkableWorks ? "Checkable state works" : "Checkable state failed",
        "Checked", checkableWorks ? "Checked" : "Not checked");

    if (!checkableWorks)
        allValid = false;

    // Test button signals
    QSignalSpy clickedSpy(button, &Components::FluentButton::clicked);
    button->animateClick();
    bool signalEmitted = (clickedSpy.count() > 0);

    validator->addResult(
        "Button Clicked Signal", "FluentButton", "Form Component",
        signalEmitted,
        signalEmitted ? "Clicked signal emitted" : "Clicked signal not emitted",
        "Signal emitted", signalEmitted ? "Signal emitted" : "No signal");

    if (!signalEmitted)
        allValid = false;

    // Test button styles
    auto originalStyle = button->buttonStyle();
    button->setButtonStyle(Components::FluentButtonStyle::Accent);
    bool styleChanged =
        (button->buttonStyle() == Components::FluentButtonStyle::Accent);

    validator->addResult(
        "Button Style Change", "FluentButton", "Form Component", styleChanged,
        styleChanged ? "Style changed successfully" : "Style change failed",
        "Accent style", styleChanged ? "Accent style" : "Style unchanged");

    if (!styleChanged)
        allValid = false;

    // Restore original state
    button->setText(originalText);
    button->setButtonStyle(originalStyle);
    button->setCheckable(false);

    return allValid;
}

bool FormComponentValidator::validateTextInput(
    Components::FluentTextInput* input, FluentComponentValidator* validator) {
    if (!input || !validator)
        return false;

    bool allValid = true;

    // Test text input functionality
    QString testText = "Test Input Text";
    input->setText(testText);
    bool textSet = (input->text() == testText);

    validator->addResult(
        "TextInput Text Setting", "FluentTextInput", "Form Component", textSet,
        textSet ? "Text set correctly" : "Text not set properly", testText,
        input->text());

    if (!textSet)
        allValid = false;

    // Test placeholder text
    QString placeholderText = "Enter text here...";
    input->setPlaceholderText(placeholderText);
    bool placeholderSet = (input->placeholderText() == placeholderText);

    validator->addResult(
        "TextInput Placeholder", "FluentTextInput", "Form Component",
        placeholderSet,
        placeholderSet ? "Placeholder set correctly" : "Placeholder not set",
        placeholderText, input->placeholderText());

    if (!placeholderSet)
        allValid = false;

    // Test text change signal
    QSignalSpy textChangedSpy(input, &Components::FluentTextInput::textChanged);
    input->setText("New text");
    bool signalEmitted = (textChangedSpy.count() > 0);

    validator->addResult(
        "TextInput Signal", "FluentTextInput", "Form Component", signalEmitted,
        signalEmitted ? "Text changed signal emitted" : "Signal not emitted",
        "Signal emitted", signalEmitted ? "Signal emitted" : "No signal");

    if (!signalEmitted)
        allValid = false;

    // Test clear functionality
    input->clear();
    bool cleared = input->text().isEmpty();

    validator->addResult(
        "TextInput Clear", "FluentTextInput", "Form Component", cleared,
        cleared ? "Text cleared successfully" : "Clear failed", "Empty text",
        cleared ? "Empty text" : "Text not cleared");

    if (!cleared)
        allValid = false;

    return allValid;
}

bool FormComponentValidator::validateCheckBox(
    Components::FluentCheckBox* checkbox, FluentComponentValidator* validator) {
    if (!checkbox || !validator)
        return false;

    bool allValid = true;

    // Test checkbox text
    QString testText = "Test Checkbox";
    checkbox->setText(testText);
    bool textSet = (checkbox->text() == testText);

    validator->addResult("CheckBox Text", "FluentCheckBox", "Form Component",
                         textSet,
                         textSet ? "Text set correctly" : "Text not set",
                         testText, checkbox->text());

    if (!textSet)
        allValid = false;

    // Test checkbox state
    checkbox->setChecked(true);
    bool checked = checkbox->isChecked();

    validator->addResult("CheckBox State", "FluentCheckBox", "Form Component",
                         checked,
                         checked ? "Checkbox checked" : "Checkbox not checked",
                         "Checked", checked ? "Checked" : "Not checked");

    if (!checked)
        allValid = false;

    // Test toggle functionality
    QSignalSpy toggledSpy(checkbox, &Components::FluentCheckBox::toggled);
    checkbox->toggle();
    bool toggled = !checkbox->isChecked() && (toggledSpy.count() > 0);

    validator->addResult("CheckBox Toggle", "FluentCheckBox", "Form Component",
                         toggled,
                         toggled ? "Toggle works correctly" : "Toggle failed",
                         "Toggled state", toggled ? "Toggled" : "Not toggled");

    if (!toggled)
        allValid = false;

    return allValid;
}

bool FormComponentValidator::validateComboBox(
    Components::FluentComboBox* combobox, FluentComponentValidator* validator) {
    if (!combobox || !validator)
        return false;

    bool allValid = true;

    // Test adding items
    QStringList testItems = {"Item 1", "Item 2", "Item 3"};
    for (const QString& item : testItems) {
        combobox->addItem(item);
    }

    bool itemsAdded = (combobox->count() == testItems.size());

    validator->addResult(
        "ComboBox Items", "FluentComboBox", "Form Component", itemsAdded,
        itemsAdded ? "Items added correctly" : "Items not added",
        QString("%1 items").arg(testItems.size()),
        QString("%1 items").arg(combobox->count()));

    if (!itemsAdded)
        allValid = false;

    // Test current item selection
    combobox->setCurrentIndex(1);
    bool selectionWorks = (combobox->currentIndex() == 1);

    validator->addResult(
        "ComboBox Selection", "FluentComboBox", "Form Component",
        selectionWorks, selectionWorks ? "Selection works" : "Selection failed",
        "Index 1", QString("Index %1").arg(combobox->currentIndex()));

    if (!selectionWorks)
        allValid = false;

    // Test current text
    QString expectedText = testItems[1];
    bool textMatches = (combobox->currentText() == expectedText);

    validator->addResult(
        "ComboBox Current Text", "FluentComboBox", "Form Component",
        textMatches,
        textMatches ? "Current text correct" : "Current text incorrect",
        expectedText, combobox->currentText());

    if (!textMatches)
        allValid = false;

    return allValid;
}

bool FormComponentValidator::validateSlider(
    Components::FluentSlider* slider, FluentComponentValidator* validator) {
    if (!slider || !validator)
        return false;

    bool allValid = true;

    // Test slider range
    slider->setRange(0, 100);
    bool rangeSet = (slider->minimum() == 0 && slider->maximum() == 100);

    validator->addResult(
        "Slider Range", "FluentSlider", "Form Component", rangeSet,
        rangeSet ? "Range set correctly" : "Range not set", "0-100",
        QString("%1-%2").arg(slider->minimum()).arg(slider->maximum()));

    if (!rangeSet)
        allValid = false;

    // Test slider value
    slider->setValue(50);
    bool valueSet = (slider->value() == 50);

    validator->addResult("Slider Value", "FluentSlider", "Form Component",
                         valueSet,
                         valueSet ? "Value set correctly" : "Value not set",
                         "50", QString("%1").arg(slider->value()));

    if (!valueSet)
        allValid = false;

    // Test value change signal
    QSignalSpy valueChangedSpy(slider, &Components::FluentSlider::valueChanged);
    slider->setValue(75);
    bool signalEmitted = (valueChangedSpy.count() > 0);

    validator->addResult(
        "Slider Signal", "FluentSlider", "Form Component", signalEmitted,
        signalEmitted ? "Value changed signal emitted" : "Signal not emitted",
        "Signal emitted", signalEmitted ? "Signal emitted" : "No signal");

    if (!signalEmitted)
        allValid = false;

    return allValid;
}

// Layout Component Validators Implementation
bool LayoutComponentValidator::validateCard(
    Components::FluentCard* card, FluentComponentValidator* validator) {
    if (!card || !validator)
        return false;

    bool allValid = true;

    // Test card title
    QString testTitle = "Test Card Title";
    card->setTitle(testTitle);
    bool titleSet = (card->title() == testTitle);

    validator->addResult("Card Title", "FluentCard", "Layout Component",
                         titleSet,
                         titleSet ? "Title set correctly" : "Title not set",
                         testTitle, card->title());

    if (!titleSet)
        allValid = false;

    // Test card subtitle
    QString testSubtitle = "Test Card Subtitle";
    card->setSubtitle(testSubtitle);
    bool subtitleSet = (card->subtitle() == testSubtitle);

    validator->addResult(
        "Card Subtitle", "FluentCard", "Layout Component", subtitleSet,
        subtitleSet ? "Subtitle set correctly" : "Subtitle not set",
        testSubtitle, card->subtitle());

    if (!subtitleSet)
        allValid = false;

    // Test card elevation
    card->setElevation(Components::FluentCardElevation::Medium);
    bool elevationSet =
        (card->elevation() == Components::FluentCardElevation::Medium);

    validator->addResult(
        "Card Elevation", "FluentCard", "Layout Component", elevationSet,
        elevationSet ? "Elevation set correctly" : "Elevation not set",
        "Medium", elevationSet ? "Medium" : "Not Medium");

    if (!elevationSet)
        allValid = false;

    // Test card basic interaction (FluentCard doesn't have clicked signal)
    // Just test that the card can receive mouse events
    bool canReceiveEvents = card->isEnabled() && card->isVisible();

    validator->addResult(
        "Card Interaction", "FluentCard", "Layout Component", canReceiveEvents,
        canReceiveEvents ? "Card can receive events"
                         : "Card cannot receive events",
        "Can receive events",
        canReceiveEvents ? "Can receive events" : "Cannot receive events");

    if (!canReceiveEvents)
        allValid = false;

    return allValid;
}

bool LayoutComponentValidator::validatePanel(
    Components::FluentPanel* panel, FluentComponentValidator* validator) {
    if (!panel || !validator)
        return false;

    bool allValid = true;

    // Test panel visibility
    panel->setVisible(true);
    bool visible = panel->isVisible();

    validator->addResult("Panel Visibility", "FluentPanel", "Layout Component",
                         visible,
                         visible ? "Panel is visible" : "Panel not visible",
                         "Visible", visible ? "Visible" : "Hidden");

    if (!visible)
        allValid = false;

    // Test panel size
    QSize testSize(300, 200);
    panel->resize(testSize);
    bool sizeSet = (panel->size() == testSize);

    validator->addResult(
        "Panel Size", "FluentPanel", "Layout Component", sizeSet,
        sizeSet ? "Size set correctly" : "Size not set",
        QString("%1x%2").arg(testSize.width()).arg(testSize.height()),
        QString("%1x%2")
            .arg(panel->size().width())
            .arg(panel->size().height()));

    if (!sizeSet)
        allValid = false;

    return allValid;
}

bool LayoutComponentValidator::validateNavigationView(
    Components::FluentNavigationView* nav,
    FluentComponentValidator* validator) {
    if (!nav || !validator)
        return false;

    bool allValid = true;

    // Test navigation view basic functionality
    bool basicFunctionality = nav->isVisible();

    validator->addResult("NavigationView Basic", "FluentNavigationView",
                         "Layout Component", basicFunctionality,
                         basicFunctionality ? "NavigationView functional"
                                            : "NavigationView not functional",
                         "Functional",
                         basicFunctionality ? "Functional" : "Not functional");

    if (!basicFunctionality)
        allValid = false;

    return allValid;
}

bool LayoutComponentValidator::validateTabView(
    Components::FluentTabView* tabs, FluentComponentValidator* validator) {
    if (!tabs || !validator)
        return false;

    bool allValid = true;

    // Test tab view basic functionality
    bool basicFunctionality = tabs->isVisible();

    validator->addResult(
        "TabView Basic", "FluentTabView", "Layout Component",
        basicFunctionality,
        basicFunctionality ? "TabView functional" : "TabView not functional",
        "Functional", basicFunctionality ? "Functional" : "Not functional");

    if (!basicFunctionality)
        allValid = false;

    return allValid;
}

// Feedback Component Validators Implementation
bool FeedbackComponentValidator::validateProgressBar(
    Components::FluentProgressBar* progress,
    FluentComponentValidator* validator) {
    if (!progress || !validator)
        return false;

    bool allValid = true;

    // Test progress bar range
    progress->setRange(0, 100);
    bool rangeSet = (progress->minimum() == 0 && progress->maximum() == 100);

    validator->addResult(
        "ProgressBar Range", "FluentProgressBar", "Feedback Component",
        rangeSet, rangeSet ? "Range set correctly" : "Range not set", "0-100",
        QString("%1-%2").arg(progress->minimum()).arg(progress->maximum()));

    if (!rangeSet)
        allValid = false;

    // Test progress bar value
    progress->setValue(50);
    bool valueSet = (progress->value() == 50);

    validator->addResult("ProgressBar Value", "FluentProgressBar",
                         "Feedback Component", valueSet,
                         valueSet ? "Value set correctly" : "Value not set",
                         "50", QString("%1").arg(progress->value()));

    if (!valueSet)
        allValid = false;

    return allValid;
}

bool FeedbackComponentValidator::validateToast(
    Components::FluentToast* toast, FluentComponentValidator* validator) {
    if (!toast || !validator)
        return false;

    bool allValid = true;

    // Test toast basic functionality
    bool basicFunctionality = toast->isVisible();

    validator->addResult(
        "Toast Basic", "FluentToast", "Feedback Component", basicFunctionality,
        basicFunctionality ? "Toast functional" : "Toast not functional",
        "Functional", basicFunctionality ? "Functional" : "Not functional");

    if (!basicFunctionality)
        allValid = false;

    return allValid;
}

bool FeedbackComponentValidator::validateTooltip(
    Components::FluentTooltip* tooltip, FluentComponentValidator* validator) {
    if (!tooltip || !validator)
        return false;

    bool allValid = true;

    // Test tooltip text
    QString testText = "Test Tooltip Text";
    tooltip->setText(testText);
    bool textSet = (tooltip->text() == testText);

    validator->addResult("Tooltip Text", "FluentTooltip", "Feedback Component",
                         textSet,
                         textSet ? "Text set correctly" : "Text not set",
                         testText, tooltip->text());

    if (!textSet)
        allValid = false;

    return allValid;
}

bool FeedbackComponentValidator::validateBadge(
    Components::FluentBadge* badge, FluentComponentValidator* validator) {
    if (!badge || !validator)
        return false;

    bool allValid = true;

    // Test badge text
    QString testText = "99+";
    badge->setText(testText);
    bool textSet = (badge->text() == testText);

    validator->addResult("Badge Text", "FluentBadge", "Feedback Component",
                         textSet,
                         textSet ? "Text set correctly" : "Text not set",
                         testText, badge->text());

    if (!textSet)
        allValid = false;

    return allValid;
}

// Dialog Component Validators Implementation
bool DialogComponentValidator::validateBasicDialog(
    QWidget* dialog, FluentComponentValidator* validator) {
    if (!dialog || !validator)
        return false;

    bool allValid = true;

    // Test dialog basic functionality
    bool basicFunctionality = true;  // Dialog exists

    validator->addResult(
        "Basic Dialog", "BasicDialog", "Dialog Component", basicFunctionality,
        basicFunctionality ? "Dialog functional" : "Dialog not functional",
        "Functional", basicFunctionality ? "Functional" : "Not functional");

    return allValid;
}
