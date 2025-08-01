#include "MainWindow.h"
#include "LoginDialog.h"
#include <QApplication>
#include <QMessageBox>
#include <QCloseEvent>
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QTimer>
#include <QDateTime>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QSplitter>
#include <QSettings>

const int MainWindow::MARKET_UPDATE_INTERVAL;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , centralWidget(nullptr)
    , mainLayout(nullptr)
    , tabWidget(nullptr)
    , dashboardWidget(nullptr)
    , tradingWidget(nullptr)
    , walletWidget(nullptr)
    , chartWidget(nullptr)
    , orderWidget(nullptr)
    , userLabel(nullptr)
    , pointsLabel(nullptr)
    , timeLabel(nullptr)
    , logoutButton(nullptr)
    , tradingEngine(std::make_unique<MerkelMain>())
    , marketTimer(new QTimer(this))
{
    setWindowTitle("Trading Desktop Application");
    setMinimumSize(1200, 800);
    resize(1400, 900);
    
    setupUI();
    setupMenuBar();
    setupStatusBar();
    connectSignals();
    
    // Load settings
    QSettings settings;
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
}

MainWindow::~MainWindow()
{
    // Save settings
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
}

void MainWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);
    
    setupTabs();
}

void MainWindow::setupTabs()
{
    tabWidget = new QTabWidget(this);
    tabWidget->setTabPosition(QTabWidget::North);
    tabWidget->setMovable(true);
    
    // Create dashboard widget
    setupDashboard();
    
    // Create tab widgets
    tradingWidget = new TradingWidget(this);
    walletWidget = new WalletWidget(this);
    chartWidget = new CandlestickChart(this);
    orderWidget = new OrderWidget(this);
    
    // Add tabs
    tabWidget->addTab(dashboardWidget, "Dashboard");
    tabWidget->addTab(tradingWidget, "Trading");
    tabWidget->addTab(chartWidget, "Charts");
    tabWidget->addTab(walletWidget, "Wallet");
    tabWidget->addTab(orderWidget, "Orders");
    
    mainLayout->addWidget(tabWidget);
}

void MainWindow::setupDashboard()
{
    dashboardWidget = new QWidget(this);
    QVBoxLayout *dashboardLayout = new QVBoxLayout(dashboardWidget);
    dashboardLayout->setContentsMargins(20, 20, 20, 20);
    dashboardLayout->setSpacing(20);
    
    // Welcome header
    QLabel *welcomeLabel = new QLabel("Welcome to Trading Desktop", dashboardWidget);
    welcomeLabel->setStyleSheet("QLabel { font-size: 24px; font-weight: bold; color: #42A5F5; margin-bottom: 10px; }");
    welcomeLabel->setAlignment(Qt::AlignCenter);
    dashboardLayout->addWidget(welcomeLabel);
    
    // User info section
    QWidget *userInfoWidget = new QWidget(dashboardWidget);
    QHBoxLayout *userInfoLayout = new QHBoxLayout(userInfoWidget);
    userInfoLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel *userInfoLabel = new QLabel("User Information", userInfoWidget);
    userInfoLabel->setStyleSheet("QLabel { font-size: 16px; font-weight: bold; }");
    userInfoLayout->addWidget(userInfoLabel);
    userInfoLayout->addStretch();
    
    dashboardLayout->addWidget(userInfoWidget);
    
    // Quick actions section
    QLabel *actionsLabel = new QLabel("Quick Actions", dashboardWidget);
    actionsLabel->setStyleSheet("QLabel { font-size: 18px; font-weight: bold; margin-top: 20px; }");
    dashboardLayout->addWidget(actionsLabel);
    
    // Action buttons grid
    QWidget *actionsWidget = new QWidget(dashboardWidget);
    QGridLayout *actionsGrid = new QGridLayout(actionsWidget);
    actionsGrid->setSpacing(15);
    
    // Create action buttons
    QPushButton *tradingBtn = new QPushButton("Start Trading", actionsWidget);
    tradingBtn->setMinimumHeight(60);
    tradingBtn->setStyleSheet("QPushButton { font-size: 14px; background-color: #4CAF50; color: white; border: none; border-radius: 8px; } QPushButton:hover { background-color: #45a049; }");
    connect(tradingBtn, &QPushButton::clicked, [this]() { tabWidget->setCurrentIndex(1); });
    
    QPushButton *walletBtn = new QPushButton("View Wallet", actionsWidget);
    walletBtn->setMinimumHeight(60);
    walletBtn->setStyleSheet("QPushButton { font-size: 14px; background-color: #2196F3; color: white; border: none; border-radius: 8px; } QPushButton:hover { background-color: #1976D2; }");
    connect(walletBtn, &QPushButton::clicked, [this]() { tabWidget->setCurrentIndex(3); });
    
    QPushButton *chartsBtn = new QPushButton("Market Charts", actionsWidget);
    chartsBtn->setMinimumHeight(60);
    chartsBtn->setStyleSheet("QPushButton { font-size: 14px; background-color: #FF9800; color: white; border: none; border-radius: 8px; } QPushButton:hover { background-color: #F57C00; }");
    connect(chartsBtn, &QPushButton::clicked, [this]() { tabWidget->setCurrentIndex(2); });
    
    QPushButton *ordersBtn = new QPushButton("Order History", actionsWidget);
    ordersBtn->setMinimumHeight(60);
    ordersBtn->setStyleSheet("QPushButton { font-size: 14px; background-color: #9C27B0; color: white; border: none; border-radius: 8px; } QPushButton:hover { background-color: #7B1FA2; }");
    connect(ordersBtn, &QPushButton::clicked, [this]() { tabWidget->setCurrentIndex(4); });
    
    // Add buttons to grid
    actionsGrid->addWidget(tradingBtn, 0, 0);
    actionsGrid->addWidget(walletBtn, 0, 1);
    actionsGrid->addWidget(chartsBtn, 1, 0);
    actionsGrid->addWidget(ordersBtn, 1, 1);
    
    dashboardLayout->addWidget(actionsWidget);
    
    // Account summary section
    QLabel *summaryLabel = new QLabel("Account Summary", dashboardWidget);
    summaryLabel->setStyleSheet("QLabel { font-size: 18px; font-weight: bold; margin-top: 20px; }");
    dashboardLayout->addWidget(summaryLabel);
    
    // Summary info widget
    QWidget *summaryWidget = new QWidget(dashboardWidget);
    QGridLayout *summaryGrid = new QGridLayout(summaryWidget);
    summaryGrid->setSpacing(10);
    
    // Summary labels
    QLabel *pointsLabelText = new QLabel("Points:", summaryWidget);
    QLabel *pointsValue = new QLabel("Loading...", summaryWidget);
    pointsValue->setObjectName("pointsValue");
    pointsValue->setStyleSheet("QLabel { font-weight: bold; color: #4CAF50; }");
    
    QLabel *tradesLabelText = new QLabel("Total Trades:", summaryWidget);
    QLabel *tradesValue = new QLabel("Loading...", summaryWidget);
    tradesValue->setObjectName("tradesValue");
    tradesValue->setStyleSheet("QLabel { font-weight: bold; color: #2196F3; }");
    
    QLabel *profitLabelText = new QLabel("Total Profit:", summaryWidget);
    QLabel *profitValue = new QLabel("Loading...", summaryWidget);
    profitValue->setObjectName("profitValue");
    profitValue->setStyleSheet("QLabel { font-weight: bold; color: #FF9800; }");
    
    summaryGrid->addWidget(pointsLabelText, 0, 0);
    summaryGrid->addWidget(pointsValue, 0, 1);
    summaryGrid->addWidget(tradesLabelText, 1, 0);
    summaryGrid->addWidget(tradesValue, 1, 1);
    summaryGrid->addWidget(profitLabelText, 2, 0);
    summaryGrid->addWidget(profitValue, 2, 1);
    
    dashboardLayout->addWidget(summaryWidget);
    
    // Add stretch to push content to top
    dashboardLayout->addStretch();
}

void MainWindow::setupMenuBar()
{
    // File menu
    QMenu *fileMenu = QMainWindow::menuBar()->addMenu("&File");
    
    QAction *logoutAction = new QAction("&Logout", this);
    logoutAction->setShortcut(QKeySequence::Close);
    connect(logoutAction, &QAction::triggered, this, &MainWindow::onLogout);
    fileMenu->addAction(logoutAction);
    
    fileMenu->addSeparator();
    
    QAction *exitAction = new QAction("E&xit", this);
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAction);
    
    // Trading menu
    QMenu *tradingMenu = QMainWindow::menuBar()->addMenu("&Trading");
    
    QAction *nextTimeframeAction = new QAction("&Next Timeframe", this);
    nextTimeframeAction->setShortcut(QKeySequence("Ctrl+N"));
    connect(nextTimeframeAction, &QAction::triggered, this, &MainWindow::onNextTimeframe);
    tradingMenu->addAction(nextTimeframeAction);
    
    QAction *refreshAction = new QAction("&Refresh Data", this);
    refreshAction->setShortcut(QKeySequence::Refresh);
    connect(refreshAction, &QAction::triggered, this, &MainWindow::updateMarketData);
    tradingMenu->addAction(refreshAction);
    
    // View menu
    QMenu *viewMenu = QMainWindow::menuBar()->addMenu("&View");
    
    QAction *dashboardTabAction = new QAction("&Dashboard", this);
    dashboardTabAction->setShortcut(QKeySequence("Ctrl+1"));
    connect(dashboardTabAction, &QAction::triggered, [this]() { tabWidget->setCurrentIndex(0); });
    viewMenu->addAction(dashboardTabAction);
    
    QAction *tradingTabAction = new QAction("&Trading", this);
    tradingTabAction->setShortcut(QKeySequence("Ctrl+2"));
    connect(tradingTabAction, &QAction::triggered, [this]() { tabWidget->setCurrentIndex(1); });
    viewMenu->addAction(tradingTabAction);
    
    QAction *chartsTabAction = new QAction("&Charts", this);
    chartsTabAction->setShortcut(QKeySequence("Ctrl+3"));
    connect(chartsTabAction, &QAction::triggered, [this]() { tabWidget->setCurrentIndex(2); });
    viewMenu->addAction(chartsTabAction);
    
    QAction *walletTabAction = new QAction("&Wallet", this);
    walletTabAction->setShortcut(QKeySequence("Ctrl+4"));
    connect(walletTabAction, &QAction::triggered, [this]() { tabWidget->setCurrentIndex(3); });
    viewMenu->addAction(walletTabAction);
    
    QAction *ordersTabAction = new QAction("&Orders", this);
    ordersTabAction->setShortcut(QKeySequence("Ctrl+5"));
    connect(ordersTabAction, &QAction::triggered, [this]() { tabWidget->setCurrentIndex(4); });
    viewMenu->addAction(ordersTabAction);
    
    // Account menu
    QMenu *accountMenu = QMainWindow::menuBar()->addMenu("&Account");
    
    QAction *profileAction = new QAction("&Profile", this);
    connect(profileAction, &QAction::triggered, this, &MainWindow::onUserProfile);
    accountMenu->addAction(profileAction);
    
    // Help menu
    QMenu *helpMenu = QMainWindow::menuBar()->addMenu("&Help");
    
    QAction *aboutAction = new QAction("&About", this);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
    helpMenu->addAction(aboutAction);
}

void MainWindow::setupStatusBar()
{
    // User info
    userLabel = new QLabel("User: Not logged in");
    QMainWindow::statusBar()->addWidget(userLabel);
    
    QMainWindow::statusBar()->addWidget(new QLabel("|"));
    
    // Points
    pointsLabel = new QLabel("Points: 0");
    QMainWindow::statusBar()->addWidget(pointsLabel);
    
    QMainWindow::statusBar()->addWidget(new QLabel("|"));
    
    // Current time
    timeLabel = new QLabel("Time: --");
    QMainWindow::statusBar()->addWidget(timeLabel);
    
    // Logout button
    logoutButton = new QPushButton("Logout");
    logoutButton->setMaximumHeight(20);
    connect(logoutButton, &QPushButton::clicked, this, &MainWindow::onLogout);
    QMainWindow::statusBar()->addPermanentWidget(logoutButton);
}

void MainWindow::connectSignals()
{
    // Connect trading widget signals
    if (tradingWidget) {
        connect(tradingWidget, &TradingWidget::tradeExecuted, 
                [this](bool successful, double profit) {
                    if (currentUser) {
                        currentUser->recordTrade(successful, profit);
                        updateUserDisplay();
                        refreshUserInfo();
                    }
                });
        
        connect(tradingWidget, &TradingWidget::walletUpdated,
                this, &MainWindow::refreshUserInfo);
    }
    
    // Connect wallet widget signals
    if (walletWidget) {
        connect(walletWidget, &WalletWidget::walletUpdated,
                this, &MainWindow::refreshUserInfo);
    }
    
    // Market data timer
    connect(marketTimer, &QTimer::timeout, this, &MainWindow::updateMarketData);
}

void MainWindow::setUser(std::shared_ptr<User> user)
{
    std::cerr << "Debug: setUser called" << std::endl;
    currentUser = user;
    
    if (currentUser) {
        std::cerr << "Debug: User is valid, setting widgets" << std::endl;
        // Set user for all widgets
        if (tradingWidget) {
            std::cerr << "Debug: Setting trading widget" << std::endl;
            tradingWidget->setUser(currentUser);
            tradingWidget->setOrderBook(&tradingEngine->getOrderBook());
        }
        
        if (walletWidget) {
            std::cerr << "Debug: Setting wallet widget" << std::endl;
            walletWidget->setUser(currentUser);
            walletWidget->setOrderBook(&tradingEngine->getOrderBook());
        }
        
        if (chartWidget) {
            std::cerr << "Debug: Setting chart widget" << std::endl;
            chartWidget->setOrderBook(&tradingEngine->getOrderBook());
        }
        
        if (orderWidget) {
            std::cerr << "Debug: Setting order widget" << std::endl;
            orderWidget->setUser(currentUser);
            orderWidget->setOrderBook(&tradingEngine->getOrderBook());
        }
        
        std::cerr << "Debug: Calling updateUserDisplay" << std::endl;
        updateUserDisplay();
        std::cerr << "Debug: Calling startMarketDataTimer" << std::endl;
        startMarketDataTimer();
        
        std::cerr << "Debug: Calling updateMarketData" << std::endl;
        // Initial data update
        updateMarketData();
        std::cerr << "Debug: setUser completed" << std::endl;
    }
}

void MainWindow::updateUserDisplay()
{
    if (currentUser && userLabel && pointsLabel) {
        userLabel->setText(QString("User: %1").arg(QString::fromStdString(currentUser->getUsername())));
        pointsLabel->setText(QString("Points: %1").arg(currentUser->getPoints()));
        
        // Update dashboard summary if dashboard exists and is properly initialized
        if (dashboardWidget && dashboardWidget->layout()) {
            QLabel *pointsValue = dashboardWidget->findChild<QLabel*>("pointsValue");
            QLabel *tradesValue = dashboardWidget->findChild<QLabel*>("tradesValue");
            QLabel *profitValue = dashboardWidget->findChild<QLabel*>("profitValue");
            
            if (pointsValue) {
                pointsValue->setText(QString::number(currentUser->getPoints()));
            }
            if (tradesValue) {
                tradesValue->setText(QString::number(currentUser->getTotalTrades()));
            }
            if (profitValue) {
                profitValue->setText(QString("$%1").arg(currentUser->getTotalProfit(), 0, 'f', 2));
            }
        }
    }
}

void MainWindow::startMarketDataTimer()
{
    if (!marketTimer->isActive()) {
        marketTimer->start(MARKET_UPDATE_INTERVAL);
    }
}

void MainWindow::updateMarketData()
{
    std::cerr << "Debug: updateMarketData started" << std::endl;
    if (!currentUser) return;
    
    std::cerr << "Debug: Getting current time from trading engine" << std::endl;
    // Get current time from trading engine
    std::string currentTime = tradingEngine->getCurrentTime();
    std::cerr << "Debug: Got current time: " << currentTime << std::endl;
    
    if (timeLabel) {
        std::cerr << "Debug: Updating time label" << std::endl;
        timeLabel->setText(QString("Time: %1").arg(QString::fromStdString(currentTime)));
    }
    
    std::cerr << "Debug: Updating widgets with market data" << std::endl;
    // Update all widgets with new market data
    if (tradingWidget) {
        std::cerr << "Debug: Updating trading widget" << std::endl;
        tradingWidget->updateMarketData(currentTime);
    }
    
    if (chartWidget) {
        std::cerr << "Debug: Updating chart widget" << std::endl;
        chartWidget->updateChart(currentTime);
    }
    
    if (walletWidget) {
        std::cerr << "Debug: Updating wallet widget" << std::endl;
        walletWidget->updateMarketPrices(currentTime);
    }
    
    if (orderWidget) {
        std::cerr << "Debug: Updating order widget" << std::endl;
        orderWidget->updateOrders(currentTime);
    }
    std::cerr << "Debug: updateMarketData completed" << std::endl;
}

void MainWindow::onNextTimeframe()
{
    if (tradingEngine) {
        tradingEngine->gotoNextTimeframe();
        updateMarketData();
    }
}

void MainWindow::refreshUserInfo()
{
    if (currentUser) {
        updateUserDisplay();
        
        if (walletWidget) {
            walletWidget->updateWallet();
        }
    }
}

void MainWindow::onLogout()
{
    int ret = QMessageBox::question(this, "Logout", 
                                   "Are you sure you want to logout?",
                                   QMessageBox::Yes | QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        marketTimer->stop();
        currentUser.reset();
        
        // Show login dialog
        LoginDialog loginDialog(this);
        if (loginDialog.exec() == QDialog::Accepted) {
            setUser(loginDialog.getAuthenticatedUser());
        } else {
            QApplication::quit();
        }
    }
}

void MainWindow::onUserProfile()
{
    if (!currentUser) return;
    
    QString profileInfo = QString(
        "Username: %1\n"
        "Email: %2\n"
        "Points: %3\n"
        "Total Profit: $%4\n"
        "Successful Trades: %5\n"
        "Total Trades: %6\n"
        "Success Rate: %7%\n"
        "Member Since: %8\n"
        "Last Login: %9"
    ).arg(QString::fromStdString(currentUser->getUsername()))
     .arg(QString::fromStdString(currentUser->getEmail()))
     .arg(currentUser->getPoints())
     .arg(currentUser->getTotalProfit(), 0, 'f', 2)
     .arg(currentUser->getSuccessfulTrades())
     .arg(currentUser->getTotalTrades())
     .arg(currentUser->getTotalTrades() > 0 ? 
          (double)currentUser->getSuccessfulTrades() / currentUser->getTotalTrades() * 100 : 0, 0, 'f', 1)
     .arg(QString::fromStdString(currentUser->getCreatedAt()))
     .arg(QString::fromStdString(currentUser->getLastLogin()));
    
    QMessageBox::information(this, "User Profile", profileInfo);
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "About Trading Desktop App",
                      "<h3>Trading Desktop Application</h3>"
                      "<p>Version 1.0.0</p>"
                      "<p>A comprehensive desktop trading platform with:</p>"
                      "<ul>"
                      "<li>Real-time candlestick charts</li>"
                      "<li>Secure user authentication</li>"
                      "<li>Encrypted data storage</li>"
                      "<li>Portfolio management</li>"
                      "<li>Order management</li>"
                      "<li>Points and rewards system</li>"
                      "</ul>"
                      "<p>Built with Qt6 and modern C++</p>");
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    int ret = QMessageBox::question(this, "Exit", 
                                   "Are you sure you want to exit?",
                                   QMessageBox::Yes | QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        // User data is automatically saved when needed
        
        marketTimer->stop();
        event->accept();
    } else {
        event->ignore();
    }
}