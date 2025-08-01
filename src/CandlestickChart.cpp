#include "GUI/CandlestickChart.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QSpinBox>
#include <QValueAxis>
#include <QDateTimeAxis>
#include <QCandlestickSeries>
#include <QCandlestickSet>
#include <QLineSeries>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QDateTime>
#include <QDebug>
#include <algorithm>
#include <numeric>
#include <cstdlib>
#include <ctime>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>

// Using Candlestick struct from CandleStick.h

CandlestickChart::CandlestickChart(QWidget *parent)
    : QWidget(parent)
    , orderBook(nullptr)
    , selectedProduct("ETH/BTC")
    , selectedTimeframe("30")
    , currentTime("")
    , timeframeDays(30)
    , currentProduct("ETH/BTC")
    , chart(new QChart())
    , chartView(new QChartView(chart))
    , candlestickSeries(new QCandlestickSeries())
    , volumeSeries(new QLineSeries())
    , ma20Series(new QLineSeries())
    , ma50Series(new QLineSeries())
{
    std::srand(std::time(nullptr));
    setupUI();
    setupChart();
    connectSignals();
}

CandlestickChart::~CandlestickChart()
{
    // Destructor implementation
}

void CandlestickChart::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Controls layout
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    
    // Product selector
    QLabel *productLabel = new QLabel("Product:");
    productCombo = new QComboBox();
    productCombo->addItems({"ETH/BTC", "DOGE/BTC", "DOGE/USDT", "BTC/USDT"});
    productCombo->setCurrentText("ETH/BTC");
    
    // Timeframe selector
    QLabel *timeframeLabel = new QLabel("Days:");
    timeframeCombo = new QComboBox();
    timeframeCombo->addItems({"1 Day", "7 Days", "30 Days", "90 Days"});
    timeframeCombo->setCurrentText("30 Days");
    
    // Zoom controls
    zoomInButton = new QPushButton("Zoom In");
    zoomOutButton = new QPushButton("Zoom Out");
    resetZoomButton = new QPushButton("Reset Zoom");
    
    // Update button
    updateChartButton = new QPushButton("Update Chart");
    updateChartButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; }");
    
    controlsLayout->addWidget(productLabel);
    controlsLayout->addWidget(productCombo);
    controlsLayout->addWidget(timeframeLabel);
    controlsLayout->addWidget(timeframeCombo);
    controlsLayout->addWidget(zoomInButton);
    controlsLayout->addWidget(zoomOutButton);
    controlsLayout->addWidget(resetZoomButton);
    controlsLayout->addWidget(updateChartButton);
    controlsLayout->addStretch();
    
    mainLayout->addLayout(controlsLayout);
    mainLayout->addWidget(chartView);
}

void CandlestickChart::setupChart()
{
    chart->setTitle("Candlestick Chart - " + currentProduct);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    
    // Setup candlestick series
    candlestickSeries->setName("Price");
    candlestickSeries->setIncreasingColor(QColor(0, 255, 0));
    candlestickSeries->setDecreasingColor(QColor(255, 0, 0));
    
    // Setup moving averages
    ma20Series->setName("MA20");
    ma20Series->setColor(QColor(255, 165, 0)); // Orange
    
    ma50Series->setName("MA50");
    ma50Series->setColor(QColor(128, 0, 128)); // Purple
    
    // Add series to chart
    chart->addSeries(candlestickSeries);
    chart->addSeries(ma20Series);
    chart->addSeries(ma50Series);
    
    // Setup axes
    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setFormat("MM-dd hh:mm");
    axisX->setTitleText("Time");
    chart->addAxis(axisX, Qt::AlignBottom);
    
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Price (USDT)");
    chart->addAxis(axisY, Qt::AlignLeft);
    
    // Attach series to axes
    candlestickSeries->attachAxis(axisX);
    candlestickSeries->attachAxis(axisY);
    ma20Series->attachAxis(axisX);
    ma20Series->attachAxis(axisY);
    ma50Series->attachAxis(axisX);
    ma50Series->attachAxis(axisY);
    
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setRubberBand(QChartView::RectangleRubberBand);
}

void CandlestickChart::connectSignals()
{
    connect(productCombo, QOverload<const QString &>::of(&QComboBox::currentTextChanged),
            this, &CandlestickChart::onProductChanged);
    connect(timeframeCombo, QOverload<const QString &>::of(&QComboBox::currentTextChanged),
            this, &CandlestickChart::onTimeframeChanged);
    connect(updateChartButton, &QPushButton::clicked, this, [this]() { updateChart(currentTime); });
    connect(zoomInButton, &QPushButton::clicked, this, &CandlestickChart::onZoomIn);
    connect(zoomOutButton, &QPushButton::clicked, this, &CandlestickChart::onZoomOut);
    connect(resetZoomButton, &QPushButton::clicked, this, &CandlestickChart::onResetZoom);
}

void CandlestickChart::setOrderBook(OrderBook *book)
{
    orderBook = book;
    updateChart(currentTime);
}

void CandlestickChart::updateChart(const std::string& currentTime)
{
    if (!orderBook) {
        qDebug() << "OrderBook not set";
        return;
    }
    
    // Clear existing data
    candlestickSeries->clear();
    ma20Series->clear();
    ma50Series->clear();
    
    // Get all timestamps for the product
    std::vector<std::string> timestamps = orderBook->getKnownTimestamps(selectedProduct);
    
    if (timestamps.empty()) {
        qDebug() << "No timestamps available";
        return;
    }
    
    // Limit to timeframe
    int maxPoints = std::min(timeframeDays * 24, static_cast<int>(timestamps.size()));
    std::vector<std::string> limitedTimestamps(timestamps.end() - maxPoints, timestamps.end());
    
    std::vector<double> closePrices;
    
    for (const std::string &timestamp : limitedTimestamps) {
        // Get candlestick data for this timestamp
        // Mock candlestick data for demonstration
        Candlestick candleData;
        candleData.timestamp = timestamp;
        candleData.open = 100.0 + (rand() % 20);
        candleData.high = candleData.open + (rand() % 10);
        candleData.low = candleData.open - (rand() % 10);
        candleData.close = candleData.low + (rand() % static_cast<int>(candleData.high - candleData.low));
        // candleData.volume = 1000 + (rand() % 5000); // Volume not used in this struct
        
        if (candleData.open > 0) { // Valid data
            QDateTime dateTime = QDateTime::fromString(QString::fromStdString(timestamp), "yyyy/MM/dd hh:mm:ss");
            
            // Create candlestick set
            QCandlestickSet *candleSet = new QCandlestickSet(dateTime.toMSecsSinceEpoch());
            candleSet->setOpen(candleData.open);
            candleSet->setHigh(candleData.high);
            candleSet->setLow(candleData.low);
            candleSet->setClose(candleData.close);
            
            candlestickSeries->append(candleSet);
            closePrices.push_back(candleData.close);
            
            // Calculate moving averages
            if (closePrices.size() >= 20) {
                double ma20 = 0;
                for (int i = closePrices.size() - 20; i < closePrices.size(); ++i) {
                    ma20 += closePrices[i];
                }
                ma20 /= 20;
                ma20Series->append(dateTime.toMSecsSinceEpoch(), ma20);
            }
            
            if (closePrices.size() >= 50) {
                double ma50 = 0;
                for (int i = closePrices.size() - 50; i < closePrices.size(); ++i) {
                    ma50 += closePrices[i];
                }
                ma50 /= 50;
                ma50Series->append(dateTime.toMSecsSinceEpoch(), ma50);
            }
        }
    }
    
    // Update chart title
    chart->setTitle("Candlestick Chart - " + currentProduct + " (" + QString::number(timeframeDays) + " days)");
    
    // Auto-scale axes
    chart->axes(Qt::Horizontal).first()->setRange(
        QDateTime::fromString(QString::fromStdString(limitedTimestamps.front()), "yyyy/MM/dd hh:mm:ss"),
        QDateTime::fromString(QString::fromStdString(limitedTimestamps.back()), "yyyy/MM/dd hh:mm:ss")
    );
}

void CandlestickChart::onProductChanged(const QString &product)
{
    selectedProduct = product.toStdString();
    currentProduct = product;
    updateChart(currentTime);
}

void CandlestickChart::onTimeframeChanged(const QString &timeframe)
{
    if (timeframe == "1 Day") {
        timeframeDays = 1;
    } else if (timeframe == "7 Days") {
        timeframeDays = 7;
    } else if (timeframe == "30 Days") {
        timeframeDays = 30;
    } else if (timeframe == "90 Days") {
        timeframeDays = 90;
    }
    updateChart(currentTime);
}

void CandlestickChart::onZoomIn()
{
    chartView->chart()->zoomIn();
}

void CandlestickChart::onZoomOut()
{
    chartView->chart()->zoomOut();
}

void CandlestickChart::onResetZoom()
{
    chartView->chart()->zoomReset();
}

void CandlestickChart::onRefreshChart()
{
    // Refresh chart implementation
    updateChart(currentTime);
}

void CandlestickChart::onToggleVolume(bool show)
{
    // Toggle volume display implementation
    if (show) {
        chart->addSeries(volumeSeries);
    } else {
        chart->removeSeries(volumeSeries);
    }
}