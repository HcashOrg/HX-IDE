#ifndef BRIDGE_H
#define BRIDGE_H

#include <QObject>
//javascript与qtwebengine的链接桥梁，主要用于转发javascript的信号
class bridge : public QObject
{
Q_OBJECT
public:
    static bridge* instance();

signals:
    void textChanged();
    void markChanged(int ,bool);
    void gutterRightClickedSignal();
public slots:
    void onTextChanged();

    void onMarkChange(int linenumber,bool isadd);

    void gutterRightClicked();
public:
    bridge(bridge &&)=delete;
    bridge(const bridge &)=delete;
    bridge &operator =(bridge &&)=delete;
    bridge&operator =(const bridge &)=delete;
private:
    explicit bridge(QObject *parent=nullptr);
    ~bridge();
    static bridge *_instance;
    class CGarbo // 它的唯一工作就是在析构函数中删除 的实例
    {
    public:
        ~CGarbo()
        {
            if (_instance)
            {
                delete _instance;
                _instance = nullptr;
            }
        }
    };
    static CGarbo Garbo; // 定义一个静态成员，在程序结束时，系统会调用它的析构函数
};

#endif // BRIDGE_H
