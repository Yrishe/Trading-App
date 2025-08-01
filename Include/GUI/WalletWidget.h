#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QProgressBar>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <memory>

#include "../Auth/User.h"
#include "../OrderBook.h"

class WalletWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(WalletWidget)
    
public:
    explicit WalletWidget(QWidget *parent = nullptr);
    ~WalletWidget();
    
    void setUser(std::shared_ptr<User> user);
    void setOrderBook(OrderBook* orderBook);
    void updateWallet();
    void updateMarketPrices(const std::string& currentTime);

signals:
    void walletUpdated();
    void transferCompleted(bool success);

public slots:
    void onRefreshWallet();
    void onTransferFunds();
    void onExportWallet();
    void onImportWallet();
    void onCurrencySelected();
    void onShowTransactionHistory();

private:
    void setupUI();
    void setupPortfolioSection();
    void setupBalanceSection();
    void setupTransferSection();
    void setupStatsSection();
    void connectSignals();
    void updatePortfolioTable();
    void updateBalanceDisplay();
    void updateStatsDisplay();
    void calculatePortfolioValue();
    void showTransferDialog();
    void showTransactionHistory();
    
    // UI Components
    QVBoxLayout *mainLayout;
    QHBoxLayout *topLayout;
    
    // Portfolio section
    QGroupBox *portfolioGroup;
    QVBoxLayout *portfolioLayout;
    QTableWidget *portfolioTable;
    QPushButton *refreshButton;
    QLabel *totalValueLabel;
    QLabel *totalChangeLabel;
    
    // Balance section
    QGroupBox *balanceGroup;
    QGridLayout *balanceLayout;
    QLabel *btcBalanceLabel;
    QLabel *ethBalanceLabel;
    QLabel *usdtBalanceLabel;
    QLabel *dogeBalanceLabel;
    QLabel *btcValueLabel;
    QLabel *ethValueLabel;
    QLabel *usdtValueLabel;
    QLabel *dogeValueLabel;
    
    // Transfer section
    QGroupBox *transferGroup;
    QHBoxLayout *transferLayout;
    QPushButton *transferButton;
    QPushButton *exportButton;
    QPushButton *importButton;
    QPushButton *historyButton;
    
    // Stats section
    QGroupBox *statsGroup;
    QGridLayout *statsLayout;
    QLabel *pointsLabel;
    QLabel *totalProfitLabel;
    QLabel *successRateLabel;
    QLabel *totalTradesLabel;
    QLabel *bestTradeLabel;
    QLabel *worstTradeLabel;
    QProgressBar *levelProgressBar;
    QLabel *levelLabel;
    
    // Data
    std::shared_ptr<User> currentUser;
    OrderBook *orderBook;
    std::string currentTime;
    
    // Portfolio data
    struct CurrencyInfo {
        std::string symbol;
        double balance;
        double currentPrice;
        double value;
        double change24h;
        double changePercent24h;
    };
    
    std::vector<CurrencyInfo> currencies;
    double totalPortfolioValue;
    double totalPortfolioChange;
    double totalPortfolioChangePercent;
    
    // User stats
    int userLevel;
    int pointsToNextLevel;
    double successRate;
    double bestTrade;
    double worstTrade;
    
    void updateCurrencyInfo();
    double getCurrentPrice(const std::string& product);
    int calculateUserLevel(int points);
    int getPointsForNextLevel(int currentLevel);
};