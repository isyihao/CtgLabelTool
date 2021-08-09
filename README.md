## CTG基线修正工具

#### 简介

本工具的主要作用是解析、展示json格式的ctg分析结果，使得用户能够预览、修正fhr基线，并保存修正结果，用于二次研究

#### 说明

1. 源数据要求必须为json文件，文件名为xxx.json格式

2. 源数据样例如下：

   ```json
   {
       "recordName":"record01_210808175936",
       "length":"11260",
       "startTime":"2020-08-01 17:59:01",
   	"fhr":[140,141,141],
   	"toco":[20,20,20],
       "baseline":[140,140,140],
       "correctBaseline":[140,140,140],
       "baselineDownsample":"40",
       "correctedBy":"张三",
       "correctedTime":"2021-08-01 17:59:01",
   }
   ```
   
3. 数据字段说明

   | 字段               | 类型       | 说明             | 字段是否必须 | 缺省值                     |
   | :----------------- | ---------- | ---------------- | ------------ | -------------------------- |
   | recordName         | string     | 档案名称         | 是           | 16位随机字符串             |
   | length             | int        | 档案长度         | 是           | /                          |
   | startTime          | string     | 胎监开始时间     |              | 当前时间                   |
   | fhr                | array<int> | 胎心率数据       | 是           | /                          |
   | toco               | array<int> | 宫缩数据         | 是           | /                          |
   | baseline           | array<int> | 基线数据         | 是           | /                          |
   | correctBaseline    | array<int> | 修订后的基线数据 |              | baseline数据降采样后的数据 |
   | baselineDownsample | int        | 基线降采样       |              | 40                         |
   | correctedBy        | string     | 修订人姓名       |              | ""                         |
   | correctedTime      | string     | 修订时间         |              | 当前时间                   |

4. 源数据文件位置为 ${WORKDIR}/records，其中 ${WORKDIR}为可执行程序exe所在目录

5. 保存数据之后会覆盖${WORKDIR}/records目录下的源数据文件，请做好备份

