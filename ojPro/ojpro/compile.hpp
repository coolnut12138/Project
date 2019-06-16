#pragma once
//头文件的顺序有讲究
//1、C/C++ 标准库文件
#include <string>
#include <atomic>	//提供原子操作
//2、操作系统头文件
#include <unistd.h>	//提供fork
#include <sys/wait.h>	//提供wait
#include <sys/stat.h>	//stat函数
#include <fcntl.h>
//3、第三方库头文件
#include <jsoncpp/json/json.h>
//4、当前项目的其他头文件
#include "util.hpp"

//为什么写成hpp文件，因为只要头文件即可
///////////////////////////////////////////////////////////////////////////
//此代码完成在线编译模块的功能
//提供一个 Compiler 类，由这个类提供一个核心的 CompileAndRun函数，由这个函数来完成编译+运行的功能
///////////////////////////////////////////////////////////////////////////

class Compiler {
    public:
	//此处看到的参数和返回结果就是笔记中记录的json格式,使用json格式是为了更方便的和http协议结合起来
	//Json::Value 类 jsoncpp中的核心类，借助这个类就可以完成序列化和反序列化的动作
	//这个类的使用方法，和map非常相似,可以使用[]完成属性的操作 Json::Value&  

	//这几步会用到的文件：
	//使用将456文件其实最后是要返回给3的，这其实就是进程间通信，
	//进程间通信管道也可以，但是使用文件有一个好处就是方便调试,可以直接打开文件看看程序是否正确加快调试的速度，
	//  而管道的数据进程一退出数据就没了，不知道哪块出错,方便调试
	//
	//1、源代码文件,此处的 name 表示当前请求的名字 
	//	请求和请求之间，name 必须是不同的
	static std::string SrcPath(const std::string& name) {
	    return "./temp_files/" + name + ".cpp";
	}
	//2、编译错误文件 
	static std::string CompileErrorPath(const std::string& name){
	    return "./temp_files/" + name + ".compile_error";
	}
	//3、可执行程序文件 
	static std::string ExePath(const std::string& name) {
	    return "./temp_files/" + name + ".exe";	//用.exe后缀表示是可执行程序，但是是linux下的可执行程序，要知道linux下的exe文件在windows上是不能运行的，因为这是两个操作系统，exe文件的格式是完全不一样的。
	}
	//4、标准输入文件 
	static std::string StdinPath(const std::string& name) {
	    return "./temp_files/" + name + ".stdin";
	}
	//5、标准输出文件 
	static std::string StdoutPath(const std::string& name) {
	    return "./temp_files/" + name + ".stdout";
	}
	//6、标准错误文件
	static std::string StderrPath(const std::string& name) {
	    return "./temp_files/" + name + ".stderr";
	}

	static bool CompileAndRun(const Json::Value& req, Json::Value* resp) {
	    //这个函数的步骤：
	    //1、根据请求对象生成源代码文件。(g++ test.cpp -o test，这个操作是在对test.cpp这个文件进行操作，所以要先生成文件)
	    if (req["code"].empty()) {
		(*resp)["error"] = 3;
		(*resp)["reason"] = "code empty";
		LOG(ERROR) << "code empty" << std::endl;
		return false;
	    }
	    //req["code"]根据 key 取出 value。value类型是Json::Value.这个类型通过asString()转成字符串
	    const std::string& code = req["code"].asString();		    
	    //通过这个函数完成把代码写到代码文件中的过程。
	    std::string file_name = WriteTmpFile(code, req["stdin"].asString());
	    
	    //2、调用 g++ 进行编译（fork + exec 或者 system）。这个步骤会生成可执行程序。但是编译失败不会生成
	    //	第二步会有两种结果：
	    //	第一种编译成功，生成可执行程序，
	    //	第二种编译失败，生成不出可执行程序编译失败并且把编译错误记录下来(重定向到文件中)。
	    bool ret = Compile(file_name);
	    if(!ret){
		//错误处理
		(*resp)["error"] = 1;
		std::string reason;
		FileUtil::Read(CompileErrorPath(file_name), &reason);
		(*resp)["reason"] = reason;
		//虽然是编译出错，但是这样的错误是用户自己的错误，不是服务器的错误，对于服务器来说，这样的编译出错不是错误，所以等级是INFO
		LOG(INFO) << "Compile failed!" << std::endl;
		return false;
	    }
	    //3、调用可执行程序,
	    //  把标准输入记录到文件中，
	    //  把文件中的内容重定向给可执行程序，
	    //  可执行程序的标准输出和标准错误内容也要重定向输出记录到文件中
	    int sig = Run(file_name);
	    if(sig != 0){   //sig != 0 表示遇到了信号
		//错误处理
		(*resp)["error"] = 2;
		//运行出错都是收到信号后出错
		(*resp)["reason"] = "Program exit by signo: " + std::to_string(sig);
		LOG(INFO) << "Program exit by signo: " << std::to_string(sig) << std::endl;
		return false;
	    }
	    //4、把程序的最终结果进行返回。即构造 resp 对象。
	    (*resp)["error"] = 0;
	    (*resp)["reason"] = "";
	    std::string str_stdout;
	    FileUtil::Read(StdoutPath(file_name), &str_stdout);
	    (*resp)["stdout"] = str_stdout;

	    std::string str_stderr;
	    FileUtil::Read(StderrPath(file_name), &str_stderr);
	    (*resp)["stderr"] = str_stderr;
	    LOG(INFO) << "Program " << file_name << " Done" << std::endl;
	    return true;
	}

    private:
	//1.把代码写到文件里。
	//2、给这次请求分配一个唯一的名字，通过返回值返回
	//  分配的名字形如 tmp_1560579981.2 的前缀
	static std::string WriteTmpFile(const std::string& code, const std::string& str_stdin) {
	    //此处有线程安全问题，不要用锁解决，锁能解决问题，但是开销代价太大，锁又叫互斥锁/挂起等待锁，就是线程1拿到锁了，线程2就要被挂起
	    //线程1释放锁了，线程2也不一定立刻拿到锁，而是cpu调度到它才会给它分配锁。这样的cpu调度成本太大了。
	    static std::atomic_int id(0);   //原子操作是依赖 CPU 的支持
	    ++id;
	    std::string file_name = "tmp_" + std::to_string(TimeUtil::TimeStamp()) + "." + std::to_string(id);	//名字这样搞是为了保证每个请求的名字不同
	    FileUtil::Write(SrcPath(file_name), code);
	    FileUtil::Write(StdinPath(file_name), str_stdin);

	    return file_name;
	} 	

	static bool Compile(const std::string& file_name) {
	    //1、先构造出编译指令
	    //	g++ file_name.cpp -o file_name.exe -std=c++11
	    //	使用execv族程序替换
	    char* command[20] = {0};
	    char buf[20][50] = {{0}};
	    for(int i = 0; i < 20; ++i){
		command[i] = buf[i];	//让command中的元素都指向栈上的空间
	    }
	    // 必须要保证 command 的指针都指向有效内存,sprintf才对
	    sprintf(command[0], "%s", "g++"); 
	    sprintf(command[1], "%s", SrcPath(file_name).c_str());
	    sprintf(command[2], "%s", "-o"); 
	    sprintf(command[3], "%s", ExePath(file_name).c_str());
	    sprintf(command[4], "%s", "-std=c++11");
	    command[5] = NULL;	    //exec函数族都是以NULL结尾的，不然程序会崩溃

	    //2、创建子进程
	    int ret = fork();
	    if(ret > 0) {
		//3、父进程进行进程等待
		//用waitpid，因为同一时刻有很多子进程
		waitpid(ret, NULL, 0);	//阻塞式等待，因为只有编译完了才能进行下一步
	    } else {
		//4、子进程进行程序替换
		////0666是8进制，表示可读可写可执行，这是错误文件所以不能执行。
		int fd = open(CompileErrorPath(file_name).c_str(), O_WRONLY | O_CREAT, 0666);			
		if (fd < 0) {
		    LOG(ERROR) << "open Compile file error" << std::endl;
		    exit(1);
		}
		dup2(fd, 2);	//期望得到的效果是写 2 能够把数据放到文件中
		execvp(command[0], command);//程序替换只是替换代码和数据，重定向是文件描述符的操作，文件描述符是在pcb中，所以不会影响dup2
		exit(0); //进程替换失败会执行到这，比如指令不对，command拼接不对,失败就让子进程退出
	    }
	    //5、代码执行到这里，是否知道编译成功与否？
	    //判定可执行文件是否存在,使用stat函数，它和ls的功能比较相似，ls就是基于stat实现的
	    struct stat st;
	    ret = stat(ExePath(file_name).c_str(), &st);
	    if (ret < 0) {
		//stat 执行失败，说明该文件不存在
		LOG(INFO) << "Compile failed" << file_name << std::endl;
		return false;
	    }
	    LOG(INFO) << "Compile " << file_name << "OK!" << std::endl;
	    return true;
	}

	static int Run(const std::string& file_name) {
	    //1、创建子进程
	    int ret = fork();
	    if(ret > 0) {
		//2、父进程进行等待
		int status = 0;
		waitpid(ret, &status, 0);
		return status & 0x7f;	//status最后 7 位存放退出信号值
	    }else {
		//3、进行重定向(标准输入，标准输出，标准错误)
		int fd_stdin = open(StdinPath(file_name).c_str(), O_RDONLY);
		dup2(fd_stdin, 0);
		int fd_stdout = open(StdoutPath(file_name).c_str(), O_WRONLY | O_CREAT, 0666);
		dup2(fd_stdout, 1);
		int fd_stderr = open(StderrPath(file_name).c_str(), O_WRONLY | O_CREAT, 0666);
		dup2(fd_stderr, 2);
		//4、子进程进行程序替换
		execl(ExePath(file_name).c_str(), ExePath(file_name).c_str(), NULL);
		exit(0);
	    }
	}
};
