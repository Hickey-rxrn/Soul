#pragma once
#include "Util.h"
#include <thread>
#include "httplib.h"
#include <boost/filesystem.hpp>

#define P2P_PORT 9000
#define MAX_IPBUFFER 16
#define MAX_RANGE (100 * 1024 * 1024)  // 1����10λ��1k������20λ��1M   100����20λ����100M
#define SHARD_PATH "./Shared/"
#define DOWNLOAD_PATH "./Download/"

class Host
{
public:
	uint32_t _ip_addr; //Ҫ���������IP��ַ
	bool _pair_ret; //���ڴ����Խ������Գɹ�true��ʧ��false
};

class Client
{
public:
	bool Start()
	{
		//�ͻ��˳�����Ҫѭ�����У���Ϊ�����ļ�����ֻ����һ��
		while (1)
		{
			GetOnlineHost();
		}
		return true;
	}
	//������Ե��߳���ں���
	void HostPair(Host* host)
	{
		//1.��֯httpЭ���ʽ����������
		//2.�һ��tcp�Ŀͻ��ˣ������ݷ���
		//3.�ȴ�����˵Ļظ��������н���
		//��������ʹ�õ�������httplibʵ��
		host->_pair_ret = false;
		char buf[MAX_IPBUFFER] = { 0 };
		inet_ntop(AF_INET, &host->_ip_addr, buf, MAX_IPBUFFER);
		httplib::Client cli(buf, P2P_PORT);  //ʵ����httplib�ͻ��˶���
		auto rsp = cli.Get("/hostpair");     //�����˷�����ԴΪ/hostpair��GET����������ʧ�ܣ�GET�᷵��NULL
		if (rsp && rsp->status == 200)              //�ж���Ӧ����Ƿ���ȷ
		{
			host->_pair_ret = true;          //����������Խ��
		}
		return;
	}

	//��ȡ��������
	bool GetOnlineHost()
	{
		char ch = 'Y'; //�Ƿ�����ƥ�䣬Ĭ���ǽ���ƥ��ģ����Ѿ�ƥ�����_online_host��Ϊ�գ����û�ѡ��
		if (!_online_host.empty())
		{
			std::cout << "�Ƿ����²鿴��������(Y/N):";
			fflush(stdout);
			std::cin >> ch;
		}
		if (ch == 'Y')
		{
			//1.��ȡ������Ϣ�������õ������������е�IP��ַ�б�
			std::vector<Adapter> list;
			AdapterUtil::GetAllAdapter(&list);

			//��ȡ��������IP��ַ����ӵ�host_list��
			std::vector<Host> host_list;
			for (int i = 0; i < list.size(); ++i)
			{
				uint32_t ip = list[i]._ip_addr;
				uint32_t mask = list[i]._mask_addr;
				//���������
				uint32_t net = (ntohl(ip & mask));
				//�������������
				uint32_t max_host = (~ntohl(mask));  //ת��ΪС���ֽ���,��Ϊ�������Ǵ���ֽ���
				std::vector<std::thread*> thr_list(max_host);
				std::vector<bool> ret_list(max_host);
				for (int j = 1; j < (int32_t)max_host; ++j)
				{
					uint32_t host_ip = net + j;  //�������IP�ļ���Ӧ��ʹ�������ֽ��������ź�������
					Host host;
					host._ip_addr = htonl(host_ip); //����������ֽ����IP��ַת���������ֽ���
					host._pair_ret = false;
					host_list.push_back(host);
				}
			}

			//��host_list�е����������߳̽������
			std::vector<std::thread*> thr_list(host_list.size());
			for (int i = 0; i < host_list.size(); ++i)
			{
				thr_list[i] = new std::thread(&Client::HostPair, this, &host_list[i]);
			}
			std::cout << "��������ƥ���У����Ժ�...\n";
			//�ȴ������߳����������ϣ��ж���Խ����������������ӵ�_online_host��
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
		
		//����������������IP��ӡ���������û�ѡ��
		for (int i = 0; i < _online_host.size(); ++i)
		{
			char buf[MAX_IPBUFFER] = { 0 };
			inet_ntop(AF_INET, &_online_host[i]._ip_addr, buf, MAX_IPBUFFER);
			std::cout << "\t" << buf << std::endl;
		}
		
		//3.���������õ���Ӧ����Ϊ��������������������IP��ַ��ӵ�_online_host��
		//4.��ӡ���������б�ʹ�û�ѡ��
		std::cout << "��ѡ�������������ȡ���������б�";
		fflush(stdout);
		std::string select_ip;
		std::cin >> select_ip;
		GetShareList(select_ip);  //�û�ѡ������֮�󣬵��û�ȡ�ļ��б�ӿ�
		return true;
	}

	//��ȡ�����ļ��б�
	bool GetShareList(const std::string& host_ip)
	{
		//�����˷���һ���ļ��б�����
		//1.�ȷ�������
		//2.�õ���Ӧ֮�󣬽�������(�ļ�����)
		httplib::Client cli(host_ip, P2P_PORT);
		auto rsp = cli.Get("/list");
		if (rsp == NULL || rsp->status != 200)
		{
			std::cerr << "��ȡ�ļ��б���Ӧ����\n";
			return false;
		}
		//��ӡ����---��ӡ�������Ӧ���ļ����ƹ��û�ѡ��
		std::cout << rsp->body << std::endl;
		std::cout << "\n��ѡ��Ҫ���ص��ļ�:";
		fflush(stdout);
		std::string filename;
		std::cin >> filename;
		RangeDownload(host_ip, filename);
		return true;
	}

	//�����ļ�
	bool DownloadFile(const std::string& host_ip, const std::string& filename)
	{
		//1.�����˷����ļ���������
		//2.�õ���Ӧ���������Ӧ����е�body���ľ����ļ�����
		//3.�����ļ������ļ�����д���ļ��У��ر��ļ�
		std::string req_path = "/download/" + filename;
		httplib::Client cli(host_ip, P2P_PORT);
		auto rsp = cli.Get(req_path.c_str());
		if (rsp == NULL || rsp->status != 200)
		{
			std::cout << "�����ļ���ȡ��Ӧ��Ϣʧ��:" << rsp->status << std::endl;
			return false;
		}
		if (!boost::filesystem::exists(DOWNLOAD_PATH))
		{
			boost::filesystem::create_directories(DOWNLOAD_PATH);
		}
		std::string realpath = DOWNLOAD_PATH + filename;
		if (FileUtil::Write(realpath, rsp->body) == false)
		{
			std::cerr << "�ļ�����ʧ��\n";
			return false;
		}
		std::cout << "�ļ����سɹ�!\n";
		return true;
	}

	bool RangeDownload(const std::string& host_ip, const std::string& name)
	{
		//1.����HEAD����ͨ����Ӧ�е�Content-Length��ȡ�ļ���С
		std::string req_path = "/download/" + name;
		httplib::Client cli(host_ip, P2P_PORT);
		auto rsp = cli.Head(req_path.c_str());
		if (rsp == NULL || rsp->status != 200)
		{
			std::cout << "��ȡ�ļ���С��Ϣʧ��\n";
			return false;
		}
		std::string clen = rsp->get_header_value("Content-Length");
		int64_t filesize = StringUtil::Str2Dig(clen);
		
		//2.�����ļ���С���зֿ�
		//int range_count = filesize / MAX_RANGE;

		//a.���ļ���СС�ڿ��С����ֱ�������ļ�
		if (filesize < MAX_RANGE)
		{
			std::cout << "�ļ���С��ֱ�������ļ�\n";
			return DownloadFile(host_ip, name);
		}
		//����ֿ����
		//b.���ļ���С�����������С����ֿ���� = (�ļ���С���Կ��С) + 1;
		//c.���ļ���С�ջ��������С����ֿ���������ļ���С��һ�ֿ��С
		std::cout << "�ļ��ϴ󣬷ֿ��������\n";
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
			std::cout << "�ͻ�������ֿ�:" << range_start << "-" << range_end << std::endl;
			//3.��һ����ֿ����ݣ��õ���Ӧ֮��д���ļ���ָ��λ��
			httplib::Headers header;
			header.insert(httplib::make_range_header({ {range_start, range_end} }));  //����һ��range����
			httplib::Client cli(host_ip.c_str(), P2P_PORT);
			auto rsp = cli.Get(req_path.c_str(), header);
			if (rsp == NULL || rsp->status != 206)
			{
				std::cout << "���������ļ�ʧ��\n";
				return false;
			}
			std::cout << "�ͻ��˷ֿ�д���ļ�[" << rsp->body << "]\n";
			FileUtil::Write(name, rsp->body, range_start);
			std::cout << "�ͻ��˷ֿ�д��ɹ�\n";
		}
		std::cout << "�ļ����سɹ�\n";

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
		//�����Կͻ�������ô���ʽ��Ӧ��ϵ
		_srv.Get("/hostpair", HostPair);
		_srv.Get("/list", ShareList);
		//��ֹ���Ϸ��������ͻ������������м���download·��
		_srv.Get("/download/.*",Download); //������ʽ���������ַ���ָ���ĸ�ʽ����ʾ����ĳ�ֹؼ��ֵ�����

		_srv.listen("0.0.0.0", P2P_PORT);
		return true;
	}
private:
	static void HostPair(const httplib::Request& req, httplib::Response& rsp)
	{
		rsp.status = 200;
		return;
	}
	//��ȡ�����ļ��б�������������һ������Ŀ¼���������Ŀ¼�µ��ļ�����Ҫ���������
	static void ShareList(const httplib::Request& req, httplib::Response& rsp)
	{
		//1.�鿴Ŀ¼�Ƿ���ڣ���Ŀ¼�����ڣ��򴴽����Ŀ¼
		if (!boost::filesystem::exists(SHARD_PATH))
		{
			boost::filesystem::create_directory(SHARD_PATH);
		}
		boost::filesystem::directory_iterator begin(SHARD_PATH); //ʵ����Ŀ¼������
		boost::filesystem::directory_iterator end; //ʵ����Ŀ¼��������ĩβ
		//��ʼ����Ŀ¼
		for (; begin != end; ++begin)
		{
			if (boost::filesystem::is_directory(begin->status()))
			{
				//����ֻ�ǻ�ȡ��ͨ�ļ����ƣ�������㼶Ŀ¼�Ĳ���
				continue;
			}
			std::string name = begin->path().filename().string();  //ֻҪ�ļ�������Ҫ·��
			rsp.body += name + "\r\n";  //filename1\r\nfilename2\r\n......
		}
		rsp.status = 200;
		return;
	}
	static void Download(const httplib::Request& req, httplib::Response& rsp)
	{
		//std::cout << "������յ��ļ���������:" << req.path << std::endl;
			//req.path �ͻ����������Դ·�� /download/filename
		boost::filesystem::path req_path(req.path);
		std::string name = req_path.filename().string(); //ֻ��ȡ�ļ�����  filename.txt
		//std::cout << "������յ�ʵ�ʵ��ļ���������:" << name << "·��" << SHARD_PATH << std::endl;
		std::string realpath = SHARD_PATH + name;  //ʵ���ļ���·��Ӧ�����ڹ����Ŀ¼��

		//std::cout << "������յ�ʵ�ʵ��ļ�����·��:" << realpath << std::endl;
		if (!boost::filesystem::exists(realpath) || boost::filesystem::is_directory(realpath))
		{
			rsp.status = 404;
			return;
		}
		if (req.method == "GET")
		{ 
			//��ǰGET���󣬾���ֱ�����������ļ������������зֿ鴫�书��
			//�ж��Ƿ��Ƿֿ鴫�䣬���ݵ�������������Ƿ����Rangeͷ���ֶ�
			if (req.has_header("Range"))
			{
				//�ֿ鴫�䣬��Ҫ֪���ֿ������
				std::string range_str = req.get_header_value("Range");
				httplib::Ranges ranges;  // vector<Range>  Range->std::pair<start, end>
				httplib::detail::parse_range_header(range_str, ranges);  //�����ͻ��˵�Range����
				int64_t range_start = ranges[0].first;  //pair��first����range��startλ��
				int64_t range_end = ranges[0].second;   //pair��second����range��endλ��
				int64_t range_len = range_end - range_start + 1; //range����ĳ���
				std::cout << "range:" << range_start << "-" << range_end << std::endl;
				FileUtil::ReadRange(realpath, &rsp.body, range_len, range_start);
				rsp.status = 206;
				std::cout << "�������Ӧ�����������:[" << rsp.body << "]\n";
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
			//��������HEAD�����---�ͻ���ֻҪͷ����Ҫ����
			int64_t filesize = FileUtil::GetFileSize(realpath);
			rsp.set_header("Content-Length", std::to_string(filesize));  //������Ӧͷ����Ϣ
			rsp.status = 200;
		}
		std::cout << "������ļ�����������Ӧ���\n";
		return;
	}
private:
	httplib::Server _srv;
};