// src/Components/FluentFilePicker.cpp
#include "FluentQt/Components/FluentFilePicker.h"
#include <QApplication>
#include <QDragMoveEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QImageReader>
#include <QListWidgetItem>
#include <QMimeDatabase>
#include <QMouseEvent>
#include <QPainter>
#include <QStandardPaths>
#include <QStyle>
#include <QStyleOption>
#include <QThread>
#include <QTimer>
#include "FluentQt/Styling/FluentTheme.h"

namespace FluentQt::Components {

FluentFilePicker::FluentFilePicker(QWidget* parent) : QWidget(parent) {
    setupUI();
    setupDropZone();
    setupFileList();
    setupUploadControls();

    setAcceptDrops(true);
    updateUI();

    // Setup animations
    m_dropZoneOpacity = new QGraphicsOpacityEffect(this);
    m_dropZone->setGraphicsEffect(m_dropZoneOpacity);

    m_dropZoneAnimation =
        new QPropertyAnimation(m_dropZoneOpacity, "opacity", this);
    m_dropZoneAnimation->setDuration(200);
    m_dropZoneAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

FluentFilePicker::~FluentFilePicker() = default;

void FluentFilePicker::setAllowMultipleFiles(bool allow) {
    if (m_allowMultipleFiles != allow) {
        m_allowMultipleFiles = allow;
        if (!allow && m_files.size() > 1) {
            // Keep only the first file
            QList<FluentFileInfo> firstFile;
            if (!m_files.isEmpty()) {
                firstFile.append(m_files.first());
            }
            m_files = firstFile;
            updateFileList();
        }
        updateUI();
    }
}

void FluentFilePicker::setAcceptedTypes(const QStringList& types) {
    m_acceptedTypes = types;
    updateUI();
}

void FluentFilePicker::setMaxFileSize(qint64 size) {
    m_maxFileSize = qMax(qint64(0), size);
}

void FluentFilePicker::setMaxFiles(int count) {
    m_maxFiles = qMax(1, count);
    if (m_files.size() > m_maxFiles) {
        m_files = m_files.mid(0, m_maxFiles);
        updateFileList();
    }
}

void FluentFilePicker::setMode(PickerMode mode) {
    m_mode = mode;
    updateUI();
}

void FluentFilePicker::addFile(const QString& filePath) {
    FluentFileInfo fileInfo = createFileInfo(filePath);
    if (fileInfo.isValid) {
        addFileInternal(fileInfo);
    }
}

void FluentFilePicker::addFiles(const QStringList& filePaths) {
    for (const QString& filePath : filePaths) {
        addFile(filePath);
    }
}

void FluentFilePicker::removeFile(int index) {
    if (index >= 0 && index < m_files.size()) {
        FluentFileInfo removedFile = m_files.takeAt(index);
        animateFileRemoval(index);
        updateFileList();
        updateUI();
        emit fileRemoved(removedFile);
    }
}

void FluentFilePicker::removeFile(const QString& fileName) {
    for (int i = 0; i < m_files.size(); ++i) {
        if (m_files[i].fileName == fileName) {
            removeFile(i);
            break;
        }
    }
}

void FluentFilePicker::clearFiles() {
    if (!m_files.isEmpty()) {
        m_files.clear();
        updateFileList();
        updateUI();
        emit filesCleared();
    }
}

void FluentFilePicker::setFileValidator(
    std::function<bool(const FluentFileInfo&)> validator) {
    m_fileValidator = validator;
}

void FluentFilePicker::setCustomErrorMessage(const QString& message) {
    m_customErrorMessage = message;
}

void FluentFilePicker::setUploadFunction(
    std::function<void(const FluentFileInfo&,
                       std::function<void(const FluentUploadProgress&)>)>
        uploadFunc) {
    m_uploadFunction = uploadFunc;
    updateUploadControls();
}

void FluentFilePicker::uploadFiles() {
    if (!m_uploadFunction || m_files.isEmpty() || m_uploading) {
        return;
    }

    m_uploading = true;
    m_currentUploadIndex = 0;
    m_uploadProgresses.clear();

    // Initialize progress for all files
    for (const auto& file : m_files) {
        FluentUploadProgress progress;
        progress.fileName = file.fileName;
        progress.totalBytes = file.fileSize;
        m_uploadProgresses.append(progress);
    }

    updateUploadControls();
    emit uploadStarted();

    // Start uploading first file
    uploadNextFile();
}

void FluentFilePicker::cancelUpload() {
    if (m_uploading) {
        m_uploading = false;
        m_currentUploadIndex = 0;
        updateUploadControls();
    }
}

void FluentFilePicker::setDropZoneText(const QString& text) {
    m_dropZoneTextString = text;
    if (m_dropZoneText) {
        m_dropZoneText->setText(text);
    }
}

void FluentFilePicker::setDropZoneIcon(const QIcon& icon) {
    m_dropZoneIconValue = icon;
    if (m_dropZoneIcon && !icon.isNull()) {
        m_dropZoneIcon->setPixmap(icon.pixmap(48, 48));
    }
}

void FluentFilePicker::setShowPreview(bool show) {
    m_showPreview = show;
    updateFileList();
}

void FluentFilePicker::openFileDialog() {
    QString filter;
    if (!m_acceptedTypes.isEmpty()) {
        QStringList filterParts;
        for (const QString& type : m_acceptedTypes) {
            filterParts.append(QString("*.%1").arg(type));
        }
        filter = QString("Accepted files (%1)").arg(filterParts.join(" "));
    }

    QStringList filePaths;
    if (m_allowMultipleFiles) {
        filePaths = QFileDialog::getOpenFileNames(this, "Select Files",
                                                  QString(), filter);
    } else {
        QString filePath = QFileDialog::getOpenFileName(this, "Select File",
                                                        QString(), filter);
        if (!filePath.isEmpty()) {
            filePaths.append(filePath);
        }
    }

    if (!filePaths.isEmpty()) {
        addFiles(filePaths);
    }
}

void FluentFilePicker::openDirectoryDialog() {
    QString dirPath =
        QFileDialog::getExistingDirectory(this, "Select Directory");
    if (!dirPath.isEmpty()) {
        addFile(dirPath);
    }
}

void FluentFilePicker::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
        setDragActive(true);
    } else {
        event->ignore();
    }
}

void FluentFilePicker::dragMoveEvent(QDragMoveEvent* event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void FluentFilePicker::dragLeaveEvent(QDragLeaveEvent* event) {
    setDragActive(false);
    event->accept();
}

void FluentFilePicker::dropEvent(QDropEvent* event) {
    setDragActive(false);

    if (event->mimeData()->hasUrls()) {
        processDraggedFiles(event->mimeData()->urls());
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void FluentFilePicker::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw border when dragging
    if (m_dragActive) {
        QPen pen(QColor(0, 120, 215), 2, Qt::DashLine);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(rect().adjusted(1, 1, -1, -1));
    }
}

void FluentFilePicker::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    updateUI();
}

void FluentFilePicker::onBrowseClicked() {
    if (m_mode == PickerMode::Directories) {
        openDirectoryDialog();
    } else {
        openFileDialog();
    }
}

void FluentFilePicker::onRemoveFileClicked() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button) {
        int index = button->property("fileIndex").toInt();
        removeFile(index);
    }
}

void FluentFilePicker::onUploadClicked() { uploadFiles(); }

void FluentFilePicker::onCancelUploadClicked() { cancelUpload(); }

void FluentFilePicker::updateUploadProgress() {
    if (!m_uploading || m_uploadProgresses.isEmpty()) {
        return;
    }

    // Calculate overall progress
    qint64 totalUploaded = 0;
    qint64 totalSize = 0;

    for (const auto& progress : m_uploadProgresses) {
        totalUploaded += progress.bytesUploaded;
        totalSize += progress.totalBytes;
    }

    double overallProgress =
        totalSize > 0 ? (double(totalUploaded) / totalSize) * 100.0 : 0.0;

    if (m_uploadProgress) {
        m_uploadProgress->setValue(static_cast<int>(overallProgress));
    }

    if (m_uploadStatus) {
        m_uploadStatus->setText(
            QString("Uploading... %1%")
                .arg(QString::number(overallProgress, 'f', 1)));
    }

    // Emit progress signal
    if (m_currentUploadIndex < m_uploadProgresses.size()) {
        emit uploadProgress(m_uploadProgresses[m_currentUploadIndex]);
    }
}

void FluentFilePicker::setupUI() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    m_mainLayout->setSpacing(10);

    setMinimumHeight(200);
}

void FluentFilePicker::setupDropZone() {
    m_dropZone = new QWidget(this);
    m_dropZone->setObjectName("dropZone");
    m_dropZone->setMinimumHeight(120);

    m_dropZoneLayout = new QVBoxLayout(m_dropZone);
    m_dropZoneLayout->setAlignment(Qt::AlignCenter);

    // Drop zone icon
    m_dropZoneIcon = new QLabel(m_dropZone);
    m_dropZoneIcon->setAlignment(Qt::AlignCenter);
    m_dropZoneIcon->setPixmap(
        style()
            ->standardIcon(QStyle::SP_FileDialogDetailedView)
            .pixmap(48, 48));
    m_dropZoneLayout->addWidget(m_dropZoneIcon);

    // Drop zone text
    m_dropZoneText = new QLabel(m_dropZoneTextString, m_dropZone);
    m_dropZoneText->setAlignment(Qt::AlignCenter);
    m_dropZoneText->setWordWrap(true);
    m_dropZoneLayout->addWidget(m_dropZoneText);

    // Browse button
    m_browseButton = new QPushButton("Browse Files", m_dropZone);
    m_browseButton->setObjectName("browseButton");
    connect(m_browseButton, &QPushButton::clicked, this,
            &FluentFilePicker::onBrowseClicked);
    m_dropZoneLayout->addWidget(m_browseButton, 0, Qt::AlignCenter);

    m_mainLayout->addWidget(m_dropZone);
}

void FluentFilePicker::setupFileList() {
    m_fileListContainer = new QWidget(this);
    m_fileListContainer->setObjectName("fileListContainer");

    auto* fileListLayout = new QVBoxLayout(m_fileListContainer);
    fileListLayout->setContentsMargins(0, 0, 0, 0);

    auto* fileListLabel = new QLabel("Selected Files:", m_fileListContainer);
    fileListLabel->setObjectName("fileListLabel");
    fileListLayout->addWidget(fileListLabel);

    m_fileList = new QListWidget(m_fileListContainer);
    m_fileList->setObjectName("fileList");
    m_fileList->setMaximumHeight(200);
    fileListLayout->addWidget(m_fileList);

    m_fileListContainer->setVisible(false);
    m_mainLayout->addWidget(m_fileListContainer);
}

void FluentFilePicker::setupUploadControls() {
    m_uploadControls = new QWidget(this);
    m_uploadControls->setObjectName("uploadControls");

    m_uploadLayout = new QHBoxLayout(m_uploadControls);

    m_uploadButton = new QPushButton("Upload Files", m_uploadControls);
    m_uploadButton->setObjectName("uploadButton");
    connect(m_uploadButton, &QPushButton::clicked, this,
            &FluentFilePicker::onUploadClicked);
    m_uploadLayout->addWidget(m_uploadButton);

    m_cancelButton = new QPushButton("Cancel", m_uploadControls);
    m_cancelButton->setObjectName("cancelButton");
    m_cancelButton->setVisible(false);
    connect(m_cancelButton, &QPushButton::clicked, this,
            &FluentFilePicker::onCancelUploadClicked);
    m_uploadLayout->addWidget(m_cancelButton);

    m_uploadProgress = new QProgressBar(m_uploadControls);
    m_uploadProgress->setObjectName("uploadProgress");
    m_uploadProgress->setVisible(false);
    m_uploadLayout->addWidget(m_uploadProgress);

    m_uploadStatus = new QLabel(m_uploadControls);
    m_uploadStatus->setObjectName("uploadStatus");
    m_uploadStatus->setVisible(false);
    m_uploadLayout->addWidget(m_uploadStatus);

    m_uploadLayout->addStretch();

    m_uploadControls->setVisible(false);
    m_mainLayout->addWidget(m_uploadControls);
}

void FluentFilePicker::updateUI() {
    // Update drop zone visibility
    bool showDropZone = m_files.isEmpty() || m_allowMultipleFiles;
    m_dropZone->setVisible(showDropZone);

    // Update file list visibility
    m_fileListContainer->setVisible(!m_files.isEmpty());

    // Update upload controls visibility
    bool showUploadControls = !m_files.isEmpty() && m_uploadFunction;
    m_uploadControls->setVisible(showUploadControls);

    // Update browse button text
    if (m_mode == PickerMode::Directories) {
        m_browseButton->setText("Browse Directories");
    } else if (m_mode == PickerMode::Both) {
        m_browseButton->setText("Browse Files & Directories");
    } else {
        m_browseButton->setText("Browse Files");
    }
}

void FluentFilePicker::updateDropZone() {
    if (m_dragActive) {
        m_dropZone->setStyleSheet(
            "QWidget#dropZone { border: 2px dashed #0078d4; background-color: "
            "rgba(0, 120, 212, 0.1); }");
    } else {
        m_dropZone->setStyleSheet(
            "QWidget#dropZone { border: 2px dashed #cccccc; background-color: "
            "#f9f9f9; }");
    }
}

void FluentFilePicker::updateFileList() {
    m_fileList->clear();

    for (int i = 0; i < m_files.size(); ++i) {
        const auto& fileInfo = m_files[i];

        auto* item = new QListWidgetItem();
        auto* itemWidget = new QWidget();
        auto* itemLayout = new QHBoxLayout(itemWidget);
        itemLayout->setContentsMargins(5, 5, 5, 5);

        // File icon
        auto* iconLabel = new QLabel();
        iconLabel->setPixmap(fileInfo.icon.pixmap(24, 24));
        itemLayout->addWidget(iconLabel);

        // File info
        auto* infoLayout = new QVBoxLayout();
        auto* nameLabel = new QLabel(fileInfo.fileName);
        nameLabel->setStyleSheet("font-weight: bold;");
        infoLayout->addWidget(nameLabel);

        auto* sizeLabel = new QLabel(formatFileSize(fileInfo.fileSize));
        sizeLabel->setStyleSheet("color: gray; font-size: 11px;");
        infoLayout->addWidget(sizeLabel);

        itemLayout->addLayout(infoLayout);
        itemLayout->addStretch();

        // Remove button
        auto* removeButton = new QPushButton("×");
        removeButton->setFixedSize(24, 24);
        removeButton->setProperty("fileIndex", i);
        connect(removeButton, &QPushButton::clicked, this,
                &FluentFilePicker::onRemoveFileClicked);
        itemLayout->addWidget(removeButton);

        item->setSizeHint(itemWidget->sizeHint());
        m_fileList->addItem(item);
        m_fileList->setItemWidget(item, itemWidget);
    }
}

void FluentFilePicker::updateUploadControls() {
    bool hasFiles = !m_files.isEmpty();
    bool hasUploadFunction = static_cast<bool>(m_uploadFunction);

    m_uploadButton->setVisible(hasFiles && hasUploadFunction && !m_uploading);
    m_cancelButton->setVisible(m_uploading);
    m_uploadProgress->setVisible(m_uploading);
    m_uploadStatus->setVisible(m_uploading);

    if (m_uploading) {
        m_uploadProgress->setRange(0, 100);
        updateUploadProgress();
    }
}

bool FluentFilePicker::validateFile(const FluentFileInfo& fileInfo) {
    // Check file size
    if (m_maxFileSize > 0 && fileInfo.fileSize > m_maxFileSize) {
        showValidationError(QString("File '%1' exceeds maximum size limit (%2)")
                                .arg(fileInfo.fileName)
                                .arg(formatFileSize(m_maxFileSize)));
        return false;
    }

    // Check accepted types
    if (!m_acceptedTypes.isEmpty() && !isAcceptedType(fileInfo.fileName)) {
        showValidationError(
            QString("File type not accepted: %1").arg(fileInfo.fileName));
        return false;
    }

    // Check max files
    if (m_files.size() >= m_maxFiles) {
        showValidationError(
            QString("Maximum number of files (%1) reached").arg(m_maxFiles));
        return false;
    }

    // Custom validation
    if (m_fileValidator && !m_fileValidator(fileInfo)) {
        showValidationError(
            m_customErrorMessage.isEmpty()
                ? QString("File validation failed: %1").arg(fileInfo.fileName)
                : m_customErrorMessage);
        return false;
    }

    return true;
}

bool FluentFilePicker::isAcceptedType(const QString& fileName) {
    if (m_acceptedTypes.isEmpty()) {
        return true;
    }

    QFileInfo fileInfo(fileName);
    QString extension = fileInfo.suffix().toLower();

    return m_acceptedTypes.contains(extension, Qt::CaseInsensitive);
}

FluentFileInfo FluentFilePicker::createFileInfo(const QString& filePath) {
    FluentFileInfo fileInfo;
    QFileInfo qFileInfo(filePath);

    if (!qFileInfo.exists()) {
        fileInfo.isValid = false;
        return fileInfo;
    }

    fileInfo.fileName = qFileInfo.fileName();
    fileInfo.filePath = qFileInfo.absoluteFilePath();
    fileInfo.fileSize = qFileInfo.size();
    fileInfo.lastModified = qFileInfo.lastModified();

    // Get MIME type
    QMimeDatabase mimeDb;
    QMimeType mimeType = mimeDb.mimeTypeForFile(qFileInfo);
    fileInfo.mimeType = mimeType.name();

    // Get file icon
    fileInfo.icon = style()->standardIcon(QStyle::SP_FileIcon);

    return fileInfo;
}

void FluentFilePicker::addFileInternal(const FluentFileInfo& fileInfo) {
    if (!validateFile(fileInfo)) {
        return;
    }

    // Check for duplicates
    for (const auto& existingFile : m_files) {
        if (existingFile.filePath == fileInfo.filePath) {
            return;  // File already added
        }
    }

    m_files.append(fileInfo);
    animateFileAddition(m_files.size() - 1);
    updateFileList();
    updateUI();

    emit filesAdded({fileInfo});
}

void FluentFilePicker::animateFileAddition(int index) {
    Q_UNUSED(index)
    // Simple animation - could be enhanced
    if (m_dropZoneAnimation) {
        m_dropZoneAnimation->setStartValue(0.7);
        m_dropZoneAnimation->setEndValue(1.0);
        m_dropZoneAnimation->start();
    }
}

void FluentFilePicker::animateFileRemoval(int index) {
    Q_UNUSED(index)
    // Simple animation - could be enhanced
    if (m_dropZoneAnimation) {
        m_dropZoneAnimation->setStartValue(1.0);
        m_dropZoneAnimation->setEndValue(0.7);
        m_dropZoneAnimation->start();
    }
}

void FluentFilePicker::showValidationError(const QString& error) {
    emit validationFailed(error);
    // Could also show a tooltip or status message
}

void FluentFilePicker::setDragActive(bool active) {
    if (m_dragActive != active) {
        m_dragActive = active;
        updateDropZone();
        repaint();
    }
}

void FluentFilePicker::processDraggedFiles(const QList<QUrl>& urls) {
    QStringList filePaths;
    for (const QUrl& url : urls) {
        if (url.isLocalFile()) {
            QString filePath = url.toLocalFile();
            QFileInfo fileInfo(filePath);

            if (m_mode == PickerMode::Files && fileInfo.isFile()) {
                filePaths.append(filePath);
            } else if (m_mode == PickerMode::Directories && fileInfo.isDir()) {
                filePaths.append(filePath);
            } else if (m_mode == PickerMode::Both) {
                filePaths.append(filePath);
            }
        }
    }

    if (!filePaths.isEmpty()) {
        addFiles(filePaths);
    }
}

void FluentFilePicker::uploadNextFile() {
    if (!m_uploading || m_currentUploadIndex >= m_files.size()) {
        // Upload completed
        m_uploading = false;
        updateUploadControls();
        emit uploadCompleted(m_files);
        return;
    }

    const FluentFileInfo& fileInfo = m_files[m_currentUploadIndex];

    // Create progress callback
    auto progressCallback = [this](const FluentUploadProgress& progress) {
        if (m_currentUploadIndex < m_uploadProgresses.size()) {
            m_uploadProgresses[m_currentUploadIndex] = progress;
            updateUploadProgress();

            if (progress.completed || progress.error) {
                if (progress.error) {
                    emit uploadFailed(progress.errorMessage);
                    m_uploading = false;
                    updateUploadControls();
                } else {
                    m_currentUploadIndex++;
                    QTimer::singleShot(100, this,
                                       &FluentFilePicker::uploadNextFile);
                }
            }
        }
    };

    // Start upload
    m_uploadFunction(fileInfo, progressCallback);
}

QString FluentFilePicker::formatFileSize(qint64 bytes) {
    const qint64 KB = 1024;
    const qint64 MB = KB * 1024;
    const qint64 GB = MB * 1024;

    if (bytes >= GB) {
        return QString::number(double(bytes) / GB, 'f', 2) + " GB";
    } else if (bytes >= MB) {
        return QString::number(double(bytes) / MB, 'f', 2) + " MB";
    } else if (bytes >= KB) {
        return QString::number(double(bytes) / KB, 'f', 2) + " KB";
    } else {
        return QString::number(bytes) + " bytes";
    }
}

// FluentFilePreview Implementation
FluentFilePreview::FluentFilePreview(QWidget* parent) : QWidget(parent) {
    setMinimumSize(m_previewSize);
}

void FluentFilePreview::setFile(const FluentFileInfo& fileInfo) {
    m_fileInfo = fileInfo;
    updatePreview();
}

void FluentFilePreview::clearPreview() {
    m_fileInfo = FluentFileInfo();
    m_previewPixmap = QPixmap();
    update();
}

void FluentFilePreview::setPreviewSize(const QSize& size) {
    m_previewSize = size;
    setMinimumSize(size);
    updatePreview();
}

void FluentFilePreview::setShowMetadata(bool show) {
    m_showMetadata = show;
    update();
}

void FluentFilePreview::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (!m_previewPixmap.isNull()) {
        QRect targetRect = rect();
        QRect sourceRect = m_previewPixmap.rect();

        // Scale to fit while maintaining aspect ratio
        QSize scaledSize =
            sourceRect.size().scaled(targetRect.size(), Qt::KeepAspectRatio);
        QRect drawRect((targetRect.width() - scaledSize.width()) / 2,
                       (targetRect.height() - scaledSize.height()) / 2,
                       scaledSize.width(), scaledSize.height());

        painter.drawPixmap(drawRect, m_previewPixmap);

        // Draw metadata if enabled
        if (m_showMetadata && !m_fileInfo.fileName.isEmpty()) {
            painter.setPen(Qt::black);
            painter.drawText(rect().adjusted(5, 5, -5, -5),
                             Qt::AlignBottom | Qt::TextWordWrap,
                             QString("%1\n%2")
                                 .arg(m_fileInfo.fileName)
                                 .arg(FluentFilePicker::formatFileSize(
                                     m_fileInfo.fileSize)));
        }
    } else {
        // Draw placeholder
        painter.setPen(Qt::gray);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(rect().adjusted(10, 10, -10, -10));

        painter.setPen(Qt::darkGray);
        painter.drawText(rect(), Qt::AlignCenter, "No Preview");
    }
}

void FluentFilePreview::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit previewClicked();
    }
    QWidget::mousePressEvent(event);
}

void FluentFilePreview::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit previewDoubleClicked();
    }
    QWidget::mouseDoubleClickEvent(event);
}

void FluentFilePreview::updatePreview() {
    if (m_fileInfo.fileName.isEmpty()) {
        clearPreview();
        return;
    }

    QString mimeType = m_fileInfo.mimeType;
    if (mimeType.startsWith("image/")) {
        loadImagePreview();
    } else if (mimeType.startsWith("text/") || mimeType.contains("document")) {
        loadDocumentPreview();
    } else {
        loadGenericPreview();
    }
}

void FluentFilePreview::loadImagePreview() {
    QImageReader reader(m_fileInfo.filePath);
    if (reader.canRead()) {
        reader.setScaledSize(m_previewSize);
        QImage image = reader.read();
        if (!image.isNull()) {
            m_previewPixmap = QPixmap::fromImage(image);
            update();
        }
    }
}

void FluentFilePreview::loadDocumentPreview() {
    // For now, just show a document icon
    loadGenericPreview();
}

void FluentFilePreview::loadGenericPreview() {
    // Create a generic file icon preview
    m_previewPixmap = QPixmap(m_previewSize);
    m_previewPixmap.fill(Qt::white);

    QPainter painter(&m_previewPixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw file icon
    QIcon fileIcon = qApp->style()->standardIcon(QStyle::SP_FileIcon);
    QRect iconRect = m_previewPixmap.rect().adjusted(20, 20, -20, -20);
    fileIcon.paint(&painter, iconRect);

    update();
}

// FluentBatchFileOperations Implementation
FluentBatchFileOperations::FluentBatchFileOperations(QWidget* parent)
    : QWidget(parent) {
    setupUI();
}

void FluentBatchFileOperations::setFiles(const QList<FluentFileInfo>& files) {
    m_files = files;
    updateOperationsList();
}

void FluentBatchFileOperations::addOperation(
    const QString& name,
    std::function<void(const QList<FluentFileInfo>&)> operation) {
    m_operations[name] = operation;
    updateOperationsList();
}

void FluentBatchFileOperations::executeOperation(const QString& name) {
    if (m_operations.contains(name) && !m_files.isEmpty()) {
        emit operationStarted(name);
        try {
            m_operations[name](m_files);
            emit operationCompleted(name);
        } catch (const std::exception& e) {
            emit operationFailed(name, QString::fromStdString(e.what()));
        }
    }
}

void FluentBatchFileOperations::executeAllOperations() {
    for (auto it = m_operations.begin(); it != m_operations.end(); ++it) {
        executeOperation(it.key());
    }
    emit allOperationsCompleted();
}

void FluentBatchFileOperations::setupUI() {
    m_layout = new QVBoxLayout(this);

    auto* titleLabel = new QLabel("Batch Operations", this);
    titleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    m_layout->addWidget(titleLabel);

    m_operationsList = new QListWidget(this);
    m_layout->addWidget(m_operationsList);

    m_executeAllButton = new QPushButton("Execute All Operations", this);
    connect(m_executeAllButton, &QPushButton::clicked, this,
            &FluentBatchFileOperations::executeAllOperations);
    m_layout->addWidget(m_executeAllButton);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);
    m_layout->addWidget(m_progressBar);
}

void FluentBatchFileOperations::updateOperationsList() {
    m_operationsList->clear();

    for (auto it = m_operations.begin(); it != m_operations.end(); ++it) {
        auto* item = new QListWidgetItem(it.key());
        item->setData(Qt::UserRole, it.key());
        m_operationsList->addItem(item);
    }

    m_executeAllButton->setEnabled(!m_operations.isEmpty() &&
                                   !m_files.isEmpty());
}

}  // namespace FluentQt::Components
