# FluentFilePicker Component

The `FluentFilePicker` component is a comprehensive file selection widget that implements Microsoft's Fluent Design principles. It provides drag & drop support, file validation, upload functionality, and rich preview capabilities for modern file handling workflows.

## Overview

FluentFilePicker offers:
- **Multiple picker modes** (Button, DropZone, Inline, Compact)
- **Drag & drop support** with visual feedback
- **File validation** with custom validators and size limits
- **Upload functionality** with progress tracking
- **File previews** with metadata display
- **Batch operations** for multiple files
- **Theme integration** with automatic styling
- **Accessibility support** with keyboard navigation

## Basic Usage

### Simple File Picker

```cpp
#include "FluentQt/Components/FluentFilePicker.h"

// Create a basic file picker
auto* filePicker = new FluentFilePicker();
filePicker->setMode(FluentFilePicker::PickerMode::Button);
filePicker->setAcceptedTypes({"*.jpg", "*.png", "*.gif"});

// Handle file selection
connect(filePicker, &FluentFilePicker::filesSelected, 
        [=](const QList<FluentFileInfo>& files) {
    for (const auto& file : files) {
        qDebug() << "Selected file:" << file.fileName;
    }
});
```

### Drop Zone Picker

```cpp
// Create a drop zone for file uploads
auto* dropZone = new FluentFilePicker();
dropZone->setMode(FluentFilePicker::PickerMode::DropZone);
dropZone->setDropZoneText("Drop files here or click to browse");
dropZone->setDropZoneIcon(QIcon(":/icons/upload.svg"));
dropZone->setAllowMultipleFiles(true);

// Configure size constraints
dropZone->setMaxFileSize(10 * 1024 * 1024);  // 10MB limit
dropZone->setMaxFiles(5);  // Maximum 5 files
```

### Inline File List

```cpp
// Create inline file picker with list view
auto* inlinePicker = new FluentFilePicker();
inlinePicker->setMode(FluentFilePicker::PickerMode::Inline);
inlinePicker->setShowPreview(true);
inlinePicker->setAcceptedTypes({"*.pdf", "*.doc", "*.docx"});
```

## Configuration Examples

### File Type Filtering

```cpp
auto* filePicker = new FluentFilePicker();

// Set accepted file types
filePicker->setAcceptedTypes({
    "*.jpg", "*.jpeg", "*.png", "*.gif",  // Images
    "*.pdf", "*.doc", "*.docx",           // Documents
    "*.mp4", "*.avi", "*.mov"             // Videos
});

// Or use MIME types
filePicker->setAcceptedMimeTypes({
    "image/*",
    "application/pdf",
    "text/plain"
});
```

### Size and Count Limits

```cpp
auto* filePicker = new FluentFilePicker();

// Configure limits
filePicker->setMaxFileSize(50 * 1024 * 1024);  // 50MB per file
filePicker->setMaxFiles(10);                    // Maximum 10 files
filePicker->setAllowMultipleFiles(true);

// Enable/disable zero-size files
filePicker->setAllowEmptyFiles(false);
```

### Custom Validation

```cpp
auto* filePicker = new FluentFilePicker();

// Set custom file validator
filePicker->setFileValidator([](const FluentFileInfo& fileInfo) -> bool {
    // Custom validation logic
    if (fileInfo.size > 1024 * 1024) {  // 1MB limit
        return false;
    }
    
    // Check file extension
    QFileInfo info(fileInfo.filePath);
    QStringList allowedExtensions = {"jpg", "png", "pdf"};
    return allowedExtensions.contains(info.suffix().toLower());
});

// Set custom error message
filePicker->setCustomErrorMessage("File must be JPG, PNG, or PDF under 1MB");
```

## State Examples

### File Management

```cpp
auto* filePicker = new FluentFilePicker();

// Add files programmatically
filePicker->addFile("/path/to/document.pdf");
filePicker->addFiles({"/path/to/image1.jpg", "/path/to/image2.png"});

// Remove files
filePicker->removeFile(0);  // Remove by index
filePicker->removeFile("document.pdf");  // Remove by filename

// Clear all files
filePicker->clearFiles();

// Get current files
QList<FluentFileInfo> files = filePicker->files();
for (const auto& file : files) {
    qDebug() << file.fileName << file.size << file.lastModified;
}
```

### Upload States

```cpp
auto* filePicker = new FluentFilePicker();

// Configure upload function
filePicker->setUploadFunction([](const FluentFileInfo& file, 
                                std::function<void(const FluentUploadProgress&)> progressCallback) {
    // Simulate upload with progress
    QTimer* timer = new QTimer;
    int progress = 0;
    
    QObject::connect(timer, &QTimer::timeout, [=]() mutable {
        progress += 10;
        FluentUploadProgress uploadProgress;
        uploadProgress.fileName = file.fileName;
        uploadProgress.bytesUploaded = (file.size * progress) / 100;
        uploadProgress.totalBytes = file.size;
        uploadProgress.percentage = progress;
        
        progressCallback(uploadProgress);
        
        if (progress >= 100) {
            timer->stop();
            timer->deleteLater();
        }
    });
    
    timer->start(100);  // Update every 100ms
});

// Start upload
filePicker->uploadFiles();
```

## Event Handling Examples

### File Selection Events

```cpp
auto* filePicker = new FluentFilePicker();

// Handle file selection
connect(filePicker, &FluentFilePicker::filesSelected, 
        [=](const QList<FluentFileInfo>& files) {
    qDebug() << "Files selected:" << files.size();
    for (const auto& file : files) {
        qDebug() << " -" << file.fileName << "(" << file.size << "bytes)";
    }
});

// Handle file addition (drag & drop or browse)
connect(filePicker, &FluentFilePicker::fileAdded, 
        [=](const FluentFileInfo& file) {
    qDebug() << "File added:" << file.fileName;
    updateFileCounter();
});

// Handle file removal
connect(filePicker, &FluentFilePicker::fileRemoved, 
        [=](const QString& fileName) {
    qDebug() << "File removed:" << fileName;
    updateFileCounter();
});
```

### Validation Events

```cpp
connect(filePicker, &FluentFilePicker::validationFailed, 
        [=](const QString& fileName, const QString& error) {
    qDebug() << "Validation failed for" << fileName << ":" << error;
    
    // Show user-friendly error message
    QMessageBox::warning(this, "File Validation Error", 
                        QString("Cannot add %1: %2").arg(fileName, error));
});

connect(filePicker, &FluentFilePicker::fileSizeExceeded, 
        [=](const QString& fileName, qint64 size, qint64 maxSize) {
    QString message = QString("File %1 (%2) exceeds maximum size limit (%3)")
                     .arg(fileName)
                     .arg(formatFileSize(size))
                     .arg(formatFileSize(maxSize));
    showErrorMessage(message);
});
```

### Upload Events

```cpp
connect(filePicker, &FluentFilePicker::uploadStarted, 
        [=](const QString& fileName) {
    qDebug() << "Upload started for:" << fileName;
    showUploadProgress(fileName);
});

connect(filePicker, &FluentFilePicker::uploadProgress, 
        [=](const FluentUploadProgress& progress) {
    updateProgressBar(progress.fileName, progress.percentage);
});

connect(filePicker, &FluentFilePicker::uploadCompleted, 
        [=](const QString& fileName) {
    qDebug() << "Upload completed for:" << fileName;
    showSuccessMessage(fileName);
});

connect(filePicker, &FluentFilePicker::uploadFailed, 
        [=](const QString& fileName, const QString& error) {
    qDebug() << "Upload failed for" << fileName << ":" << error;
    showErrorMessage(QString("Upload failed: %1").arg(error));
});
```

## Advanced Usage

### Custom File Preview

```cpp
class CustomFilePicker : public FluentFilePicker {
    Q_OBJECT
    
public:
    CustomFilePicker(QWidget* parent = nullptr) : FluentFilePicker(parent) {
        setShowPreview(true);
        setupCustomPreview();
    }
    
private:
    void setupCustomPreview() {
        // Create custom preview widget
        m_previewWidget = new FluentFilePreview(this);
        m_previewWidget->setPreviewSize(QSize(300, 200));
        m_previewWidget->setShowMetadata(true);
        
        // Connect to file selection
        connect(this, &FluentFilePicker::fileSelected, 
                [=](const FluentFileInfo& file) {
            m_previewWidget->setFile(file);
        });
        
        // Add preview to layout
        layout()->addWidget(m_previewWidget);
    }
    
    FluentFilePreview* m_previewWidget;
};
```

### Batch File Operations

```cpp
auto* filePicker = new FluentFilePicker();
auto* batchOps = new FluentBatchFileOperations();

// Connect file picker to batch operations
connect(filePicker, &FluentFilePicker::filesChanged, 
        [=](const QList<FluentFileInfo>& files) {
    batchOps->setFiles(files);
});

// Add custom operations
batchOps->addOperation("Compress All", [](const QList<FluentFileInfo>& files) {
    for (const auto& file : files) {
        compressFile(file.filePath);
    }
});

batchOps->addOperation("Generate Thumbnails", [](const QList<FluentFileInfo>& files) {
    for (const auto& file : files) {
        if (isImageFile(file.filePath)) {
            generateThumbnail(file.filePath);
        }
    }
});

// Execute operations
connect(batchOps, &FluentBatchFileOperations::operationCompleted, 
        [=](const QString& operationName) {
    qDebug() << "Batch operation completed:" << operationName;
});
```

### Integration with Cloud Storage

```cpp
class CloudFilePicker : public FluentFilePicker {
    Q_OBJECT
    
public:
    CloudFilePicker(QWidget* parent = nullptr) : FluentFilePicker(parent) {
        setupCloudUpload();
    }
    
private:
    void setupCloudUpload() {
        setUploadFunction([this](const FluentFileInfo& file, 
                                std::function<void(const FluentUploadProgress&)> progressCallback) {
            // Upload to cloud storage
            m_cloudService->uploadFile(file.filePath, [=](int progress) {
                FluentUploadProgress uploadProgress;
                uploadProgress.fileName = file.fileName;
                uploadProgress.percentage = progress;
                uploadProgress.bytesUploaded = (file.size * progress) / 100;
                uploadProgress.totalBytes = file.size;
                progressCallback(uploadProgress);
            });
        });
    }
    
    CloudStorageService* m_cloudService;
};
```

## Accessibility Examples

### Screen Reader Support

```cpp
auto* filePicker = new FluentFilePicker();

// Set accessible properties
filePicker->setAccessibleName("File upload area");
filePicker->setAccessibleDescription("Drop files here or click to browse and select files");

// Update accessibility when files change
connect(filePicker, &FluentFilePicker::filesChanged, 
        [=](const QList<FluentFileInfo>& files) {
    QString description = QString("%1 file%2 selected for upload")
                         .arg(files.size())
                         .arg(files.size() == 1 ? "" : "s");
    filePicker->setAccessibleDescription(description);
});
```

### Keyboard Navigation

```cpp
auto* filePicker = new FluentFilePicker();
filePicker->setFocusPolicy(Qt::TabFocus);

// Handle keyboard shortcuts
filePicker->installEventFilter(this);

bool eventFilter(QObject* obj, QEvent* event) override {
    if (obj == filePicker && event->type() == QEvent::KeyPress) {
        auto* keyEvent = static_cast<QKeyEvent*>(event);
        
        if (keyEvent->key() == Qt::Key_Space || keyEvent->key() == Qt::Key_Return) {
            // Trigger file browser
            filePicker->browseFiles();
            return true;
        } else if (keyEvent->key() == Qt::Key_Delete) {
            // Remove selected files
            filePicker->removeSelectedFiles();
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}
```

## Theming Examples

### Theme Integration

```cpp
#include "FluentQt/Styling/FluentTheme.h"

auto* filePicker = new FluentFilePicker();

// File picker automatically uses theme colors
auto& theme = FluentTheme::instance();

// Customize drop zone appearance
filePicker->setDropZoneStyle(QString(R"(
    QWidget {
        border: 2px dashed %1;
        border-radius: 8px;
        background-color: %2;
    }
    QWidget:hover {
        border-color: %3;
        background-color: %4;
    }
)").arg(theme.color("border.primary").name())
   .arg(theme.color("surface.secondary").name())
   .arg(theme.color("accent.primary").name())
   .arg(theme.color("surface.tertiary").name()));
```

### Custom Styling

```cpp
auto* filePicker = new FluentFilePicker();

// Apply custom stylesheet
filePicker->setStyleSheet(R"(
    FluentFilePicker {
        border: 1px solid #E0E0E0;
        border-radius: 8px;
        background-color: #FAFAFA;
    }
    
    FluentFilePicker[dragActive="true"] {
        border-color: #0078D4;
        background-color: #F3F9FF;
    }
    
    FluentFilePicker QPushButton {
        background-color: #0078D4;
        color: white;
        border: none;
        border-radius: 4px;
        padding: 8px 16px;
    }
)");
```

## Real-World Scenarios

### Document Upload Form

```cpp
void createDocumentUploadForm() {
    auto* form = new QWidget;
    auto* layout = new QVBoxLayout(form);
    
    // Title
    auto* titleLabel = new QLabel("Upload Documents");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    
    // File picker
    auto* filePicker = new FluentFilePicker();
    filePicker->setMode(FluentFilePicker::PickerMode::DropZone);
    filePicker->setAcceptedTypes({"*.pdf", "*.doc", "*.docx", "*.txt"});
    filePicker->setMaxFileSize(25 * 1024 * 1024);  // 25MB
    filePicker->setMaxFiles(10);
    filePicker->setDropZoneText("Drop documents here or click to browse");
    
    // Progress area
    auto* progressArea = new QWidget;
    auto* progressLayout = new QVBoxLayout(progressArea);
    
    // Upload button
    auto* uploadButton = new QPushButton("Upload Files");
    uploadButton->setEnabled(false);
    
    // Connect signals
    connect(filePicker, &FluentFilePicker::filesChanged, 
            [=](const QList<FluentFileInfo>& files) {
        uploadButton->setEnabled(!files.isEmpty());
    });
    
    connect(uploadButton, &QPushButton::clicked, [=]() {
        filePicker->uploadFiles();
    });
    
    layout->addWidget(titleLabel);
    layout->addWidget(filePicker);
    layout->addWidget(progressArea);
    layout->addWidget(uploadButton);
    
    form->show();
}
```

### Image Gallery Upload

```cpp
void createImageGalleryUpload() {
    auto* gallery = new QWidget;
    auto* layout = new QVBoxLayout(gallery);
    
    auto* imagePicker = new FluentFilePicker();
    imagePicker->setMode(FluentFilePicker::PickerMode::Inline);
    imagePicker->setAcceptedTypes({"*.jpg", "*.jpeg", "*.png", "*.gif", "*.webp"});
    imagePicker->setAllowMultipleFiles(true);
    imagePicker->setShowPreview(true);
    imagePicker->setMaxFileSize(10 * 1024 * 1024);  // 10MB per image
    
    // Custom validation for images
    imagePicker->setFileValidator([](const FluentFileInfo& file) -> bool {
        QImageReader reader(file.filePath);
        if (!reader.canRead()) {
            return false;
        }
        
        QSize imageSize = reader.size();
        return imageSize.width() >= 100 && imageSize.height() >= 100;
    });
    
    imagePicker->setCustomErrorMessage("Image must be at least 100x100 pixels");
    
    layout->addWidget(new QLabel("Upload Images to Gallery"));
    layout->addWidget(imagePicker);
    
    gallery->show();
}
```

## API Reference

### Enums

```cpp
enum class PickerMode { Button, DropZone, Inline, Compact };
```

### Properties

| Property | Type | Description |
|----------|------|-------------|
| `mode` | `PickerMode` | Display mode of the file picker |
| `acceptedTypes` | `QStringList` | Accepted file type patterns |
| `maxFileSize` | `qint64` | Maximum file size in bytes |
| `maxFiles` | `int` | Maximum number of files |
| `allowMultipleFiles` | `bool` | Whether multiple files can be selected |
| `showPreview` | `bool` | Whether to show file previews |
| `dropZoneText` | `QString` | Text displayed in drop zone |
| `dropZoneIcon` | `QIcon` | Icon displayed in drop zone |

### Methods

| Method | Description |
|--------|-------------|
| `addFile(QString)` | Add file by path |
| `addFiles(QStringList)` | Add multiple files |
| `removeFile(int)` | Remove file by index |
| `removeFile(QString)` | Remove file by name |
| `clearFiles()` | Remove all files |
| `files()` | Get current file list |
| `uploadFiles()` | Start upload process |
| `cancelUpload()` | Cancel ongoing upload |

### Signals

| Signal | Description |
|--------|-------------|
| `filesSelected(QList<FluentFileInfo>)` | Emitted when files are selected |
| `fileAdded(FluentFileInfo)` | Emitted when a file is added |
| `fileRemoved(QString)` | Emitted when a file is removed |
| `uploadStarted(QString)` | Emitted when upload starts |
| `uploadProgress(FluentUploadProgress)` | Emitted during upload |
| `uploadCompleted(QString)` | Emitted when upload completes |
| `uploadFailed(QString, QString)` | Emitted when upload fails |

## Best Practices

### Performance Guidelines

- Set appropriate file size limits to prevent memory issues
- Use file validation to reject unsuitable files early
- Implement lazy loading for file previews
- Consider using background threads for file operations

### Accessibility Guidelines

- Provide clear instructions and feedback
- Support keyboard navigation
- Use descriptive accessible names and descriptions
- Announce file selection changes to screen readers

### Design Guidelines

- Use appropriate picker modes for your use case
- Provide clear visual feedback for drag & drop
- Show upload progress for better user experience
- Handle errors gracefully with helpful messages

## Related Components

- **[FluentButton](FluentButton.md)** - For file picker trigger buttons
- **[FluentProgressBar](FluentProgressBar.md)** - For upload progress indication
- **[FluentCard](FluentCard.md)** - For file preview cards
- **[FluentNotification](FluentNotification.md)** - For upload status notifications
