#include "analysisresult.h"

#include <QFile>
#include <QJsonArray>
#include <qjsondocument.h>
#include <QDebug>

AnalysisResult::AnalysisResult()
{

}

AnalysisResult::AnalysisResult(QJsonObject analysisResult)
{
    if(!analysisResult.isEmpty()){
        // 解析档案名
        this->recordName = QString("unknown");
        if(analysisResult.contains("fetalName")){
            this->recordName = analysisResult.value("fetalName").toString();
        }

        // 解析档案长度
        this->length = 0;
        if(analysisResult.contains("length")){
            this->length =  analysisResult.value("length").toInt();
        }
        // 解析fhr1
        this->fhr1.clear();
        if(analysisResult.contains("fhr")){
            QJsonValue value = analysisResult.value("fhr");
            if (value.isArray()) {
                QJsonArray array = value.toArray();
                int nSize = array.size();
                for (int i = 0; i < nSize; ++i) {
                    QJsonValue v = array.at(i);
                    if (!v.isNull()) {
                        this->fhr1.append(v.toInt());
                    }
                }
            }
        }
        // 解析toco
        this->toco.clear();
        if(analysisResult.contains("toco")){
            QJsonValue value = analysisResult.value("toco");
            if (value.isArray()) {
                QJsonArray array = value.toArray();
                int nSize = array.size();
                for (int i = 0; i < nSize; ++i) {
                    QJsonValue v = array.at(i);
                    if (!v.isNull()) {
                        this->toco.append(v.toInt());
                    }
                }
            }
        }
        // 解析基线
        this->baseline.clear();
        if(analysisResult.contains("baseline")){
            QJsonValue value = analysisResult.value("baseline");
            if (value.isArray()) {
                QJsonArray array = value.toArray();
                int nSize = array.size();
                for (int i = 0; i < nSize; ++i) {
                    QJsonValue v = array.at(i);
                    if (!v.isNull()) {
                        this->baseline.append(v.toInt());
                    }
                }
            }
        }

        // 解析修正后的基线
        this->correctBaseline.clear();
        if(analysisResult.contains("correctBaseline")){
            QJsonValue value = analysisResult.value("correctBaseline");
            if (value.isArray()) {
                QJsonArray array = value.toArray();
                int nSize = array.size();
                for (int i = 0; i < nSize; ++i) {
                    QJsonValue v = array.at(i);
                    if (!v.isNull()) {
                        this->correctBaseline.append(v.toInt());
                    }
                }
            }
        }else{ // 如果没有，则从原始基线降采样
            qDebug()<<"没有correctBaseline，则从原始基线降采样";
            for(int i=0;i<length;i+= Config::BASELINE_DOWNSAMPLE){
                this->correctBaseline.append(baseline.at(i));
            }
            // 补齐最后一个点
            if(length % Config::BASELINE_DOWNSAMPLE != 0){
                this->correctBaseline.append(baseline.at(length-1));
            }
        }

        if(analysisResult.contains("startTime")){

        }else{
            this->startTime = QDateTime::currentDateTime();
        }
    }
}

QString AnalysisResult::toJsonString(){
    QJsonObject data;
    data.insert("fetalName", this->recordName);
    data.insert("length", this->length);
    data.insert("correctedBy",this->correctedBy);
    data.insert("baselineDownsample",this->baselineDownsample);

    QJsonArray fhr1;
    QJsonArray toco;
    QJsonArray baseline;
    for(int i=0;i<this->length;i++){
        fhr1.append(this->fhr1.at(i));
        toco.append(this->toco.at(i));
        baseline.append(this->baseline.at(i));
    }

    QJsonArray correctBaseline;
    for(int i=0;i<this->correctBaseline.size();i++){
        correctBaseline.append(this->correctBaseline.at(i));
    }

    data.insert("fhr",fhr1);
    data.insert("toco",toco);
    data.insert("baseline",baseline);
    data.insert("correctBaseline",correctBaseline);
    QJsonDocument document;
    document.setObject(data);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    return  strJson;
}

void AnalysisResult::save(){
    qDebug()<<"json save";
//    qDebug()<<"json str"<<this->toJsonString();
    qDebug()<<"filePath "<<this->filePath;
    QFile file(this->filePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() <<"文件打开失败："<<this->filePath;
        return ;
    }
    qDebug()<<"json str";
    QTextStream txtOutput(&file);
    txtOutput << this->toJsonString() << endl;
    file.close();
    qDebug()<<"save str finished";
}
