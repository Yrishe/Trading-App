#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QComboBox>
#include <QDateEdit>
#include <QLineEdit>
#include <QCheckBox>
#include <memory>

#include "../Auth/User.h"
#include "../OrderBook.h"
#include "../OrderBookEntry.h"

class OrderWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(OrderWidget)
    
public:
    explicit OrderWidget(QWidget *parent = nullptr);
    ~OrderWidget();
    
    void setUser(std::shared_ptr<User> user);
    void setOrderBook(OrderBook* orderBook);
    void updateOrders(const std::string& currentTime);
    void addOrder(const OrderBookEntry& order);

signals:
    void orderCancelled(const QString &orderId);
    void orderModified(const QString &orderId);

public slots:
    void onCancelOrder();
    void onModifyOrder();
    void onRefreshOrders();
    void onFilterChanged();
    void onExportHistory();
    void onOrderSelected();
    void onTabChanged(int index);

private:
    void setupUI();
    void setupActiveOrdersTab();
    void setupOrderHistoryTab();
    void setupTradeHistoryTab();
    void connectSignals();
    void updateActiveOrdersTable();
    void updateOrderHistoryTable();
    void updateTradeHistoryTable();
    void applyFilters();
    void showOrderDetails(const OrderBookEntry& order);
    void calculateTradeStatistics();
    
    // UI Components
    QVBoxLayout *mainLayout;
    QTabWidget *tabWidget;
    
    // Active Orders Tab
    QWidget *activeOrdersTab;
    QVBoxLayout *activeOrdersLayout;
    QHBoxLayout *activeOrdersControlsLayout;
    QTableWidget *activeOrdersTable;
    QPushButton *cancelOrderButton;
    QPushButton *modifyOrderButton;
    QPushButton *refreshOrdersButton;
    QLabel *activeOrdersCountLabel;
    
    // Order History Tab
    QWidget *orderHistoryTab;
    QVBoxLayout *orderHistoryLayout;
    QHBoxLayout *historyControlsLayout;
    QTableWidget *orderHistoryTable;
    QComboBox *statusFilterCombo;
    QComboBox *productFilterCombo;
    QDateEdit *fromDateEdit;
    QDateEdit *toDateEdit;
    QPushButton *applyFilterButton;
    QPushButton *clearFilterButton;
    QPushButton *exportHistoryButton;
    QLabel *historyCountLabel;
    
    // Trade History Tab
    QWidget *tradeHistoryTab;
    QVBoxLayout *tradeHistoryLayout;
    QHBoxLayout *tradeControlsLayout;
    QTableWidget *tradeHistoryTable;
    QGroupBox *tradeStatsGroup;
    QGridLayout *tradeStatsLayout;
    QLabel *totalTradesLabel;
    QLabel *successfulTradesLabel;
    QLabel *successRateLabel;
    QLabel *totalProfitLabel;
    QLabel *avgProfitLabel;
    QLabel *bestTradeLabel;
    QLabel *worstTradeLabel;
    QLabel *totalVolumeLabel;
    QPushButton *refreshTradesButton;
    QPushButton *exportTradesButton;
    
    // Data
    std::shared_ptr<User> currentUser;
    OrderBook *orderBook;
    std::string currentTime;
    
    // Order data
    struct OrderInfo {
        std::string id;
        std::string timestamp;
        std::string product;
        OrderBookType type;
        double price;
        double amount;
        double filled;
        std::string status;
        double profit;
    };
    
    std::vector<OrderInfo> activeOrders;
    std::vector<OrderInfo> orderHistory;
    std::vector<OrderInfo> tradeHistory;
    
    // Filter state
    QString statusFilter;
    QString productFilter;
    QDate fromDate;
    QDate toDate;
    
    // Statistics
    struct TradeStats {
        int totalTrades;
        int successfulTrades;
        double successRate;
        double totalProfit;
        double avgProfit;
        double bestTrade;
        double worstTrade;
        double totalVolume;
    };
    
    TradeStats tradeStats;
    
    // Helper methods
    void setupTableHeaders(QTableWidget* table, const QStringList& headers);
    void populateOrderTable(QTableWidget* table, const std::vector<OrderInfo>& orders);
    QString formatCurrency(double value, const QString& currency = "USDT");
    QString formatPercentage(double value);
    QString getOrderStatusText(const OrderInfo& order);
    QColor getOrderStatusColor(const OrderInfo& order);
    void updateTradeStats();
    
    // Constants
    static const QStringList ACTIVE_ORDERS_HEADERS;
    static const QStringList ORDER_HISTORY_HEADERS;
    static const QStringList TRADE_HISTORY_HEADERS;
    static const QStringList ORDER_STATUS_FILTERS;
};