// include/FluentQt/Components/FluentContentDialog.h
#pragma once

#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QVBoxLayout>
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

enum class FluentDialogResult { None, Primary, Secondary, Close };

class FluentContentDialog : public QDialog {
    Q_OBJECT

public:
    explicit FluentContentDialog(QWidget* parent = nullptr);

    // Content
    void setTitle(const QString& title);
    QString title() const;

    void setContent(const QString& content);
    QString content() const;

    void setContentWidget(QWidget* widget);
    QWidget* contentWidget() const;

    // Buttons
    void setPrimaryButtonText(const QString& text);
    QString primaryButtonText() const;

    void setSecondaryButtonText(const QString& text);
    QString secondaryButtonText() const;

    void setCloseButtonText(const QString& text);
    QString closeButtonText() const;

    void setPrimaryButtonEnabled(bool enabled);
    void setSecondaryButtonEnabled(bool enabled);

    // Result
    FluentDialogResult result() const { return m_result; }

    // Static convenience methods
    static FluentDialogResult showMessageDialog(
        QWidget* parent, const QString& title, const QString& message,
        const QString& primaryButton = "OK",
        const QString& secondaryButton = "",
        const QString& closeButton = "Cancel");

    static FluentDialogResult showConfirmDialog(
        QWidget* parent, const QString& title, const QString& message,
        const QString& confirmButton = "Yes",
        const QString& cancelButton = "No");

protected:
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onPrimaryButtonClicked();
    void onSecondaryButtonClicked();
    void onCloseButtonClicked();

private:
    void setupUI();
    void setupAnimations();
    void updateTheme();
    void animateShow();
    void animateHide();

private:
    QVBoxLayout* m_mainLayout;
    QLabel* m_titleLabel;
    QLabel* m_contentLabel;
    QWidget* m_contentWidget;
    QHBoxLayout* m_buttonLayout;
    QPushButton* m_primaryButton;
    QPushButton* m_secondaryButton;
    QPushButton* m_closeButton;

    FluentDialogResult m_result{FluentDialogResult::None};
    std::unique_ptr<QPropertyAnimation> m_showAnimation;
    std::unique_ptr<QPropertyAnimation> m_hideAnimation;
};

}  // namespace FluentQt::Components
