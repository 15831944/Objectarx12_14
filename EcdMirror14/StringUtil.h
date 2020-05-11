#pragma once
#include <vector>
class CStringUtil
{
public:
	CStringUtil();
	~CStringUtil();
	
	// ��һ���ַ���Ϊ�ָ������ַ������зָ�
	// bAddEmptyItem: �Ƿ���ӿ��ַ���Ԫ��
	static void Split(const CString &text, const TCHAR* separator, std::vector<CString> &result, bool bAddEmptyItem = true);

	// ͨ���ָ����ϲ��ַ�������
	static CString Join(const std::vector<CString> &items, const TCHAR* separator);

};

