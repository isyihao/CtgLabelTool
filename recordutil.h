#ifndef RECORDUTIL_H
#define RECORDUTIL_H

#include <QString>
#include <QJsonObject>
#include <QFile>
#include <QJsonParseError>
#include <QDebug>



class RecordUtil
{
public:
    RecordUtil();

    /**
     * @brief readJsonFromFile 从文件中读取json
     * @param filePath 文件路径
     * @return
     */
    static QJsonObject readJsonFromFile(QString filePath);
};

#endif // RECORDUTIL_H
