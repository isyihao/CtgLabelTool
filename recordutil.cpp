#include "recordutil.h"

#include <QFile>


RecordUtil::RecordUtil()
{

}

QJsonObject RecordUtil::readJsonFromFile(QString filePath)
{
    QFile loadFile(filePath);

    // 读取Json文件
    if(!loadFile.open(QFile::ReadOnly | QFile::Text))
    {
       qDebug() << "could't open projects json";
    }

    QByteArray allData = loadFile.readAll();
    loadFile.close();

    QJsonParseError jsonError;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &jsonError));

    if(jsonError.error != QJsonParseError::NoError)
    {
       qDebug() << "json error!" << jsonError.errorString();
    }

    QJsonObject rootObj = jsonDoc.object();
    return rootObj;
}
