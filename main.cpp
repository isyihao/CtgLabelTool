/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QLineSeries>
#include <QTime>

#include <QtCharts/QValueAxis>

#include "analysisresult.h"
#include "chart.h"
#include "charview.h"
#include "mainwindow.h"
#include "recordutil.h"



QT_CHARTS_USE_NAMESPACE

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    QLineSeries *series = new QLineSeries();


//    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
//    for(int i=0;i<240* 60;i++){
//        int fhr =10+ qrand()%(30);
//        series->append(i, fhr);
//    }

//    //*series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);

//    Chart *chart = new Chart(0, 0, series);
//    chart->legend()->hide();
//    chart->addSeries(series);
//    QPen p = series->pen();
//    p.setWidth(5);
//    series->setPen(p);
//    chart->setTitle("Drag'n drop to move data points");

//    QValueAxis *axisX = new QValueAxis();
//    chart->addAxis(axisX, Qt::AlignBottom);
//    axisX->setRange(0, 240* 60);
//    series->attachAxis(axisX);

//    QValueAxis *axisY = new QValueAxis();
//    chart->addAxis(axisY, Qt::AlignLeft);
//    axisY->setRange(0, 50);
//    series->attachAxis(axisY);

//    QObject::connect(series, &QLineSeries::pressed, chart, &Chart::clickPoint);

//    ChartView *chartView = new ChartView(chart);
//    chartView->setRenderHint(QPainter::Antialiasing);

//    QMainWindow window;
//    window.setCentralWidget(chartView);
//    window.resize(1200, 900);
//    window.show();



    MainWindow m;
    m.show();
    return a.exec();
}
