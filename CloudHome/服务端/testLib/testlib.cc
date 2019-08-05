#include "httplib.h"
#include "testview.hpp"

#define BaseIp "0.0.0.0"
#define BasePort 9000

void hello(const httplib::Request& req, httplib::Response &resp){
    std::string html;
    View::Render
    resp.set_content("<html>hello</html>", "text/html");
}

int main()
{
    using namespace httplib;
    Server server;
    server.set_base_dir("./www");   //设置相对根目录
   //server.Get("/", [](const Request& req, Response& resp){	//服务端发送一个Get请求时我们如何处理
   //    resp.set_content("<html>hello</html>", "text/html");
   //});
    server.Get("/", hello);	//当别人请求我们的/目录时应该怎么做，做法就是后面的函数做的事情
    server.listen(BaseIp, BasePort);
    return 0;
}
