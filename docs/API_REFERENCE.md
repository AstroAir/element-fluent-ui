# Element Fluent UI - API Reference

This document provides comprehensive API documentation for all public classes, methods, properties, and signals in Element Fluent UI.

## Table of Contents

1. [Core System](#core-system)
   - [FluentComponent](#fluentcomponent)
   - [FluentTheme](#fluenttheme)
   - [FluentAnimator](#fluentanimator)
2. [Form Components](#form-components)
   - [FluentButton](#fluentbutton)
   - [FluentTextInput](#fluenttextinput)
   - [FluentCheckBox](#fluentcheckbox)
   - [FluentRadioButton](#fluentradiobutton)
   - [FluentComboBox](#fluentcombobox)
   - [FluentSlider](#fluentslider)
   - [FluentToggleSwitch](#fluenttoggleswitch)
3. [Layout Components](#layout-components)
   - [FluentPanel](#fluentpanel)
   - [FluentCard](#fluentcard)
   - [FluentNavigationView](#fluentnavigationview)
   - [FluentTabView](#fluenttabview)
   - [FluentBreadcrumb](#fluentbreadcrumb)
4. [Feedback Components](#feedback-components)
   - [FluentProgressBar](#fluentprogressbar)
   - [FluentLoadingIndicator](#fluentloadingindicator)
   - [FluentTooltip](#fluenttooltip)
   - [FluentNotification](#fluentnotification)
5. [Specialized Components](#specialized-components)
   - [FluentCalendar](#fluentcalendar)
   - [FluentTimePicker](#fluenttimepicker)
   - [FluentColorPicker](#fluentcolorpicker)
   - [FluentTreeView](#fluenttreeview)
   - [FluentRichTextEditor](#fluentrichtexteditor)

---

## Core System

### FluentComponent

Base class for all Fluent UI components providing state management, animations, and theming.

**Header:** `FluentQt/Core/FluentComponent.h`

#### Enums

```cpp
enum class FluentState { 
    Normal, 
    Hovered, 
    Pressed, 
    Disabled, 
    Focused 
};

enum class FluentCornerRadius { 
    None = 0, 
    Small = 4, 
    Medium = 8, 
    Large = 16 
};
```

#### Properties

| Property | Type | Description |
|----------|------|-------------|
| `state` | `FluentState` | Current component state |
| `animated` | `bool` | Whether animations are enabled |
| `cornerRadius` | `int` | Corner radius in pixels |

#### Public Methods

```cpp
// Constructor
explicit FluentComponent(QWidget* parent = nullptr);

// State management
FluentState state() const noexcept;
void setState(FluentState state);

// Animation properties
bool isAnimated() const noexcept;
void setAnimated(bool animated);

// Corner radius
int cornerRadius() const noexcept;
void setCornerRadius(int radius);

// Reactive state management
template<typename T>
void bindProperty(const QString& property, std::function<T()> getter);
```

#### Signals

```cpp
void stateChanged(FluentState state);
void themeChanged();
```

#### Protected Methods

```cpp
virtual void updateStateStyle();
virtual void performStateTransition(FluentState from, FluentState to);
```

---

### FluentTheme

Centralized theme management system providing design tokens, colors, typography, and styling.

**Header:** `FluentQt/Styling/FluentTheme.h`

#### Enums

```cpp
enum class FluentThemeMode { 
    Light, 
    Dark, 
    System 
};

enum class FluentAccentColor {
    Blue,
    Purple, 
    Green,
    Orange,
    Red,
    Yellow,
    Custom
};
```

#### Static Methods

```cpp
static FluentTheme& instance();
```

#### Theme Management

```cpp
// Theme mode
FluentThemeMode mode() const noexcept;
void setMode(FluentThemeMode mode);

// Accent color
FluentAccentColor accentColor() const noexcept;
void setAccentColor(FluentAccentColor color);
void setCustomAccentColor(const QColor& color);

// Palette access
const FluentColorPalette& lightPalette() const noexcept;
const FluentColorPalette& darkPalette() const noexcept;
const FluentColorPalette& currentPalette() const noexcept;
```

#### Color System

```cpp
// Dynamic color access
QColor color(std::string_view colorName) const;
QBrush brush(std::string_view brushName) const;

// Color utilities
QColor lighten(const QColor& color, qreal amount = 0.1) const;
QColor darken(const QColor& color, qreal amount = 0.1) const;
QColor adjustAlpha(const QColor& color, qreal alpha) const;
QColor blend(const QColor& color1, const QColor& color2, qreal ratio = 0.5) const;
```

#### Typography System

```cpp
// Font methods (14 available)
QFont captionFont() const;      // 12px
QFont bodyFont() const;         // 14px  
QFont subtitleFont() const;     // 16px
QFont titleFont() const;        // 20px
QFont displayFont() const;      // 28px
QFont buttonFont() const;       // Optimized for buttons
QFont linkFont() const;         // Underlined for links
QFont codeFont() const;         // Monospace for code
```

#### Design Tokens

```cpp
// Spacing system
int spacing(std::string_view size) const;           // xs, small, medium, large, xl
QMargins margins(std::string_view size) const;      // Component margins
QMargins padding(std::string_view size) const;      // Component padding

// Sizing system  
QSize iconSize(std::string_view size) const;        // Icon sizes
int componentHeight(std::string_view size) const;   // Component heights
int componentWidth(std::string_view size) const;    // Component widths

// Border and elevation
int borderRadius(std::string_view size) const;      // Border radius values
int elevation(std::string_view level) const;        // Shadow elevation
int strokeWidth(std::string_view size) const;       // Border widths
```

#### Accessibility

```cpp
// High contrast mode
bool isHighContrastMode() const;
void setHighContrastMode(bool enabled);

// Contrast checking (WCAG compliance)
qreal contrastRatio(const QColor& foreground, const QColor& background) const;
QColor ensureContrast(const QColor& foreground, const QColor& background, qreal minRatio = 4.5) const;
```

#### Signals

```cpp
void themeChanged();
void modeChanged(FluentThemeMode mode);
void accentColorChanged(FluentAccentColor color);
void highContrastModeChanged(bool enabled);
```

---

### FluentAnimator

Comprehensive animation system with 36 easing functions and 28 animation types.

**Header:** `FluentQt/Animation/FluentAnimator.h`

#### Enums

```cpp
enum class FluentEasing {
    // Basic easing
    Linear,
    QuadIn, QuadOut, QuadInOut,
    CubicIn, CubicOut, CubicInOut,
    QuartIn, QuartOut, QuartInOut,
    QuintIn, QuintOut, QuintInOut,
    
    // Advanced easing
    SineIn, SineOut, SineInOut,
    ExpoIn, ExpoOut, ExpoInOut,
    CircIn, CircOut, CircInOut,
    BounceIn, BounceOut, BounceInOut,
    ElasticIn, ElasticOut, ElasticInOut,
    BackIn, BackOut, BackInOut
};

enum class FluentAnimationType {
    FadeIn, FadeOut,
    SlideUp, SlideDown, SlideLeft, SlideRight,
    ScaleIn, ScaleOut,
    RotateIn, RotateOut,
    // ... 28 total types
};
```

#### Configuration

```cpp
struct FluentAnimationConfig {
    std::chrono::milliseconds duration{300};
    FluentEasing easing{FluentEasing::CubicOut};
    bool enableMicroInteractions{true};
    bool respectReducedMotion{true};
    bool enableHardwareAcceleration{true};
    bool debugMode{false};
};
```

#### Static Animation Methods

```cpp
// Basic animations
static std::unique_ptr<QPropertyAnimation> fadeIn(
    QWidget* target, 
    const FluentAnimationConfig& config = {}
);

static std::unique_ptr<QPropertyAnimation> fadeOut(
    QWidget* target,
    const FluentAnimationConfig& config = {}
);

static std::unique_ptr<QPropertyAnimation> scaleIn(
    QWidget* target,
    const FluentAnimationConfig& config = {}
);

static std::unique_ptr<QPropertyAnimation> scaleOut(
    QWidget* target,
    const FluentAnimationConfig& config = {}
);

// Movement animations
static std::unique_ptr<QPropertyAnimation> slideUp(
    QWidget* target,
    int distance = 50,
    const FluentAnimationConfig& config = {}
);

static std::unique_ptr<QPropertyAnimation> slideDown(
    QWidget* target,
    int distance = 50,
    const FluentAnimationConfig& config = {}
);

static std::unique_ptr<QPropertyAnimation> slideLeft(
    QWidget* target,
    int distance = 50,
    const FluentAnimationConfig& config = {}
);

static std::unique_ptr<QPropertyAnimation> slideRight(
    QWidget* target,
    int distance = 50,
    const FluentAnimationConfig& config = {}
);
```

#### Advanced Animations

```cpp
// Color transition animation
static std::unique_ptr<QPropertyAnimation> colorTransition(
    QObject* target,
    const QByteArray& property,
    const QColor& from,
    const QColor& to,
    const FluentAnimationConfig& config = {}
);

// Morph animation (geometry + opacity)
static std::unique_ptr<QParallelAnimationGroup> morphAnimation(
    QWidget* target,
    const QRect& fromGeometry,
    const QRect& toGeometry,
    const FluentAnimationConfig& config = {}
);

// Reveal animation (Fluent Design signature)
static std::unique_ptr<QSequentialAnimationGroup> revealAnimation(
    QWidget* target,
    const QPoint& center = {},
    const FluentAnimationConfig& config = {}
);

// Connected animations (for navigations)
static std::unique_ptr<QParallelAnimationGroup> connectedAnimation(
    QWidget* from,
    QWidget* to,
    const FluentAnimationConfig& config = {}
);
```

#### Micro-interactions

```cpp
// Hover effects
static std::unique_ptr<QPropertyAnimation> hoverEffect(
    QWidget* target,
    const FluentAnimationConfig& config = {}
);

// Press feedback
static std::unique_ptr<QPropertyAnimation> pressEffect(
    QWidget* target,
    const FluentAnimationConfig& config = {}
);

// Focus indication
static std::unique_ptr<QPropertyAnimation> focusEffect(
    QWidget* target,
    const FluentAnimationConfig& config = {}
);

// Attention effects
static std::unique_ptr<QPropertyAnimation> pulseEffect(
    QWidget* target,
    const FluentAnimationConfig& config = {}
);

static std::unique_ptr<QPropertyAnimation> shakeEffect(
    QWidget* target,
    const FluentAnimationConfig& config = {}
);
```

#### Template Methods

```cpp
// Stagger animations for lists
template<typename Container>
static std::unique_ptr<QSequentialAnimationGroup> staggerAnimation(
    const Container& widgets,
    FluentAnimationType type,
    std::chrono::milliseconds staggerDelay = 50ms,
    const FluentAnimationConfig& config = {}
);
```

---

## Form Components

### FluentButton

Enhanced button component with multiple styles, states, and built-in animations.

**Header:** `FluentQt/Components/FluentButton.h`

#### Enums

```cpp
enum class FluentButtonStyle {
    Default,        // Standard button
    Primary,        // Primary action button
    Accent,         // Accent color button
    Subtle,         // Subtle/ghost button
    Outline,        // Outlined button
    Text,           // Text-only button
    Icon,           // Icon-only button
    Toggle,         // Toggle button
    Split           // Split button with dropdown
};

enum class FluentButtonSize {
    Small,          // 24px height
    Medium,         // 32px height (default)
    Large           // 40px height
};

enum class FluentIconPosition {
    Left,           // Icon on left side
    Right,          // Icon on right side
    Top,            // Icon above text
    Bottom          // Icon below text
};
```

#### Properties

| Property | Type | Description |
|----------|------|-------------|
| `text` | `QString` | Button text content |
| `icon` | `QIcon` | Button icon |
| `buttonStyle` | `FluentButtonStyle` | Visual style of the button |
| `buttonSize` | `FluentButtonSize` | Size of the button |
| `iconPosition` | `FluentIconPosition` | Position of icon relative to text |
| `loading` | `bool` | Whether button shows loading state |
| `checkable` | `bool` | Whether button can be toggled |
| `checked` | `bool` | Current checked state (if checkable) |

#### Constructors

```cpp
explicit FluentButton(QWidget* parent = nullptr);
explicit FluentButton(const QString& text, QWidget* parent = nullptr);
explicit FluentButton(const QIcon& icon, QWidget* parent = nullptr);
explicit FluentButton(const QIcon& icon, const QString& text, QWidget* parent = nullptr);
```

#### Public Methods

```cpp
// Text and icon
QString text() const;
void setText(const QString& text);

QIcon icon() const;
void setIcon(const QIcon& icon);

// Style properties
FluentButtonStyle buttonStyle() const;
void setButtonStyle(FluentButtonStyle style);

FluentButtonSize buttonSize() const;
void setButtonSize(FluentButtonSize size);

FluentIconPosition iconPosition() const;
void setIconPosition(FluentIconPosition position);

// State management
bool isLoading() const;
void setLoading(bool loading);

bool isCheckable() const;
void setCheckable(bool checkable);

bool isChecked() const;
void setChecked(bool checked);

// Convenience static methods
static FluentButton* createPrimaryButton(const QString& text, QWidget* parent = nullptr);
static FluentButton* createAccentButton(const QString& text, QWidget* parent = nullptr);
static FluentButton* createIconButton(const QIcon& icon, QWidget* parent = nullptr);
```

#### Public Slots

```cpp
void animateClick();
void toggle();
```

#### Signals

```cpp
void textChanged(const QString& text);
void iconChanged(const QIcon& icon);
void styleChanged(FluentButtonStyle style);
void sizeChanged(FluentButtonSize size);
void iconPositionChanged(FluentIconPosition position);
void loadingChanged(bool loading);
void checkedChanged(bool checked);
void clicked(bool checked = false);
void pressed();
void released();
void toggled(bool checked);
```

---

### FluentTextInput

Enhanced text input component with validation, states, and animations.

**Header:** `FluentQt/Components/FluentTextInput.h`

#### Enums

```cpp
enum class FluentTextInputType {
    Text,           // Standard text input
    Password,       // Password input with visibility toggle
    Email,          // Email input with validation
    Url,            // URL input with validation
    Number,         // Numeric input
    Phone,          // Phone number input
    Search,         // Search input with search icon
    Multiline       // Multi-line text area
};

enum class FluentTextInputState {
    Normal,         // Default state
    Focused,        // Input has focus
    Error,          // Validation error state
    Warning,        // Warning state
    Success,        // Success/valid state
    Disabled        // Disabled state
};

enum class FluentTextInputValidation {
    None,           // No validation
    Required,       // Required field validation
    Email,          // Email format validation
    Url,            // URL format validation
    Number,         // Numeric validation
    Phone,          // Phone number validation
    Custom          // Custom validation function
};
```

#### Properties

| Property | Type | Description |
|----------|------|-------------|
| `text` | `QString` | Current text content |
| `placeholderText` | `QString` | Placeholder text when empty |
| `labelText` | `QString` | Label text above input |
| `helperText` | `QString` | Helper text below input |
| `errorText` | `QString` | Error message text |
| `inputType` | `FluentTextInputType` | Type of input |
| `inputState` | `FluentTextInputState` | Current visual state |
| `validationType` | `FluentTextInputValidation` | Validation type |
| `maxLength` | `int` | Maximum character length |
| `readOnly` | `bool` | Whether input is read-only |

#### Constructors

```cpp
explicit FluentTextInput(QWidget* parent = nullptr);
explicit FluentTextInput(const QString& placeholderText, QWidget* parent = nullptr);
```

#### Public Methods

```cpp
// Text content
QString text() const;
void setText(const QString& text);

QString placeholderText() const;
void setPlaceholderText(const QString& text);

// Labels and help text
QString labelText() const;
void setLabelText(const QString& text);

QString helperText() const;
void setHelperText(const QString& text);

QString errorText() const;
void setErrorText(const QString& text);

// Input type and state
FluentTextInputType inputType() const;
void setInputType(FluentTextInputType type);

FluentTextInputState inputState() const;
void setInputState(FluentTextInputState state);

// Validation
FluentTextInputValidation validationType() const;
void setValidationType(FluentTextInputValidation type);

void setCustomValidator(std::function<bool(const QString&)> validator);

bool isValid() const;
void validate();
void clearValidation();

// Properties
bool isClearButtonVisible() const;
void setClearButtonVisible(bool visible);

int maxLength() const;
void setMaxLength(int length);

bool isReadOnly() const;
void setReadOnly(bool readOnly);

// Focus management
void setFocus();
void clearFocus();

// Selection
void selectAll();
void selectText(int start, int length);
QString selectedText() const;
```

#### Public Slots

```cpp
void clear();
void undo();
void redo();
void copy();
void cut();
void paste();
```

#### Signals

```cpp
void textChanged(const QString& text);
void textEdited(const QString& text);
void returnPressed();
void editingFinished();
void validationStateChanged(bool isValid);
void inputStateChanged(FluentTextInputState state);
void focusChanged(bool hasFocus);
```

---

### FluentCheckBox

Enhanced checkbox component with animations and tri-state support.

**Header:** `FluentQt/Components/FluentCheckBox.h`

#### Constructors

```cpp
explicit FluentCheckBox(QWidget* parent = nullptr);
explicit FluentCheckBox(const QString& text, QWidget* parent = nullptr);
```

#### Public Methods

```cpp
// Text content
QString text() const;
void setText(const QString& text);

// Check state
Qt::CheckState checkState() const;
void setCheckState(Qt::CheckState state);

bool isChecked() const;
void setChecked(bool checked);

// Tri-state support
bool isTristate() const;
void setTristate(bool tristate);

// Animation
bool isAnimated() const;
void setAnimated(bool animated);
```

#### Signals

```cpp
void stateChanged(Qt::CheckState state);
void toggled(bool checked);
void clicked(bool checked);
```

---

### FluentRadioButton

Radio button component with grouping and animations.

**Header:** `FluentQt/Components/FluentRadioButton.h`

#### Constructors

```cpp
explicit FluentRadioButton(QWidget* parent = nullptr);
explicit FluentRadioButton(const QString& text, QWidget* parent = nullptr);
```

#### Public Methods

```cpp
// Text content
QString text() const;
void setText(const QString& text);

// Check state
bool isChecked() const;
void setChecked(bool checked);

// Group management
QString groupName() const;
void setGroupName(const QString& name);

// Animation
bool isAnimated() const;
void setAnimated(bool animated);
```

#### Signals

```cpp
void toggled(bool checked);
void clicked(bool checked);
```

---

## Layout Components

### FluentPanel

Versatile panel component with multiple types and elevation effects.

**Header:** `FluentQt/Components/FluentPanel.h`

#### Panel Type Enums

```cpp
enum class FluentPanelType {
    Card,           // Elevated card with shadow
    Surface,        // Flat surface with border
    Acrylic,        // Translucent acrylic effect
    Mica,           // Mica material effect
    Sidebar,        // Navigation sidebar panel
    Content,        // Main content area
    Dialog,         // Modal dialog panel
    Flyout          // Popup flyout panel
};

enum class FluentPanelElevation {
    None,           // No elevation
    Low,            // 2px shadow
    Medium,         // 8px shadow
    High            // 16px shadow
};

enum class FluentPanelCornerRadius {
    None,           // No rounded corners
    Small,          // 4px radius
    Medium,         // 8px radius
    Large           // 16px radius
};
```

#### Panel Constructors

```cpp
explicit FluentPanel(QWidget* parent = nullptr);
explicit FluentPanel(FluentPanelType type, QWidget* parent = nullptr);
explicit FluentPanel(const QString& title, QWidget* parent = nullptr);
```

#### Panel Methods

```cpp
// Panel type and appearance
FluentPanelType panelType() const;
void setPanelType(FluentPanelType type);

FluentPanelElevation elevation() const;
void setElevation(FluentPanelElevation elevation);

FluentPanelCornerRadius cornerRadius() const;
void setCornerRadius(FluentPanelCornerRadius radius);

// Colors and styling
QColor backgroundColor() const;
void setBackgroundColor(const QColor& color);

QColor borderColor() const;
void setBorderColor(const QColor& color);

int borderWidth() const;
void setBorderWidth(int width);

QMargins padding() const;
void setPadding(const QMargins& padding);

// Title and collapsing
QString title() const;
void setTitle(const QString& title);

bool isCollapsible() const;
void setCollapsible(bool collapsible);

bool isCollapsed() const;
void setCollapsed(bool collapsed);

// Layout creation helpers
QVBoxLayout* createVerticalLayout();
QHBoxLayout* createHorizontalLayout();
QGridLayout* createGridLayout();
QStackedLayout* createStackedLayout();
```

#### Panel Signals

```cpp
void titleChanged(const QString& title);
void collapsedChanged(bool collapsed);
void clicked();
```

---

### FluentCard

Content card component with header, content, and footer sections.

**Header:** `FluentQt/Components/FluentCard.h`

#### Card Enums

```cpp
enum class FluentCardElevation {
    None,           // Flat card
    Low,            // Subtle shadow
    Medium,         // Standard shadow
    High            // Prominent shadow
};

enum class FluentCardStyle {
    Default,        // Standard card
    Outlined,       // Card with border
    Filled,         // Filled background
    Elevated        // Elevated with shadow
};
```

#### Card Constructors

```cpp
explicit FluentCard(QWidget* parent = nullptr);
explicit FluentCard(const QString& title, QWidget* parent = nullptr);
explicit FluentCard(const QString& title, const QString& subtitle, QWidget* parent = nullptr);
```

#### Card Header Methods

```cpp
// Header properties
QString title() const;
void setTitle(const QString& title);

QString subtitle() const;
void setSubtitle(const QString& subtitle);

QIcon headerIcon() const;
void setHeaderIcon(const QIcon& icon);

bool isHeaderVisible() const;
void setHeaderVisible(bool visible);
```

#### Card Style Methods

```cpp
// Style properties
FluentCardElevation elevation() const;
void setElevation(FluentCardElevation elevation);

FluentCardStyle cardStyle() const;
void setCardStyle(FluentCardStyle style);

// Selection and interaction
bool isSelectable() const;
void setSelectable(bool selectable);

bool isSelected() const;
void setSelected(bool selected);

// Expansion
bool isExpandable() const;
void setExpandable(bool expandable);

bool isExpanded() const;
void setExpanded(bool expanded);
```

#### Card Content Methods

```cpp
// Content management
QWidget* contentWidget() const;
void setContentWidget(QWidget* widget);

QVBoxLayout* createVerticalLayout();
QHBoxLayout* createHorizontalLayout();
QGridLayout* createGridLayout();

// Footer management
bool isFooterVisible() const;
void setFooterVisible(bool visible);

void addFooterAction(QAction* action);
void removeFooterAction(QAction* action);
void clearFooterActions();
```

#### Card Signals

```cpp
void titleChanged(const QString& title);
void subtitleChanged(const QString& subtitle);
void headerIconChanged(const QIcon& icon);
void elevationChanged(FluentCardElevation elevation);
void cardStyleChanged(FluentCardStyle style);
void selectedChanged(bool selected);
void expandedChanged(bool expanded);
void clicked();
void doubleClicked();
```

---

## Feedback Components

### FluentProgressBar

Multi-type progress indicator with animations.

**Header:** `FluentQt/Components/FluentProgressBar.h`

#### Progress Enums

```cpp
enum class FluentProgressType {
    Determinate,    // Shows specific progress percentage
    Indeterminate,  // Shows ongoing activity
    Ring,           // Circular progress indicator
    Linear          // Linear progress bar
};

enum class FluentProgressSize {
    Small,          // 16px height/diameter
    Medium,         // 24px height/diameter
    Large           // 32px height/diameter
};
```

#### Progress Constructors

```cpp
explicit FluentProgressBar(QWidget* parent = nullptr);
explicit FluentProgressBar(FluentProgressType type, QWidget* parent = nullptr);
```

#### Progress Methods

```cpp
// Type and appearance
FluentProgressType type() const;
void setType(FluentProgressType type);

FluentProgressSize size() const;
void setSize(FluentProgressSize size);

// Value and range
int value() const;
void setValue(int value);

int minimum() const;
void setMinimum(int minimum);

int maximum() const;
void setMaximum(int maximum);

void setRange(int minimum, int maximum);

// Appearance
QColor accentColor() const;
void setAccentColor(const QColor& color);

bool showText() const;
void setShowText(bool show);

QString text() const;
void setText(const QString& text);

// Animation control
void start();
void stop();
void pause();
void resume();

bool isAnimated() const;
void setAnimated(bool animated);
```

#### Progress Signals

```cpp
void valueChanged(int value);
void typeChanged(FluentProgressType type);
void sizeChanged(FluentProgressSize size);
void started();
void stopped();
void paused();
void resumed();
```

---

### FluentLoadingIndicator

Animated loading indicators with multiple styles.

**Header:** `FluentQt/Components/FluentLoadingIndicator.h`

#### Loading Enums

```cpp
enum class FluentLoadingType {
    Spinner,        // Rotating spinner
    Dots,           // Animated dots
    Pulse,          // Pulsing circle
    Bars,           // Animated bars
    Ring,           // Ring spinner
    Wave            // Wave animation
};

enum class FluentLoadingSize {
    Small,          // 16px
    Medium,         // 24px
    Large           // 32px
};
```

#### Loading Methods

```cpp
// Type and appearance
FluentLoadingType type() const;
void setType(FluentLoadingType type);

FluentLoadingSize size() const;
void setSize(FluentLoadingSize size);

// Text and styling
QString text() const;
void setText(const QString& text);

QColor color() const;
void setColor(const QColor& color);

// Animation control
void start();
void stop();

bool isRunning() const;

qreal speed() const;
void setSpeed(qreal speed);
```

---

### FluentTooltip

Rich tooltips with smart positioning.

**Header:** `FluentQt/Components/FluentTooltip.h`

#### Tooltip Enums

```cpp
enum class FluentTooltipPosition {
    Auto,           // Smart positioning
    TopLeft,
    TopCenter,
    TopRight,
    BottomLeft,
    BottomCenter,
    BottomRight,
    LeftTop,
    LeftCenter,
    LeftBottom,
    RightTop,
    RightCenter,
    RightBottom
};
```

#### Tooltip Static Methods

```cpp
// Simple tooltip
static void showTooltip(QWidget* widget, const QString& text);
static void hideTooltip(QWidget* widget);
```

#### Tooltip Instance Methods

```cpp
// Content
QString title() const;
void setTitle(const QString& title);

QString body() const;
void setBody(const QString& body);

QIcon icon() const;
void setIcon(const QIcon& icon);

// Positioning
FluentTooltipPosition position() const;
void setPosition(FluentTooltipPosition position);

// Display
void showFor(QWidget* widget);
void hide();

bool isVisible() const;

// Timing
int showDelay() const;
void setShowDelay(int delay);

int hideDelay() const;
void setHideDelay(int delay);
```

---

## Specialized Components

### FluentCalendar

Date picker and calendar component.

**Header:** `FluentQt/Components/FluentCalendar.h`

#### Calendar Methods

```cpp
// Date selection
QDate selectedDate() const;
void setSelectedDate(const QDate& date);

QDate minimumDate() const;
void setMinimumDate(const QDate& date);

QDate maximumDate() const;
void setMaximumDate(const QDate& date);

// Display
int firstDayOfWeek() const;
void setFirstDayOfWeek(int dayOfWeek);

bool isGridVisible() const;
void setGridVisible(bool visible);

// Navigation
void showToday();
void showMonth(int year, int month);
void showPreviousMonth();
void showNextMonth();
void showPreviousYear();
void showNextYear();
```

#### Calendar Signals

```cpp
void selectionChanged();
void clicked(const QDate& date);
void activated(const QDate& date);
void currentPageChanged(int year, int month);
```

---

### FluentNavigationView

Navigation sidebar component.

**Header:** `FluentQt/Components/FluentNavigationView.h`

#### Navigation Methods

```cpp
// Items management
void addItem(const QString& text);
void addItem(const QIcon& icon, const QString& text);
void addItem(const QString& text, QWidget* widget);
void addItem(const QIcon& icon, const QString& text, QWidget* widget);

void insertItem(int index, const QString& text);
void insertItem(int index, const QIcon& icon, const QString& text);

void removeItem(int index);
void clear();

// Selection
int currentIndex() const;
void setCurrentIndex(int index);

QString currentText() const;
QWidget* currentWidget() const;

// Appearance
bool isCompact() const;
void setCompact(bool compact);

bool isExpandable() const;
void setExpandable(bool expandable);

bool isExpanded() const;
void setExpanded(bool expanded);
```

#### Navigation Signals

```cpp
void currentChanged(int index);
void itemClicked(int index);
void expandedChanged(bool expanded);
```

---

## Usage Examples

### Basic Component Usage

```cpp
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Components/FluentCard.h"

// Create a primary button
auto* button = FluentButton::createPrimaryButton("Click Me");
button->setIcon(QIcon(":/icons/action"));

// Create a validated text input
auto* emailInput = new FluentTextInput("Enter email...");
emailInput->setInputType(FluentTextInputType::Email);
emailInput->setValidationType(FluentTextInputValidation::Email);

// Create a card container
auto* card = new FluentCard("User Information");
card->setElevation(FluentCardElevation::Medium);

auto* layout = card->createVerticalLayout();
layout->addWidget(emailInput);
layout->addWidget(button);
```

### Animation Integration

```cpp
#include "FluentQt/Animation/FluentAnimator.h"

auto* animator = new FluentAnimator(this);

// Animate button click
connect(button, &FluentButton::clicked, [=]() {
    auto* animation = animator->pressEffect(button);
    animation->start();
});

// Animate form validation
connect(emailInput, &FluentTextInput::validationStateChanged, [=](bool valid) {
    if (!valid) {
        auto* shake = animator->shakeEffect(emailInput);
        shake->start();
    }
});
```

### Theme Integration

```cpp
#include "FluentQt/Styling/FluentTheme.h"

auto& theme = FluentTheme::instance();

// Apply theme colors
button->setStyleSheet(QString("background-color: %1").arg(theme.color("accent").name()));

// Use theme fonts
emailInput->setFont(theme.bodyFont());

// Respond to theme changes
connect(&theme, &FluentTheme::themeChanged, [=]() {
    updateComponentStyles();
});
```

---

## Error Handling

### Validation Errors

```cpp
// Handle validation errors
connect(textInput, &FluentTextInput::validationStateChanged, [=](bool valid) {
    if (!valid) {
        textInput->setInputState(FluentTextInputState::Error);
        textInput->setErrorText("Please enter a valid email address");
    } else {
        textInput->setInputState(FluentTextInputState::Success);
        textInput->setErrorText("");
    }
});
```

### Animation Errors

```cpp
// Handle animation completion
auto* animation = FluentAnimator::fadeIn(widget);
connect(animation.get(), &QPropertyAnimation::finished, [=]() {
    qDebug() << "Animation completed successfully";
});

connect(animation.get(), QOverload<QAbstractAnimation::State>::of(&QPropertyAnimation::stateChanged),
    [=](QAbstractAnimation::State newState) {
        if (newState == QAbstractAnimation::Stopped) {
            // Handle animation stop
        }
    });
```

---

*This API reference covers the core public interface of Element Fluent UI. For implementation details and advanced usage, see the individual component documentation and source code.*
