#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QGroupBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QProgressBar>
#include <memory>

#include "../OrderBook.h"
#include "../Auth/User.h"

class TradingWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(TradingWidget)
    
public:
    explicit TradingWidget(QWidget *parent = nullptr);
    ~TradingWidget();
    
    void setOrderBook(OrderBook* orderBook);
    void setUser(std::shared_ptr<User> user);
    void updateMarketData(const std::string& currentTime);

signals:
    void tradeExecuted(bool successful, double profit);
    void walletUpdated();

public slots:
    void onProductChanged(const QString &product);
    void onBuyOrder();
    void onSellOrder();
    void onCancelOrder();
    void onOrderTypeChanged();
    void updateOrderBook();
    void onQuickBuy();
    void onQuickSell();

private:
    void setupUI();
    void setupOrderForm();
    void setupMarketData();
    void setupOrderBook();
    void setupQuickTrade();
    void connectSignals();
    void updatePriceDisplay();
    void updateOrderBookDisplay();
    void validateOrderForm();
    void executeOrder(bool isBuy);
    void showOrderResult(bool success, const QString &message);
    void calculateOrderTotal();
    
    // UI Components
    QVBoxLayout *mainLayout;
    QHBoxLayout *topLayout;
    
    // Market data section
    QGroupBox *marketDataGroup;
    QGridLayout *marketDataLayout;
    QComboBox *productCombo;
    QLabel *currentPriceLabel;
    QLabel *bidPriceLabel;
    QLabel *askPriceLabel;
    QLabel *spreadLabel;
    QLabel *volumeLabel;
    QLabel *changeLabel;
    QLabel *highLabel;
    QLabel *lowLabel;
    
    // Order form section
    QGroupBox *orderFormGroup;
    QFormLayout *orderFormLayout;
    QButtonGroup *orderTypeGroup;
    QRadioButton *marketOrderRadio;
    QRadioButton *limitOrderRadio;
    QDoubleSpinBox *priceSpinBox;
    QDoubleSpinBox *amountSpinBox;
    QLabel *totalLabel;
    QPushButton *buyButton;
    QPushButton *sellButton;
    QLabel *orderStatusLabel;
    
    // Quick trade section
    QGroupBox *quickTradeGroup;
    QGridLayout *quickTradeLayout;
    QPushButton *quickBuy1Button;
    QPushButton *quickBuy2Button;
    QPushButton *quickBuy3Button;
    QPushButton *quickSell1Button;
    QPushButton *quickSell2Button;
    QPushButton *quickSell3Button;
    
    // Order book section
    QGroupBox *orderBookGroup;
    QVBoxLayout *orderBookLayout;
    QTableWidget *orderBookTable;
    QPushButton *refreshOrderBookButton;
    
    // Data
    OrderBook *orderBook;
    std::shared_ptr<User> currentUser;
    std::string selectedProduct;
    std::string currentTime;
    
    // Market data
    double currentPrice;
    double bidPrice;
    double askPrice;
    double volume24h;
    double priceChange24h;
    double highPrice24h;
    double lowPrice24h;
    
    // Order state
    bool isFormValid;
    double orderTotal;
    
    // Constants
    static const QStringList AVAILABLE_PRODUCTS;
    static const double QUICK_TRADE_AMOUNTS[];
};