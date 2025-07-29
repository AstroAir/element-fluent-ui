# FluentCard Component

The `FluentCard` component is a versatile container that implements Microsoft's Fluent Design card pattern. It provides elevation, shadows, and structured content areas for organizing information in an appealing and accessible way.

## Overview

FluentCard offers:
- **Structured layout** with header, content, and footer sections
- **Elevation and shadows** following Fluent Design principles
- **Interactive states** with hover and click animations
- **Flexible content** support for any Qt widgets
- **Theme integration** with automatic color adaptation
- **Accessibility support** with proper ARIA roles

## Basic Usage

### Simple Card

```cpp
#include "FluentQt/Components/FluentCard.h"

// Create a basic card
auto* card = new FluentCard("Card Title");
card->setContent("This is the card content area.");

// Add to layout
layout->addWidget(card);
```

### Card with Custom Content

```cpp
auto* card = new FluentCard("User Profile");

// Create custom content widget
auto* contentWidget = new QWidget;
auto* contentLayout = new QVBoxLayout(contentWidget);

auto* nameLabel = new QLabel("John Doe");
nameLabel->setFont(QFont("Segoe UI", 14, QFont::Bold));

auto* emailLabel = new QLabel("john.doe@example.com");
emailLabel->setStyleSheet("color: #666;");

auto* editButton = new FluentButton("Edit Profile");
editButton->setButtonStyle(FluentButtonStyle::Primary);

contentLayout->addWidget(nameLabel);
contentLayout->addWidget(emailLabel);
contentLayout->addWidget(editButton);

// Set the custom content
card->setContentWidget(contentWidget);
```

## Card Structure

### Header, Content, and Footer

```cpp
auto* card = new FluentCard();

// Header with title and subtitle
card->setTitle("Project Status");
card->setSubtitle("Last updated 2 hours ago");

// Header actions (optional)
auto* headerActions = new QWidget;
auto* actionsLayout = new QHBoxLayout(headerActions);
actionsLayout->addWidget(new FluentButton("Refresh"));
actionsLayout->addWidget(new FluentButton("Settings"));
card->setHeaderActions(headerActions);

// Main content
auto* contentWidget = new QWidget;
auto* contentLayout = new QVBoxLayout(contentWidget);

auto* progressBar = new FluentProgressBar;
progressBar->setValue(75);
progressBar->setFormat("75% Complete");

auto* statusLabel = new QLabel("3 of 4 tasks completed");

contentLayout->addWidget(progressBar);
contentLayout->addWidget(statusLabel);
card->setContentWidget(contentWidget);

// Footer with actions
auto* footerWidget = new QWidget;
auto* footerLayout = new QHBoxLayout(footerWidget);
footerLayout->addStretch();
footerLayout->addWidget(new FluentButton("View Details"));
footerLayout->addWidget(FluentButton::createPrimaryButton("Continue"));
card->setFooterWidget(footerWidget);
```

## Card Types and Styling

### Elevation Levels

```cpp
auto* card = new FluentCard("Elevated Card");

// Set elevation level (0-5)
card->setElevation(0);  // Flat, no shadow
card->setElevation(1);  // Subtle shadow
card->setElevation(2);  // Default elevation
card->setElevation(3);  // Medium elevation
card->setElevation(4);  // High elevation
card->setElevation(5);  // Maximum elevation
```

### Interactive Cards

```cpp
auto* clickableCard = new FluentCard("Clickable Card");
clickableCard->setClickable(true);  // Enable click interactions

// Handle card clicks
connect(clickableCard, &FluentCard::clicked, [=]() {
    qDebug() << "Card was clicked!";
    // Navigate to detail view, expand content, etc.
});

// Hover effects are automatically enabled for clickable cards
connect(clickableCard, &FluentCard::hovered, [=](bool isHovered) {
    if (isHovered) {
        // Custom hover animation
        auto* animator = new FluentAnimator(this);
        animator->scaleEffect(clickableCard, 1.02, 200);
    }
});
```

### Card Variants

```cpp
// Outlined card (border instead of shadow)
auto* outlinedCard = new FluentCard("Outlined Card");
outlinedCard->setVariant(FluentCardVariant::Outlined);

// Filled card (background color)
auto* filledCard = new FluentCard("Filled Card");
filledCard->setVariant(FluentCardVariant::Filled);

// Subtle card (minimal styling)
auto* subtleCard = new FluentCard("Subtle Card");
subtleCard->setVariant(FluentCardVariant::Subtle);
```

## Animations

FluentCard includes smooth animations for state transitions:

```cpp
auto* card = new FluentCard("Animated Card");

// Enable/disable animations
card->setAnimated(true);   // Default: enabled

// Custom entrance animation
auto* animator = new FluentAnimator(this);
animator->slideInFromBottom(card, 300);

// Animate content changes
connect(updateButton, &FluentButton::clicked, [=]() {
    // Fade out old content
    auto* fadeOut = animator->fadeOut(card->contentWidget(), 150);
    
    connect(fadeOut, &QPropertyAnimation::finished, [=]() {
        // Update content
        updateCardContent(card);
        
        // Fade in new content
        animator->fadeIn(card->contentWidget(), 150);
    });
    
    fadeOut->start();
});
```

## Layout Patterns

### Card Grid Layout

```cpp
auto* gridWidget = new QWidget;
auto* gridLayout = new QGridLayout(gridWidget);
gridLayout->setSpacing(16);

// Create a grid of cards
for (int row = 0; row < 3; ++row) {
    for (int col = 0; col < 4; ++col) {
        auto* card = new FluentCard(QString("Card %1,%2").arg(row).arg(col));
        card->setContent(QString("Content for card at position %1,%2").arg(row).arg(col));
        card->setFixedSize(200, 150);
        
        gridLayout->addWidget(card, row, col);
    }
}
```

### Card List Layout

```cpp
auto* scrollArea = new QScrollArea;
auto* listWidget = new QWidget;
auto* listLayout = new QVBoxLayout(listWidget);

// Create a vertical list of cards
QStringList items = {"Task 1", "Task 2", "Task 3", "Task 4", "Task 5"};
for (const QString& item : items) {
    auto* card = new FluentCard(item);
    
    // Add content specific to each item
    auto* contentWidget = new QWidget;
    auto* contentLayout = new QHBoxLayout(contentWidget);
    
    auto* descriptionLabel = new QLabel(QString("Description for %1").arg(item));
    auto* actionButton = new FluentButton("Action");
    
    contentLayout->addWidget(descriptionLabel);
    contentLayout->addStretch();
    contentLayout->addWidget(actionButton);
    
    card->setContentWidget(contentWidget);
    card->setClickable(true);
    
    listLayout->addWidget(card);
}

listLayout->addStretch();
scrollArea->setWidget(listWidget);
scrollArea->setWidgetResizable(true);
```

## Accessibility

FluentCard provides comprehensive accessibility support:

```cpp
auto* card = new FluentCard("Accessible Card");

// Accessible properties (automatically set)
card->setAccessibleName("User profile card");
card->setAccessibleDescription("Contains user information and profile actions");

// For clickable cards
if (card->isClickable()) {
    card->setAccessibleRole(QAccessible::Button);
    card->setAccessibleDescription("Click to view detailed user profile");
}

// Keyboard navigation (automatic for clickable cards)
// - Tab to focus the card
// - Enter/Space to activate
// - Arrow keys for navigation between cards
```

## Theming Integration

FluentCard automatically adapts to theme changes:

```cpp
#include "FluentQt/Styling/FluentTheme.h"

auto* card = new FluentCard("Themed Card");

// Card automatically uses theme colors
auto& theme = FluentTheme::instance();

// Custom theme integration (if needed)
connect(&theme, &FluentTheme::themeChanged, [=]() {
    // Update custom styling based on theme
    if (theme.isDarkMode()) {
        card->setStyleSheet("FluentCard { border: 1px solid #333; }");
    } else {
        card->setStyleSheet("FluentCard { border: 1px solid #e0e0e0; }");
    }
});
```

## Advanced Usage

### Dynamic Content Updates

```cpp
auto* newsCard = new FluentCard("Latest News");

// Function to update card content
auto updateNews = [=](const QJsonObject& newsData) {
    auto* contentWidget = new QWidget;
    auto* layout = new QVBoxLayout(contentWidget);
    
    // Update title and subtitle
    newsCard->setTitle(newsData["title"].toString());
    newsCard->setSubtitle(newsData["publishedAt"].toString());
    
    // Update content
    auto* summaryLabel = new QLabel(newsData["summary"].toString());
    summaryLabel->setWordWrap(true);
    
    auto* readMoreButton = new FluentButton("Read More");
    connect(readMoreButton, &FluentButton::clicked, [=]() {
        QDesktopServices::openUrl(QUrl(newsData["url"].toString()));
    });
    
    layout->addWidget(summaryLabel);
    layout->addWidget(readMoreButton);
    
    // Animate content change
    auto* animator = new FluentAnimator(this);
    auto* fadeOut = animator->fadeOut(newsCard, 200);
    
    connect(fadeOut, &QPropertyAnimation::finished, [=]() {
        newsCard->setContentWidget(contentWidget);
        animator->fadeIn(newsCard, 200);
    });
    
    fadeOut->start();
};

// Update news every 5 minutes
auto* timer = new QTimer(this);
connect(timer, &QTimer::timeout, [=]() {
    // Fetch latest news and update
    fetchLatestNews(updateNews);
});
timer->start(300000); // 5 minutes
```

### Card with Loading State

```cpp
auto* dataCard = new FluentCard("Data Overview");

// Show loading state
auto showLoading = [=]() {
    auto* loadingWidget = new QWidget;
    auto* layout = new QVBoxLayout(loadingWidget);
    
    auto* loadingIndicator = new FluentLoadingIndicator;
    auto* loadingLabel = new QLabel("Loading data...");
    loadingLabel->setAlignment(Qt::AlignCenter);
    
    layout->addWidget(loadingIndicator);
    layout->addWidget(loadingLabel);
    
    dataCard->setContentWidget(loadingWidget);
};

// Show data
auto showData = [=](const QJsonObject& data) {
    auto* dataWidget = new QWidget;
    auto* layout = new QVBoxLayout(dataWidget);
    
    // Create data visualization
    auto* chartWidget = new FluentChartWidget;
    chartWidget->setData(data);
    
    layout->addWidget(chartWidget);
    
    dataCard->setContentWidget(dataWidget);
};

// Load data asynchronously
showLoading();
loadDataAsync([=](const QJsonObject& data) {
    showData(data);
});
```

## Best Practices

### Card Design Guidelines
1. **Keep content focused** - Each card should represent a single concept or task
2. **Use consistent sizing** - Maintain uniform card dimensions in grids
3. **Provide clear actions** - Make primary actions obvious and accessible
4. **Consider information hierarchy** - Use title, subtitle, and content appropriately
5. **Optimize for scanning** - Users should quickly understand card content

### Performance Considerations
- Use appropriate elevation levels (higher elevation = more rendering cost)
- Limit the number of animated cards visible simultaneously
- Consider virtualization for large lists of cards
- Cache complex content widgets when possible

### Accessibility Guidelines
- Provide meaningful titles and descriptions
- Ensure sufficient color contrast
- Support keyboard navigation
- Test with screen readers
- Use semantic markup for card content

## Common Patterns

### Dashboard Cards

```cpp
// Metric card pattern
auto createMetricCard = [](const QString& title, const QString& value, const QString& change) {
    auto* card = new FluentCard(title);
    
    auto* contentWidget = new QWidget;
    auto* layout = new QVBoxLayout(contentWidget);
    
    auto* valueLabel = new QLabel(value);
    valueLabel->setFont(QFont("Segoe UI", 24, QFont::Bold));
    
    auto* changeLabel = new QLabel(change);
    changeLabel->setStyleSheet(change.startsWith("+") ? "color: green;" : "color: red;");
    
    layout->addWidget(valueLabel);
    layout->addWidget(changeLabel);
    
    card->setContentWidget(contentWidget);
    card->setFixedSize(200, 120);
    
    return card;
};

// Usage
auto* salesCard = createMetricCard("Sales", "$12,345", "+5.2%");
auto* usersCard = createMetricCard("Users", "1,234", "+12.1%");
auto* revenueCard = createMetricCard("Revenue", "$45,678", "-2.3%");
```

## Signals Reference

```cpp
// Interaction signals
void clicked();
void doubleClicked();
void hovered(bool isHovered);

// Content change signals
void titleChanged(const QString& title);
void contentChanged();
```

## Standalone Examples Collection

### Example 1: Social Media Feed Interface

```cpp
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentTextInput.h"

struct PostData {
    QString author;
    QString content;
    QString timestamp;
    int likes;
    int comments;
    QString avatarPath;
};

class SocialFeedWidget : public QWidget {
    Q_OBJECT
public:
    SocialFeedWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        loadSamplePosts();
        connectSignals();
    }

private slots:
    void createNewPost() {
        QString content = m_postInput->text().trimmed();
        if (content.isEmpty()) return;

        PostData newPost;
        newPost.author = "You";
        newPost.content = content;
        newPost.timestamp = "Just now";
        newPost.likes = 0;
        newPost.comments = 0;
        newPost.avatarPath = ":/avatars/user.png";

        auto* postCard = createPostCard(newPost);
        m_feedLayout->insertWidget(1, postCard); // Insert after compose card

        m_postInput->clear();
        m_postButton->setText("✓ Posted!");
        m_postButton->setButtonStyle(FluentButtonStyle::Accent);

        QTimer::singleShot(2000, [this]() {
            m_postButton->setText("Post");
            m_postButton->setButtonStyle(FluentButtonStyle::Primary);
        });
    }

    void likePost(FluentCard* card, QLabel* likeLabel, int* likeCount) {
        (*likeCount)++;
        likeLabel->setText(QString("❤ %1").arg(*likeCount));

        // Add animation effect
        card->setStyleSheet("FluentCard { border: 2px solid #e74c3c; }");
        QTimer::singleShot(300, [card]() {
            card->setStyleSheet("");
        });
    }

    void sharePost(const QString& content) {
        // Simulate sharing functionality
        qDebug() << "Sharing post:" << content;
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);

        // Header
        auto* headerCard = new FluentCard();
        headerCard->setCardStyle(FluentCardStyle::Elevated);
        headerCard->setElevation(FluentCardElevation::Low);

        auto* headerWidget = new QWidget;
        auto* headerLayout = new QHBoxLayout(headerWidget);

        auto* titleLabel = new QLabel("Social Feed");
        titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");

        auto* refreshButton = new FluentButton("Refresh");
        refreshButton->setButtonStyle(FluentButtonStyle::Subtle);

        headerLayout->addWidget(titleLabel);
        headerLayout->addStretch();
        headerLayout->addWidget(refreshButton);

        headerCard->setContentWidget(headerWidget);

        // Compose post card
        auto* composeCard = createComposeCard();

        // Feed scroll area
        auto* scrollArea = new QScrollArea;
        auto* feedWidget = new QWidget;
        m_feedLayout = new QVBoxLayout(feedWidget);
        m_feedLayout->setSpacing(10);

        scrollArea->setWidget(feedWidget);
        scrollArea->setWidgetResizable(true);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        layout->addWidget(headerCard);
        layout->addWidget(composeCard);
        layout->addWidget(scrollArea);
    }

    FluentCard* createComposeCard() {
        auto* card = new FluentCard("What's on your mind?");
        card->setCardStyle(FluentCardStyle::Outlined);
        card->setElevation(FluentCardElevation::Low);

        auto* contentWidget = new QWidget;
        auto* layout = new QVBoxLayout(contentWidget);

        m_postInput = new FluentTextInput();
        m_postInput->setPlaceholderText("Share your thoughts...");
        m_postInput->setInputType(FluentTextInputType::Multiline);
        m_postInput->setMaximumHeight(100);

        auto* buttonLayout = new QHBoxLayout;

        auto* attachButton = new FluentButton("📎 Attach");
        attachButton->setButtonStyle(FluentButtonStyle::Subtle);
        attachButton->setButtonSize(FluentButtonSize::Small);

        auto* emojiButton = new FluentButton("😊 Emoji");
        emojiButton->setButtonStyle(FluentButtonStyle::Subtle);
        emojiButton->setButtonSize(FluentButtonSize::Small);

        m_postButton = new FluentButton("Post");
        m_postButton->setButtonStyle(FluentButtonStyle::Primary);
        m_postButton->setButtonSize(FluentButtonSize::Small);

        buttonLayout->addWidget(attachButton);
        buttonLayout->addWidget(emojiButton);
        buttonLayout->addStretch();
        buttonLayout->addWidget(m_postButton);

        layout->addWidget(m_postInput);
        layout->addLayout(buttonLayout);

        card->setContentWidget(contentWidget);
        return card;
    }

    FluentCard* createPostCard(const PostData& post) {
        auto* card = new FluentCard();
        card->setCardStyle(FluentCardStyle::Default);
        card->setElevation(FluentCardElevation::Low);
        card->setSelectable(true);

        auto* contentWidget = new QWidget;
        auto* layout = new QVBoxLayout(contentWidget);

        // Post header
        auto* headerLayout = new QHBoxLayout;

        auto* avatarLabel = new QLabel;
        avatarLabel->setFixedSize(40, 40);
        avatarLabel->setStyleSheet("border-radius: 20px; background-color: #0078d4;");
        avatarLabel->setAlignment(Qt::AlignCenter);
        avatarLabel->setText(post.author.left(1).toUpper());
        avatarLabel->setStyleSheet(avatarLabel->styleSheet() + " color: white; font-weight: bold;");

        auto* authorInfoLayout = new QVBoxLayout;
        auto* authorLabel = new QLabel(post.author);
        authorLabel->setStyleSheet("font-weight: bold; font-size: 14px;");

        auto* timestampLabel = new QLabel(post.timestamp);
        timestampLabel->setStyleSheet("color: gray; font-size: 12px;");

        authorInfoLayout->addWidget(authorLabel);
        authorInfoLayout->addWidget(timestampLabel);
        authorInfoLayout->setSpacing(2);

        auto* moreButton = new FluentButton("⋯");
        moreButton->setButtonStyle(FluentButtonStyle::Subtle);
        moreButton->setButtonSize(FluentButtonSize::Small);
        moreButton->setFixedSize(30, 30);

        headerLayout->addWidget(avatarLabel);
        headerLayout->addLayout(authorInfoLayout);
        headerLayout->addStretch();
        headerLayout->addWidget(moreButton);

        // Post content
        auto* contentLabel = new QLabel(post.content);
        contentLabel->setWordWrap(true);
        contentLabel->setStyleSheet("font-size: 14px; margin: 10px 0;");

        // Post actions
        auto* actionsLayout = new QHBoxLayout;

        auto* likeButton = new FluentButton(QString("❤ %1").arg(post.likes));
        likeButton->setButtonStyle(FluentButtonStyle::Subtle);
        likeButton->setButtonSize(FluentButtonSize::Small);

        auto* commentButton = new FluentButton(QString("💬 %1").arg(post.comments));
        commentButton->setButtonStyle(FluentButtonStyle::Subtle);
        commentButton->setButtonSize(FluentButtonSize::Small);

        auto* shareButton = new FluentButton("🔗 Share");
        shareButton->setButtonStyle(FluentButtonStyle::Subtle);
        shareButton->setButtonSize(FluentButtonSize::Small);

        // Store like count for this post
        int* likeCount = new int(post.likes);
        auto* likeLabel = qobject_cast<QLabel*>(likeButton);

        connect(likeButton, &FluentButton::clicked, [this, card, likeButton, likeCount]() {
            (*likeCount)++;
            likeButton->setText(QString("❤ %1").arg(*likeCount));

            // Visual feedback
            card->setStyleSheet("FluentCard { border: 2px solid #e74c3c; }");
            QTimer::singleShot(300, [card]() {
                card->setStyleSheet("");
            });
        });

        connect(shareButton, &FluentButton::clicked, [this, post]() {
            sharePost(post.content);
        });

        actionsLayout->addWidget(likeButton);
        actionsLayout->addWidget(commentButton);
        actionsLayout->addWidget(shareButton);
        actionsLayout->addStretch();

        layout->addLayout(headerLayout);
        layout->addWidget(contentLabel);
        layout->addLayout(actionsLayout);

        card->setContentWidget(contentWidget);
        return card;
    }

    void loadSamplePosts() {
        QList<PostData> samplePosts = {
            {"Alice Johnson", "Just finished an amazing book! 📚 'The Design of Everyday Things' by Don Norman. Highly recommend it to anyone interested in UX design.", "2 hours ago", 15, 3, ""},
            {"Bob Smith", "Beautiful sunset today! 🌅 Sometimes you need to stop and appreciate the simple things in life.", "4 hours ago", 23, 7, ""},
            {"Carol Davis", "Excited to announce that I'll be speaking at the upcoming Tech Conference! Looking forward to sharing insights about modern UI frameworks.", "6 hours ago", 42, 12, ""},
            {"David Wilson", "Coffee and code - the perfect combination for a productive morning! ☕💻 Working on some exciting new features.", "8 hours ago", 18, 5, ""}
        };

        for (const auto& post : samplePosts) {
            auto* postCard = createPostCard(post);
            m_feedLayout->addWidget(postCard);
        }

        m_feedLayout->addStretch();
    }

    void connectSignals() {
        connect(m_postButton, &FluentButton::clicked, this, &SocialFeedWidget::createNewPost);

        // Enable posting when text is entered
        connect(m_postInput, &FluentTextInput::textChanged, [this](const QString& text) {
            m_postButton->setEnabled(!text.trimmed().isEmpty());
        });

        m_postButton->setEnabled(false);
    }

    QVBoxLayout* m_feedLayout;
    FluentTextInput* m_postInput;
    FluentButton* m_postButton;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    SocialFeedWidget feed;
    feed.resize(600, 800);
    feed.show();

    return app.exec();
}

#include "social_feed.moc"
```

## See Also

- [FluentPanel](FluentPanel.md) - For general container components
- [FluentButton](FluentButton.md) - For card action buttons
- [FluentNavigationView](FluentNavigationView.md) - For navigation with cards
- [API Reference - FluentCard](../API_REFERENCE.md#fluentcard) - Complete API documentation
