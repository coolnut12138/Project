/*工具类*/
#pragma once	    //这种防止头文件被包两次的做法好，因为那种#if__的编译很麻烦，而且头文件也不能有重复的。
#include <cstdlib>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sys/time.h>	//时间的头文件
#include <boost/algorithm/string.hpp>	//boost库


////////////////////////////////////////////////////////////////
//准备一个时间戳获取工具
////////////////////////////////////////////////////////////////

class TimeUtil
{
public:
    //获取当前的时间戳(1970-1-1 00:00:00是基准值，计算当前的时间与基准值的秒数之差就是时间戳)
    //使用有符号数64位，因为时间戳有可能做差，如果是无符号，那么很有可能前者比后者小，导致变成很大的数，64位是为了保险，32位最大只能表示42亿
    static int64_t TimeStamp() {
	struct timeval tv;  //这tv是输出型参数
	::gettimeofday(&tv, NULL);	    //这个函数可以获取到微秒级的时间戳
	return tv.tv_sec;
    }

    static int64_t TimeStampMS() {  //毫秒时间戳
	struct timeval tv; 
	::gettimeofday(&tv, NULL);	    
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
    }
};

////////////////////////////////////////////////////////////////
//打印日志的工具
////////////////////////////////////////////////////////////////

//期望打印出的日志格式如下：
//[I1550892581 util.hpp:31] hello
//[W1550892581 util.hpp:31] hello
//[E1550892581 util.hpp:31] hello
//[F1550892581 util.hpp:31] hello
//日志的使用方式如下：
//LOG(INFO) << "hello" << "\n";
//日志的级别：
//  FATAL 致命
//  ERROR 错误
//  WARNING 警告
//  INFO    提示
enum Level {
    INFO,
    WARING,
    ERROR,
    FATAL,
};

//这里使用inline是因为我们函数的定义是在头文件里，如果直接在头文件中实现函数的定义并且不加inline编译会失败，如果在两个.c文件中都调用了这个函数，那么链接的时候就会报错，认为重复定义。
inline std::ostream& Log(Level level, const std::string& file_name, int line_num) {
   std::string prefix = "[";
   if(level == INFO) {
    prefix += "I";
   } else if(level == WARING) {
    prefix += "W";
   } else if(level == ERROR) {
    prefix += "E";
   } else if(level == FATAL) {
    prefix += "F";
   }
   //to_string 是将数字改为字符串
   prefix += std::to_string(TimeUtil::TimeStamp());
   prefix += " ";
   prefix += file_name;
   prefix += ":";
   prefix += std::to_string(line_num);
   prefix += "]";

   std::cout << prefix;
   return std::cout;
}

//define 函数
#define LOG(level) Log(level, __FILE__, __LINE__)   //__FILE__ 和 __LINE__是c语言的宏，会自动替换成file_name和line_num，那为什么不将这两个宏
						    //直接写在函数体内部呢？因为我们需要的文件名是调用的文件名和行号，如果直接写在函数里面
						    //那么就会直接展开为我们的util.hpp，行号展开也就是我们这块的72行，所以需要宏定义，在调用
						    //宏的地方展开。


///////////////////////////////////////////////////////////////////////////////////////////////
//  准备一下文件相关工具类
///////////////////////////////////////////////////////////////////////////////////////////////
class FileUtil {
public:
    //传入一个文件路径，帮我们把文件的所有内容都读出来放到 content 字符串中
    static bool Read(const std::string& file_path, std::string* content) {
	content->clear();
	std::ifstream file(file_path.c_str());	//ifstream 从文件读，ofstream 往文件写
	if(!file.is_open()) {
	    return false;
	}

	//采取按行读，然后将每一行拼接起来放到content中
	std::string line;
	while(std::getline(file, line)) {   //getline是按行读，读成功一行返回true，读到文件末尾读不到数据了返回false
	    *content += line + "\n";
	}
	file.close();
	return true;
    }

    static bool Write(const std::string& file_path, const std::string& content) {
	std::ofstream file(file_path.c_str());
	if(!file.is_open()) {
	    return false;
	}
	file.write(content.c_str(), content.size());
	file.close();
	return true;
    }
};  

//字符串切分怎么搞？
//1、strtok
//2、stringstream
//3、boost split 函数
class StringUtil {
public:
    static void Split(const std::string& input, const std::string& spilt_char, std::vector<std::string>* output) {
	//is_any_of 的意思就是分隔符允许存在多个，因为我们传的参数是字符串，当然分隔符就有多个,使用is_any_of就可以让字符串正确分割
	//token_compress_off 的意思：关闭分隔符的压缩 ----- 比如一个字符串aaa bbb ccc 这样分出来就是三个，如果字符串是aaa  bbb ccc(a、b之间)
	//有两个空格，那么如果使用token_compress_off,那么分割出来就会分割出四个部分，aaa、bbb、ccc、空串，如果使用token_compress_on那么就会
	//压缩分隔符，就会将两个空格合并成一个，就会分割成三个部分没有空串
	boost::split(*output, input, boost::is_any_of(spilt_char), boost::token_compress_on);
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////
// URL / body 解析模块 
///////////////////////////////////////////////////////////////////////////////////////////////
class UrlUtil {
public:
    static void ParseBody(const std::string& body, std::unordered_map<std::string, std::string>* params) {
	//1、先对这里的 body 字符串进行切分，切分成键值对的形式
	//  a)先按照 & 符号切分
	//  b)再按照 = 切分
	std::vector<std::string> kvs;
	StringUtil::Split(body, "&", &kvs);
	for(size_t i = 0; i < kvs.size(); ++i) {
	    std::vector<std::string> kv;
	    //kvs[i] 里面存的是一个键值对
	    StringUtil::Split(kvs[i], "=", &kv);
	    if(kv.size() != 2) {
		continue;
	    }
	    //对于unordered_map []操作的行为：如果key不存在，就新增
	    //如果key存在，就获取到对应的value
	    //2、对这里的键值对进行 urldecode 
	    (*params)[kv[0]] = UrlDecode(kv[1]);
	}
    }

    //static std::string UrlDecode(const std::string& str) {
    //    //解码使用现成的，网上查
    //    }
    static unsigned char ToHex(unsigned char x) 
    { 
	return  x > 9 ? x + 55 : x + 48; 
    }

    static unsigned char FromHex(unsigned char x) 
    { 
	unsigned char y;
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if (x >= '0' && x <= '9') y = x - '0';
	else assert(0);
	return y;
    }

    static std::string UrlEncode(const std::string& str)
    {
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
	    if (isalnum((unsigned char)str[i]) || 
		    (str[i] == '-') ||
		    (str[i] == '_') || 
		    (str[i] == '.') || 
		    (str[i] == '~'))
		strTemp += str[i];
	    else if (str[i] == ' ')
		strTemp += "+";
	    else
	    {
		strTemp += '%';
		strTemp += ToHex((unsigned char)str[i] >> 4);
		strTemp += ToHex((unsigned char)str[i] % 16);
	    }
	}
	return strTemp;
    }

    static std::string UrlDecode(const std::string& str)
    {
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
	    if (str[i] == '+') strTemp += ' ';
	    else if (str[i] == '%')
	    {
		assert(i + 2 < length);
		unsigned char high = FromHex((unsigned char)str[++i]);
		unsigned char low = FromHex((unsigned char)str[++i]);
		strTemp += high*16 + low;
	    }
	    else strTemp += str[i];
	}
	return strTemp;
    }
};



