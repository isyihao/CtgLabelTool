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

#include "charview.h"
#include <QtGui/QMouseEvent>
#include "chart.h"
#include <QDebug>
#include <QValueAxis>

 ChartView:: ChartView(Chart *chart, QWidget *parent) :
    QChartView(chart, parent)
{
    m_chart = chart;
}

 QPoint ChartView::chartPos2ViewPos(QPoint chartPos)
 {
     // 获取chart的x轴、y轴
     QAbstractAxis *axisx = m_chart->axes(Qt::Horizontal).first();
     QValueAxis *haxis = 0;
     if (axisx->type() == QAbstractAxis::AxisTypeValue)
         haxis = qobject_cast<QValueAxis *>(axisx);

     QAbstractAxis *axisy = m_chart->axes(Qt::Vertical).first();
     QValueAxis *vaxis = 0;
     if (axisy->type() == QAbstractAxis::AxisTypeValue)
         vaxis = qobject_cast<QValueAxis *>(axisy);
     if (haxis && vaxis){
         // 计算两轴单位长度
         double xUnit = m_chart->plotArea().width() / haxis->max();
         double yUnit = m_chart->plotArea().height() / (vaxis->max()-vaxis->min());
         // 将chart的轴坐标位置转化为视图的空间位置
         int x = chartPos.x() * xUnit +m_chart->plotArea().x();
         int y = this->height()-m_chart->plotArea().y() - (chartPos.y() - 60 ) * yUnit;
         return QPoint(x,y);
     }
     return QPoint(0,0);

 }

void  ChartView::mousePressEvent(QMouseEvent *event)
{
    // 记录当前鼠标点击位置（该位置是ChartView的空间位置，锚点为左上角）
    m_mousePos = event->pos();
    // 获取chart的x轴、y轴
    QAbstractAxis *axisx = m_chart->axes(Qt::Horizontal).first();
    QValueAxis *haxis = 0;
    if (axisx->type() == QAbstractAxis::AxisTypeValue)
        haxis = qobject_cast<QValueAxis *>(axisx);

    QAbstractAxis *axisy = m_chart->axes(Qt::Vertical).first();
    QValueAxis *vaxis = 0;
    if (axisy->type() == QAbstractAxis::AxisTypeValue)
        vaxis = qobject_cast<QValueAxis *>(axisy);

    if (haxis && vaxis){
        // 计算两轴单位长度
        double xUnit = m_chart->plotArea().width() / haxis->max();
        double yUnit = m_chart->plotArea().height() / (vaxis->max()-vaxis->min());
        // 将鼠标点击的空间位置转化为chart的轴坐标位置
        int x = (event->pos().x()-m_chart->plotArea().x()) / xUnit;
        int y = (this->height()-m_chart->plotArea().y()-event->pos().y())/yUnit + 60;
        qDebug()<<"x "<<x << " y"<<y;
        if(x>0&&y>0){ // 如果点击在chart坐标内
            // 触发chart的点击事件
            m_chart->clickPoint(QPoint(x,y));
        }

    }
}

void  ChartView::mouseMoveEvent(QMouseEvent *event)
{
    m_chart->handlePointMove(event->pos());
    QChartView::mouseMoveEvent(event);
}

void  ChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->pos() != m_mousePos) {
        m_chart->handlePointMove(event->pos());

    }
    m_chart->setPointClicked(false);
    QChartView::mouseReleaseEvent(event);
}
