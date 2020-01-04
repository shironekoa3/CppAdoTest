#include "stdafx.h"
#include "AdoConn.h"


AdoConn::AdoConn()
{
	::CoInitialize(NULL);//��ʼ��OLE/COM�⻷����Ϊ����ADO�ӿ���׼��
	m_pConnection = NULL;
}


AdoConn::~AdoConn()
{
	if (NULL != m_pRecordset)
		m_pRecordset->Close();
	if (NULL != m_pConnection)
		m_pConnection->Close();
	::CoUninitialize();
}

bool AdoConn::CreateConn()
{
	HRESULT hr = m_pConnection.CreateInstance(__uuidof(Connection));
	if (FAILED(hr))return false;
	else return true;
}

bool AdoConn::OpenConn(string connstr, string connUserId, string connUserPwd)
{
	if (NULL == m_pConnection)
		if (!CreateConn()) return false;

	_bstr_t strConnect = connstr.c_str();

	//connecting to the database server now:
	try
	{
		m_pConnection->Open(strConnect, connUserId.c_str(), connUserPwd.c_str(), NULL);
		if (NULL == m_pConnection) return false;
	}
	catch (_com_error err)
	{
		//printf("�������ݿ�ʧ��!������Ϣ:%ls", err.ErrorMessage());
		return false;
	}
	return true;

}

vector<vector<string>> AdoConn::SelectData(string selectTxt)
{
	vector<vector<string>> result;
	if (NULL == m_pConnection) return result;
	try
	{
		if (NULL == m_pRecordset)
			m_pRecordset.CreateInstance(__uuidof(Recordset));
		m_pRecordset->Open(selectTxt.c_str(), m_pConnection.GetInterfacePtr(),
			adOpenDynamic, adLockOptimistic, adCmdText);
		m_pRecordset->MoveFirst();
		while (m_pRecordset->adoEOF == VARIANT_FALSE)
		{
			vector<string> temp;
			temp.resize(m_pRecordset->GetFields()->Count);
			for (UINT i = 0; i < temp.size(); i++)
			{
				temp[i] = (char *)(_bstr_t)m_pRecordset->GetCollect(m_pRecordset->GetFields()->GetItem((long)i)->GetName());
			}
			result.push_back(temp);
			m_pRecordset->MoveNext();//�Ƶ���һ����¼
		}
	}
	catch (_com_error err)
	{
		return result;
	}
	return result;

	//��CursorType������ͣ�������������ֵ֮һ,�뿴���ö�ٽṹ:
	/*enum CursorTypeEnum
	{
		adOpenUnspecified = -1,///�����ر�ָ��
		adOpenForwardOnly = 0,///ǰ����̬��ꡣ���ֹ��ֻ����ǰ�����¼����������MoveNext��ǰ����,��

		�ַ�ʽ�����������ٶȡ�������BookMark, RecordCount, AbsolutePosition, AbsolutePage������ʹ��
		adOpenKeyset = 1,///�������ֹ��ļ�¼�������������û���������ɾ�������������ڸ���ԭ�м�¼��

		���������ǿɼ��ġ�
		adOpenDynamic = 2,///��̬��ꡣ�������ݿ�Ĳ������������ڸ��û���¼���Ϸ�Ӧ������
		adOpenStatic = 3///��̬��ꡣ��Ϊ��ļ�¼������һ����̬���ݣ��������û���������ɾ�������²�

		������ļ�¼����˵�ǲ��ɼ��ġ�
	};*/

	//��LockType�������ͣ�������������ֵ֮һ���뿴����ö�ٽṹ��
	/*enum LockTypeEnum
	{
		adLockUnspecified = -1,///δָ��
		adLockReadOnly = 1,///ֻ����¼��
		adLockPessimistic = 2, ����������ʽ�������ڸ���ʱ�����������ж����������ȫ����������
		adLockOptimistic = 3, �ֹ�������ʽ��ֻ���������Update����ʱ��������¼���ڴ�֮ǰ��Ȼ��������

		�ݵĸ��¡����롢ɾ���ȶ���
		adLockBatchOptimistic = 4���ֹ۷������¡��༭ʱ��¼�������������ġ����뼰ɾ������������ģʽ

		����ɡ�
	};*/

	//5.option����ȡ����ֵ 
	/*adCmdText:����CommandText���ı�����
	adCmdTable : ����CommandText��һ������
	adCmdProc : ����CommandText��һ���洢����
	adCmdUnknown : δ֪*/

}

void AdoConn::ExecProc(string procName)
{
	//����ֻ��һ�����ӣ�ִ�������洢�����ڴ˻����ϸĲ���������
	if (NULL == m_pConnection) return;
	try
	{
		_CommandPtr cmmd;
		HRESULT hr = cmmd.CreateInstance(__uuidof(Command));

		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t(KsDecry({ 238,97,84,98,100,91,99,1 }).c_str()), adVarChar, adParamOutput, 50, ""));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t(KsDecry({ 112,56,43,57,59,50,58,86 }).c_str()), adVarChar, adParamOutput, 50, ""));

		cmmd->CommandText = _bstr_t(procName.c_str());
		cmmd->ActiveConnection = m_pConnection;
		cmmd->CommandType = adCmdStoredProc;
		cmmd->Execute(NULL, NULL, adCmdStoredProc);
		string strRet = (const char*)(_bstr_t)cmmd->Parameters->GetItem(KsDecry({ 210,141,128,142,144,135,143,73 }).c_str())->GetValue();
		
	}
	catch (_com_error err)
	{
		
	}
}

int AdoConn::InsertUser(string username,string userpwd,string userqq, string userdiskdriver)
{
	int result = 0;
	if (NULL == m_pConnection) return result;
	try
	{
		_CommandPtr cmmd;
		HRESULT hr = cmmd.CreateInstance(__uuidof(Command));

		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_user_name"), adVarChar, adParamInput, 12, username.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_user_password"), adVarChar, adParamInput, 20, userpwd.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_user_qq"), adVarChar, adParamInput, 10, userqq.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_user_diskdriver"), adVarChar, adParamInput, 50, userdiskdriver.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("result"), adInteger, adParamOutput, 4, 0));

		cmmd->CommandText = _bstr_t("proc_createuser");
		cmmd->ActiveConnection = m_pConnection;
		cmmd->CommandType = adCmdStoredProc;
		cmmd->Execute(NULL, NULL, adCmdStoredProc);
		result = cmmd->Parameters->GetItem("result")->GetValue();
	}
	catch (_com_error err)
	{
		result = ADOERROR_INSERT_USER_ERROR;
	}
	return result;
}
int AdoConn::LoginUser(string username, string userpwd, string userdiskdriver)
{
	int result = 0;
	if (NULL == m_pConnection) return result;
	try
	{
		_CommandPtr cmmd;
		HRESULT hr = cmmd.CreateInstance(__uuidof(Command));

		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_user_name"), adVarChar, adParamInput, 12, username.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_user_password"), adVarChar, adParamInput, 20, userpwd.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_user_diskdriver"), adVarChar, adParamInput, 50, userdiskdriver.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("result"), adInteger, adParamOutput, 4, 0));

		cmmd->CommandText = _bstr_t("proc_loginuser");
		cmmd->ActiveConnection = m_pConnection;
		cmmd->CommandType = adCmdStoredProc;
		cmmd->Execute(NULL, NULL, adCmdStoredProc);
		result = cmmd->Parameters->GetItem("result")->GetValue();
	}
	catch (_com_error err)
	{
		result = ADOERROR_LOGIN_USER_ERROR;
	}
	return result;
}
int AdoConn::RechargeUser(string username, string userpwd, string cardId)
{
	int result = 0;
	if (NULL == m_pConnection) return result;
	try
	{
		_CommandPtr cmmd;
		HRESULT hr = cmmd.CreateInstance(__uuidof(Command));

		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_user_name"), adVarChar, adParamInput, 12, username.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_user_password"), adVarChar, adParamInput, 20, userpwd.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_card_id"), adVarChar, adParamInput, 50, cardId.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("result"), adInteger, adParamOutput, 4, 0));

		cmmd->CommandText = _bstr_t("proc_rechargeuser");
		cmmd->ActiveConnection = m_pConnection;
		cmmd->CommandType = adCmdStoredProc;
		cmmd->Execute(NULL, NULL, adCmdStoredProc);
		result = cmmd->Parameters->GetItem("result")->GetValue();
	}
	catch (_com_error err)
	{
		result = ADOERROR_RECHARGE_USER_ERROR;
	}
	return result;
}
int AdoConn::GetPt(string username, string userpwd)
{
	int result = 0;
	if (NULL == m_pConnection) return result;
	try
	{
		_CommandPtr cmmd;
		HRESULT hr = cmmd.CreateInstance(__uuidof(Command));

		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_user_name"), adVarChar, adParamInput, 12, username.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_user_password"), adVarChar, adParamInput, 20, userpwd.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("result"), adInteger, adParamOutput, 4, 0));

		cmmd->CommandText = _bstr_t("proc_getpt");
		cmmd->ActiveConnection = m_pConnection;
		cmmd->CommandType = adCmdStoredProc;
		cmmd->Execute(NULL, NULL, adCmdStoredProc);
		result = cmmd->Parameters->GetItem("result")->GetValue();
	}
	catch (_com_error err)
	{
		result = 0;
	}
	return result;
}
string AdoConn::GetRestTime(string username,string userpwd)
{
	string sResult;
	if (NULL == m_pConnection) return sResult;
	try
	{
		_CommandPtr cmmd;
		HRESULT hr = cmmd.CreateInstance(__uuidof(Command));

		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_user_name"), adVarChar, adParamInput, 12, username.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("psmy_user_password"), adVarChar, adParamInput, 20, userpwd.c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t("result"), adInteger, adParamOutput, 4, 0));

		cmmd->CommandText = _bstr_t("proc_getresttime");
		cmmd->ActiveConnection = m_pConnection;
		cmmd->CommandType = adCmdStoredProc;
		cmmd->Execute(NULL, NULL, adCmdStoredProc);
		ULONG uTime = cmmd->Parameters->GetItem("result")->GetValue();
		sResult = DiffToString(uTime);
	}
	catch (_com_error err)
	{
		sResult = "";
	}
	return sResult;
}

string AdoConn::GetDiskDriverID()
{
	string sResult;
	string sName;
	string cmd = "wmic diskdrive get serialnumber";
	char cResult[100] = { 0 };
	FILE* fp;
	if (NULL != (fp = _popen(cmd.c_str(), "r")))
	{
		fgets(cResult, sizeof(cResult), fp);
		sName = cResult;
		fgets(cResult, sizeof(cResult), fp);
		sResult = cResult;
		sResult = sResult.substr(0, sResult.find_first_of(" "));
		_pclose(fp);
	}
	return sResult;
}
string AdoConn::DiffToString(ULONG pDiff)
{
	string sResult;
	char cResult[255];
	UINT second = pDiff % 60;
	UINT minute = (pDiff / 60) % 60;
	UINT hour = (pDiff / 60 / 60) % 24;
	UINT day = pDiff / 60 / 60 / 24;
	sprintf_s(cResult, "%d��%02dʱ%02d��%02d��", day, hour, minute, second);
	sResult = cResult;
	return cResult;
}


string AdoConn::KsDecry(vector<BYTE> data)
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


