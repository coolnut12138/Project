#include "httplib.h"

int main()
{
    //命名空间写在函数里面好，防止命名冲突
    using namespace httplib;
    Server server;
    //Get注册了一个回调函数，这个函数的调用时机是处理Get方法的时候
    // lambda 表达式？是匿名函数，用一次后再不用了 --- [](){} --- [] 告诉我们这是lambda表达式，()放参数
    server.Get("/", [](const Request &req, Response &resp){
	//根据具体的问题场景，根据请求计算出响应结果，也就是传的回调函数做的事情
	resp.set_content("<htm>Hello World!</html>", "text/plain");
    }); 
    server.listen("0.0.0.0", 9092);	//ip和端口号
    return 0;
}
