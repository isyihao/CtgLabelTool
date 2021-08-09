#ifndef ANALYSISRESULT_H
#define ANALYSISRESULT_H

#include <QJsonObject>
#include <QTime>
#include <QVector>
#include "config.h"


/**
 * @brief CTG分析结果实体类
 */
class AnalysisResult
{
public:
    AnalysisResult();

    AnalysisResult(QJsonObject analysisResult);

    QString toJsonString();

    void save();

    // 档案名称
    QString recordName;
    // 档案文件路径（全绝对路径如: D:/xx/xxx/xxx.json)
    QString filePath;
    // 档案长度
    int length;
    // 监护开始时间
    QDateTime startTime;
    // 第一胎心数据
    QVector<int> fhr1;
    // 第二胎心数据
    QVector<int> fhr2;
    // 第三胎心数据
    QVector<int> fhr3;
    // 宫缩数据
    QVector<int> toco;
    // 基线数据
    QVector<int> baseline;
    // 基线降采样
    int baselineDownsample = 40;
    // 修订后的基线数据
    QVector<int> correctBaseline;
    // 修订人姓名
    QString correctedBy;
    // 修订人身份、联系方式、所属机构等其他信息
    QString correctionInfo;
    // 修订时间
    QString correctedTime;
};

#endif // ANALYSISRESULT_H
