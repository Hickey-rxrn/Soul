#pragma once
#include "Util.h"
#include <thread>
#include "httplib.h"
#include <boost/filesystem.hpp>

#define P2P_PORT 9000
#define MAX_IPBUFFER 16
#define MAX_RANGE (100 * 1024 * 1024)  // 1左移10位是1k，左移20位是1M   100左移20位就是100M
#define SHARD_PATH "./Shared/"
#define DOWNLOAD_PATH "./Download/"

class Host
{
public:
	uint32_t _ip_addr; //要配对主机的IP地址
	bool _pair_ret; //用于存放配对结果，配对成功true，失败false
};

class Client
{
public:
	bool Start()
	{
		//客户端程序需要循环运行，因为下载文件不是只下载一次
		while (1)
		{
			GetOnlineHost();
		}
		return true;
	}
	//主机配对的线程入口函数
	void HostPair(Host* host)
	{
		//1.组织http协议格式的请求数据
		//2.搭建一个tcp的客户端，将数据发送
		//3.等待服务端的回复，并进行解析
		//整过过程使用第三方库httplib实现
		host->_pair_ret = false;
		char buf[MAX_IPBUFFER] = { 0 };
		inet_ntop(AF_INET, &host->_ip_addr, buf, MAX_IPBUFFER);
		httplib::Client cli(buf, P2P_PORT);  //实例化httplib客户端对象
		auto rsp = cli.Get("/hostpair");     //向服务端发送资源为/hostpair的GET请求，若连接失败，GET会返回NULL
		if (rsp && rsp->status == 200)              //判断响应结果是否正确
		{
			host->_pair_ret = true;          //重置主机配对结果
		}
		return;
	}

	//获取在线主机
	bool GetOnlineHost()
	{
		char ch = 'Y'; //是否重新匹配，默认是进行匹配的，若已经匹配过，_online_host不为空，供用户选择
		if (!_online_host.empty())
		{
			std::cout << "是否重新查看在线主机(Y/N):";
			fflush(stdout);
			std::cin >> ch;
		}
		if (ch == 'Y')
		{
			//1.获取网卡信息，进而得到局域网中所有的IP地址列表
			std::vector<Adapter> list;
			AdapterUtil::GetAllAdapter(&list);

			//获取所有主机IP地址，添加到host_list中
			std::vector<Host> host_list;
			for (int i = 0; i < list.size(); ++i)
			{
				uint32_t ip = list[i]._ip_addr;
				uint32_t mask = list[i]._mask_addr;
				//计算网络号
				uint32_t net = (ntohl(ip & mask));
				//计算最大主机号
				uint32_t max_host = (~ntohl(mask));  //转换为小端字节序,因为主机号是大端字节序
				std::vector<std::thread*> thr_list(max_host);
				std::vector<bool> ret_list(max_host);
				for (int j = 1; j < (int32_t)max_host; ++j)
				{
					uint32_t host_ip = net + j;  //这个主机IP的计算应该使用主机字节序的网络号和主机号
					Host host;
					host._ip_addr = htonl(host_ip); //将这个主机字节序的IP地址转换成网络字节序
					host._pair_ret = false;
					host_list.push_back(host);
				}
			}

			//对host_list中的主机创建线程进行配对
			std::vector<std::thread*> thr_list(host_list.size());
			for (int i = 0; i < host_list.size(); ++i)
			{
				thr_list[i] = new std::thread(&Client::HostPair, this, &host_list[i]);
			}
			std::cout << "正在主机匹配中，请稍后...\n";
			//等待所有线程主机配对完毕，判断配对结果，将在线主机添加到_online_host中
			for (int i = 0; i < host_list.size(); ++i)
			{
				thr_list[i]->join();
				if (host_list[i]._pair_ret == true)
				{
					_online_host.push_back(host_list[i]);
				}
				delete thr_list[i];
			}
		}
		
		//将所有在线主机的IP打印出来，供用户选择
		for (int i = 0; i < _online_host.size(); ++i)
		{
			char buf[MAX_IPBUFFER] = { 0 };
			inet_ntop(AF_INET, &_online_host[i]._ip_addr, buf, MAX_IPBUFFER);
			std::cout << "\t" << buf << std::endl;
		}
		
		//3.若配对请求得到响应，即为在线主机，将在线主机IP地址添加到_online_host中
		//4.打印在线主机列表，使用户选择
		std::cout << "请选择配对主机，获取共享主机列表：";
		fflush(stdout);
		std::string select_ip;
		std::cin >> select_ip;
		GetShareList(select_ip);  //用户选择主机之后，调用获取文件列表接口
		return true;
	}

	//获取共享文件列表
	bool GetShareList(const std::string& host_ip)
	{
		//向服务端发送一个文件列表请求
		//1.先发送请求
		//2.得到响应之后，解析正文(文件名称)
		httplib::Client cli(host_ip, P2P_PORT);
		auto rsp = cli.Get("/list");
		if (rsp == NULL || rsp->status != 200)
		{
			std::cerr << "获取文件列表响应错误\n";
			return false;
		}
		//打印正文---打印服务端响应的文件名称供用户选择
		std::cout << rsp->body << std::endl;
		std::cout << "\n请选择要下载的文件:";
		fflush(stdout);
		std::string filename;
		std::cin >> filename;
		RangeDownload(host_ip, filename);
		return true;
	}

	//下载文件
	bool DownloadFile(const std::string& host_ip, const std::string& filename)
	{
		//1.向服务端发送文件下载请求
		//2.得到响应结果，则响应结果中的body正文就是文件数据
		//3.创建文件，将文件数据写入文件中，关闭文件
		std::string req_path = "/download/" + filename;
		httplib::Client cli(host_ip, P2P_PORT);
		auto rsp = cli.Get(req_path.c_str());
		if (rsp == NULL || rsp->status != 200)
		{
			std::cout << "下载文件获取响应信息失败:" << rsp->status << std::endl;
			return false;
		}
		if (!boost::filesystem::exists(DOWNLOAD_PATH))
		{
			boost::filesystem::create_directories(DOWNLOAD_PATH);
		}
		std::string realpath = DOWNLOAD_PATH + filename;
		if (FileUtil::Write(realpath, rsp->body) == false)
		{
			std::cerr << "文件下载失败\n";
			return false;
		}
		std::cout << "文件下载成功!\n";
		return true;
	}

	bool RangeDownload(const std::string& host_ip, const std::string& name)
	{
		//1.发送HEAD请求，通过响应中的Content-Length获取文件大小
		std::string req_path = "/download/" + name;
		httplib::Client cli(host_ip, P2P_PORT);
		auto rsp = cli.Head(req_path.c_str());
		if (rsp == NULL || rsp->status != 200)
		{
			std::cout << "获取文件大小信息失败\n";
			return false;
		}
		std::string clen = rsp->get_header_value("Content-Length");
		int64_t filesize = StringUtil::Str2Dig(clen);
		
		//2.根据文件大小进行分块
		//int range_count = filesize / MAX_RANGE;

		//a.若文件大小小于块大小，则直接下载文件
		if (filesize < MAX_RANGE)
		{
			std::cout << "文件较小，直接下载文件\n";
			return DownloadFile(host_ip, name);
		}
		//计算分块个数
		//b.若文件大小不能整除块大小，则分块个数 = (文件大小除以块大小) + 1;
		//c.若文件大小刚还整除块大小，则分块个数就是文件大小初一分块大小
		std::cout << "文件较大，分块进行下载\n";
		int range_count = 0;
		if ((filesize % MAX_RANGE) == 0)
		{
			range_count = filesize / MAX_RANGE;
		}
		else
		{
			range_count = (filesize / MAX_RANGE) + 1;
		}
		int64_t range_start = 0, range_end = 0;
		for (int i = 0; i < range_count; ++i)
		{
			range_start = i * MAX_RANGE;
			if (i == (range_count - 1))
			{
				range_end = filesize - 1;
			}
			else
			{
				range_end = ((i + 1) * MAX_RANGE) - 1;
			}
			std::cout << "客户端请求分块:" << range_start << "-" << range_end << std::endl;
			//3.逐一请求分块数据，得到响应之后写入文件的指定位置
			httplib::Headers header;
			header.insert(httplib::make_range_header({ {range_start, range_end} }));  //设置一个range区间
			httplib::Client cli(host_ip.c_str(), P2P_PORT);
			auto rsp = cli.Get(req_path.c_str(), header);
			if (rsp == NULL || rsp->status != 206)
			{
				std::cout << "区间下载文件失败\n";
				return false;
			}
			std::cout << "客户端分块写入文件[" << rsp->body << "]\n";
			FileUtil::Write(name, rsp->body, range_start);
			std::cout << "客户端分块写入成功\n";
		}
		std::cout << "文件下载成功\n";

		return true;
	}
private:
	std::vector<Host> _online_host;
};

class Server
{
public:
	bool Start()
	{
		//添加针对客户端请求得处理方式对应关系
		_srv.Get("/hostpair", HostPair);
		_srv.Get("/list", ShareList);
		//防止与上方的请求冲突，因此在请求中加入download路径
		_srv.Get("/download/.*",Download); //正则表达式：将特殊字符以指定的格式来表示具有某种关键字的特征

		_srv.listen("0.0.0.0", P2P_PORT);
		return true;
	}
private:
	static void HostPair(const httplib::Request& req, httplib::Response& rsp)
	{
		rsp.status = 200;
		return;
	}
	//获取共享文件列表，在主机上设置一个共享目录，凡是这个目录下的文件都是要共享给别人
	static void ShareList(const httplib::Request& req, httplib::Response& rsp)
	{
		//1.查看目录是否存在，若目录不存在，则创建这个目录
		if (!boost::filesystem::exists(SHARD_PATH))
		{
			boost::filesystem::create_directory(SHARD_PATH);
		}
		boost::filesystem::directory_iterator begin(SHARD_PATH); //实例化目录迭代器
		boost::filesystem::directory_iterator end; //实例化目录迭代器的末尾
		//开始迭代目录
		for (; begin != end; ++begin)
		{
			if (boost::filesystem::is_directory(begin->status()))
			{
				//这里只是获取普通文件名称，不做多层级目录的操作
				continue;
			}
			std::string name = begin->path().filename().string();  //只要文件名，不要路径
			rsp.body += name + "\r\n";  //filename1\r\nfilename2\r\n......
		}
		rsp.status = 200;
		return;
	}
	static void Download(const httplib::Request& req, httplib::Response& rsp)
	{
		//std::cout << "服务端收到文件下载请求:" << req.path << std::endl;
			//req.path 客户端请求的资源路径 /download/filename
		boost::filesystem::path req_path(req.path);
		std::string name = req_path.filename().string(); //只获取文件名称  filename.txt
		//std::cout << "服务端收到实际的文件下载名称:" << name << "路径" << SHARD_PATH << std::endl;
		std::string realpath = SHARD_PATH + name;  //实际文件的路径应该是在共享的目录下

		//std::cout << "服务端收到实际的文件下载路径:" << realpath << std::endl;
		if (!boost::filesystem::exists(realpath) || boost::filesystem::is_directory(realpath))
		{
			rsp.status = 404;
			return;
		}
		if (req.method == "GET")
		{ 
			//以前GET请求，就是直接下载完整文件，但是现在有分块传输功能
			//判断是否是分块传输，依据的是这次请求中是否包含Range头部字段
			if (req.has_header("Range"))
			{
				//分块传输，需要知道分块的区间
				std::string range_str = req.get_header_value("Range");
				httplib::Ranges ranges;  // vector<Range>  Range->std::pair<start, end>
				httplib::detail::parse_range_header(range_str, ranges);  //解析客户端的Range数据
				int64_t range_start = ranges[0].first;  //pair中first就是range的start位置
				int64_t range_end = ranges[0].second;   //pair中second就是range的end位置
				int64_t range_len = range_end - range_start + 1; //range区间的长度
				std::cout << "range:" << range_start << "-" << range_end << std::endl;
				FileUtil::ReadRange(realpath, &rsp.body, range_len, range_start);
				rsp.status = 206;
				std::cout << "服务端响应区间数据完毕:[" << rsp.body << "]\n";
			}
			else
			{
				if (FileUtil::Read(realpath, &rsp.body) == false)
				{
					rsp.status = 500;
					return;
				}
				rsp.status = 200;
			}
		}
		else
		{
			//这个是针对HEAD请求得---客户端只要头部不要正文
			int64_t filesize = FileUtil::GetFileSize(realpath);
			rsp.set_header("Content-Length", std::to_string(filesize));  //设置响应头部信息
			rsp.status = 200;
		}
		std::cout << "服务端文件下载请求响应完毕\n";
		return;
	}
private:
	httplib::Server _srv;
};