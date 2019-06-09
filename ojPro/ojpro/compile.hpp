#pragma once
#include <string>
//为什么写成hpp文件，因为只要头文件即可
///////////////////////////////////////////////////////////////////////////
//此代码完成在线编译模块的功能
//提供一个 Compiler 类，由这个类提供一个核心的 CompileAndRun函数，由这个函数来完成编译+运行的功能
///////////////////////////////////////////////////////////////////////////

class Compiler {
public:
    //此处看到的参数和返回结果就是笔记中记录的json格式,使用json格式是为了更方便的和http协议结合起来
    static bool CompileAndRun(const std::string& req_json, std::string* resp_json) {
	
    }
};
