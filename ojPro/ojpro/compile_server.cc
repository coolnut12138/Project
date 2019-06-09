#include "httplib.h"
#include "compile.hpp"

int main()
{
    //命名空间写在函数里面好，防止命名冲突
    using namespace httplib;
    Server server;
    //Get注册了一个回调函数，这个函数的调用时机是处理Get方法的时候
    // lambda 表达式？是匿名函数，用一次后再不用了 --- [](){} --- [] 告诉我们这是lambda表达式，()放参数

    // 对应关系：路由,什么样的path对应什么函数，下面的/compile就是path，对应后面的回调函数
    server.Get("/compile", [](const Request &req, Response &resp){
	//根据具体的问题场景，根据请求计算出响应结果，也就是传的回调函数做的事情
	(void)req;
	//如何从req中获取到JSON请求，JSON如何和HTTP协议结合，JSON如何进行解析和构造？
	//  在这里调用CompileAndRun函数
	std::string req_json;	//从req对象中获取
	std::string resp_json;	//从resp_json 放到响应中
	Compiler::CompileAndRun(req_json, &resp_json);
	resp.set_content(resp_json, "text/plain");
    }); 
    server.listen("0.0.0.0", 9092);	//ip和端口号
    return 0;
}
