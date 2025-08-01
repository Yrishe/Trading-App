#include "LoginDialog.h"
#include <QApplication>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QThread>
#include <QMovie>
#include <QPixmap>
#include <QIcon>
#include <QTimer>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , mainLayout(nullptr)
    , tabWidget(nullptr)
    , loginTab(nullptr)
    , registerTab(nullptr)
    , loginUsernameEdit(nullptr)
    , loginPasswordEdit(nullptr)
    , loginButton(nullptr)
    , showPasswordCheck(nullptr)
    , loginErrorLabel(nullptr)
    , registerUsernameEdit(nullptr)
    , registerEmailEdit(nullptr)
    , registerPasswordEdit(nullptr)
    , confirmPasswordEdit(nullptr)
    , registerButton(nullptr)
    , registerErrorLabel(nullptr)
    , passwordRequirementsLabel(nullptr)
    , progressBar(nullptr)
    , statusLabel(nullptr)
    , userManager(std::make_unique<UserManager>())
{
    setWindowTitle("Trading App - Login");
    setFixedSize(450, 600);
    setModal(true);
    
    // Initialize user manager
    if (!userManager->initialize()) {
        QMessageBox::critical(this, "Error", "Failed to initialize authentication system.");
        reject();
        return;
    }
    
    setupUI();
    connectSignals();
    
    // Set focus to username field
    loginUsernameEdit->setFocus();
}

LoginDialog::~LoginDialog()
{
}

void LoginDialog::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);
    
    // Title
    QLabel *titleLabel = new QLabel("<h2>Trading Desktop App</h2>");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: #2c3e50; font-weight: bold;");
    mainLayout->addWidget(titleLabel);
    
    // Subtitle
    QLabel *subtitleLabel = new QLabel("Secure cryptocurrency trading platform");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("color: #7f8c8d; font-size: 12px;");
    mainLayout->addWidget(subtitleLabel);
    
    mainLayout->addSpacing(10);
    
    // Tab widget
    tabWidget = new QTabWidget(this);
    tabWidget->setStyleSheet(R"(
        QTabWidget::pane {
            border: 1px solid #bdc3c7;
            border-radius: 5px;
        }
        QTabBar::tab {
            background: #ecf0f1;
            padding: 10px 20px;
            margin-right: 2px;
            border-top-left-radius: 5px;
            border-top-right-radius: 5px;
        }
        QTabBar::tab:selected {
            background: #3498db;
            color: white;
        }
    )");
    
    setupLoginTab();
    setupRegisterTab();
    
    tabWidget->addTab(loginTab, "Login");
    tabWidget->addTab(registerTab, "Register");
    
    mainLayout->addWidget(tabWidget);
    
    // Progress bar
    progressBar = new QProgressBar(this);
    progressBar->setVisible(false);
    progressBar->setStyleSheet(R"(
        QProgressBar {
            border: 1px solid #bdc3c7;
            border-radius: 5px;
            text-align: center;
        }
        QProgressBar::chunk {
            background-color: #3498db;
            border-radius: 5px;
        }
    )");
    mainLayout->addWidget(progressBar);
    
    // Status label
    statusLabel = new QLabel(this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setVisible(false);
    mainLayout->addWidget(statusLabel);
}

void LoginDialog::setupLoginTab()
{
    loginTab = new QWidget();
    loginLayout = new QFormLayout(loginTab);
    loginLayout->setSpacing(15);
    
    // Username/Email field
    loginUsernameEdit = new QLineEdit();
    loginUsernameEdit->setPlaceholderText("Username or Email");
    loginUsernameEdit->setStyleSheet(R"(
        QLineEdit {
            padding: 10px;
            border: 2px solid #bdc3c7;
            border-radius: 5px;
            font-size: 14px;
        }
        QLineEdit:focus {
            border-color: #3498db;
        }
    )");
    loginLayout->addRow("Username/Email:", loginUsernameEdit);
    
    // Password field
    loginPasswordEdit = new QLineEdit();
    loginPasswordEdit->setPlaceholderText("Password");
    loginPasswordEdit->setEchoMode(QLineEdit::Password);
    loginPasswordEdit->setStyleSheet(loginUsernameEdit->styleSheet());
    loginLayout->addRow("Password:", loginPasswordEdit);
    
    // Show password checkbox
    showPasswordCheck = new QCheckBox("Show password");
    loginLayout->addRow("", showPasswordCheck);
    
    // Error label
    loginErrorLabel = new QLabel();
    loginErrorLabel->setStyleSheet("color: #e74c3c; font-weight: bold;");
    loginErrorLabel->setVisible(false);
    loginLayout->addRow(loginErrorLabel);
    
    // Login button
    loginButton = new QPushButton("Login");
    loginButton->setEnabled(false);
    loginButton->setStyleSheet(R"(
        QPushButton {
            background-color: #3498db;
            color: white;
            border: none;
            padding: 12px;
            border-radius: 5px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #2980b9;
        }
        QPushButton:disabled {
            background-color: #bdc3c7;
        }
    )");
    loginLayout->addRow(loginButton);
}

void LoginDialog::setupRegisterTab()
{
    registerTab = new QWidget();
    registerLayout = new QFormLayout(registerTab);
    registerLayout->setSpacing(15);
    
    // Username field
    registerUsernameEdit = new QLineEdit();
    registerUsernameEdit->setPlaceholderText("Choose a username");
    registerUsernameEdit->setStyleSheet(loginUsernameEdit->styleSheet());
    registerLayout->addRow("Username:", registerUsernameEdit);
    
    // Email field
    registerEmailEdit = new QLineEdit();
    registerEmailEdit->setPlaceholderText("your.email@example.com");
    registerEmailEdit->setStyleSheet(loginUsernameEdit->styleSheet());
    registerLayout->addRow("Email:", registerEmailEdit);
    
    // Password field
    registerPasswordEdit = new QLineEdit();
    registerPasswordEdit->setPlaceholderText("Create a strong password");
    registerPasswordEdit->setEchoMode(QLineEdit::Password);
    registerPasswordEdit->setStyleSheet(loginUsernameEdit->styleSheet());
    registerLayout->addRow("Password:", registerPasswordEdit);
    
    // Confirm password field
    confirmPasswordEdit = new QLineEdit();
    confirmPasswordEdit->setPlaceholderText("Confirm your password");
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    confirmPasswordEdit->setStyleSheet(loginUsernameEdit->styleSheet());
    registerLayout->addRow("Confirm Password:", confirmPasswordEdit);
    
    // Password requirements
    passwordRequirementsLabel = new QLabel(
        "<small>Password must contain:<br>"
        "• At least 8 characters<br>"
        "• One uppercase letter<br>"
        "• One lowercase letter<br>"
        "• One number</small>"
    );
    passwordRequirementsLabel->setStyleSheet("color: #7f8c8d;");
    registerLayout->addRow(passwordRequirementsLabel);
    
    // Error label
    registerErrorLabel = new QLabel();
    registerErrorLabel->setStyleSheet("color: #e74c3c; font-weight: bold;");
    registerErrorLabel->setVisible(false);
    registerLayout->addRow(registerErrorLabel);
    
    // Register button
    registerButton = new QPushButton("Create Account");
    registerButton->setEnabled(false);
    registerButton->setStyleSheet(R"(
        QPushButton {
            background-color: #27ae60;
            color: white;
            border: none;
            padding: 12px;
            border-radius: 5px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #229954;
        }
        QPushButton:disabled {
            background-color: #bdc3c7;
        }
    )");
    registerLayout->addRow(registerButton);
}

void LoginDialog::connectSignals()
{
    // Login tab signals
    connect(loginUsernameEdit, &QLineEdit::textChanged, this, &LoginDialog::validateLoginForm);
    connect(loginPasswordEdit, &QLineEdit::textChanged, this, &LoginDialog::validateLoginForm);
    connect(loginPasswordEdit, &QLineEdit::returnPressed, this, &LoginDialog::onLogin);
    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::onLogin);
    connect(showPasswordCheck, &QCheckBox::toggled, this, &LoginDialog::onShowPassword);
    
    // Register tab signals
    connect(registerUsernameEdit, &QLineEdit::textChanged, this, &LoginDialog::validateRegisterForm);
    connect(registerEmailEdit, &QLineEdit::textChanged, this, &LoginDialog::validateRegisterForm);
    connect(registerPasswordEdit, &QLineEdit::textChanged, this, &LoginDialog::validateRegisterForm);
    connect(registerPasswordEdit, &QLineEdit::textChanged, this, &LoginDialog::updatePasswordStrength);
    connect(confirmPasswordEdit, &QLineEdit::textChanged, this, &LoginDialog::validateRegisterForm);
    connect(confirmPasswordEdit, &QLineEdit::returnPressed, this, &LoginDialog::onRegister);
    connect(registerButton, &QPushButton::clicked, this, &LoginDialog::onRegister);
    
    // Tab change signal
    connect(tabWidget, &QTabWidget::currentChanged, this, &LoginDialog::onTabChanged);
}

void LoginDialog::onLogin()
{
    if (!isLoginFormValid()) {
        return;
    }
    
    setLoading(true);
    loginErrorLabel->setVisible(false);
    
    QString username = loginUsernameEdit->text().trimmed();
    QString password = loginPasswordEdit->text();
    
    // Check if user exists first
    bool userExists = userManager->userExists(username.toStdString()) || userManager->emailExists(username.toStdString());
    
    if (!userExists) {
        setLoading(false);
        showError("Account does not exist. Please check your username/email or register a new account.");
        loginPasswordEdit->clear();
        loginPasswordEdit->setFocus();
        return;
    }
    
    // Authenticate user
    authenticatedUser = userManager->authenticateUser(username.toStdString(), password.toStdString());
    
    setLoading(false);
    
    if (authenticatedUser) {
        showSuccess("Login successful!");
        QTimer::singleShot(500, this, &QDialog::accept);
    } else {
        showError("Invalid password. Please try again.");
        loginPasswordEdit->clear();
        loginPasswordEdit->setFocus();
    }
}

void LoginDialog::onRegister()
{
    if (!isRegisterFormValid()) {
        return;
    }
    
    setLoading(true);
    registerErrorLabel->setVisible(false);
    
    QString username = registerUsernameEdit->text().trimmed();
    QString email = registerEmailEdit->text().trimmed();
    QString password = registerPasswordEdit->text();
    
    // Register user
    bool success = userManager->registerUser(username.toStdString(), email.toStdString(), password.toStdString());
    
    setLoading(false);
    
    if (success) {
        showSuccess("Account created successfully! Please login.");
        
        // Switch to login tab and fill username
        tabWidget->setCurrentIndex(0);
        loginUsernameEdit->setText(username);
        loginPasswordEdit->setFocus();
        
        // Clear register form
        registerUsernameEdit->clear();
        registerEmailEdit->clear();
        registerPasswordEdit->clear();
        confirmPasswordEdit->clear();
    } else {
        if (userManager->userExists(username.toStdString())) {
            showError("Username already exists. Please choose a different username.");
        } else if (userManager->emailExists(email.toStdString())) {
            showError("Email already registered. Please use a different email.");
        } else {
            showError("Registration failed. Please try again.");
        }
    }
}

void LoginDialog::onTabChanged(int index)
{
    // Clear error messages when switching tabs
    loginErrorLabel->setVisible(false);
    registerErrorLabel->setVisible(false);
    statusLabel->setVisible(false);
    
    // Set focus to appropriate field
    if (index == 0) { // Login tab
        loginUsernameEdit->setFocus();
    } else { // Register tab
        registerUsernameEdit->setFocus();
    }
}

void LoginDialog::onShowPassword(bool show)
{
    loginPasswordEdit->setEchoMode(show ? QLineEdit::Normal : QLineEdit::Password);
}

void LoginDialog::validateLoginForm()
{
    bool valid = isLoginFormValid();
    loginButton->setEnabled(valid);
    
    if (loginErrorLabel->isVisible() && valid) {
        loginErrorLabel->setVisible(false);
    }
}

void LoginDialog::validateRegisterForm()
{
    bool valid = isRegisterFormValid();
    registerButton->setEnabled(valid);
    
    if (registerErrorLabel->isVisible() && valid) {
        registerErrorLabel->setVisible(false);
    }
}

bool LoginDialog::isLoginFormValid() const
{
    return !loginUsernameEdit->text().trimmed().isEmpty() && 
           !loginPasswordEdit->text().isEmpty();
}

bool LoginDialog::isRegisterFormValid() const
{
    QString username = registerUsernameEdit->text().trimmed();
    QString email = registerEmailEdit->text().trimmed();
    QString password = registerPasswordEdit->text();
    QString confirmPassword = confirmPasswordEdit->text();
    
    return !username.isEmpty() && 
           username.length() >= 3 &&
           UserManager::isValidEmail(email.toStdString()) &&
           UserManager::isValidPassword(password.toStdString()) &&
           password == confirmPassword;
}

void LoginDialog::updatePasswordStrength()
{
    QString password = registerPasswordEdit->text();
    QString strengthText = getPasswordStrengthText(password);
    
    if (!password.isEmpty()) {
        passwordRequirementsLabel->setText(strengthText);
    } else {
        passwordRequirementsLabel->setText(
            "<small>Password must contain:<br>"
            "• At least 8 characters<br>"
            "• One uppercase letter<br>"
            "• One lowercase letter<br>"
            "• One number</small>"
        );
    }
}

QString LoginDialog::getPasswordStrengthText(const QString &password) const
{
    QString text = "<small>Password requirements:<br>";
    
    // Length check
    if (password.length() >= 8) {
        text += "<span style='color: #27ae60;'>✓ At least 8 characters</span><br>";
    } else {
        text += "<span style='color: #e74c3c;'>✗ At least 8 characters</span><br>";
    }
    
    // Uppercase check
    if (password.contains(QRegularExpression("[A-Z]"))) {
        text += "<span style='color: #27ae60;'>✓ One uppercase letter</span><br>";
    } else {
        text += "<span style='color: #e74c3c;'>✗ One uppercase letter</span><br>";
    }
    
    // Lowercase check
    if (password.contains(QRegularExpression("[a-z]"))) {
        text += "<span style='color: #27ae60;'>✓ One lowercase letter</span><br>";
    } else {
        text += "<span style='color: #e74c3c;'>✗ One lowercase letter</span><br>";
    }
    
    // Number check
    if (password.contains(QRegularExpression("[0-9]"))) {
        text += "<span style='color: #27ae60;'>✓ One number</span>";
    } else {
        text += "<span style='color: #e74c3c;'>✗ One number</span>";
    }
    
    text += "</small>";
    return text;
}

void LoginDialog::showError(const QString &message)
{
    if (tabWidget->currentIndex() == 0) {
        loginErrorLabel->setText(message);
        loginErrorLabel->setVisible(true);
    } else {
        registerErrorLabel->setText(message);
        registerErrorLabel->setVisible(true);
    }
}

void LoginDialog::showSuccess(const QString &message)
{
    statusLabel->setText(QString("<span style='color: #27ae60; font-weight: bold;'>%1</span>").arg(message));
    statusLabel->setVisible(true);
}

void LoginDialog::setLoading(bool loading)
{
    loginButton->setEnabled(!loading && isLoginFormValid());
    registerButton->setEnabled(!loading && isRegisterFormValid());
    
    if (loading) {
        progressBar->setRange(0, 0); // Indeterminate progress
        progressBar->setVisible(true);
        statusLabel->setText("Processing...");
        statusLabel->setVisible(true);
    } else {
        progressBar->setVisible(false);
    }
}