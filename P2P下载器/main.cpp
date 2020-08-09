

#include "Client.h"

void helloWorld(const httplib::Request& req, httplib::Response& rsp)
{
	printf("httplib server recv a req:%s\n", req.path.c_str());
	rsp.set_content("<html><h1>HelloWorld</h1></html>", "text/html");
	rsp.status = 200;
}

void Scandir()
{
	const char* ptr = "./";
	boost::filesystem::directory_iterator begin(ptr);  //定义一个目录迭代器对象
	boost::filesystem::directory_iterator end;
	for (; begin != end; ++begin)
	{
		//begin->status()  目录中当前文件的状态信息
		//boost::filesystem::is_directory()  判断当前文件是一个目录
		if (boost::filesystem::is_directory(begin->status()))
		{
			//获取当前迭代文件的文件名
			std::cout << begin->path().string() << "是一个目录\n";
		}
		else
		{
			std::cout << begin->path().string() << "是一个普通文件\n";
		}
	}
}

void Test()
{
	/*std::vector<Adapter> list;
	AdapterUtil::GetAllAdapter(&list);*/

	/*httplib::Server srv;
	srv.Get("/", helloWorld);
	srv.listen("0.0.0.0", 9000);*/
	//Scandir();
}

void ClientRun()
{
	Sleep(1);
	Client cli;
	cli.Start();
}
int main()
{
	//在主线程中要运行客户端模块以及服务端模块
	//创建一个线程运行客户端模块，主线程运行服务端模块
	std::thread thr_client(ClientRun); 
	Server srv;
	srv.Start();
	return 0;
}