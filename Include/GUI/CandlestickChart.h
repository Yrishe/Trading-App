#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QTimer>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QCandlestickSeries>
#include <QtCharts/QCandlestickSet>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#include <vector>
#include <memory>

#include "../OrderBook.h"
#include "../CandleStick.h"

class CandlestickChart : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(CandlestickChart)
    
public:
    explicit CandlestickChart(QWidget *parent = nullptr);
    ~CandlestickChart();
    
    void setOrderBook(OrderBook* orderBook);
    void updateChart(const std::string& currentTime);
    void setSelectedProduct(const std::string& product);

public slots:
    void onProductChanged(const QString &product);
    void onTimeframeChanged(const QString &timeframe);
    void onRefreshChart();
    void onToggleVolume(bool show);
    void onZoomIn();
    void onZoomOut();
    void onResetZoom();

private:
    void setupUI();
    void setupChart();
    void setupControls();
    void connectSignals();
    void updateCandlestickData();
    void updateVolumeData();
    void addMovingAverage(int period);
    void clearChart();
    
    std::vector<Candlestick> getCandlestickData(const std::string& product, const std::string& timeframe);
    QDateTime stringToDateTime(const std::string& timestamp);
    
    // UI Components
    QVBoxLayout *mainLayout;
    QHBoxLayout *controlsLayout;
    
    // Controls
    QComboBox *productCombo;
    QComboBox *timeframeCombo;
    QPushButton *refreshButton;
    QPushButton *updateChartButton;
    QPushButton *zoomInButton;
    QPushButton *zoomOutButton;
    QPushButton *resetZoomButton;
    QCheckBox *showVolumeCheck;
    QCheckBox *showMA20Check;
    QCheckBox *showMA50Check;
    QLabel *priceLabel;
    QLabel *volumeLabel;
    QLabel *changeLabel;
    
    // Chart components
    QChart *chart;
    QChartView *chartView;
    QCandlestickSeries *candlestickSeries;
    QLineSeries *volumeSeries;
    QLineSeries *ma20Series;
    QLineSeries *ma50Series;
    
    // Axes
    QDateTimeAxis *axisX;
    QValueAxis *axisY;
    QValueAxis *volumeAxisY;
    
    // Data
    OrderBook *orderBook;
    CandleStick candleStick;
    std::string selectedProduct;
    std::string selectedTimeframe;
    std::string currentTime;
    int timeframeDays;
    QString currentProduct;
    
    // Chart state
    bool volumeVisible;
    bool ma20Visible;
    bool ma50Visible;
    double minPrice;
    double maxPrice;
    
    // Constants
    static const QStringList AVAILABLE_PRODUCTS;
    static const QStringList AVAILABLE_TIMEFRAMES;
};