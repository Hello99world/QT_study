#ifndef SWITCHBUTTON_H
#define SWITCHBUTTON_H
#include <QWidget>

class SwitchButton : public QWidget
{
    Q_OBJECT

public:
    enum ButtonStyle {
        ButtonStyle_Rect = 0,       //圆角矩形
        ButtonStyle_CircleIn = 1,   //内圆形
        ButtonStyle_CircleOut = 2   //外圆形
    };

    explicit SwitchButton(QWidget *parent = 0);
    ~SwitchButton();

protected:
    void mousePressEvent(QMouseEvent *);
    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *);
    void drawBg(QPainter *painter);
    void drawSlider(QPainter *painter);

private:
    int space;              //滑块离背景间隔
    int rectRadius;         //圆角角度
    bool checked;           //是否选中
    bool showText;          //显示文字
    bool showCircle;        //显示小圆
    bool animation;         //动画过渡
    ButtonStyle buttonStyle;//开关按钮样式

    QColor bgColorOff;      //关闭时背景颜色
    QColor bgColorOn;       //打开时背景颜色
    QColor sliderColorOff;  //关闭时滑块颜色
    QColor sliderColorOn;   //打开时滑块颜色
    QColor textColorOff;    //关闭时文字颜色
    QColor textColorOn;     //打开时文字颜色

    QString textOff;        //关闭时显示的文字
    QString textOn;         //打开时显示的文字

    int step;               //每次移动的步长
    int startX;             //滑块开始X轴坐标
    int endX;               //滑块结束X轴坐标
    QTimer *timer;          //定时器绘制

private slots:
    void change();
    void updateValue();

public:
    //默认尺寸和最小尺寸
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    //获取和设置间隔
    int getSpace() const;
    void setSpace(int space);

    //获取和设置圆角角度
    int getRectRadius() const;
    void setRectRadius(int rectRadius);

    //获取和设置是否选中
    bool getChecked() const;
    void setChecked(bool checked);

    //获取和设置是否显示文字
    bool getShowText() const;
    void setShowText(bool showText);

    //获取和设置是否显示小圆
    bool getShowCircle() const;
    void setShowCircle(bool showCircle);

    //获取和设置是否动画过渡
    bool getAnimation() const;
    void setAnimation(bool animation);

    //获取和设置风格样式
    ButtonStyle getButtonStyle() const;
    void setButtonStyle(const ButtonStyle &buttonStyle);

    //获取和设置关闭时背景颜色
    QColor getBgColorOff() const;
    void setBgColorOff(const QColor &bgColorOff);

    //获取和设置打开时背景颜色
    QColor getBgColorOn() const;
    void setBgColorOn(const QColor &bgColorOn);

    //获取和设置关闭时滑块颜色
    QColor getSliderColorOff() const;
    void setSliderColorOff(const QColor &sliderColorOff);

    //获取和设置打开时滑块颜色
    QColor getSliderColorOn() const;
    void setSliderColorOn(const QColor &sliderColorOn);

    //获取和设置关闭时文字颜色
    QColor getTextColorOff() const;
    void setTextColorOff(const QColor &textColorOff);

    //获取和设置打开时文字颜色
    QColor getTextColorOn() const;
    void setTextColorOn(const QColor &textColorOn);

    //获取和设置关闭时的文字
    QString getTextOff() const;
    void setTextOff(const QString &textOff);

    //获取和设置打开时的文字
    QString getTextOn() const;
    void setTextOn(const QString &textOn);

Q_SIGNALS:
    void checkedChanged(bool checked);
};

#endif // SWITCHBUTTON_H
