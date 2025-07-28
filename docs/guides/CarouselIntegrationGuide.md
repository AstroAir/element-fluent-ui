# Carousel Integration Guide

## Overview

This guide provides comprehensive instructions for integrating FluentCarousel components into your Element Fluent UI applications, including setup, configuration, styling, and best practices.

## Quick Start

### 1. Include Headers

```cpp
#include "FluentQt/Components/FluentCarousel.h"
#include "FluentQt/Components/FluentBasicCarousel.h"
#include "FluentQt/Components/FluentAutoCarousel.h"
#include "FluentQt/Components/FluentIndicatorCarousel.h"
#include "FluentQt/Components/FluentTouchCarousel.h"
```

### 2. Basic Integration

```cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QWidget>
#include "FluentQt/Components/FluentCarousel.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    QMainWindow window;
    auto* centralWidget = new QWidget(&window);
    auto* layout = new QVBoxLayout(centralWidget);
    
    // Create carousel
    auto* carousel = new FluentCarousel(centralWidget);
    
    // Add some content
    for (int i = 0; i < 5; ++i) {
        auto* item = new QLabel(QString("Slide %1").arg(i + 1));
        item->setAlignment(Qt::AlignCenter);
        item->setStyleSheet("background-color: lightblue; padding: 40px; margin: 10px;");
        carousel->addItem(item);
    }
    
    layout->addWidget(carousel);
    window.setCentralWidget(centralWidget);
    window.resize(600, 400);
    window.show();
    
    return app.exec();
}
```

## Project Setup

### CMake Integration

Add to your `CMakeLists.txt`:

```cmake
# Find FluentQt
find_package(FluentQt REQUIRED COMPONENTS Core Components)

# Link against FluentQt
target_link_libraries(your_target
    FluentQt::Core
    FluentQt::Components
)

# Include directories
target_include_directories(your_target PRIVATE
    ${FluentQt_INCLUDE_DIRS}
)
```

### qmake Integration

Add to your `.pro` file:

```pro
# Include FluentQt
include($$PWD/path/to/fluentqt/fluentqt.pri)

# Or if installed system-wide
CONFIG += fluentqt
QT += fluentqt-core fluentqt-components

# Headers
INCLUDEPATH += $$PWD/path/to/fluentqt/include
```

## Architecture Integration

### MVC Pattern Integration

```cpp
// Model
class CarouselItemModel : public QAbstractListModel {
    Q_OBJECT

public:
    struct CarouselItem {
        QString title;
        QString description;
        QPixmap image;
        QVariant data;
    };

    void addItem(const CarouselItem& item) {
        beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
        m_items.append(item);
        endInsertRows();
    }

    // QAbstractListModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override {
        Q_UNUSED(parent)
        return m_items.size();
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= m_items.size())
            return QVariant();

        const CarouselItem& item = m_items[index.row()];
        switch (role) {
        case Qt::DisplayRole:
            return item.title;
        case Qt::DecorationRole:
            return item.image;
        case Qt::UserRole:
            return item.description;
        case Qt::UserRole + 1:
            return item.data;
        }
        return QVariant();
    }

private:
    QList<CarouselItem> m_items;
};

// View
class ModelDrivenCarousel : public FluentCarousel {
    Q_OBJECT

public:
    explicit ModelDrivenCarousel(QWidget* parent = nullptr)
        : FluentCarousel(parent), m_model(nullptr) {}

    void setModel(CarouselItemModel* model) {
        if (m_model) {
            disconnect(m_model, nullptr, this, nullptr);
        }

        m_model = model;
        if (m_model) {
            connect(m_model, &QAbstractItemModel::rowsInserted,
                    this, &ModelDrivenCarousel::onRowsInserted);
            connect(m_model, &QAbstractItemModel::rowsRemoved,
                    this, &ModelDrivenCarousel::onRowsRemoved);
            connect(m_model, &QAbstractItemModel::dataChanged,
                    this, &ModelDrivenCarousel::onDataChanged);
            
            rebuildItems();
        }
    }

private slots:
    void onRowsInserted(const QModelIndex& parent, int first, int last) {
        Q_UNUSED(parent)
        for (int i = first; i <= last; ++i) {
            insertItemFromModel(i);
        }
    }

    void onRowsRemoved(const QModelIndex& parent, int first, int last) {
        Q_UNUSED(parent)
        for (int i = last; i >= first; --i) {
            removeItemAt(i);
        }
    }

    void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) {
        for (int i = topLeft.row(); i <= bottomRight.row(); ++i) {
            updateItemFromModel(i);
        }
    }

private:
    void rebuildItems() {
        clear();
        if (!m_model) return;

        for (int i = 0; i < m_model->rowCount(); ++i) {
            insertItemFromModel(i);
        }
    }

    void insertItemFromModel(int index) {
        if (!m_model) return;

        QModelIndex modelIndex = m_model->index(index);
        QString title = m_model->data(modelIndex, Qt::DisplayRole).toString();
        QString description = m_model->data(modelIndex, Qt::UserRole).toString();
        QPixmap image = m_model->data(modelIndex, Qt::DecorationRole).value<QPixmap>();

        auto* widget = createItemWidget(title, description, image);
        insertItem(index, widget);
    }

    void updateItemFromModel(int index) {
        // Update existing item with new model data
        if (auto* widget = itemAt(index)) {
            // Update widget content based on model data
        }
    }

    QWidget* createItemWidget(const QString& title, const QString& description, const QPixmap& image) {
        auto* widget = new QWidget();
        auto* layout = new QVBoxLayout(widget);

        if (!image.isNull()) {
            auto* imageLabel = new QLabel();
            imageLabel->setPixmap(image.scaled(200, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            layout->addWidget(imageLabel);
        }

        auto* titleLabel = new QLabel(title);
        titleLabel->setStyleSheet("font-weight: bold; font-size: 16px;");
        layout->addWidget(titleLabel);

        if (!description.isEmpty()) {
            auto* descLabel = new QLabel(description);
            descLabel->setWordWrap(true);
            layout->addWidget(descLabel);
        }

        return widget;
    }

    CarouselItemModel* m_model;
};
```

### Signal-Slot Integration

```cpp
class CarouselController : public QObject {
    Q_OBJECT

public:
    explicit CarouselController(FluentCarousel* carousel, QObject* parent = nullptr)
        : QObject(parent), m_carousel(carousel) {
        
        // Connect carousel signals to controller slots
        connect(m_carousel, &FluentCarousel::currentIndexChanged,
                this, &CarouselController::onCurrentIndexChanged);
        connect(m_carousel, &FluentCarousel::itemChanged,
                this, &CarouselController::onItemChanged);
        connect(m_carousel, &FluentCarousel::transitionStarted,
                this, &CarouselController::onTransitionStarted);
        connect(m_carousel, &FluentCarousel::transitionFinished,
                this, &CarouselController::onTransitionFinished);
    }

public slots:
    void navigateToItem(int index) {
        if (m_carousel) {
            m_carousel->goToIndex(index);
        }
    }

    void startSlideshow() {
        if (m_carousel) {
            m_carousel->setAutoPlayEnabled(true);
            m_carousel->startAutoPlay();
        }
    }

    void stopSlideshow() {
        if (m_carousel) {
            m_carousel->stopAutoPlay();
        }
    }

signals:
    void itemSelected(int index);
    void slideshowStarted();
    void slideshowStopped();

private slots:
    void onCurrentIndexChanged(int index) {
        emit itemSelected(index);
        
        // Update external UI elements
        updateNavigationState(index);
        updateStatusBar(index);
    }

    void onItemChanged(QWidget* current, QWidget* previous) {
        Q_UNUSED(previous)
        
        // Perform actions when item changes
        if (current) {
            // Focus management, analytics, etc.
        }
    }

    void onTransitionStarted(FluentCarouselTransition transition) {
        // Show loading indicator, disable controls, etc.
        setControlsEnabled(false);
    }

    void onTransitionFinished() {
        // Hide loading indicator, enable controls, etc.
        setControlsEnabled(true);
    }

private:
    void updateNavigationState(int index) {
        // Update external navigation controls
        bool hasNext = (index < m_carousel->itemCount() - 1) || m_carousel->wrapAround();
        bool hasPrevious = (index > 0) || m_carousel->wrapAround();
        
        // Emit signals or update UI directly
    }

    void updateStatusBar(int index) {
        QString status = QString("Item %1 of %2").arg(index + 1).arg(m_carousel->itemCount());
        // Update status bar or other UI elements
    }

    void setControlsEnabled(bool enabled) {
        // Enable/disable external controls during transitions
    }

    FluentCarousel* m_carousel;
};
```

## Styling Integration

### Theme Integration

```cpp
class ThemedCarousel : public FluentCarousel {
    Q_OBJECT

public:
    explicit ThemedCarousel(QWidget* parent = nullptr)
        : FluentCarousel(parent) {
        
        // Connect to theme changes
        connect(&FluentTheme::instance(), &FluentTheme::themeChanged,
                this, &ThemedCarousel::updateTheme);
        
        // Apply initial theme
        updateTheme();
    }

private slots:
    void updateTheme() {
        auto& theme = FluentTheme::instance();
        
        // Update carousel styling based on current theme
        QString styleSheet = QString(
            "FluentCarousel {"
            "    background-color: %1;"
            "    border: 1px solid %2;"
            "    border-radius: %3px;"
            "}"
        ).arg(theme.color("surface").name())
         .arg(theme.color("neutralQuaternary").name())
         .arg(theme.borderRadius("medium"));
        
        setStyleSheet(styleSheet);
        
        // Update configuration for theme
        FluentCarouselConfig config = configuration();
        config.transitionDuration = theme.isDarkMode() ? 400 : 300;
        setConfiguration(config);
    }
};
```

### Custom Styling

```cpp
class CustomStyledCarousel : public FluentCarousel {
    Q_OBJECT

public:
    explicit CustomStyledCarousel(QWidget* parent = nullptr)
        : FluentCarousel(parent) {
        
        // Apply custom styling
        applyCustomStyle();
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        // Custom painting before base implementation
        QPainter painter(this);
        paintCustomBackground(painter);
        
        // Call base implementation
        FluentCarousel::paintEvent(event);
        
        // Custom painting after base implementation
        paintCustomOverlay(painter);
    }

private:
    void applyCustomStyle() {
        // Set custom properties
        setProperty("customStyle", true);
        
        // Apply custom stylesheet
        setStyleSheet(R"(
            FluentCarousel[customStyle="true"] {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                    stop:0 #f0f0f0, stop:1 #e0e0e0);
                border: 2px solid #0078d4;
                border-radius: 8px;
            }
        )");
    }

    void paintCustomBackground(QPainter& painter) {
        // Custom background painting
        painter.setRenderHint(QPainter::Antialiasing);
        
        QLinearGradient gradient(rect().topLeft(), rect().bottomLeft());
        gradient.setColorAt(0, QColor(240, 240, 240));
        gradient.setColorAt(1, QColor(224, 224, 224));
        
        painter.fillRect(rect(), gradient);
    }

    void paintCustomOverlay(QPainter& painter) {
        // Custom overlay painting (e.g., watermark, decorations)
        if (itemCount() > 0) {
            painter.setPen(QPen(QColor(0, 120, 212, 100), 2));
            painter.drawRect(rect().adjusted(5, 5, -5, -5));
        }
    }
};
```

## Data Integration

### JSON Data Loading

```cpp
class JsonCarouselLoader {
public:
    static void loadFromJson(FluentCarousel* carousel, const QString& jsonPath) {
        QFile file(jsonPath);
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Could not open JSON file:" << jsonPath;
            return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonArray items = doc.array();

        for (const QJsonValue& value : items) {
            QJsonObject item = value.toObject();
            
            QString title = item["title"].toString();
            QString description = item["description"].toString();
            QString imagePath = item["image"].toString();
            
            auto* widget = createItemFromJson(item);
            carousel->addItem(widget);
        }
    }

private:
    static QWidget* createItemFromJson(const QJsonObject& item) {
        auto* widget = new QWidget();
        auto* layout = new QVBoxLayout(widget);

        // Create UI elements based on JSON data
        if (item.contains("title")) {
            auto* titleLabel = new QLabel(item["title"].toString());
            titleLabel->setStyleSheet("font-weight: bold; font-size: 18px;");
            layout->addWidget(titleLabel);
        }

        if (item.contains("image")) {
            auto* imageLabel = new QLabel();
            QPixmap pixmap(item["image"].toString());
            imageLabel->setPixmap(pixmap.scaled(300, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            layout->addWidget(imageLabel);
        }

        if (item.contains("description")) {
            auto* descLabel = new QLabel(item["description"].toString());
            descLabel->setWordWrap(true);
            layout->addWidget(descLabel);
        }

        return widget;
    }
};

// Usage
auto* carousel = new FluentCarousel(this);
JsonCarouselLoader::loadFromJson(carousel, ":/data/carousel_items.json");
```

### Database Integration

```cpp
class DatabaseCarousel : public FluentCarousel {
    Q_OBJECT

public:
    explicit DatabaseCarousel(QSqlDatabase database, QWidget* parent = nullptr)
        : FluentCarousel(parent), m_database(database) {}

    void loadFromDatabase(const QString& tableName, const QString& whereClause = QString()) {
        if (!m_database.isOpen()) {
            qWarning() << "Database is not open";
            return;
        }

        QString queryString = QString("SELECT * FROM %1").arg(tableName);
        if (!whereClause.isEmpty()) {
            queryString += " WHERE " + whereClause;
        }

        QSqlQuery query(queryString, m_database);
        
        clear(); // Clear existing items

        while (query.next()) {
            auto* widget = createItemFromQuery(query);
            addItem(widget);
        }
    }

private:
    QWidget* createItemFromQuery(const QSqlQuery& query) {
        auto* widget = new QWidget();
        auto* layout = new QVBoxLayout(widget);

        // Extract data from query
        QString title = query.value("title").toString();
        QString description = query.value("description").toString();
        QByteArray imageData = query.value("image").toByteArray();

        // Create UI elements
        if (!title.isEmpty()) {
            auto* titleLabel = new QLabel(title);
            titleLabel->setStyleSheet("font-weight: bold; font-size: 16px;");
            layout->addWidget(titleLabel);
        }

        if (!imageData.isEmpty()) {
            QPixmap pixmap;
            pixmap.loadFromData(imageData);
            auto* imageLabel = new QLabel();
            imageLabel->setPixmap(pixmap.scaled(250, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            layout->addWidget(imageLabel);
        }

        if (!description.isEmpty()) {
            auto* descLabel = new QLabel(description);
            descLabel->setWordWrap(true);
            layout->addWidget(descLabel);
        }

        return widget;
    }

    QSqlDatabase m_database;
};
```

## Performance Optimization

### Lazy Loading Implementation

```cpp
class LazyLoadingCarousel : public FluentCarousel {
    Q_OBJECT

public:
    explicit LazyLoadingCarousel(QWidget* parent = nullptr)
        : FluentCarousel(parent), m_preloadDistance(2) {
        
        // Connect to index changes for lazy loading
        connect(this, &FluentCarousel::currentIndexChanged,
                this, &LazyLoadingCarousel::onCurrentIndexChanged);
    }

    void setDataSource(const QStringList& dataSources) {
        m_dataSources = dataSources;
        
        // Create placeholder items
        clear();
        for (int i = 0; i < dataSources.size(); ++i) {
            auto* placeholder = createPlaceholder(i);
            addItem(placeholder);
        }
        
        // Load initial items
        loadItemsAroundIndex(0);
    }

private slots:
    void onCurrentIndexChanged(int index) {
        loadItemsAroundIndex(index);
    }

private:
    void loadItemsAroundIndex(int centerIndex) {
        int start = std::max(0, centerIndex - m_preloadDistance);
        int end = std::min(m_dataSources.size() - 1, centerIndex + m_preloadDistance);

        for (int i = start; i <= end; ++i) {
            if (!m_loadedItems.contains(i)) {
                loadItem(i);
            }
        }

        // Unload distant items to save memory
        unloadDistantItems(centerIndex);
    }

    void loadItem(int index) {
        if (index < 0 || index >= m_dataSources.size()) return;

        // Load item asynchronously
        auto* loader = new QThread(this);
        auto* worker = new ItemLoader(m_dataSources[index]);
        worker->moveToThread(loader);

        connect(loader, &QThread::started, worker, &ItemLoader::loadItem);
        connect(worker, &ItemLoader::itemLoaded, this, [this, index](QWidget* widget) {
            replaceItemAt(index, widget);
            m_loadedItems.insert(index);
        });
        connect(worker, &ItemLoader::finished, loader, &QThread::quit);
        connect(loader, &QThread::finished, loader, &QThread::deleteLater);

        loader->start();
    }

    void unloadDistantItems(int centerIndex) {
        QSet<int> toUnload;
        for (int loadedIndex : m_loadedItems) {
            if (std::abs(loadedIndex - centerIndex) > m_preloadDistance * 2) {
                toUnload.insert(loadedIndex);
            }
        }

        for (int index : toUnload) {
            auto* placeholder = createPlaceholder(index);
            replaceItemAt(index, placeholder);
            m_loadedItems.remove(index);
        }
    }

    QWidget* createPlaceholder(int index) {
        auto* placeholder = new QLabel(QString("Loading item %1...").arg(index + 1));
        placeholder->setAlignment(Qt::AlignCenter);
        placeholder->setStyleSheet("background-color: #f0f0f0; padding: 40px;");
        return placeholder;
    }

    void replaceItemAt(int index, QWidget* newWidget) {
        if (auto* oldWidget = itemAt(index)) {
            removeItemAt(index);
            insertItem(index, newWidget);
        }
    }

    QStringList m_dataSources;
    QSet<int> m_loadedItems;
    int m_preloadDistance;
};

class ItemLoader : public QObject {
    Q_OBJECT

public:
    explicit ItemLoader(const QString& dataSource, QObject* parent = nullptr)
        : QObject(parent), m_dataSource(dataSource) {}

public slots:
    void loadItem() {
        // Simulate loading time
        QThread::msleep(100);

        // Create actual item widget
        auto* widget = new QLabel(QString("Loaded: %1").arg(m_dataSource));
        widget->setAlignment(Qt::AlignCenter);
        widget->setStyleSheet("background-color: lightblue; padding: 40px;");

        emit itemLoaded(widget);
        emit finished();
    }

signals:
    void itemLoaded(QWidget* widget);
    void finished();

private:
    QString m_dataSource;
};
```

## Error Handling

### Robust Error Handling

```cpp
class RobustCarousel : public FluentCarousel {
    Q_OBJECT

public:
    explicit RobustCarousel(QWidget* parent = nullptr)
        : FluentCarousel(parent) {
        
        // Set up error handling
        connect(this, &FluentCarousel::itemAdded, this, &RobustCarousel::validateItem);
    }

    bool addItemSafely(QWidget* item, QString* errorMessage = nullptr) {
        if (!item) {
            if (errorMessage) *errorMessage = "Cannot add null item";
            return false;
        }

        if (item->parent() && item->parent() != this) {
            if (errorMessage) *errorMessage = "Item already has a different parent";
            return false;
        }

        try {
            addItem(item);
            return true;
        } catch (const std::exception& e) {
            if (errorMessage) *errorMessage = QString("Exception: %1").arg(e.what());
            return false;
        } catch (...) {
            if (errorMessage) *errorMessage = "Unknown exception occurred";
            return false;
        }
    }

    bool navigateToIndexSafely(int index, QString* errorMessage = nullptr) {
        if (index < 0 || index >= itemCount()) {
            if (errorMessage) {
                *errorMessage = QString("Index %1 is out of range [0, %2]")
                                .arg(index).arg(itemCount() - 1);
            }
            return false;
        }

        if (currentIndex() == index) {
            if (errorMessage) *errorMessage = "Already at the specified index";
            return true; // Not an error, but no action needed
        }

        try {
            goToIndex(index);
            return true;
        } catch (const std::exception& e) {
            if (errorMessage) *errorMessage = QString("Navigation failed: %1").arg(e.what());
            return false;
        }
    }

private slots:
    void validateItem(QWidget* item, int index) {
        Q_UNUSED(index)
        
        if (!item) {
            qWarning() << "Null item added to carousel";
            return;
        }

        if (item->size().isEmpty()) {
            qWarning() << "Item with empty size added to carousel";
        }

        // Additional validation as needed
    }
};
```

## Testing Integration

### Unit Test Helpers

```cpp
class CarouselTestHelper {
public:
    static void addTestItems(FluentCarousel* carousel, int count) {
        for (int i = 0; i < count; ++i) {
            auto* item = new QLabel(QString("Test Item %1").arg(i + 1));
            item->setFixedSize(200, 150);
            carousel->addItem(item);
        }
    }

    static bool waitForTransition(FluentCarousel* carousel, int timeoutMs = 5000) {
        QSignalSpy spy(carousel, &FluentCarousel::transitionFinished);
        return spy.wait(timeoutMs);
    }

    static void simulateUserNavigation(FluentCarousel* carousel, int targetIndex) {
        // Simulate realistic user navigation timing
        QTimer::singleShot(100, [carousel, targetIndex]() {
            carousel->goToIndex(targetIndex);
        });
    }

    static QPixmap captureCarousel(FluentCarousel* carousel) {
        return carousel->grab();
    }

    static bool compareCarouselStates(FluentCarousel* carousel1, FluentCarousel* carousel2) {
        return carousel1->currentIndex() == carousel2->currentIndex() &&
               carousel1->itemCount() == carousel2->itemCount() &&
               carousel1->configuration().transition == carousel2->configuration().transition;
    }
};
```

## Deployment Considerations

### Resource Management

```cpp
// Ensure proper resource cleanup
class ResourceManagedCarousel : public FluentCarousel {
public:
    ~ResourceManagedCarousel() {
        // Stop any running animations
        stopAutoPlay();
        
        // Clear items (automatically deletes child widgets)
        clear();
        
        // Additional cleanup if needed
    }
};
```

### Platform-Specific Optimizations

```cpp
void optimizeForPlatform(FluentCarousel* carousel) {
#ifdef Q_OS_ANDROID
    // Mobile optimizations
    FluentCarouselTouchConfig touchConfig;
    touchConfig.gestureSensitivity = FluentCarouselGestureSensitivity::High;
    if (auto* touchCarousel = qobject_cast<FluentTouchCarousel*>(carousel)) {
        touchCarousel->setTouchConfiguration(touchConfig);
    }
#endif

#ifdef Q_OS_WIN
    // Windows-specific optimizations
    carousel->setProperty("useNativeRendering", true);
#endif

#ifdef Q_OS_MACOS
    // macOS-specific optimizations
    carousel->setAttribute(Qt::WA_MacShowFocusRect, false);
#endif
}
```

## See Also

- [FluentCarousel API Reference](../components/FluentCarousel.md)
- [Carousel Variants Guide](../components/FluentCarouselVariants.md)
- [Styling and Theming Guide](../styling/README.md)
- [Performance Best Practices](../performance/README.md)
- [Accessibility Guidelines](../accessibility/README.md)
