#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "analysisresult.h"
#include "chart.h"
#include "charview.h"
#include <QMainWindow>
#include <QGraphicsTextItem>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void onSensitivitySliderValueChanged(int value);
    void onTimeout();

    void on_recordsListView_clicked(const QModelIndex &index);

    void on_saveButton_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

private:
    ChartView *fhrChartView;
    ChartView *tocoChartView;
    Ui::MainWindow *ui;
    // ctg分析结果数据
    AnalysisResult data;

    /**
     * @brief 初始化胎监图
     * @param result 胎监分析结果
     */
    void initCTGView();
};

#endif // MAINWINDOW_H
