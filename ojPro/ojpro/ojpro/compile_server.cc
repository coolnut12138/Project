#include <unordered_map>
#include "httplib.h"
#include "compile.hpp"
#include <jsoncpp/json/json.h>

int main()
{
    //命名空间写在函数里面好，防止命名冲突
    using namespace httplib;
    Server server;
    //Get注册了一个回调函数，这个函数的调用时机是处理Get方法的时候
    // lambda 表达式？是匿名函数，因为没有函数名，所以用一次后再不用了 --- [](){} --- [] 告诉我们这是lambda表达式，()放参数

    // 对应关系：路由,什么样的path对应什么函数，下面的/compile就是path，对应后面的回调函数
    server.Post("/compile", [](const Request &req, Response &resp){  //path是compile那么就会触发回调函数
	//根据具体的问题场景，根据请求计算出响应结果，也就是传的回调函数做的事情
	(void)req;
	//问题1：如何从req中获取到JSON请求?
	//问题2：JSON如何和HTTP协议结合?
	//需要的请求格式是 JSON 格式，而HTTP能够提供的格式，是另外一种键值对的格式，所以此处要进行格式的转换
	//此处由于提交的代码可能会包含一些特殊符号，这些特殊符号想要正确传输，就需要进行转义，但是转义的过程浏览器帮我们完成了
	//但是浏览器帮我们转义之后，我们的服务器收到之后如何编译呢？所以我们还要先转义回来。
	//所以服务器收到请求之后要做的第一件事就是先切分，再 urldecode; 然后解析这个数据整理成需要的 JSON 格式
	//我们将数据放在body中，其实放url也可以。放在body中，所以我们要使用 Post 方法
	//键值对，用哪个数据结构表示？用 unordered_map 表示
	std::unordered_map<std::string, std::string> body_kv;
	UrlUtil::ParseBody(req.body, &body_kv);	//解析函数，将HTTP中的 body 数据解析成键值对
	
	//问题3：JSON如何进行解析和构造?	答：使用jsoncpp 第三方库,使用yum install jsoncpp-devel.x86_64 安装第三方库
	//  在这里调用CompileAndRun函数
	Json::Value req_json;	//从req对象中获取
	for(auto p : body_kv) {
	    //p的类型和 *it 得到的类型是一致的
	   req_json[p.first] = p.second;
	}

	Json::Value resp_json;	//从resp_json 放到响应中
	Compiler::CompileAndRun(req_json, &resp_json);
	//需要把JSON::Value对象序列化成一个字符串，才能返回
	Json::FastWriter writer;
	resp.set_content(writer.write(resp_json), "text/plain");
    }); 
    server.listen("0.0.0.0", 9092);	//ip和端口号
    return 0;
}
