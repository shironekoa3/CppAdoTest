// AdoTest.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <string>
#include <iostream>
#include "AdoConn.h"

using namespace std;

string KsDecry(vector<BYTE> data)
{
	UINT len = data.size();
	BYTE datF = data[0];
	BYTE datL = data[len - 1];
	char cResult[MAX_PATH] = {};
	for (UINT i = 0; i < len - 2; i++)
	{
		cResult[i] = data[i + 1] - datF - datL;
	}
	string sResult = cResult;
	return sResult;
}
string GetGamePath()
{
	string a = "REG QUERY HKEY_CURRENT_USER\\Software\\Tencent\\DNF\\ /v InstallPath";
	string result;
	char cResult[100] = { 0 };
	FILE* fp;
	if (NULL != (fp = _popen(a.c_str(), "r")))
	{
		fgets(cResult, sizeof(cResult), fp);
		fgets(cResult, sizeof(cResult), fp);
		fgets(cResult, sizeof(cResult), fp);
		result = cResult;
		_pclose(fp);
	}
	result = result.substr(result.find_first_of("REG_SZ"));
	result = result.substr(result.find_first_of(" "));
	result = result.substr(result.find_first_not_of(" "));
	result.pop_back();
	return result;
}

void UserLogin()
{
	char uName[MAX_PATH];
	char uPwd[MAX_PATH];

	GetPrivateProfileStringA("CONFIG", "USERNAME", "", uName, sizeof(uName), "C:\theUser.ini");
	GetPrivateProfileStringA("CONFIG", "USERPASSWORD", "", uPwd, sizeof(uPwd), "C:\theUser.ini");

	if (strlen(uName) == 0 || strlen(uPwd) == 0)
	{
		cout << "请输入账号：" << endl;
		cin >> uName;
		cout << "请输入密码：" << endl;
		cin >> uPwd;
		cout << "正在登陆......" << endl;
	}

	string connStr = "Provider=SQLOLEDB.1;Persist Security Info=False;User ID=sa;Initial Catalog=smy;Data Source=58.87.125.98,1433";
	string connUserId = "sa+sa0+sa1";

	AdoConn adoConn;
	bool connRet = adoConn.OpenConn(connStr, connUserId.substr(0, 2), connUserId.substr(2));
	if (connRet)
	{
		int result = adoConn.LoginUser(uName, uPwd, adoConn.GetDiskDriverID());
		if (ADOERROR_USER_ERROR == result)
			cout << "登陆失败，用户名或密码不正确！" << endl;
		else if (ADOERROR_USER_BANED == result)
			cout << "登陆失败，此用户已被禁用！" << endl;
		else if (ADOERROR_USER_NOREST == result)
			cout << "登陆失败，用户已到期！" << endl;
		else if (ADOERROR_USER_BANDED == result)
			cout << "登陆失败，未到自动解绑时间(换机需要等两个小时自动解绑)！" << endl;
		else if (result <= 0)
			cout << "登陆失败，未知错误！" << endl;
		else
		{
			cout << "登陆成功。" << endl;
			cout << "剩余时间：" << adoConn.GetRestTime(uName, uPwd) << endl;

			WritePrivateProfileStringA("CONFIG", "USERNAME", uName, "C:\theUser.ini");
			WritePrivateProfileStringA("CONFIG", "USERPASSWORD", uPwd, "C:\theUser.ini");

			int tempPt = adoConn.GetPt(uName, uPwd);
			if (tempPt > 0)
			{
				/*HMODULE hCurrentHandle = GetModuleHandleA(NULL);
				HRSRC hRes = FindResourceA(hCurrentHandle, MAKEINTRESOURCEA(IDR_DAT1), "DAT");
				DWORD len = SizeofResource(hCurrentHandle, hRes);
				HGLOBAL hg = LoadResource(hCurrentHandle, hRes);
				LPVOID lp = (LPSTR)LockResource(hg);

				char* ptr = (char*)VirtualAlloc(NULL, len, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
				if (ptr) memcpy_s(ptr, len, lp, len);
				for (UINT i = 0; i < len; i++)
				{
					ptr[i] ^= tempPt;
				}

				//写入目录
				string gamePath = GetGamePath();
				gamePath.append("\\dfscli.dll");

				FILE* fp;
				fopen_s(&fp, gamePath.c_str(), "wb");
				if (fp)
				{
					for (UINT i = 0; i < len; i++)
					{
						fputc(ptr[i], fp);
					}
				}
				fclose(fp);
				cout << "上游戏自动呼出。" << endl;*/
			}

			Sleep(2000);
			exit(0);
		}
	}
	else
	{
		cout << "服务器连接失败！" << endl;
	}
	cout << "" << endl;
}

void UserRegist()
{
	string uName;
	string uPwd;
	string uQQ;
	cout << "请输入账号(12位以内中英文数字皆可)：" << endl;
	cin >> uName;
	cout << "请输入密码(20位以内)：" << endl;
	cin >> uPwd;
	cout << "请输入QQ(找回密码时需要)：" << endl;
	cin >> uQQ;
	cout << "正在注册......" << endl;

	string connStr = "Provider=SQLOLEDB.1;Persist Security Info=False;User ID=sa;Initial Catalog=smy;Data Source=58.87.125.98,1433";
	string connUserId = "sa+sa0+sa1";

	AdoConn adoConn;
	bool connRet = adoConn.OpenConn(connStr, connUserId.substr(0, 2), connUserId.substr(2));
	if (connRet)
	{
		int result = adoConn.InsertUser(uName, uPwd, uQQ, adoConn.GetDiskDriverID());
		if (ADOERROR_USER_EXISTED == result)
			cout << "注册失败，用户名已存在！" << endl;
		else if (result <= 0)
			cout << "注册失败，未知错误！" << endl;
		else
			cout << "注册成功。" << endl;
	}
	else
	{
		cout << "服务器连接失败！" << endl;
	}
	cout << "" << endl;
}

void UserRecharge()
{
	string uName;
	string uPwd;
	string cId;
	cout << "请输入账号：" << endl;
	cin >> uName;
	cout << "请输入密码：" << endl;
	cin >> uPwd;
	cout << "请输入卡号：" << endl;
	cin >> cId;
	cout << "正在充值......" << endl;



	string connStr = "Provider=SQLOLEDB.1;Persist Security Info=False;User ID=sa;Initial Catalog=smy;Data Source=58.87.125.98,1433";
	string connUserId = "sa+sa0+sa1";

	AdoConn adoConn;
	bool connRet = adoConn.OpenConn(connStr, connUserId.substr(0, 2), connUserId.substr(2));
	if (connRet)
	{
		int result = adoConn.RechargeUser(uName, uPwd, cId);
		if (ADOERROR_CARD_NOT_EXISTED == result)
			cout << "充值失败，卡号不存在！" << endl;
		else if (ADOERROR_CARD_USED == result)
			cout << "充值失败，此卡已被使用！" << endl;
		else if (result <= 0)
			cout << "充值失败，未知错误！" << endl;
		else
		{
			cout << "充值成功。" << endl;
			cout << "剩余时间：" << adoConn.GetRestTime(uName, uPwd) << endl;
		}
	}
	else
	{
		cout << "服务器连接失败！" << endl;
	}
	cout << "" << endl;
}

void Clear()
{
	string path = GetGamePath().append("\\dfscli.dll");
	remove(path.c_str());
}

int main()
{
	/*if (GetGamePath() == "")
	{
		cout << "游戏路径未找到，请运行一次游戏登录器。" << endl;
		return 0;
	}*/
	do
	{
		int ires = 0;
		cout << "1:登陆 2:注册 3:充值 4:清除辅助 其他:退出" << endl;
		cout << "请选择序号:";
		cin >> ires;
		cout << "" << endl;
		switch (ires)
		{
		case 1:
			UserLogin();
			break;
		case 2:
			UserRegist();
			break;
		case 3:
			UserRecharge();
			break;
		case 4:
			Clear();
			cout << "清理完毕。" << endl;
			break;
		default:
			cout << "退出程序。" << endl;
			Sleep(1000);
			return 0;
			break;
		}
	} while (true);
}


