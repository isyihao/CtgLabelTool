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

#include "chart.h"
#include <QtCharts/QValueAxis>
#include <QtCharts/QAbstractAxis>
#include <QtCore/QtMath>
#include <QDebug>

#include<algorithm>
#include <qgraphicslinearlayout.h>
using namespace std;

Chart::Chart(QLineSeries *series)
    : QChart(QChart::ChartTypeCartesian, 0, 0), m_series(series)
{
    m_clicked = false;
}

Chart::~Chart()
{
}

void Chart::clickPoint(const QPointF &mousePressPoint)
{
    if(m_series==0){
        return;
    }
    // 从chart监听的折线Series中找到与点击位置距离最近的点
    m_movingPoint = QPoint(0,0);
    m_clicked = false;
    const auto points = m_series->points();

    for (QPointF seriesPoint : points) {
        int dis = distance(seriesPoint, mousePressPoint);
        if (dis <25 && dis < distance(m_movingPoint, mousePressPoint)) {
            m_movingPoint = seriesPoint;
            m_clicked = true;
        }
    }
}

qreal Chart::distance(const QPointF &p1, const QPointF &p2)
{
    return qSqrt((p1.x() - p2.x()) * (p1.x() - p2.x())
                + (p1.y() - p2.y()) * (p1.y() - p2.y()));
}

void Chart::setPointClicked(bool clicked)
{
    m_clicked = clicked;
}

void Chart::handlePointMove(const QPoint &point)
{
    if(m_series==0){
        return;
    }
    if (m_clicked) {
        //Map the point clicked from the ChartView
        //to the area occupied by the chart.
        QPoint mappedPoint = point;
        mappedPoint.setX(point.x() - this->plotArea().x());
        mappedPoint.setY(point.y() - this->plotArea().y());

        //Get the x- and y axis to be able to convert the mapped
        //coordinate point to the charts scale.
        QAbstractAxis *axisx = axes(Qt::Horizontal).first();
        QValueAxis *haxis = 0;
        if (axisx->type() == QAbstractAxis::AxisTypeValue)
            haxis = qobject_cast<QValueAxis *>(axisx);

        QAbstractAxis *axisy = axes(Qt::Vertical).first();
        QValueAxis *vaxis = 0;
        if (axisy->type() == QAbstractAxis::AxisTypeValue)
            vaxis = qobject_cast<QValueAxis *>(axisy);

        if (haxis && vaxis) {
            //Calculate the "unit" between points on the x
            //y axis.
            double xUnit = this->plotArea().width() / haxis->max();
            double yUnit = this->plotArea().height() / (vaxis->max()-vaxis->min());

            //Convert the mappedPoint to the actual chart scale.
            double x = mappedPoint.x() / xUnit;
            double y = vaxis->max() - mappedPoint.y() / yUnit;

            //Replace the old point with the new one


//            if(((int)m_movingPoint.x()%this->baselineDownSample)==0){ // 如果是最后一个点 则不操作
//                return;
//            }

            int effectiveArea = Config::SENSITIVITY * Config::BASELINE_DOWNSAMPLE;

            qDebug()<<"Config::SENSITIVITY "<<Config::SENSITIVITY;
            qDebug()<<"Config::BASELINE_DOWNSAMPLE "<<Config::BASELINE_DOWNSAMPLE;
            qDebug()<<"effectiveArea "<<effectiveArea;
            int startX = max<int>(0,m_movingPoint.x() -  effectiveArea);
            int midX = m_movingPoint.x();
            int endX = min<int>(m_movingPoint.x() + effectiveArea,haxis->max());
            // 线性插值
            const QPointF startPoint = m_series->at(startX/Config::BASELINE_DOWNSAMPLE);
            const QPointF midPoint = m_series->at(midX/Config::BASELINE_DOWNSAMPLE);
            const QPointF endPoint = m_series->at(endX/Config::BASELINE_DOWNSAMPLE);

//            qDebug()<<"startX "<<startX << " midX"<<midX<< " endX"<<endX;
//            qDebug()<<"startPoint "<<startPoint << " midPoint"<<midPoint<< " endPoint"<<endPoint;
//            qDebug()<<"x "<<x << " y"<<y;

            double leftK = (y-startPoint.y())/(midPoint.x()-startPoint.x());
            qDebug()<<"leftK "<<leftK;
            for(int i=startX+Config::BASELINE_DOWNSAMPLE;i<=midX;i+=Config::BASELINE_DOWNSAMPLE){
                const QPointF originPoint = m_series->at(i/Config::BASELINE_DOWNSAMPLE);
                int newY = leftK*(i-startX)+startPoint.y();
                newY = std::min(newY,200);
                newY = std::max(newY,60);
                QPointF newPoint = QPointF(i,newY);
                qDebug()<<"originPoint left "<<originPoint << " newPoint"<<newPoint;
                m_series->replace(originPoint, newPoint);
            }
            double rightK = (endPoint.y()-y)/(endPoint.x()-midPoint.x());
            qDebug()<<"rightK "<<rightK;
            for(int i=midX+Config::BASELINE_DOWNSAMPLE;i<=endX;i+=Config::BASELINE_DOWNSAMPLE){
                const QPointF originPoint = m_series->at(i/Config::BASELINE_DOWNSAMPLE);
                int newY = rightK*(i-midX)+y;
                newY = std::min(newY,200);
                newY = std::max(newY,60);
                QPointF newPoint = QPointF(i,newY);
                qDebug()<<"originPoint right "<<originPoint << " newPoint"<<newPoint;
                m_series->replace(originPoint, newPoint);
            }
            qDebug()<<"originPoint "<<m_movingPoint << " newPoint"<<QPointF(m_movingPoint.x(), y);

//            m_series->replace(m_movingPoint, QPointF(m_movingPoint.x(), y));

            //Update the m_movingPoint so we are able to
            //do the replace also during mousemoveEvent.
            //m_movingPoint.setX(x);
            m_movingPoint.setY(y);
        }
    }
}

QLineSeries* Chart::getCorrectSeries(){
    return this->m_series;
}

