#ifndef DATADEFINE_H
#define DATADEFINE_H

#include <QString>
#include <QStringList>
#include <QColor>
#include <mutex>
namespace DataDefine
{
    //主题类型
    enum ThemeStyle{Black_Theme,White_Theme};
    static const QColor BLACK_BACKGROUND = QColor(41,41,41);
    static const QColor DARK_CLACK_BACKGROUND = QColor(30,30,30);
    static const QColor WHITE_BACKGROUND = QColor(255,255,255);

    //合约文件后缀
    static const QString GLUA_SUFFIX = "glua";
    static const QString JAVA_SUFFIX = "java";
    static const QString CSHARP_SUFFIX = "csharp";
    static const QString KOTLIN_SUFFIX = "kotlin";

    //合约存放路径
    static const QString GLUA_DIR = "contracts/glua";
    static const QString JAVA_DIR = "contracts/java";
    static const QString CSHARP_DIR = "contracts/csharp";
    static const QString KOTLIN_DIR = "contracts/kotlin";

    //编译工具路径
    static const QString GLUA_COMPILE_PATH = "compile/glua_compiler.exe";
    static const QString JAVA_CORE_PATH = "compile/gjavac-core-1.0.1-dev.jar";
    static const QString JAVA_COMPILE_PATH = "compile/gjavac-compiler-1.0.1-dev-jar-with-dependencies.jar";
    static const QString UVM_ASS_PATH = "compile/uvm_ass.exe";
    static const QString PACKAGE_GPC_PATH = "compile/package_gpc.exe";

    //合约api类
    class ApiEvent
    {
    public:
        ApiEvent(){

        }
        void addApi(const QString &name){
            std::lock_guard<std::mutex> lock(lockMutex);
            apis<<name;
        }
        void addEvent(const QString &name){
            std::lock_guard<std::mutex> lock(lockMutex);
            events<<name;
        }

        const QStringList &getAllApiName()const{
            return apis;
        }
        const QStringList &getAllEventName()const{
            return events;
        }
        void clear(){
            std::lock_guard<std::mutex> lock(lockMutex);
            apis.clear();
            events.clear();
        }

    private:
        QStringList apis;
        QStringList events;
        std::mutex lockMutex;

    };
}

#endif // DATADEFINE_H
