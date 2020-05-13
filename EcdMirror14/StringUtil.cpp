#include "stdafx.h"
#include "StringUtil.h"


CStringUtil::CStringUtil()
{
}


CStringUtil::~CStringUtil()
{
}

void CStringUtil::Split(const CString & text, const TCHAR * separator, std::vector<CString>& result, bool bAddEmptyItem)
{

	int prev = 0;
	int current = 0;		// �ҵ��ĵ�ǰƥ�����һ��ƥ��
	CString strCopyText = text;
	strCopyText.TrimRight(TEXT(" "));

	// ��һ���Ȳ���Ŀ���ַ�������û�а����ָ���
	if (strCopyText.Find(separator, 0) < 0)
	{
		if (strCopyText.GetLength() > 0)
		{
			result.push_back(strCopyText);
		}
		return;
	}

	// ѭ����ȡ�м���ַ�
	int length = strCopyText.GetLength();
	bool bExit = false;
	while (1)
	{
		current = strCopyText.Find(separator, prev);
		if (current == -1)		// �����һ����
		{
			current = length;
			bExit = true;
		}

		// ��һ���ָ���֮�������ָ���֮ǰ���ַ���������Ч���ַ���
		CString item = strCopyText.Mid(prev, current - prev);
		if (bAddEmptyItem || item.GetLength() > 0)
		{
			result.push_back(item);
		}

		if (bExit)
		{
			break;
		}

		prev = current + (int)_tcslen(separator);
	}

}

CString CStringUtil::Join(const std::vector<CString>& items, const TCHAR * separator)
{
	CString strResult;
	for (int i = 0; i < (int)items.size(); i++)
	{
		strResult += items[i] + separator;
	}

	strResult.TrimRight(separator);

	return strResult;
}