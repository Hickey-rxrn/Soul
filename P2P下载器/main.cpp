

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
	boost::filesystem::directory_iterator begin(ptr);  //����һ��Ŀ¼����������
	boost::filesystem::directory_iterator end;
	for (; begin != end; ++begin)
	{
		//begin->status()  Ŀ¼�е�ǰ�ļ���״̬��Ϣ
		//boost::filesystem::is_directory()  �жϵ�ǰ�ļ���һ��Ŀ¼
		if (boost::filesystem::is_directory(begin->status()))
		{
			//��ȡ��ǰ�����ļ����ļ���
			std::cout << begin->path().string() << "��һ��Ŀ¼\n";
		}
		else
		{
			std::cout << begin->path().string() << "��һ����ͨ�ļ�\n";
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
	//�����߳���Ҫ���пͻ���ģ���Լ������ģ��
	//����һ���߳����пͻ���ģ�飬���߳����з����ģ��
	std::thread thr_client(ClientRun); 
	Server srv;
	srv.Start();
	return 0;
}