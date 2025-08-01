#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTabWidget>
#include <QCheckBox>
#include <QProgressBar>
#include <memory>

#include "../Auth/UserManager.h"

class LoginDialog : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(LoginDialog)

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();
    
    std::shared_ptr<User> getAuthenticatedUser() const { return authenticatedUser; }

private slots:
    void onLogin();
    void onRegister();
    void onTabChanged(int index);
    void onShowPassword(bool show);
    void validateLoginForm();
    void validateRegisterForm();

private:
    void setupUI();
    void setupLoginTab();
    void setupRegisterTab();
    void connectSignals();
    void showError(const QString &message);
    void showSuccess(const QString &message);
    void setLoading(bool loading);
    
    // UI Components
    QVBoxLayout *mainLayout;
    QTabWidget *tabWidget;
    
    // Login tab
    QWidget *loginTab;
    QFormLayout *loginLayout;
    QLineEdit *loginUsernameEdit;
    QLineEdit *loginPasswordEdit;
    QPushButton *loginButton;
    QCheckBox *showPasswordCheck;
    QLabel *loginErrorLabel;
    
    // Register tab
    QWidget *registerTab;
    QFormLayout *registerLayout;
    QLineEdit *registerUsernameEdit;
    QLineEdit *registerEmailEdit;
    QLineEdit *registerPasswordEdit;
    QLineEdit *confirmPasswordEdit;
    QPushButton *registerButton;
    QLabel *registerErrorLabel;
    QLabel *passwordRequirementsLabel;
    
    // Common
    QProgressBar *progressBar;
    QLabel *statusLabel;
    
    // Logic
    std::unique_ptr<UserManager> userManager;
    std::shared_ptr<User> authenticatedUser;
    
    // Validation
    bool isLoginFormValid() const;
    bool isRegisterFormValid() const;
    QString getPasswordStrengthText(const QString &password) const;
    void updatePasswordStrength();
};