#pragma once

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <memory>

#include "../Auth/User.h"
#include "TradingWidget.h"
#include "WalletWidget.h"
#include "CandlestickChart.h"
#include "OrderWidget.h"
#include "../MerkelMain.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY(MainWindow)

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    void setUser(std::shared_ptr<User> user);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onLogout();
    void onAbout();
    void onUserProfile();
    void updateMarketData();
    void onNextTimeframe();
    void refreshUserInfo();

private:
    void setupUI();
    void setupMenuBar();
    void setupStatusBar();
    void setupTabs();
    void setupDashboard();
    void connectSignals();
    void updateUserDisplay();
    void startMarketDataTimer();
    
    // UI Components
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QTabWidget *tabWidget;
    
    // Tabs
    QWidget *dashboardWidget;
    TradingWidget *tradingWidget;
    WalletWidget *walletWidget;
    CandlestickChart *chartWidget;
    OrderWidget *orderWidget;
    
    // Menu and status
    QMenuBar *menuBar;
    QStatusBar *statusBar;
    QLabel *userLabel;
    QLabel *pointsLabel;
    QLabel *timeLabel;
    QPushButton *logoutButton;
    
    // Data and logic
    std::shared_ptr<User> currentUser;
    std::unique_ptr<MerkelMain> tradingEngine;
    QTimer *marketTimer;
    
    // Constants
    static const int MARKET_UPDATE_INTERVAL = 5000; // 5 seconds
};