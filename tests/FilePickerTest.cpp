// tests/FilePickerTest.cpp
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include "FluentQt/Components/FluentFilePicker.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt::Components;
using namespace FluentQt::Styling;

class FilePickerTestWidget : public QWidget {
    Q_OBJECT

public:
    explicit FilePickerTestWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        connectSignals();

        // Set window properties
        setWindowTitle("FluentFilePicker Test");
        setMinimumSize(1000, 800);

        // Apply Fluent theme
        FluentTheme::instance().setMode(FluentThemeMode::Light);
    }

private slots:
    void onFilesAdded(const QList<FluentFileInfo>& files) {
        m_statusLabel->setText(QString("Files added: %1").arg(files.size()));
        for (const auto& file : files) {
            m_logEdit->append(
                QString("Added: %1 (%2)")
                    .arg(file.fileName)
                    .arg(FluentFilePicker::formatFileSize(file.fileSize)));
        }
    }

    void onFileRemoved(const FluentFileInfo& file) {
        m_statusLabel->setText(QString("File removed: %1").arg(file.fileName));
        m_logEdit->append(QString("Removed: %1").arg(file.fileName));
    }

    void onFilesCleared() {
        m_statusLabel->setText("All files cleared");
        m_logEdit->append("All files cleared");
    }

    void onUploadStarted() {
        m_statusLabel->setText("Upload started");
        m_logEdit->append("Upload started");
    }

    void onUploadProgress(const FluentUploadProgress& progress) {
        m_statusLabel->setText(
            QString("Uploading %1: %2%")
                .arg(progress.fileName)
                .arg(QString::number(progress.percentage, 'f', 1)));

        if (m_uploadProgressBar) {
            m_uploadProgressBar->setValue(
                static_cast<int>(progress.percentage));
        }
    }

    void onUploadCompleted(const QList<FluentFileInfo>& files) {
        m_statusLabel->setText(
            QString("Upload completed: %1 files").arg(files.size()));
        m_logEdit->append(
            QString("Upload completed: %1 files").arg(files.size()));

        if (m_uploadProgressBar) {
            m_uploadProgressBar->setValue(100);
        }
    }

    void onUploadFailed(const QString& error) {
        m_statusLabel->setText(QString("Upload failed: %1").arg(error));
        m_logEdit->append(QString("Upload failed: %1").arg(error));
    }

    void onValidationFailed(const QString& error) {
        m_statusLabel->setText(QString("Validation failed: %1").arg(error));
        m_logEdit->append(QString("Validation failed: %1").arg(error));
    }

    void onThemeToggle() {
        auto& theme = FluentTheme::instance();
        auto currentMode = theme.mode();
        auto newMode = (currentMode == FluentThemeMode::Light)
                           ? FluentThemeMode::Dark
                           : FluentThemeMode::Light;
        theme.setMode(newMode);

        m_themeButton->setText(newMode == FluentThemeMode::Dark
                                   ? "Switch to Light Theme"
                                   : "Switch to Dark Theme");
    }

    void onModeChanged() {
        FluentFilePicker::PickerMode mode =
            static_cast<FluentFilePicker::PickerMode>(
                m_modeCombo->currentIndex());
        m_filePicker->setMode(mode);
        m_statusLabel->setText(
            QString("Mode changed to: %1").arg(m_modeCombo->currentText()));
    }

    void onMultipleFilesToggle(bool allow) {
        m_filePicker->setAllowMultipleFiles(allow);
        m_statusLabel->setText(allow ? "Multiple files enabled"
                                     : "Single file mode");
    }

    void onMaxFilesChanged(int max) {
        m_filePicker->setMaxFiles(max);
        m_statusLabel->setText(QString("Max files set to: %1").arg(max));
    }

    void onMaxFileSizeChanged(int sizeMB) {
        qint64 sizeBytes = static_cast<qint64>(sizeMB) * 1024 * 1024;
        m_filePicker->setMaxFileSize(sizeBytes);
        m_statusLabel->setText(
            QString("Max file size set to: %1 MB").arg(sizeMB));
    }

    void onAcceptedTypesChanged() {
        QString typesText = m_acceptedTypesEdit->text().trimmed();
        QStringList types;
        if (!typesText.isEmpty()) {
            types = typesText.split(',', Qt::SkipEmptyParts);
            for (QString& type : types) {
                type = type.trimmed();
            }
        }
        m_filePicker->setAcceptedTypes(types);
        m_statusLabel->setText(
            QString("Accepted types: %1").arg(types.join(", ")));
    }

    void onShowPreviewToggle(bool show) {
        m_filePicker->setShowPreview(show);
        m_filePreview->setVisible(show);
        m_statusLabel->setText(show ? "Preview enabled" : "Preview disabled");
    }

    void onClearFiles() { m_filePicker->clearFiles(); }

    void onSimulateUpload() {
        // Set up a mock upload function
        m_filePicker->setUploadFunction(
            [this](const FluentFileInfo& fileInfo,
                   std::function<void(const FluentUploadProgress&)>
                       progressCallback) {
                // Simulate upload with timer
                auto* timer = new QTimer(this);
                int progress = 0;

                connect(timer, &QTimer::timeout, [=]() mutable {
                    progress += 10;

                    FluentUploadProgress uploadProgress;
                    uploadProgress.fileName = fileInfo.fileName;
                    uploadProgress.bytesUploaded =
                        (fileInfo.fileSize * progress) / 100;
                    uploadProgress.totalBytes = fileInfo.fileSize;
                    uploadProgress.percentage = progress;
                    uploadProgress.status =
                        QString("Uploading... %1%").arg(progress);
                    uploadProgress.completed = (progress >= 100);

                    progressCallback(uploadProgress);

                    if (progress >= 100) {
                        timer->deleteLater();
                    }
                });

                timer->start(200);  // Update every 200ms
            });

        m_filePicker->uploadFiles();
    }

private:
    void setupUI() {
        auto* mainLayout = new QHBoxLayout(this);

        // Left panel - controls and file picker
        auto* leftPanel = new QWidget(this);
        leftPanel->setFixedWidth(600);
        setupLeftPanel(leftPanel);
        mainLayout->addWidget(leftPanel);

        // Right panel - preview and log
        auto* rightPanel = new QWidget(this);
        setupRightPanel(rightPanel);
        mainLayout->addWidget(rightPanel);
    }

    void setupLeftPanel(QWidget* parent) {
        auto* layout = new QVBoxLayout(parent);

        // Title
        auto* titleLabel = new QLabel("FluentFilePicker Component Test");
        titleLabel->setStyleSheet(
            "font-size: 18px; font-weight: bold; margin: 10px;");
        layout->addWidget(titleLabel);

        // Status label
        m_statusLabel =
            new QLabel("Drag & drop files or use controls to test file picker");
        m_statusLabel->setStyleSheet("color: blue; margin: 5px;");
        layout->addWidget(m_statusLabel);

        // Control panel
        setupControlPanel(layout);

        // File picker
        setupFilePicker(layout);

        layout->addStretch();
    }

    void setupRightPanel(QWidget* parent) {
        auto* layout = new QVBoxLayout(parent);

        // File preview
        auto* previewGroup = new QGroupBox("File Preview");
        auto* previewLayout = new QVBoxLayout(previewGroup);

        m_filePreview = new FluentFilePreview(this);
        m_filePreview->setMinimumSize(300, 200);
        previewLayout->addWidget(m_filePreview);

        layout->addWidget(previewGroup);

        // Upload progress
        auto* uploadGroup = new QGroupBox("Upload Progress");
        auto* uploadLayout = new QVBoxLayout(uploadGroup);

        m_uploadProgressBar = new QProgressBar(this);
        m_uploadProgressBar->setRange(0, 100);
        uploadLayout->addWidget(m_uploadProgressBar);

        auto* simulateUploadButton = new QPushButton("Simulate Upload");
        connect(simulateUploadButton, &QPushButton::clicked, this,
                &FilePickerTestWidget::onSimulateUpload);
        uploadLayout->addWidget(simulateUploadButton);

        layout->addWidget(uploadGroup);

        // Event log
        auto* logGroup = new QGroupBox("Event Log");
        auto* logLayout = new QVBoxLayout(logGroup);

        m_logEdit = new QTextEdit(this);
        m_logEdit->setMaximumHeight(200);
        m_logEdit->setReadOnly(true);
        logLayout->addWidget(m_logEdit);

        auto* clearLogButton = new QPushButton("Clear Log");
        connect(clearLogButton, &QPushButton::clicked, m_logEdit,
                &QTextEdit::clear);
        logLayout->addWidget(clearLogButton);

        layout->addWidget(logGroup);
    }

    void setupControlPanel(QVBoxLayout* mainLayout) {
        auto* controlGroup = new QGroupBox("Controls");
        auto* controlLayout = new QVBoxLayout(controlGroup);

        // First row of controls
        auto* row1Layout = new QHBoxLayout();

        m_themeButton = new QPushButton("Switch to Dark Theme");
        row1Layout->addWidget(m_themeButton);

        row1Layout->addSpacing(20);

        // Mode control
        row1Layout->addWidget(new QLabel("Mode:"));
        m_modeCombo = new QComboBox();
        m_modeCombo->addItems({"Files", "Directories", "Both"});
        row1Layout->addWidget(m_modeCombo);

        row1Layout->addStretch();
        controlLayout->addLayout(row1Layout);

        // Second row of controls
        auto* row2Layout = new QHBoxLayout();

        // Multiple files control
        m_multipleFilesCheckBox = new QCheckBox("Allow Multiple Files");
        m_multipleFilesCheckBox->setChecked(true);
        row2Layout->addWidget(m_multipleFilesCheckBox);

        row2Layout->addSpacing(20);

        // Max files control
        row2Layout->addWidget(new QLabel("Max Files:"));
        m_maxFilesSpinBox = new QSpinBox();
        m_maxFilesSpinBox->setRange(1, 100);
        m_maxFilesSpinBox->setValue(10);
        row2Layout->addWidget(m_maxFilesSpinBox);

        row2Layout->addStretch();
        controlLayout->addLayout(row2Layout);

        // Third row of controls
        auto* row3Layout = new QHBoxLayout();

        // Max file size control
        row3Layout->addWidget(new QLabel("Max Size (MB):"));
        m_maxFileSizeSpinBox = new QSpinBox();
        m_maxFileSizeSpinBox->setRange(1, 1000);
        m_maxFileSizeSpinBox->setValue(100);
        row3Layout->addWidget(m_maxFileSizeSpinBox);

        row3Layout->addSpacing(20);

        // Show preview control
        m_showPreviewCheckBox = new QCheckBox("Show Preview");
        m_showPreviewCheckBox->setChecked(true);
        row3Layout->addWidget(m_showPreviewCheckBox);

        row3Layout->addStretch();
        controlLayout->addLayout(row3Layout);

        // Fourth row - accepted types
        auto* row4Layout = new QHBoxLayout();

        row4Layout->addWidget(new QLabel("Accepted Types:"));
        m_acceptedTypesEdit = new QLineEdit();
        m_acceptedTypesEdit->setPlaceholderText(
            "e.g., jpg,png,pdf,txt (leave empty for all)");
        row4Layout->addWidget(m_acceptedTypesEdit);

        auto* applyTypesButton = new QPushButton("Apply");
        connect(applyTypesButton, &QPushButton::clicked, this,
                &FilePickerTestWidget::onAcceptedTypesChanged);
        row4Layout->addWidget(applyTypesButton);

        controlLayout->addLayout(row4Layout);

        // Fifth row - action buttons
        auto* row5Layout = new QHBoxLayout();

        auto* clearFilesButton = new QPushButton("Clear Files");
        connect(clearFilesButton, &QPushButton::clicked, this,
                &FilePickerTestWidget::onClearFiles);
        row5Layout->addWidget(clearFilesButton);

        row5Layout->addStretch();
        controlLayout->addLayout(row5Layout);

        mainLayout->addWidget(controlGroup);
    }

    void setupFilePicker(QVBoxLayout* mainLayout) {
        auto* filePickerGroup = new QGroupBox("File Picker");
        auto* filePickerLayout = new QVBoxLayout(filePickerGroup);

        m_filePicker = new FluentFilePicker(this);
        m_filePicker->setMinimumHeight(300);
        filePickerLayout->addWidget(m_filePicker);

        mainLayout->addWidget(filePickerGroup);
    }

    void connectSignals() {
        // File picker signals
        connect(m_filePicker, &FluentFilePicker::filesAdded, this,
                &FilePickerTestWidget::onFilesAdded);
        connect(m_filePicker, &FluentFilePicker::fileRemoved, this,
                &FilePickerTestWidget::onFileRemoved);
        connect(m_filePicker, &FluentFilePicker::filesCleared, this,
                &FilePickerTestWidget::onFilesCleared);
        connect(m_filePicker, &FluentFilePicker::uploadStarted, this,
                &FilePickerTestWidget::onUploadStarted);
        connect(m_filePicker, &FluentFilePicker::uploadProgress, this,
                &FilePickerTestWidget::onUploadProgress);
        connect(m_filePicker, &FluentFilePicker::uploadCompleted, this,
                &FilePickerTestWidget::onUploadCompleted);
        connect(m_filePicker, &FluentFilePicker::uploadFailed, this,
                &FilePickerTestWidget::onUploadFailed);
        connect(m_filePicker, &FluentFilePicker::validationFailed, this,
                &FilePickerTestWidget::onValidationFailed);

        // Control signals
        connect(m_themeButton, &QPushButton::clicked, this,
                &FilePickerTestWidget::onThemeToggle);
        connect(m_modeCombo,
                QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                &FilePickerTestWidget::onModeChanged);
        connect(m_multipleFilesCheckBox, &QCheckBox::toggled, this,
                &FilePickerTestWidget::onMultipleFilesToggle);
        connect(m_maxFilesSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                this, &FilePickerTestWidget::onMaxFilesChanged);
        connect(m_maxFileSizeSpinBox,
                QOverload<int>::of(&QSpinBox::valueChanged), this,
                &FilePickerTestWidget::onMaxFileSizeChanged);
        connect(m_showPreviewCheckBox, &QCheckBox::toggled, this,
                &FilePickerTestWidget::onShowPreviewToggle);
    }

private:
    // UI components
    QLabel* m_statusLabel{nullptr};
    QPushButton* m_themeButton{nullptr};
    QComboBox* m_modeCombo{nullptr};
    QCheckBox* m_multipleFilesCheckBox{nullptr};
    QSpinBox* m_maxFilesSpinBox{nullptr};
    QSpinBox* m_maxFileSizeSpinBox{nullptr};
    QCheckBox* m_showPreviewCheckBox{nullptr};
    QLineEdit* m_acceptedTypesEdit{nullptr};

    // File picker components
    FluentFilePicker* m_filePicker{nullptr};
    FluentFilePreview* m_filePreview{nullptr};

    // Results and log
    QProgressBar* m_uploadProgressBar{nullptr};
    QTextEdit* m_logEdit{nullptr};
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    FilePickerTestWidget window;
    window.show();

    return app.exec();
}

#include "FilePickerTest.moc"
