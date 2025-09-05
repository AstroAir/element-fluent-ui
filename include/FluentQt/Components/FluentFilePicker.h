#pragma once

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMimeData>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>
#include <functional>

#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

/**
 * @brief File information structure
 */
struct FluentFileInfo {
    QString fileName;        // File name
    QString filePath;        // Full file path
    qint64 fileSize;         // File size in bytes
    QString mimeType;        // MIME type
    QDateTime lastModified;  // Last modification time
    QIcon icon;              // File icon
    bool isValid = true;     // Whether file is valid
    QVariantMap metadata;    // Additional metadata
};

/**
 * @brief Upload progress information
 */
struct FluentUploadProgress {
    QString fileName;          // File being uploaded
    qint64 bytesUploaded = 0;  // Bytes uploaded
    qint64 totalBytes = 0;     // Total bytes
    double percentage = 0.0;   // Upload percentage
    QString status;            // Upload status
    bool completed = false;    // Whether upload is complete
    bool error = false;        // Whether there was an error
    QString errorMessage;      // Error message if any
};

/**
 * @brief Modern file picker component with drag & drop support
 */
class FluentFilePicker : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(
        bool multipleFiles READ allowMultipleFiles WRITE setAllowMultipleFiles)
    Q_PROPERTY(
        QStringList acceptedTypes READ acceptedTypes WRITE setAcceptedTypes)
    Q_PROPERTY(qint64 maxFileSize READ maxFileSize WRITE setMaxFileSize)
    Q_PROPERTY(int maxFiles READ maxFiles WRITE setMaxFiles)
    Q_PROPERTY(PickerMode mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(bool showPreview READ showPreview WRITE setShowPreview NOTIFY
                   showPreviewChanged)
    Q_PROPERTY(QString dropZoneText READ dropZoneText WRITE setDropZoneText
                   NOTIFY dropZoneTextChanged)
    Q_PROPERTY(QIcon dropZoneIcon READ dropZoneIcon WRITE setDropZoneIcon NOTIFY
                   dropZoneIconChanged)

public:
    enum class PickerMode {
        Files,        // File selection only
        Directories,  // Directory selection only
        Both          // Both files and directories
    };

    explicit FluentFilePicker(QWidget* parent = nullptr);
    ~FluentFilePicker() override;

    // Configuration
    bool allowMultipleFiles() const { return m_allowMultipleFiles; }
    void setAllowMultipleFiles(bool allow);

    QStringList acceptedTypes() const { return m_acceptedTypes; }
    void setAcceptedTypes(const QStringList& types);

    qint64 maxFileSize() const { return m_maxFileSize; }
    void setMaxFileSize(qint64 size);

    int maxFiles() const { return m_maxFiles; }
    void setMaxFiles(int count);

    PickerMode mode() const { return m_mode; }
    void setMode(PickerMode mode);

    // Files
    QList<FluentFileInfo> files() const { return m_files; }
    void addFile(const QString& filePath);
    void addFiles(const QStringList& filePaths);
    void removeFile(int index);
    void removeFile(const QString& fileName);
    void clearFiles();

    // Validation
    void setFileValidator(std::function<bool(const FluentFileInfo&)> validator);
    void setCustomErrorMessage(const QString& message);

    // Upload functionality
    void setUploadFunction(
        std::function<void(const FluentFileInfo&,
                           std::function<void(const FluentUploadProgress&)>)>
            uploadFunc);
    void uploadFiles();
    void cancelUpload();

    // Appearance
    QString dropZoneText() const { return m_dropZoneTextString; }
    void setDropZoneText(const QString& text);

    QIcon dropZoneIcon() const { return m_dropZoneIconValue; }
    void setDropZoneIcon(const QIcon& icon);

    bool showPreview() const { return m_showPreview; }
    void setShowPreview(bool show);

    // Utility methods
    static QString formatFileSize(qint64 bytes);

    // Accessibility methods
    void setAccessibleName(const QString& name);
    void setAccessibleDescription(const QString& description);
    QString accessibleFileListDescription() const;
    void announceFileAdded(const FluentFileInfo& fileInfo);
    void announceFileRemoved(const FluentFileInfo& fileInfo);

public slots:
    void openFileDialog();
    void openDirectoryDialog();

signals:
    void filesAdded(const QList<FluentFileInfo>& files);
    void fileRemoved(const FluentFileInfo& file);
    void filesCleared();
    void uploadStarted();
    void uploadProgress(const FluentUploadProgress& progress);
    void uploadCompleted(const QList<FluentFileInfo>& files);
    void uploadFailed(const QString& error);
    void validationFailed(const QString& error);
    void modeChanged(PickerMode mode);
    void showPreviewChanged(bool show);
    void dropZoneTextChanged(const QString& text);
    void dropZoneIconChanged(const QIcon& icon);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onBrowseClicked();
    void onRemoveFileClicked();
    void onUploadClicked();
    void onCancelUploadClicked();
    void updateUploadProgress();

private:
    void setupUI();
    void setupDropZone();
    void setupFileList();
    void setupUploadControls();
    void updateUI();
    void updateDropZone();
    void updateFileList();
    void updateUploadControls();

    bool validateFile(const FluentFileInfo& fileInfo);
    bool isAcceptedType(const QString& fileName);
    FluentFileInfo createFileInfo(const QString& filePath);
    void addFileInternal(const FluentFileInfo& fileInfo);
    void animateFileAddition(int index);
    void animateFileRemoval(int index);
    void showValidationError(const QString& error);

    // Drag & drop helpers
    void setDragActive(bool active);
    void processDraggedFiles(const QList<QUrl>& urls);

    // Upload helpers
    void uploadNextFile();

    // Accessibility helpers
    void setupAccessibility();
    void updateAccessibilityInfo();
    void setAccessibleProperties();
    void setupKeyboardNavigation();
    void updateAriaLabels();

    // UI components
    QVBoxLayout* m_mainLayout = nullptr;
    QWidget* m_dropZone = nullptr;
    QVBoxLayout* m_dropZoneLayout = nullptr;
    QLabel* m_dropZoneIcon = nullptr;
    QLabel* m_dropZoneText = nullptr;
    QPushButton* m_browseButton = nullptr;

    QWidget* m_fileListContainer = nullptr;
    QListWidget* m_fileList = nullptr;

    QWidget* m_uploadControls = nullptr;
    QHBoxLayout* m_uploadLayout = nullptr;
    QPushButton* m_uploadButton = nullptr;
    QPushButton* m_cancelButton = nullptr;
    QProgressBar* m_uploadProgress = nullptr;
    QLabel* m_uploadStatus = nullptr;

    // Data
    QList<FluentFileInfo> m_files;
    QList<FluentUploadProgress> m_uploadProgresses;

    // Configuration
    bool m_allowMultipleFiles = true;
    QStringList m_acceptedTypes;
    qint64 m_maxFileSize = 100 * 1024 * 1024;  // 100MB default
    int m_maxFiles = 10;
    PickerMode m_mode = PickerMode::Files;

    // Additional FluentUI properties
    bool m_showPreview = false;
    bool m_enableDragDrop = true;
    bool m_showFileSize = true;
    bool m_showFileType = true;
    bool m_autoUpload = false;
    QString m_placeholderText = "No files selected";
    int m_thumbnailSize = 48;

    // Validation
    std::function<bool(const FluentFileInfo&)> m_fileValidator;
    QString m_customErrorMessage;

    // Upload
    std::function<void(const FluentFileInfo&,
                       std::function<void(const FluentUploadProgress&)>)>
        m_uploadFunction;
    bool m_uploading = false;
    int m_currentUploadIndex = 0;

    // Appearance
    QString m_dropZoneTextString = "Drag & drop files here or click to browse";
    QIcon m_dropZoneIconValue;
    bool m_dragActive = false;

    // Animations
    QPropertyAnimation* m_dropZoneAnimation = nullptr;
    QGraphicsOpacityEffect* m_dropZoneOpacity = nullptr;
    std::unique_ptr<Animation::FluentAnimator> m_animator;
};

/**
 * @brief File preview widget for images and documents
 */
class FluentFilePreview : public QWidget {
    Q_OBJECT

public:
    explicit FluentFilePreview(QWidget* parent = nullptr);

    void setFile(const FluentFileInfo& fileInfo);
    void clearPreview();

    // Configuration
    void setPreviewSize(const QSize& size);
    void setShowMetadata(bool show);

signals:
    void previewClicked();
    void previewDoubleClicked();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    void updatePreview();
    void loadImagePreview();
    void loadDocumentPreview();
    void loadGenericPreview();

    FluentFileInfo m_fileInfo;
    QPixmap m_previewPixmap;
    QSize m_previewSize = QSize(200, 150);
    bool m_showMetadata = true;
};

/**
 * @brief Batch file operations widget
 */
class FluentBatchFileOperations : public QWidget {
    Q_OBJECT

public:
    explicit FluentBatchFileOperations(QWidget* parent = nullptr);

    void setFiles(const QList<FluentFileInfo>& files);
    void addOperation(
        const QString& name,
        std::function<void(const QList<FluentFileInfo>&)> operation);

public slots:
    void executeOperation(const QString& name);
    void executeAllOperations();

signals:
    void operationStarted(const QString& name);
    void operationCompleted(const QString& name);
    void operationFailed(const QString& name, const QString& error);
    void allOperationsCompleted();

private:
    void setupUI();
    void updateOperationsList();

    QList<FluentFileInfo> m_files;
    QHash<QString, std::function<void(const QList<FluentFileInfo>&)>>
        m_operations;

    QVBoxLayout* m_layout = nullptr;
    QListWidget* m_operationsList = nullptr;
    QPushButton* m_executeAllButton = nullptr;
    QProgressBar* m_progressBar = nullptr;
};

}  // namespace FluentQt::Components
