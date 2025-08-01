#include "GUI/OrderWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTabWidget>
#include <QGroupBox>
#include <QHeaderView>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QDateEdit>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QDate>
#include <iomanip>
#include <sstream>

OrderWidget::OrderWidget(QWidget *parent)
    : QWidget(parent)
    , currentUser(nullptr)
    , orderBook(nullptr)
{
    setupUI();
    connectSignals();
    
    // Setup timer for periodic updates
    QTimer *updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, [this]() { updateOrders(""); });
    updateTimer->start(10000); // Update every 10 seconds
}

void OrderWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Create tab widget
    QTabWidget *tabWidget = new QTabWidget();
    
    // Active Orders tab
    activeOrdersTab = new QWidget();
    setupActiveOrdersTab();
    tabWidget->addTab(activeOrdersTab, "Active Orders");
    
    // Order History tab
    orderHistoryTab = new QWidget();
    setupOrderHistoryTab();
    tabWidget->addTab(orderHistoryTab, "Order History");
    
    // Trade History tab
    tradeHistoryTab = new QWidget();
    setupTradeHistoryTab();
    tabWidget->addTab(tradeHistoryTab, "Trade History");
    
    mainLayout->addWidget(tabWidget);
}

void OrderWidget::setupActiveOrdersTab()
{
    QVBoxLayout *layout = new QVBoxLayout(activeOrdersTab);
    
    // Controls section
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    
    QLabel *filterLabel = new QLabel("Filter by Product:");
    productFilterCombo = new QComboBox();
    productFilterCombo->addItems({"All", "BTC/USDT", "ETH/USDT", "ETH/BTC", "DOGE/BTC", "DOGE/USDT"});
    
    QPushButton *refreshButton = new QPushButton("Refresh");
    refreshButton->setStyleSheet("QPushButton { background-color: #2196F3; color: white; padding: 8px 16px; }");
    
    QPushButton *cancelAllButton = new QPushButton("Cancel All");
    cancelAllButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 8px 16px; }");
    
    controlsLayout->addWidget(filterLabel);
    controlsLayout->addWidget(productFilterCombo);
    controlsLayout->addWidget(refreshButton);
    controlsLayout->addWidget(cancelAllButton);
    controlsLayout->addStretch();
    
    // Active orders table
    activeOrdersTable = new QTableWidget();
    activeOrdersTable->setColumnCount(7);
    activeOrdersTable->setHorizontalHeaderLabels({
        "Time", "Product", "Type", "Side", "Amount", "Price", "Status"
    });
    activeOrdersTable->horizontalHeader()->setStretchLastSection(true);
    activeOrdersTable->setAlternatingRowColors(true);
    activeOrdersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    activeOrdersTable->verticalHeader()->setVisible(false);
    
    // Set column widths
    activeOrdersTable->setColumnWidth(0, 120);
    activeOrdersTable->setColumnWidth(1, 100);
    activeOrdersTable->setColumnWidth(2, 80);
    activeOrdersTable->setColumnWidth(3, 60);
    activeOrdersTable->setColumnWidth(4, 100);
    activeOrdersTable->setColumnWidth(5, 100);
    
    layout->addLayout(controlsLayout);
    layout->addWidget(activeOrdersTable);
    
    // Connect signals
    connect(refreshButton, &QPushButton::clicked, this, [this]() { updateOrders(""); });
    connect(cancelAllButton, &QPushButton::clicked, this, &OrderWidget::onCancelOrder);
    connect(productFilterCombo, QOverload<const QString &>::of(&QComboBox::currentTextChanged),
            this, [this]() { updateOrders(""); });
}

void OrderWidget::setupOrderHistoryTab()
{
    QVBoxLayout *layout = new QVBoxLayout(orderHistoryTab);
    
    // Filter controls
    QHBoxLayout *filterLayout = new QHBoxLayout();
    
    QLabel *dateFromLabel = new QLabel("From:");
    fromDateEdit = new QDateEdit(QDate::currentDate().addDays(-30));
    fromDateEdit->setCalendarPopup(true);
    
    QLabel *dateToLabel = new QLabel("To:");
    toDateEdit = new QDateEdit(QDate::currentDate());
    toDateEdit->setCalendarPopup(true);
    
    QLabel *statusLabel = new QLabel("Status:");
    statusFilterCombo = new QComboBox();
    statusFilterCombo->addItems({"All", "Filled", "Cancelled", "Partially Filled"});
    
    QPushButton *applyFilterButton = new QPushButton("Apply Filter");
    applyFilterButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 8px 16px; }");
    
    filterLayout->addWidget(dateFromLabel);
    filterLayout->addWidget(fromDateEdit);
    filterLayout->addWidget(dateToLabel);
    filterLayout->addWidget(toDateEdit);
    filterLayout->addWidget(statusLabel);
    filterLayout->addWidget(statusFilterCombo);
    filterLayout->addWidget(applyFilterButton);
    filterLayout->addStretch();
    
    // Order history table
    orderHistoryTable = new QTableWidget();
    orderHistoryTable->setColumnCount(8);
    orderHistoryTable->setHorizontalHeaderLabels({
        "Date", "Product", "Type", "Side", "Amount", "Price", "Filled", "Status"
    });
    orderHistoryTable->horizontalHeader()->setStretchLastSection(true);
    orderHistoryTable->setAlternatingRowColors(true);
    orderHistoryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    orderHistoryTable->verticalHeader()->setVisible(false);
    
    layout->addLayout(filterLayout);
    layout->addWidget(orderHistoryTable);
    
    connect(applyFilterButton, &QPushButton::clicked, this, &OrderWidget::updateOrderHistoryTable);
}

void OrderWidget::setupTradeHistoryTab()
{
    QVBoxLayout *layout = new QVBoxLayout(tradeHistoryTab);
    
    // Statistics section
    QGroupBox *statsGroup = new QGroupBox("Trading Statistics");
    QHBoxLayout *statsLayout = new QHBoxLayout(statsGroup);
    
    totalTradesLabel = new QLabel("Total Trades: 0");
    totalVolumeLabel = new QLabel("Total Volume: $0.00");
    avgProfitLabel = new QLabel("Avg Trade: $0.00");
    successRateLabel = new QLabel("Win Rate: 0%");
    
    totalTradesLabel->setStyleSheet("font-weight: bold; color: #2196F3;");
    totalVolumeLabel->setStyleSheet("font-weight: bold; color: #4CAF50;");
    avgProfitLabel->setStyleSheet("font-weight: bold; color: #FF9800;");
    successRateLabel->setStyleSheet("font-weight: bold; color: #9C27B0;");
    
    statsLayout->addWidget(totalTradesLabel);
    statsLayout->addWidget(totalVolumeLabel);
    statsLayout->addWidget(avgProfitLabel);
    statsLayout->addWidget(successRateLabel);
    
    // Trade history table
    tradeHistoryTable = new QTableWidget();
    tradeHistoryTable->setColumnCount(7);
    tradeHistoryTable->setHorizontalHeaderLabels({
        "Date", "Product", "Side", "Amount", "Price", "Total", "P&L"
    });
    tradeHistoryTable->horizontalHeader()->setStretchLastSection(true);
    tradeHistoryTable->setAlternatingRowColors(true);
    tradeHistoryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    tradeHistoryTable->verticalHeader()->setVisible(false);
    
    layout->addWidget(statsGroup);
    layout->addWidget(tradeHistoryTable);
}

void OrderWidget::connectSignals()
{
    // Signals are connected in individual setup methods
}

void OrderWidget::setUser(std::shared_ptr<User> u)
{
    currentUser = u;
    updateOrders("");
    updateOrderHistoryTable();
    updateTradeHistoryTable();
}

void OrderWidget::setOrderBook(OrderBook *book)
{
    orderBook = book;
    updateOrders("");
}

void OrderWidget::updateOrders(const std::string& currentTime)
{
    if (!currentUser || !orderBook) return;
    
    // Clear existing data
    activeOrdersTable->setRowCount(0);
    
    // For demonstration, we'll create some mock active orders
    // In a real application, this would fetch from a database or order management system
    QString productFilter = productFilterCombo->currentText();
    
    // Add some mock data directly to the table
    activeOrdersTable->setRowCount(3);
    
    activeOrdersTable->setItem(0, 0, new QTableWidgetItem("2024/01/15 10:30:00"));
    activeOrdersTable->setItem(0, 1, new QTableWidgetItem("BTC/USDT"));
    activeOrdersTable->setItem(0, 2, new QTableWidgetItem("Limit"));
    activeOrdersTable->setItem(0, 3, new QTableWidgetItem("Buy"));
    activeOrdersTable->setItem(0, 4, new QTableWidgetItem("0.001000"));
    activeOrdersTable->setItem(0, 5, new QTableWidgetItem("45000.00"));
    activeOrdersTable->setItem(0, 6, new QTableWidgetItem("Open"));
    
    activeOrdersTable->setItem(1, 0, new QTableWidgetItem("2024/01/15 11:15:00"));
    activeOrdersTable->setItem(1, 1, new QTableWidgetItem("ETH/USDT"));
    activeOrdersTable->setItem(1, 2, new QTableWidgetItem("Limit"));
    activeOrdersTable->setItem(1, 3, new QTableWidgetItem("Sell"));
    activeOrdersTable->setItem(1, 4, new QTableWidgetItem("0.500000"));
    activeOrdersTable->setItem(1, 5, new QTableWidgetItem("3100.00"));
    activeOrdersTable->setItem(1, 6, new QTableWidgetItem("Open"));
    
    activeOrdersTable->setItem(2, 0, new QTableWidgetItem("2024/01/15 12:00:00"));
    activeOrdersTable->setItem(2, 1, new QTableWidgetItem("DOGE/USDT"));
    activeOrdersTable->setItem(2, 2, new QTableWidgetItem("Market"));
    activeOrdersTable->setItem(2, 3, new QTableWidgetItem("Buy"));
    activeOrdersTable->setItem(2, 4, new QTableWidgetItem("1000.000000"));
    activeOrdersTable->setItem(2, 5, new QTableWidgetItem("0.25"));
    activeOrdersTable->setItem(2, 6, new QTableWidgetItem("Pending"));
}

void OrderWidget::updateOrderHistoryTable()
{
    if (!currentUser) return;
    
    // Clear existing data
    orderHistoryTable->setRowCount(0);
    
    // Mock order history data
    QString statusFilter = statusFilterCombo->currentText();
    QDate fromDate = fromDateEdit->date();
    QDate toDate = toDateEdit->date();
    
    // Add some mock data directly to the table
    orderHistoryTable->setRowCount(4);
    
    orderHistoryTable->setItem(0, 0, new QTableWidgetItem("2024/01/14 15:30:00"));
    orderHistoryTable->setItem(0, 1, new QTableWidgetItem("BTC/USDT"));
    orderHistoryTable->setItem(0, 2, new QTableWidgetItem("Market"));
    orderHistoryTable->setItem(0, 3, new QTableWidgetItem("Buy"));
    orderHistoryTable->setItem(0, 4, new QTableWidgetItem("0.002000"));
    orderHistoryTable->setItem(0, 5, new QTableWidgetItem("44500.00"));
    orderHistoryTable->setItem(0, 6, new QTableWidgetItem("0.002000"));
    orderHistoryTable->setItem(0, 7, new QTableWidgetItem("Filled"));
    
    orderHistoryTable->setItem(1, 0, new QTableWidgetItem("2024/01/14 16:45:00"));
    orderHistoryTable->setItem(1, 1, new QTableWidgetItem("ETH/USDT"));
    orderHistoryTable->setItem(1, 2, new QTableWidgetItem("Limit"));
    orderHistoryTable->setItem(1, 3, new QTableWidgetItem("Sell"));
    orderHistoryTable->setItem(1, 4, new QTableWidgetItem("1.000000"));
    orderHistoryTable->setItem(1, 5, new QTableWidgetItem("2950.00"));
    orderHistoryTable->setItem(1, 6, new QTableWidgetItem("1.000000"));
    orderHistoryTable->setItem(1, 7, new QTableWidgetItem("Filled"));
    
    orderHistoryTable->setItem(2, 0, new QTableWidgetItem("2024/01/13 09:15:00"));
    orderHistoryTable->setItem(2, 1, new QTableWidgetItem("DOGE/USDT"));
    orderHistoryTable->setItem(2, 2, new QTableWidgetItem("Market"));
    orderHistoryTable->setItem(2, 3, new QTableWidgetItem("Buy"));
    orderHistoryTable->setItem(2, 4, new QTableWidgetItem("2000.000000"));
    orderHistoryTable->setItem(2, 5, new QTableWidgetItem("0.24"));
    orderHistoryTable->setItem(2, 6, new QTableWidgetItem("2000.000000"));
    orderHistoryTable->setItem(2, 7, new QTableWidgetItem("Filled"));
    
    orderHistoryTable->setItem(3, 0, new QTableWidgetItem("2024/01/12 14:20:00"));
    orderHistoryTable->setItem(3, 1, new QTableWidgetItem("BTC/USDT"));
    orderHistoryTable->setItem(3, 2, new QTableWidgetItem("Limit"));
    orderHistoryTable->setItem(3, 3, new QTableWidgetItem("Sell"));
    orderHistoryTable->setItem(3, 4, new QTableWidgetItem("0.001000"));
    orderHistoryTable->setItem(3, 5, new QTableWidgetItem("46000.00"));
    orderHistoryTable->setItem(3, 6, new QTableWidgetItem("0.000000"));
    orderHistoryTable->setItem(3, 7, new QTableWidgetItem("Cancelled"));
}

void OrderWidget::updateTradeHistoryTable()
{
    if (!currentUser) return;
    
    // Clear existing data
    tradeHistoryTable->setRowCount(0);
    
    // Mock trade history data
    double totalVolume = 0.0;
    double totalPnL = 0.0;
    int profitableTrades = 0;
    
    // Add some mock data directly to the table
    tradeHistoryTable->setRowCount(4);
    
    tradeHistoryTable->setItem(0, 0, new QTableWidgetItem("2024/01/14 15:30:00"));
    tradeHistoryTable->setItem(0, 1, new QTableWidgetItem("BTC/USDT"));
    tradeHistoryTable->setItem(0, 2, new QTableWidgetItem("Buy"));
    tradeHistoryTable->setItem(0, 3, new QTableWidgetItem("0.002000"));
    tradeHistoryTable->setItem(0, 4, new QTableWidgetItem("44500.00"));
    tradeHistoryTable->setItem(0, 5, new QTableWidgetItem("89.00"));
    tradeHistoryTable->setItem(0, 6, new QTableWidgetItem("0.00"));
    
    tradeHistoryTable->setItem(1, 0, new QTableWidgetItem("2024/01/14 16:45:00"));
    tradeHistoryTable->setItem(1, 1, new QTableWidgetItem("ETH/USDT"));
    tradeHistoryTable->setItem(1, 2, new QTableWidgetItem("Sell"));
    tradeHistoryTable->setItem(1, 3, new QTableWidgetItem("1.000000"));
    tradeHistoryTable->setItem(1, 4, new QTableWidgetItem("2950.00"));
    tradeHistoryTable->setItem(1, 5, new QTableWidgetItem("2950.00"));
    tradeHistoryTable->setItem(1, 6, new QTableWidgetItem("50.00"));
    
    tradeHistoryTable->setItem(2, 0, new QTableWidgetItem("2024/01/13 09:15:00"));
    tradeHistoryTable->setItem(2, 1, new QTableWidgetItem("DOGE/USDT"));
    tradeHistoryTable->setItem(2, 2, new QTableWidgetItem("Buy"));
    tradeHistoryTable->setItem(2, 3, new QTableWidgetItem("2000.000000"));
    tradeHistoryTable->setItem(2, 4, new QTableWidgetItem("0.24"));
    tradeHistoryTable->setItem(2, 5, new QTableWidgetItem("480.00"));
    tradeHistoryTable->setItem(2, 6, new QTableWidgetItem("0.00"));
    
    tradeHistoryTable->setItem(3, 0, new QTableWidgetItem("2024/01/12 14:20:00"));
    tradeHistoryTable->setItem(3, 1, new QTableWidgetItem("BTC/USDT"));
    tradeHistoryTable->setItem(3, 2, new QTableWidgetItem("Sell"));
    tradeHistoryTable->setItem(3, 3, new QTableWidgetItem("0.001000"));
    tradeHistoryTable->setItem(3, 4, new QTableWidgetItem("46000.00"));
    tradeHistoryTable->setItem(3, 5, new QTableWidgetItem("46.00"));
    tradeHistoryTable->setItem(3, 6, new QTableWidgetItem("2.00"));
    
    totalVolume = 89.0 + 2950.0 + 480.0 + 46.0;
    totalPnL = 0.0 + 50.0 + 0.0 + 2.0;
    profitableTrades = 2;
    
    // Update statistics
     updateTradeStats();
}

void OrderWidget::updateTradeStats()
{
    totalTradesLabel->setText("Total Trades: 4");
    totalVolumeLabel->setText("Total Volume: $3565.00");
    avgProfitLabel->setText("Avg Trade: $13.00");
    successRateLabel->setText("Win Rate: 50.0%");
}

void OrderWidget::onCancelOrder()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Cancel All Orders", 
        "Are you sure you want to cancel all active orders?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        // In a real application, this would cancel all orders through the trading API
        activeOrdersTable->setRowCount(0);
        QMessageBox::information(this, "Orders Cancelled", "All active orders have been cancelled.");
    }
}

void OrderWidget::onTabChanged(int index)
{
    // Handle tab change
    if (index == 1) {
        updateOrderHistoryTable();
    } else if (index == 2) {
        updateTradeHistoryTable();
    }
}

void OrderWidget::onModifyOrder()
{
    QMessageBox::information(this, "Modify Order", "Order modification functionality would be implemented here.");
}

void OrderWidget::onExportHistory()
{
    QMessageBox::information(this, "Export", "Export history functionality would be implemented here.");
}

void OrderWidget::onFilterChanged()
{
    updateOrderHistoryTable();
}

void OrderWidget::onOrderSelected()
{
    // Handle order selection
    int row = activeOrdersTable->currentRow();
    if (row >= 0) {
        QMessageBox::information(this, "Order Selected", "Order details would be displayed here.");
    }
}

void OrderWidget::onRefreshOrders()
{
    updateOrders("");
}

OrderWidget::~OrderWidget()
{
    // Destructor implementation
}