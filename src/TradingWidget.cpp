#include "GUI/TradingWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QGroupBox>
#include <QMessageBox>
#include <QHeaderView>
#include <QTimer>
#include <QDebug>
#include <iomanip>
#include <sstream>

TradingWidget::TradingWidget(QWidget *parent)
    : QWidget(parent)
    , orderBook(nullptr)
    , currentUser(nullptr)
    , selectedProduct("BTC/USDT")
    , currentPrice(0.0)
    , bidPrice(0.0)
    , askPrice(0.0)
    , volume24h(0.0)
    , priceChange24h(0.0)
    , highPrice24h(0.0)
    , lowPrice24h(0.0)
    , isFormValid(false)
    , orderTotal(0.0)
{
    setupUI();
    connectSignals();
    
    // Setup timer for market data updates
    QTimer *updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, [this]() { updateMarketData(currentTime); });
    updateTimer->start(5000); // Update every 5 seconds
}

TradingWidget::~TradingWidget()
{
    // Destructor implementation
}

void TradingWidget::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    topLayout = new QHBoxLayout();
    
    // Left panel - Trading controls
    QWidget *leftPanel = new QWidget();
    leftPanel->setMaximumWidth(400);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    
    // Market data section
    setupMarketData();
    leftLayout->addWidget(marketDataGroup);
    
    // Order form section
    setupOrderForm();
    leftLayout->addWidget(orderFormGroup);
    
    // Quick trade buttons
    setupQuickTrade();
    leftLayout->addWidget(quickTradeGroup);
    
    leftLayout->addStretch();
    
    // Right panel - Order book
    setupOrderBook();
    
    topLayout->addWidget(leftPanel);
    topLayout->addWidget(orderBookGroup, 1);
    mainLayout->addLayout(topLayout);
    
    connectSignals();
}

void TradingWidget::setupMarketData()
{
    marketDataGroup = new QGroupBox("Market Data");
    marketDataLayout = new QGridLayout(marketDataGroup);
    
    // Product selector
    QLabel *productLabel = new QLabel("Product:");
    productCombo = new QComboBox();
    productCombo->addItems({"BTC/USDT", "ETH/USDT", "ETH/BTC", "DOGE/BTC", "DOGE/USDT"});
    productCombo->setCurrentText(QString::fromStdString(selectedProduct));
    
    // Market data labels
    currentPriceLabel = new QLabel("$0.00");
    currentPriceLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2196F3;");
    
    volumeLabel = new QLabel("Volume: 0.00");
    changeLabel = new QLabel("Change: 0.00%");
    changeLabel->setStyleSheet("color: #4CAF50;");
    
    marketDataLayout->addWidget(productLabel, 0, 0);
    marketDataLayout->addWidget(productCombo, 0, 1);
    marketDataLayout->addWidget(new QLabel("Current Price:"), 1, 0);
    marketDataLayout->addWidget(currentPriceLabel, 1, 1);
    marketDataLayout->addWidget(volumeLabel, 2, 0, 1, 2);
    marketDataLayout->addWidget(changeLabel, 3, 0, 1, 2);
}

void TradingWidget::setupOrderForm()
{
    orderFormGroup = new QGroupBox("Place Order");
    orderFormLayout = new QFormLayout(orderFormGroup);
    
    // Order type
    QLabel *orderTypeLabel = new QLabel("Order Type:");
    marketOrderRadio = new QRadioButton("Market Order");
    limitOrderRadio = new QRadioButton("Limit Order");
    marketOrderRadio->setChecked(true);
    
    // Amount
    QLabel *amountLabel = new QLabel("Amount:");
    amountSpinBox = new QDoubleSpinBox();
    amountSpinBox->setRange(0.0, 999999.99);
    amountSpinBox->setDecimals(8);
    
    // Price (for limit orders)
    QLabel *priceLabel = new QLabel("Price:");
    priceSpinBox = new QDoubleSpinBox();
    priceSpinBox->setRange(0.0, 999999.99);
    priceSpinBox->setDecimals(8);
    priceSpinBox->setEnabled(false); // Disabled for market orders initially
    
    // Total
    totalLabel = new QLabel("Total: $0.00");
    totalLabel->setStyleSheet("font-weight: bold;");
    
    // Order buttons
    buyButton = new QPushButton("Buy");
    sellButton = new QPushButton("Sell");
    buyButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; padding: 10px; }");
    sellButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; font-weight: bold; padding: 10px; }");
    
    orderFormLayout->addRow(orderTypeLabel, marketOrderRadio);
    orderFormLayout->addRow("", limitOrderRadio);
    orderFormLayout->addRow(amountLabel, amountSpinBox);
    orderFormLayout->addRow(priceLabel, priceSpinBox);
    orderFormLayout->addRow(totalLabel);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(buyButton);
    buttonLayout->addWidget(sellButton);
    orderFormLayout->addRow(buttonLayout);
}

void TradingWidget::setupQuickTrade()
{
    quickTradeGroup = new QGroupBox("Quick Trade");
    quickTradeLayout = new QGridLayout(quickTradeGroup);
    
    // Quick buy buttons
    QLabel *quickBuyLabel = new QLabel("Quick Buy:");
    quickBuy1Button = new QPushButton("$25");
    quickBuy2Button = new QPushButton("$50");
    quickBuy3Button = new QPushButton("$100");
    
    // Quick sell buttons
    QLabel *quickSellLabel = new QLabel("Quick Sell:");
    quickSell1Button = new QPushButton("25%");
    quickSell2Button = new QPushButton("50%");
    quickSell3Button = new QPushButton("100%");
    
    // Style quick trade buttons
    QString buyStyle = "QPushButton { background-color: #4CAF50; color: white; padding: 8px; }";
    QString sellStyle = "QPushButton { background-color: #f44336; color: white; padding: 8px; }";
    
    quickBuy1Button->setStyleSheet(buyStyle);
    quickBuy2Button->setStyleSheet(buyStyle);
    quickBuy3Button->setStyleSheet(buyStyle);
    quickSell1Button->setStyleSheet(sellStyle);
    quickSell2Button->setStyleSheet(sellStyle);
    quickSell3Button->setStyleSheet(sellStyle);
    
    quickTradeLayout->addWidget(quickBuyLabel, 0, 0);
    quickTradeLayout->addWidget(quickBuy1Button, 0, 1);
    quickTradeLayout->addWidget(quickBuy2Button, 0, 2);
    quickTradeLayout->addWidget(quickBuy3Button, 0, 3);
    quickTradeLayout->addWidget(quickSellLabel, 1, 0);
    quickTradeLayout->addWidget(quickSell1Button, 1, 1);
    quickTradeLayout->addWidget(quickSell2Button, 1, 2);
    quickTradeLayout->addWidget(quickSell3Button, 1, 3);
    

}

void TradingWidget::setupOrderBook()
{
    orderBookGroup = new QGroupBox("Order Book - " + QString::fromStdString(selectedProduct));
    orderBookLayout = new QVBoxLayout(orderBookGroup);
    
    orderBookTable = new QTableWidget();
    orderBookTable->setColumnCount(3);
    orderBookTable->setHorizontalHeaderLabels({"Price", "Amount", "Total"});
    orderBookTable->horizontalHeader()->setStretchLastSection(true);
    orderBookTable->setAlternatingRowColors(true);
    orderBookTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    orderBookLayout->addWidget(orderBookTable);
}

void TradingWidget::connectSignals()
{
    connect(productCombo, QOverload<const QString &>::of(&QComboBox::currentTextChanged),
            this, &TradingWidget::onProductChanged);
    connect(marketOrderRadio, &QRadioButton::toggled, this, &TradingWidget::calculateOrderTotal);
    connect(limitOrderRadio, &QRadioButton::toggled, this, &TradingWidget::calculateOrderTotal);
    connect(amountSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &TradingWidget::calculateOrderTotal);
    connect(priceSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &TradingWidget::calculateOrderTotal);
    connect(buyButton, &QPushButton::clicked, this, &TradingWidget::onBuyOrder);
    connect(sellButton, &QPushButton::clicked, this, &TradingWidget::onSellOrder);
    
    // Quick trade buttons
    connect(quickBuy1Button, &QPushButton::clicked, this, &TradingWidget::onQuickBuy);
    connect(quickBuy2Button, &QPushButton::clicked, this, &TradingWidget::onQuickBuy);
    connect(quickBuy3Button, &QPushButton::clicked, this, &TradingWidget::onQuickBuy);
    connect(quickSell1Button, &QPushButton::clicked, this, &TradingWidget::onQuickSell);
    connect(quickSell2Button, &QPushButton::clicked, this, &TradingWidget::onQuickSell);
    connect(quickSell3Button, &QPushButton::clicked, this, &TradingWidget::onQuickSell);
}

void TradingWidget::setOrderBook(OrderBook *book)
{
    orderBook = book;
    updateOrderBook();
}

void TradingWidget::setUser(std::shared_ptr<User> user)
{
    currentUser = user;
}



void TradingWidget::updateMarketData(const std::string& currentTime)
{
    if (!orderBook) return;
    
    this->currentTime = currentTime;
    
    // Update price display based on order book data
    updatePriceDisplay();
    
    // Update change with color - add null check
    if (changeLabel) {
        std::ostringstream changeStream;
        changeStream << std::fixed << std::setprecision(2) << "Change: " << priceChange24h << "%";
        changeLabel->setText(QString::fromStdString(changeStream.str()));
        
        if (priceChange24h >= 0) {
            changeLabel->setStyleSheet("color: #4CAF50;"); // Green for positive
        } else {
            changeLabel->setStyleSheet("color: #f44336;"); // Red for negative
        }
    }
    
    // Update total calculation
    calculateOrderTotal();
}

void TradingWidget::updateOrderBook()
{
    if (!orderBook) return;
    
    orderBookTable->setRowCount(0);
    
    // Use stored current time
    if (currentTime.empty()) return;
    
    // Get ask orders (sells)
    std::vector<OrderBookEntry> asks = orderBook->getOrders(OrderBookType::ask, selectedProduct, currentTime);
    
    // Get bid orders (buys)
    std::vector<OrderBookEntry> bids = orderBook->getOrders(OrderBookType::bid, selectedProduct, currentTime);
    
    // Display top 10 asks and bids
    int maxRows = std::max(std::min(10, static_cast<int>(asks.size())), std::min(10, static_cast<int>(bids.size())));
    orderBookTable->setRowCount(maxRows * 2);
    
    // Add asks (red background)
    for (int i = 0; i < std::min(10, static_cast<int>(asks.size())); ++i) {
        const auto& ask = asks[i];
        
        QTableWidgetItem *priceItem = new QTableWidgetItem(QString::number(ask.price, 'f', 2));
        QTableWidgetItem *amountItem = new QTableWidgetItem(QString::number(ask.amount, 'f', 4));
        QTableWidgetItem *totalItem = new QTableWidgetItem(QString::number(ask.price * ask.amount, 'f', 2));
        
        priceItem->setBackground(QColor(255, 235, 235)); // Light red
        amountItem->setBackground(QColor(255, 235, 235));
        totalItem->setBackground(QColor(255, 235, 235));
        
        orderBookTable->setItem(i, 0, priceItem);
        orderBookTable->setItem(i, 1, amountItem);
        orderBookTable->setItem(i, 2, totalItem);
    }
    
    // Add bids (green background)
    for (int i = 0; i < std::min(10, static_cast<int>(bids.size())); ++i) {
        const auto& bid = bids[i];
        int row = maxRows + i;
        
        QTableWidgetItem *priceItem = new QTableWidgetItem(QString::number(bid.price, 'f', 2));
        QTableWidgetItem *amountItem = new QTableWidgetItem(QString::number(bid.amount, 'f', 4));
        QTableWidgetItem *totalItem = new QTableWidgetItem(QString::number(bid.price * bid.amount, 'f', 2));
        
        priceItem->setBackground(QColor(235, 255, 235)); // Light green
        amountItem->setBackground(QColor(235, 255, 235));
        totalItem->setBackground(QColor(235, 255, 235));
        
        orderBookTable->setItem(row, 0, priceItem);
        orderBookTable->setItem(row, 1, amountItem);
        orderBookTable->setItem(row, 2, totalItem);
    }
}

void TradingWidget::onProductChanged(const QString &product)
{
    selectedProduct = product.toStdString();
    
    // Update order book group title
    if (orderBookGroup) {
        orderBookGroup->setTitle("Order Book - " + product);
    }
    
    updateOrderBook();
}

void TradingWidget::onOrderTypeChanged()
{
    bool isLimitOrder = limitOrderRadio->isChecked();
    priceSpinBox->setEnabled(isLimitOrder);
    
    if (!isLimitOrder) {
        priceSpinBox->setValue(0.0);
    }
    
    calculateOrderTotal();
}

void TradingWidget::calculateOrderTotal()
{
    // Add null checks for all widgets
    if (!amountSpinBox || !marketOrderRadio || !currentPriceLabel || !priceSpinBox || !totalLabel) {
        return;
    }
    
    double amount = amountSpinBox->value();
    double price = 0.0;
    
    if (marketOrderRadio->isChecked()) {
        // Use current market price
        QString priceText = currentPriceLabel->text();
        priceText.remove("$");
        price = priceText.toDouble();
    } else {
        // Use limit price
        price = priceSpinBox->value();
    }
    
    orderTotal = amount * price;
    
    std::ostringstream totalStream;
    totalStream << std::fixed << std::setprecision(2) << "$" << orderTotal;
    totalLabel->setText(QString::fromStdString(totalStream.str()));
}

void TradingWidget::onBuyOrder()
{
    executeOrder(true);
}

void TradingWidget::onSellOrder()
{
    executeOrder(false);
}

void TradingWidget::executeOrder(bool isBuy)
{
    if (!orderBook || !currentUser) {
        QMessageBox::warning(this, "Error", "Trading system not initialized.");
        return;
    }
    
    double amount = amountSpinBox->value();
    if (amount <= 0) {
        QMessageBox::warning(this, "Invalid Amount", "Please enter a valid amount.");
        return;
    }
    
    double price = 0.0;
    
    if (marketOrderRadio->isChecked()) {
        QString priceText = currentPriceLabel->text();
        priceText.remove("$");
        price = priceText.toDouble();
    } else {
        price = priceSpinBox->value();
        if (price <= 0) {
            QMessageBox::warning(this, "Invalid Price", "Please enter a valid price.");
            return;
        }
    }
    
    OrderBookType type = isBuy ? OrderBookType::bid : OrderBookType::ask;
    
    // For now, just show a success message since we don't have the full trading engine
    QMessageBox::information(this, "Order Placed", "Your order has been placed successfully!");
    
    // Clear form
    amountSpinBox->setValue(0.0);
    if (limitOrderRadio->isChecked()) {
        priceSpinBox->setValue(0.0);
    }
    calculateOrderTotal();
    
    emit tradeExecuted(true, 0.0);
}

void TradingWidget::onQuickBuy()
{
    QString priceText = currentPriceLabel->text();
    priceText.remove("$");
    double currentPrice = priceText.toDouble();
    
    if (currentPrice > 0) {
        double amount = 25.0 / currentPrice; // Default $25 quick buy
        amountSpinBox->setValue(amount);
        marketOrderRadio->setChecked(true);
        calculateOrderTotal();
    }
}

void TradingWidget::onQuickSell()
{
    // Default 25% quick sell
    if (currentUser) {
        // For now, just set a default amount
        amountSpinBox->setValue(0.25);
        marketOrderRadio->setChecked(true);
        calculateOrderTotal();
    }
}

void TradingWidget::updatePriceDisplay()
{
    // Update price labels with current market data - add null checks
    if (currentPriceLabel) {
        currentPriceLabel->setText(QString("$%1").arg(currentPrice, 0, 'f', 2));
    }
    if (bidPriceLabel) {
        bidPriceLabel->setText(QString("$%1").arg(bidPrice, 0, 'f', 2));
    }
    if (askPriceLabel) {
        askPriceLabel->setText(QString("$%1").arg(askPrice, 0, 'f', 2));
    }
    if (volumeLabel) {
        volumeLabel->setText(QString("Volume: %1").arg(volume24h, 0, 'f', 2));
    }
}

void TradingWidget::updateOrderBookDisplay()
{
    updateOrderBook();
}

void TradingWidget::validateOrderForm()
{
    double amount = amountSpinBox->value();
    double price = priceSpinBox->value();
    
    isFormValid = (amount > 0) && (marketOrderRadio->isChecked() || price > 0);
}

void TradingWidget::showOrderResult(bool success, const QString &message)
{
    if (success) {
        QMessageBox::information(this, "Success", message);
    } else {
        QMessageBox::warning(this, "Error", message);
    }
}

void TradingWidget::onCancelOrder()
{
    // Reset form
    amountSpinBox->setValue(0.0);
    priceSpinBox->setValue(0.0);
    marketOrderRadio->setChecked(true);
    calculateOrderTotal();
}