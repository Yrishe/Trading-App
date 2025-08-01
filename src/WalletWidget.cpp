#include "GUI/WalletWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QGroupBox>
#include <QMessageBox>
#include <QHeaderView>
#include <QProgressBar>
#include <QFrame>
#include <QComboBox>
#include <QDebug>
#include <iomanip>
#include <sstream>

WalletWidget::WalletWidget(QWidget *parent)
    : QWidget(parent)
    , orderBook(nullptr)
{
    setupUI();
    connectSignals();
}

void WalletWidget::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    
    // Portfolio overview section
    setupPortfolioSection();
    mainLayout->addWidget(portfolioGroup);
    
    // Balance details section
    setupBalanceSection();
    mainLayout->addWidget(balanceGroup);
    
    // Fund transfer section
    setupTransferSection();
    mainLayout->addWidget(transferGroup);
    
    // User statistics section
    setupStatsSection();
    mainLayout->addWidget(statsGroup);
}

void WalletWidget::setupPortfolioSection()
{
    portfolioGroup = new QGroupBox("Portfolio Overview");
    portfolioLayout = new QVBoxLayout(portfolioGroup);
    
    // Total balance display
    QFrame *balanceFrame = new QFrame();
    balanceFrame->setFrameStyle(QFrame::Box);
    balanceFrame->setStyleSheet("QFrame { background-color: #f0f0f0; border: 2px solid #ddd; border-radius: 10px; padding: 10px; }");
    
    QVBoxLayout *balanceFrameLayout = new QVBoxLayout(balanceFrame);
    
    QLabel *totalBalanceLabel = new QLabel("Total Portfolio Value");
    totalBalanceLabel->setStyleSheet("font-size: 14px; color: #666;");
    
    totalValueLabel = new QLabel("$0.00");
    totalValueLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #2196F3;");
    
    totalChangeLabel = new QLabel("P&L: $0.00 (0.00%)");
    totalChangeLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #4CAF50;");
    
    // Portfolio table
    portfolioTable = new QTableWidget();
    portfolioTable->setColumnCount(4);
    portfolioTable->setHorizontalHeaderLabels({"Currency", "Balance", "Value", "Change"});
    portfolioTable->horizontalHeader()->setStretchLastSection(true);
    
    // Refresh button
    refreshButton = new QPushButton("Refresh");
    
    balanceFrameLayout->addWidget(totalBalanceLabel);
    balanceFrameLayout->addWidget(totalValueLabel);
    balanceFrameLayout->addWidget(totalChangeLabel);
    
    portfolioLayout->addWidget(balanceFrame);
    portfolioLayout->addWidget(portfolioTable);
    portfolioLayout->addWidget(refreshButton);
}

void WalletWidget::setupBalanceSection()
{
    balanceGroup = new QGroupBox("Currency Balances");
    balanceLayout = new QGridLayout(balanceGroup);
    
    // Create balance labels for each currency
    balanceLayout->addWidget(new QLabel("BTC:"), 0, 0);
    btcBalanceLabel = new QLabel("0.00000000");
    balanceLayout->addWidget(btcBalanceLabel, 0, 1);
    btcValueLabel = new QLabel("$0.00");
    balanceLayout->addWidget(btcValueLabel, 0, 2);
    
    balanceLayout->addWidget(new QLabel("ETH:"), 1, 0);
    ethBalanceLabel = new QLabel("0.00000000");
    balanceLayout->addWidget(ethBalanceLabel, 1, 1);
    ethValueLabel = new QLabel("$0.00");
    balanceLayout->addWidget(ethValueLabel, 1, 2);
    
    balanceLayout->addWidget(new QLabel("USDT:"), 2, 0);
    usdtBalanceLabel = new QLabel("0.00");
    balanceLayout->addWidget(usdtBalanceLabel, 2, 1);
    usdtValueLabel = new QLabel("$0.00");
    balanceLayout->addWidget(usdtValueLabel, 2, 2);
    
    balanceLayout->addWidget(new QLabel("DOGE:"), 3, 0);
    dogeBalanceLabel = new QLabel("0.00000000");
    balanceLayout->addWidget(dogeBalanceLabel, 3, 1);
    dogeValueLabel = new QLabel("$0.00");
    balanceLayout->addWidget(dogeValueLabel, 3, 2);
}

void WalletWidget::setupTransferSection()
{
    transferGroup = new QGroupBox("Fund Transfer");
    transferLayout = new QHBoxLayout(transferGroup);
    
    // Transfer button
    transferButton = new QPushButton("Transfer Funds");
    transferButton->setStyleSheet("QPushButton { background-color: #2196F3; color: white; font-weight: bold; padding: 10px; }");
    
    // Export/Import buttons
    exportButton = new QPushButton("Export Wallet");
    importButton = new QPushButton("Import Wallet");
    historyButton = new QPushButton("Transaction History");
    
    transferLayout->addWidget(transferButton);
    transferLayout->addWidget(exportButton);
    transferLayout->addWidget(importButton);
    transferLayout->addWidget(historyButton);
}

void WalletWidget::setupStatsSection()
{
    statsGroup = new QGroupBox("Trading Statistics");
    statsLayout = new QGridLayout(statsGroup);
    
    // User points
    pointsLabel = new QLabel("0");
    pointsLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #FF9800;");
    
    // Total trades
    totalTradesLabel = new QLabel("0");
    
    // Success rate
    successRateLabel = new QLabel("0%");
    
    // Total profit
    totalProfitLabel = new QLabel("$0.00");
    
    // Best and worst trades
    bestTradeLabel = new QLabel("$0.00");
    worstTradeLabel = new QLabel("$0.00");
    
    // Progress bar for trading level
    levelProgressBar = new QProgressBar();
    levelProgressBar->setRange(0, 1000);
    levelProgressBar->setValue(0);
    levelProgressBar->setFormat("Level 1 - %p%");
    
    levelLabel = new QLabel("Level 1");
    
    statsLayout->addWidget(new QLabel("Points:"), 0, 0);
    statsLayout->addWidget(pointsLabel, 0, 1);
    statsLayout->addWidget(new QLabel("Total Trades:"), 1, 0);
    statsLayout->addWidget(totalTradesLabel, 1, 1);
    statsLayout->addWidget(new QLabel("Success Rate:"), 2, 0);
    statsLayout->addWidget(successRateLabel, 2, 1);
    statsLayout->addWidget(new QLabel("Total Profit:"), 3, 0);
    statsLayout->addWidget(totalProfitLabel, 3, 1);
    statsLayout->addWidget(new QLabel("Best Trade:"), 4, 0);
    statsLayout->addWidget(bestTradeLabel, 4, 1);
    statsLayout->addWidget(new QLabel("Worst Trade:"), 5, 0);
    statsLayout->addWidget(worstTradeLabel, 5, 1);
    statsLayout->addWidget(new QLabel("Level:"), 6, 0);
    statsLayout->addWidget(levelLabel, 6, 1);
    statsLayout->addWidget(levelProgressBar, 7, 0, 1, 2);
}

void WalletWidget::connectSignals()
{
    connect(transferButton, &QPushButton::clicked, this, &WalletWidget::onTransferFunds);
}

void WalletWidget::setUser(std::shared_ptr<User> u)
{
    currentUser = u;
    updateWallet();
}

void WalletWidget::setOrderBook(OrderBook *book)
{
    orderBook = book;
    updateWallet();
}

void WalletWidget::updateWallet()
{
    if (!currentUser) return;
    
    updateBalanceDisplay();
    updatePortfolioTable();
    updateStatsDisplay();
}

void WalletWidget::updateBalanceDisplay()
{
    if (!currentUser) return;
    
    // Update balance labels with default values since getCurrencyAmount is not available
    btcBalanceLabel->setText("0.00000000");
    btcValueLabel->setText("$0.00");
    
    ethBalanceLabel->setText("0.00000000");
    ethValueLabel->setText("$0.00");
    
    usdtBalanceLabel->setText("0.00");
    usdtValueLabel->setText("$0.00");
    
    dogeBalanceLabel->setText("0.00000000");
    dogeValueLabel->setText("$0.00");
}

void WalletWidget::updatePortfolioTable()
{
    if (!currentUser) return;
    
    // Update portfolio with mock data
    totalValueLabel->setText("$0.00");
    totalChangeLabel->setText("P&L: $0.00 (0.0%)");
    totalChangeLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #4CAF50;");
}

void WalletWidget::updateStatsDisplay()
{
    if (!currentUser) return;
    
    // Update points
    pointsLabel->setText(QString::number(currentUser->getPoints()));
    
    // Update trading statistics with mock data
    totalTradesLabel->setText("0");
    successRateLabel->setText("0.0%");
    
    // Update total profit
    std::ostringstream profitStream;
    profitStream << std::fixed << std::setprecision(2) << "$" << currentUser->getTotalProfit();
    totalProfitLabel->setText(QString::fromStdString(profitStream.str()));
    
    // Update level progress
    int points = currentUser->getPoints();
    int level = (points / 1000) + 1;
    int levelProgress = points % 1000;
    
    levelProgressBar->setValue(levelProgress);
    levelProgressBar->setFormat("Level " + QString::number(level) + " - %p%");
}

void WalletWidget::onTransferFunds()
{
    if (!currentUser) {
        QMessageBox::warning(this, "Error", "User not set.");
        return;
    }
    
    // Simple transfer dialog for demonstration
    QMessageBox::information(this, "Transfer", "Transfer functionality would be implemented here.");
    updateWallet();
}

double WalletWidget::getCurrentPrice(const std::string& currency)
{
    // Mock prices for demonstration
    // In a real application, this would fetch from the order book or price feed
    if (currency == "BTC") return 45000.0;
    if (currency == "ETH") return 3000.0;
    if (currency == "USDT") return 1.0;
    if (currency == "DOGE") return 0.25;
    return 1.0;
}

void WalletWidget::onExportWallet()
{
    QMessageBox::information(this, "Export", "Export wallet functionality would be implemented here.");
}

void WalletWidget::onImportWallet()
{
    QMessageBox::information(this, "Import", "Import wallet functionality would be implemented here.");
}

void WalletWidget::onRefreshWallet()
{
    updateWallet();
}

void WalletWidget::onCurrencySelected()
{
    // Handle currency selection
    updatePortfolioTable();
}

void WalletWidget::updateMarketPrices(const std::string& currentTime)
{
    // Update market prices implementation
    updateWallet();
}

void WalletWidget::onShowTransactionHistory()
{
    QMessageBox::information(this, "Transaction History", "Transaction history would be displayed here.");
}

WalletWidget::~WalletWidget()
{
    // Destructor implementation
}