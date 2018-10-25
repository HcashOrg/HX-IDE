#ifndef DATADEFINE_H
#define DATADEFINE_H

#include <memory>
#include <mutex>
#include <vector>

#include <QString>
#include <QStringList>
namespace DataDefine
{
    //主题类型
    enum ThemeStyle{Black_Theme,White_Theme};
    //语言类型
    enum Language{SimChinese,English};
    //链类型--种类
    enum BlockChainClass{UB,HX,CTC};
    //链类型--测试、正式
    enum ChainType{NONE = 0,TEST = 1 << 0,FORMAL = 1 << 1};
    typedef int ChainTypes;

    //codemirror路径
    static const QString CODEMIRROR_BLACK = "codemirror/editor_black.html";
    static const QString CODEMIRROR_WHITE = "codemirror/editor_white.html";

    //帮助网页路径
    static const QString HELP_PATH = "help/html/index.html";

    //合约文件后缀
    static const QString META_SUFFIX = "meta.json";
    static const QString BYTE_OUT_SUFFIX = "out";
    static const QString CONTRACT_SUFFIX = "gpc";
    static const QString GLUA_SUFFIX = "uvlua";
    static const QString JAVA_SUFFIX = "java";
    static const QString CSHARP_SUFFIX = "cs";
    static const QString KOTLIN_SUFFIX = "kt";

    //后台程序路径
#ifdef  WIN32
    static const QString UBCD_NODE_EXE = "backstage/ubcd.exe";
    static const QString UBCD_CLIENT_EXE = "backstage/ubc-cli.exe";

    static const QString LINK_NODE_EXE = "backstage/hx_node.exe";
    static const QString LINK_CLIENT_EXE = "backstage/hx_client.exe";

    static const QString CTC_NODE_TEST_EXE = "backstage/ctc_test.exe";
    static const QString CTC_TEST_SCRIPT = "backstage/import_wif_keys.json";
    static const QString CTC_NODE_FORMAL_EXE = "backstage/ctc_formal.exe";
#else
    static const QString UBCD_NODE_EXE = "backstage/ubcd";
    static const QString UBCD_CLIENT_EXE = "backstage/ubc-cli";

    static const QString LINK_NODE_EXE = "backstage/hx_node";
    static const QString LINK_CLIENT_EXE = "backstage/hx_client";

    static const QString CTC_NODE_TEST_EXE = "backstage/ctc_test";
    static const QString CTC_TEST_SCRIPT = "backstage/import_wif_keys.json";
    static const QString CTC_NODE_FORMAL_EXE = "backstage/ctc_formal";
#endif

    //合约存放路径
    static const QString CONTRACT_DIR = "contracts";
    static const QString GLUA_DIR = "contracts/uvlua";
    static const QString JAVA_DIR = "contracts/java";
    static const QString CSHARP_DIR = "contracts/csharp";
    static const QString KOTLIN_DIR = "contracts/kotlin";

    static const QString LOCAL_CONTRACT_TEST_PATH = "contracts/contracts_test.contract";
    static const QString LOCAL_CONTRACT_FORMAL_PATH = "contracts/contracts_formal.contract";


    //编译工具路径
#ifdef WIN32
    static const QString GLUA_COMPILE_PATH = "compile/glua/glua_compiler.exe";

    static const QString JAVA_CORE_PATH = "compile/java/gjavac-core-1.0.1-dev.jar";
    static const QString JAVA_COMPILE_PATH = "compile/java/gjavac-compiler-1.0.1-dev-jar-with-dependencies.jar";
    static const QString JAVA_UVM_ASS_PATH = "compile/java/uvm_ass.exe";
    static const QString JAVA_PACKAGE_GPC_PATH = "compile/java/package_gpc.exe";

    static const QString CSHARP_COMPILER_EXE_PATH = "compile/csharp/win/Roslyn/csc.exe";//本地csc.exe路径
    static const QString CSHARP_COMPILE_DIR = "compile/csharp/win/";
    static const QString CSHARP_JSON_DLL_PATH = "compile/csharp/win/Newtonsoft.Json.dll";
    static const QString CSHARP_CORE_DLL_PATH = "compile/csharp/win/GluaCoreLib.dll";
    static const QString CSHARP_COMPILE_PATH = "compile/csharp/win/gsharpc.exe";

    static const QString KOTLIN_COMPILE_PATH = "compile/kotlin/kotlinc/bin/kotlinc-jvm.bat";
#else
    static const QString GLUA_COMPILE_PATH = "compile/glua/glua_compiler";

    static const QString JAVA_CORE_PATH = "compile/java/gjavac-core-1.0.1-dev.jar";
    static const QString JAVA_COMPILE_PATH = "compile/java/gjavac-compiler-1.0.1-dev-jar-with-dependencies.jar";
    static const QString JAVA_UVM_ASS_PATH = "compile/java/uvm_ass";
    static const QString JAVA_PACKAGE_GPC_PATH = "compile/java/package_gpc";

    static const QString CSHARP_COMPILER_EXE_PATH = "/usr/local/share/dotnet/dotnet";//本地csc.exe路径
    static const QString CSHARP_UVMCORE_LIB_PATH = "compile/csharp/unix/uvmcorelib.dll";
    static const QString CSHARP_PROJECT_TEMPLATE_PATH = "compile/csharp/unix/build.csproj";
    static const QString CSHARP_GSHARPCORE_DLL_PATH = "compile/csharp/unix/gsharpccore.dll";

    static const QString CSHARP_COMPILE_DIR = "compile/csharp/unix";
    static const QString CSHARP_JSON_DLL_PATH = "compile/csharp/Newtonsoft.Json.dll";
    static const QString CSHARP_CORE_DLL_PATH = "compile/csharp/GluaCoreLib.dll";
    static const QString CSHARP_COMPILE_PATH = "compile/csharp/gsharpc.exe";

    static const QString KOTLIN_COMPILE_PATH = "compile/kotlin/kotlinc/bin/kotlinc-jvm.bat";

#endif
    //编译临时路径(文件夹)
    static const QString GLUA_COMPILE_TEMP_DIR = "gluaTemp";
    static const QString JAVA_COMPILE_TEMP_DIR = "javaTemp";
    static const QString CSHARP_COMPILE_TEMP_DIR = "csharpTemp";
    static const QString KOTLIN_COMPILE_TEMP_DIR = "kotlinTemp";

    //调试器路径
#ifdef WIN32
    static const QString DEBUGGER_UVM_PATH = "debug/uvm_single.exe";
#else
    static const QString DEBUGGER_UVM_PATH = "debug/uvm_single";
#endif
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

        void addOfflineApi(const QString &name){
            std::lock_guard<std::mutex> lock(lockMutex);
            offlineApis<<name;
        }

        void addEvent(const QString &name){
            std::lock_guard<std::mutex> lock(lockMutex);
            events<<name;
        }

        const QStringList &getAllApiName()const{
            return apis;
        }

        const QStringList &getAllOfflineApiName()const{
            return offlineApis;
        }

        const QStringList &getAllEventName()const{
            return events;
        }
        void clear(){
            std::lock_guard<std::mutex> lock(lockMutex);
            apis.clear();
            offlineApis.clear();
            events.clear();
        }

    private:
        QStringList apis;
        QStringList offlineApis;
        QStringList events;
        std::mutex lockMutex;
    };
    typedef std::shared_ptr<DataDefine::ApiEvent> ApiEventPtr;

}

#endif // DATADEFINE_H
