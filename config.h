#ifndef CONFIG_H
#define CONFIG_H


class Config
{
public:
    Config();
    // 基线降采样
    static int BASELINE_DOWNSAMPLE;
    // 基线修正拖拽灵敏度
    static int SENSITIVITY;
};

#endif // CONFIG_H
