#include "mainwindow.h"
#include "recordutil.h"
#include "ui_mainwindow.h"
#include <QLineSeries>
#include <QTime>
#include <QValueAxis>
#include <qlineseries.h>
#include <QValueAxis>
#include <QCategoryAxis>
#include <QStyle>
#include <QGraphicsLayout>
#include <QGraphicsGridLayout>
#include <QGraphicsLinearLayout>
#include <QTimer>
#include <QDir>
#include <QStandardItemModel>
#include <QSplitter>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    QDir dir(QString("%1/%2").arg(QCoreApplication::applicationDirPath()).arg("records"));
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Name);

    QFileInfoList list = dir.entryInfoList();
    qDebug()<<list.size();
    QStandardItemModel *recordsModel = new QStandardItemModel(this);
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        QStandardItem *item = new QStandardItem(QString("%1").arg(fileInfo.fileName()));
        recordsModel->appendRow(item);
    }

    QFile file(QString("%1/%2").arg(QCoreApplication::applicationDirPath()).arg("styles/QListView.qss"));
    file.open(QFile::ReadOnly);
    QTextStream filetext(&file);
    QString stylesheet = filetext.readAll();
    this->ui->recordsListView->setStyleSheet(stylesheet);
    this->ui->recordsListView->setTextElideMode(Qt::ElideNone);

    this->ui->recordsListView->setModel(recordsModel);
    this->ui->recordsListView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(ui->sensitivitySlider,SIGNAL(valueChanged(int)),this,SLOT(onSensitivitySliderValueChanged(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onSensitivitySliderValueChanged(int value)
{
    Config::SENSITIVITY= value;
    this->ui->sensitivityLabel->setText(QString("%1 s").arg(value*10));
}

void MainWindow::onTimeout()
{
    qDebug()<<"onTimeout";
}

void MainWindow::initCTGView(){
    // 开始构建胎心图
    QLineSeries *fhr1Series = new QLineSeries();
    QLineSeries *baselineSeries = new QLineSeries();
    for(int i=0;i<data.length;i++){
        fhr1Series->append(i, data.fhr1.at(i));
    }
    // 基线数据 注意降采样
    for(int i=0;i<data.correctBaseline.length();i++){
        int x = std::min(i*data.baselineDownsample,data.length);
        int y = data.correctBaseline.at(i);
        baselineSeries->append(x, y);
    }
    // FHR1画笔
    QPen fhr1Pen = fhr1Series->pen();
    fhr1Pen.setWidth(1);
    fhr1Pen.setColor(QColor("#265580"));
    fhr1Series->setPen(fhr1Pen);
    // 基线画笔
    QPen baselinePen = baselineSeries->pen();
    baselinePen.setWidth(2);
    baselinePen.setColor(QColor("#e41906"));
    baselineSeries->setPen(baselinePen);

    // 胎心图,其中baselineSeries为可编辑的曲线
    Chart *fhrChart = new Chart(baselineSeries);
    fhrChart->legend()->hide();
    fhrChart->addSeries(fhr1Series);
    fhrChart->addSeries(baselineSeries);
    fhrChart->setTitle(data.recordName);
    fhrChart->setMargins(QMargins(-5,-27,0,-10));

    // 胎心图X刻度网格线
    QValueAxis *fhrValueAxisX = new QValueAxis();
    fhrChart->addAxis(fhrValueAxisX, Qt::AlignTop); // 放置在图标上方，避免占用下方文字坐标轴位置
    fhrValueAxisX->setRange(0, data.length-1);
    fhrValueAxisX->setTickType(QValueAxis::TickType::TicksDynamic); // 动态刻度
    fhrValueAxisX->setTickInterval(240);// 设置刻度间距为240
    fhrValueAxisX->setTickAnchor(0); // 设置锚点坐标为0
    fhrValueAxisX->setMinorTickCount(2);
    fhrValueAxisX->setLabelsVisible(false);

    fhr1Series->attachAxis(fhrValueAxisX);
    baselineSeries->attachAxis(fhrValueAxisX);

    // 网格线画笔
    QPen xGridLinePen;
    xGridLinePen.setColor(QColor("#adadad"));
    xGridLinePen.setWidth(2);
    fhrValueAxisX->setGridLinePen(xGridLinePen);
    // ctg图X文字轴 用于显示时间字符
    QCategoryAxis *ctgCategoryAxisX = new QCategoryAxis;
    ctgCategoryAxisX->setMin(0);
    ctgCategoryAxisX->setMax(data.length-1);
    ctgCategoryAxisX->setStartValue(0);
    ctgCategoryAxisX->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    for(int i=0;i<data.length;i+=240){
        // 从第0分钟开始，每隔1分钟显示【hh:mm】
        int offsetMin = i/240;
        QDateTime t = data.startTime.addSecs(offsetMin*60);
        QString timeLabel = t.toString("hh:mm");
        ctgCategoryAxisX->append(timeLabel, i);
        // 从第1分钟开始，每隔1分钟显示【x分】
        i+=240;
        if(i<data.length){
            QString minuteLabel = QString("%1分").arg(i/240);
            ctgCategoryAxisX->append(minuteLabel, i);
        }
    }
    fhrChart->addAxis(ctgCategoryAxisX,Qt::AlignBottom);

    // 胎心图Y值轴
    QValueAxis *fhrValueAxisY = new QValueAxis();
    fhrChart->addAxis(fhrValueAxisY, Qt::AlignLeft);
    fhrValueAxisY->setRange(60, 210);
    fhrValueAxisY->setTickCount(6);
    fhrValueAxisY->setMinorTickCount(2);
    fhrValueAxisY->setLabelFormat("%d");

    // 胎心图Y值轴刻度线画笔
    QPen yGridLinePen;
    yGridLinePen.setColor(QColor("#adadad"));
    yGridLinePen.setWidth(2);
    fhrValueAxisY->setGridLinePen(yGridLinePen);
    fhr1Series->attachAxis(fhrValueAxisY);
    baselineSeries->attachAxis(fhrValueAxisY);

    // 胎心图视图
    this->fhrChartView = new ChartView(fhrChart);
    fhrChartView->setRenderHint(QPainter::Antialiasing);
    fhrChartView->setMinimumSize(data.length * 0.75,500);
    fhrChartView->setStyleSheet("background-color:white;padding:0;");

    // 开始构建宫缩图
    QLineSeries *tocoSeries = new QLineSeries();
    for(int i=0;i<data.length;i++){
        tocoSeries->append(i, data.toco.at(i));
    }

    // toco画笔
    QPen tocoPen = tocoSeries->pen();
    tocoPen.setWidth(1);
    tocoPen.setColor(QColor("#000000"));
    tocoSeries->setPen(tocoPen);

    // 胎心图,其中baselineSeries为可编辑的曲线
    Chart *tocoChart = new Chart(0);
    tocoChart->legend()->hide();
    tocoChart->addSeries(tocoSeries);
    tocoChart->setMargins(QMargins(-5,-27,0,-10));

    // 宫缩图X刻度网格线
    QValueAxis *tocoValueAxisX = new QValueAxis();
    tocoChart->addAxis(tocoValueAxisX, Qt::AlignTop); // 放置在图标上方，避免占用下方文字坐标轴位置
    tocoValueAxisX->setRange(0, data.length-1);
    tocoValueAxisX->setTickType(QValueAxis::TickType::TicksDynamic); // 动态刻度
    tocoValueAxisX->setTickInterval(240);// 设置刻度间距为240
    tocoValueAxisX->setTickAnchor(0); // 设置锚点坐标为0
    tocoValueAxisX->setMinorTickCount(2);
    tocoValueAxisX->setLabelsVisible(false);

    // 网格线画笔
    QPen tocoXGridLinePen;
    tocoXGridLinePen.setColor(QColor("#adadad"));
    tocoXGridLinePen.setWidth(2);
    tocoValueAxisX->setGridLinePen(tocoXGridLinePen);
    tocoSeries->attachAxis(tocoValueAxisX);

    // 宫缩图Y值轴
    QValueAxis *tocoValueAxisY = new QValueAxis();
    tocoChart->addAxis(tocoValueAxisY, Qt::AlignLeft);
    tocoValueAxisY->setRange(0, 100);
    tocoValueAxisY->setTickCount(6);
    tocoValueAxisY->setMinorTickCount(1);
    tocoValueAxisY->setLabelFormat("%d");

    // 宫缩Y值轴刻度线画笔
    QPen tocoYGridLinePen;
    tocoYGridLinePen.setColor(QColor("#adadad"));
    tocoYGridLinePen.setWidth(2);
    tocoValueAxisY->setGridLinePen(tocoYGridLinePen);
    tocoSeries->attachAxis(tocoValueAxisY);

    // 宫缩图视图
    this->tocoChartView = new  ChartView(tocoChart);
    tocoChartView->setRenderHint(QPainter::Antialiasing);
    tocoChartView->setMinimumSize(data.length*0.75,300);
    tocoChartView->setStyleSheet("background-color:white;padding:0;");

    QVBoxLayout *vboxLayout= new QVBoxLayout();
    vboxLayout->addWidget(fhrChartView);
    vboxLayout->addWidget(tocoChartView);
    vboxLayout->setMargin(0);
    vboxLayout->setSpacing(0);
    vboxLayout->setContentsMargins(0,0,0,0);

    QWidget *widget = new QWidget();
    widget->setStyleSheet("background-color:white;padding:0;margin:0");
    widget->setLayout(vboxLayout);

    ui->scrollArea->setWidget(widget);
    ui->scrollArea->setWidgetResizable(true);

    QAbstractAxis *axisx = fhrChart->axes(Qt::Horizontal).first();
    QValueAxis *haxis = 0;
    if (axisx->type() == QAbstractAxis::AxisTypeValue)
        haxis = qobject_cast<QValueAxis *>(axisx);

    QAbstractAxis *axisy = fhrChart->axes(Qt::Vertical).first();
    QValueAxis *vaxis = 0;
    if (axisy->type() == QAbstractAxis::AxisTypeValue)
        vaxis = qobject_cast<QValueAxis *>(axisy);

    double xUnit = fhrChart->plotArea().width() / haxis->max();
    double yUnit = fhrChart->plotArea().height() / (vaxis->max()-vaxis->min());

    for(int i=240;i<data.length;i+=720){
        for(int j=0;j<4;j++){
            QGraphicsTextItem *testText = new QGraphicsTextItem(fhrChart);

            QFont textFont;
            textFont.setBold(true);
            textFont.setFamily("Microsoft YaHei");
            testText->setFont(textFont);
            testText->setDefaultTextColor("#265580");
            int chartX = i;
            int chartY = 90+j*30;
            testText->setPlainText(QString("%1").arg(chartY));
            testText->setPos(QPointF(fhrChartView->chartPos2ViewPos(QPoint(chartX,chartY+7))));
        }
    }
}

void MainWindow::on_recordsListView_clicked(const QModelIndex &index)
{
    QVariant variant = ui->recordsListView->model()->data(index, Qt::DisplayRole);  //获取当前选择的项的文本
    if (variant.isValid())
    {
        QString filePath = QString("%1/%2/%3").arg(QCoreApplication::applicationDirPath()).arg("records").arg(variant.toString());
        // 解析、载入数据
        RecordUtil recordUtil;
        QJsonObject localReadJsonFromFile = recordUtil.readJsonFromFile(filePath);
        AnalysisResult result(localReadJsonFromFile);
        result.filePath = filePath;
        this->data = result;
        this->ui->currentRecordLabel->setText(this->data.recordName);
        initCTGView();
    }
}

void MainWindow::on_saveButton_clicked()
{
    QLineSeries *correctSeries = this->fhrChartView->m_chart->getCorrectSeries();
    QVector<int> correctBaseline;
    for(int i=0;i<correctSeries->count();i++){
        correctBaseline.append(correctSeries->at(i).y());
    }
    // 第一个点和最后一个点抹平
    if(correctBaseline.size()>2){
        correctBaseline.replace(0,correctBaseline.at(1));
        correctBaseline.replace(correctBaseline.size()-1,correctBaseline.at(correctBaseline.size()-2));
    }
    this->data.correctBaseline = correctBaseline;
    this->data.save();
}

void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    if(arg1 != nullptr && arg1.length()>0){
        this->data.correctedBy = arg1;
    }
}
