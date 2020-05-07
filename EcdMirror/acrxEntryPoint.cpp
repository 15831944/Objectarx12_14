#include "StdAfx.h"
#include "resource.h"
#include "acdocman.h"
#include"acedads.h"
#include "acedCmdNF.h"
#include<vector>
#include"MoveJig.h"
#include <shlwapi.h>

#pragma comment(lib,"Shlwapi.lib")

using namespace std;
//-----------------------------------------------------------------------------
#define szRDS _RXST("ECD")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CEcdMirrorApp : public AcRxArxApp {

public:
	CEcdMirrorApp() : AcRxArxApp() {}

	virtual AcRx::AppRetCode On_kInitAppMsg(void *pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg(pkt);

		// TODO: Add your initialization code here

		return (retCode);
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg(void *pkt) {
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode = AcRxArxApp::On_kUnloadAppMsg(pkt);

		// TODO: Unload dependencies here

		return (retCode);
	}

	virtual void RegisterServerComponents() {
	}

	static void ECDMyGroupEcdRR()
	{


		ads_name ent;
		ads_point pt2, pt3;
		if (acedSSGet(NULL, NULL, NULL, NULL, ent) != RTNORM) {
			return;
		}

		if (acedGetPoint(NULL, L"Pt:", pt2) != RTNORM) {
			return;
		}
		if (acedGetPoint(pt2, L"Pt:", pt3) != RTNORM) {
			return;
		}
		ads_name n1;

		long len = 0;

		acedSSLength(ent, &len);



		for (int i = 0; i < len; i++)
		{
			ads_name a;
			acedSSName(ent, i, a);
			acedSSAdd(a, NULL, n1);
			acedCommandS(RTSTR, _T("MIRROR"),

				RTPICKS, n1,
				RTSTR, _T(""),
				RTPOINT, pt2,
				RTPOINT, pt3,
				RTSTR, _T("N"),
				RTSTR, _T(""),
				RTNONE);

			acedSSFree(n1);
		}

		acedSSFree(ent);

	}

	static void  ECDMyGroupEcdUpMR() {

		AcDbObjectIdArray oIds;
		GetSel(oIds);

		AcGePoint3d formPt, toPt;
		bool findPt = false;
		for (int i = 0; i < oIds.length(); i++)
		{
			AcDbEntity * l = NULL;

			if (acdbOpenObject(l, oIds[i], AcDb::kForRead) == Acad::eOk) {

				CString appName = "XDataDCZ";
				resbuf * rb = l->xData(appName);

				if (rb != NULL) {

					resbuf *tempRs = rb;

					tempRs = tempRs->rbnext;


					int flag = tempRs->resval.rint;
					tempRs = tempRs->rbnext;
					if (flag == 1) {
						formPt = asPnt3d(tempRs->resval.rpoint);
						tempRs = tempRs->rbnext;
						toPt = asPnt3d(tempRs->resval.rpoint);
						acutRelRb(rb);
						findPt = true;
						break;
					}
					acutRelRb(rb);

				}
				l->close();
			}
		}

		if (!findPt) {
			acutPrintf(L"没有找到对称轴数据");
			return;
		}


		for (int i = 0; i < oIds.length(); i++)
		{
			AcDbEntity * l = NULL;

			if (acdbOpenObject(l, oIds[i], AcDb::kForRead) == Acad::eOk) {

				if (l->isA() == AcDbBlockReference::desc()) {

					l->close();
					continue;
				}
			}
		}

		CMoveJig jig(formPt, toPt);

		jig.UpdateDoIt(oIds, true);

		/*if (jig.m_idsC.length() == 0) {
			return;
		}*/

		

		/*CopyFile(newPath, copyPath, TRUE);

		DeleteFile(newPath);

		otherIds.append(jig.m_idsC);

		if (DeepClone(otherIds, newPath))
		{
			DeleteFile(copyPath);
		}
		else {
			CopyFile(copyPath, newPath, FALSE);

			DeleteFile(copyPath);
		}*/

		/*for (int i = 0; i < jig.m_idsC.length(); i++)
		{
			AcDbEntity * l = NULL;

			if (acdbOpenObject(l, jig.m_idsC[i], AcDb::kForWrite) == Acad::eOk) {

				l->erase();
				l->close();
				l = NULL;
			}
		}*/

		//DeepClone(otherIds, newPath);
		
	}


	static void ECDMyGroupEcdMR() {

		CString path = acDocManager->curDocument()->fileName();

		int lastInt = path.ReverseFind('\\');


		CString fileName = path.Mid(lastInt + 1);
		int lastDot = fileName.ReverseFind('.');

		CString newFileName;
		newFileName.Format(L"%s_M", fileName.Mid(0, lastDot));

		CString  newPath;
		newPath.Format(L"%s\\%s.dwg", path.Mid(0, lastInt), newFileName);

		CString aixPath;
		aixPath.Format(L"%s\\%s.txt", path.Mid(0, lastInt), newFileName);


		AcDbObjectIdArray oIds;

		GetSel(oIds);
		ads_point pt1;

		AcGePoint3d fPt;

		if (acedGetPoint(NULL, L"轴线第一个点：\n", pt1) != RTNORM) {
			return;
		}


		fPt = asPnt3d(pt1);

		
		for (int i = 0; i < oIds.length(); i++)
		{
			AcDbEntity * l = NULL;

			if (acdbOpenObject(l, oIds[i], AcDb::kForRead) == Acad::eOk) {

				if (l->isA() == AcDbBlockReference::desc()) {

					l->close();

					AfxMessageBox(L"所选集合中包含了块");
					return;
					//continue;
				}
			}
		}


		CMoveJig jig(fPt);

		jig.doIt(oIds, true);


		if (jig.m_idsC.length() == 0) {
			return;
		}

	CString strHdl;
	ACHAR *buffer;

	for (int i = 0; i < oIds.length(); i++)
		{
			AcDbEntity * l = NULL;

			if (acdbOpenObject(l, oIds[i], AcDb::kForWrite) == Acad::eOk) {

				CString appName = "XDataDCZ";

				resbuf * rb = l->xData(appName);

				if (rb == NULL) {

					acdbRegApp(appName);

				}
				else {
					rb = NULL;
					l->setXData(rb);
				}

				rb = acutBuildList(AcDb::kDxfRegAppName, appName,
					AcDb::kDxfXdInteger16, 1,
					AcDb::kDxfXdWorldXCoord, pt1,
					AcDb::kDxfXdWorldXCoord, asDblArray(jig.m_ToPoint),
					RTNONE);

				l->setXData(rb);

				acutRelRb(rb);
				l->close();

			}
		
			AcDbHandle h=oIds[i].handle();

			buffer=new ACHAR[5];

			h.getIntoAsciiBuffer(buffer);
			strHdl+=buffer;
			delete []buffer;

			buffer=NULL;
		}
		
	CString strFpt;
	strFpt.Format(L"%f,%f,%f",fPt.x,fPt.y,fPt.z);

	CString strEpt;
	strEpt.Format(L"%f,%f,%f",jig.m_ToPoint.x,jig.m_ToPoint.y,jig.m_ToPoint.z);

	CString wStr;

	wStr.Format(L"%s|%s|%s",strHdl,strFpt,strEpt);

	CFile f;

	if(PathFileExists(aixPath)==FALSE){
		if(f.Open(aixPath, CFile::modeCreate | CFile::modeWrite)!=0){

			f.Write(wStr,wStr.GetLength()*sizeof(wchar_t));

			f.Close();
		}
	}
	else{

		CString strE;
		if (f.Open(aixPath, CFile::modeReadWrite, NULL)) {
			char buffer1[256];
			if (f.Read(buffer1, 2) == 2)
			{
				switch (buffer1[0])
				{
				case 0xBBEF:
					strE=ReadUTF8(f);
					return;
				case 0xFFFE:
					strE=ReadUnicode(f);
					return;
				}
			}
			strE=ReadAnsi(f);
		}
		else {
			AfxMessageBox(_T("打开记录轴的文件失败！"));
			f.Close();
			return;
			
		}
	f.Close();
		
	wStr+=L"\r\n"+strE;
	
		AfxMessageBox(strE);
	

		if(f.Open(aixPath,CFile::modeNoTruncate|CFile::modeWrite)!=0){

			f.SeekToBegin();

			f.Write(wStr,wStr.GetLength()*sizeof(wchar_t));

			f.Close();

		}
	}

		
	}
	static void ECDMyGroupEcdCC(){
	
		CString path = acDocManager->curDocument()->fileName();

		int lastInt = path.ReverseFind('\\');


		CString fileName = path.Mid(lastInt + 1);
		int lastDot = fileName.ReverseFind('.');

		CString newFileName;
		newFileName.Format(L"%s_M", fileName.Mid(0, lastDot));

		CString  newPath;
		newPath.Format(L"%s\\%s.dwg", path.Mid(0, lastInt), newFileName);

		AcDbObjectIdArray  ids;
		
		GetSel(ids);

		if(ids.length()==0){
			return;
		}

		if(DeepClone(ids,newPath))
		{
		for (int i = 0; i > ids.length(); i++)
		{
			AcDbEntity * l = NULL;

			if (acdbOpenObject(l, ids[i], AcDb::kForWrite) == Acad::eOk) {

				l->erase();
				l->close();
				l = NULL;
			}
		}
		}
		else{
			AfxMessageBox(L"复制时出错，请重新操作。");
		}
	}
	static void   GetSel( AcDbObjectIdArray & ids) {
		ads_name aName;
		
		if (acedSSGet(NULL, NULL, NULL, NULL, aName) != RTNORM) {
			return ;
		}

		long len = 0;
		acedSSLength(aName, &len);

		for (int i = 0; i < len; i++)
		{
			ads_name temp;

			acedSSName(aName, i, temp);

			AcDbObjectId tempId;
			acdbGetObjectId(tempId, temp);

			ids.append(tempId);

		}
		acedSSFree(aName);
		
	}

	static  bool DeepClone(AcDbObjectIdArray & ids, CString path)
	{
		ErrorStatus es = ErrorStatus::eOk;
		

		AcDbObjectId  modelSpaceId;


		AcDbBlockTable *pBT1 = NULL;


		AcDbDatabase* pTempDb = new AcDbDatabase();

		AcDbIdMapping idMap;
		es = idMap.setDestDb(pTempDb);

		if (!CopyTextStyleIdInfo(acdbHostApplicationServices()->workingDatabase(), pTempDb)) {

			return false;
		}

		pTempDb->getSymbolTable(pBT1, AcDb::kForRead);
		pBT1->getAt(ACDB_MODEL_SPACE, modelSpaceId);
		
		pBT1->close();

		es = pTempDb->wblockCloneObjects(ids, modelSpaceId, idMap, AcDb::kDrcIgnore);
		
		

		if (es != ErrorStatus::eOk) {
			delete pTempDb;
			pTempDb=NULL;
			return false;
		}
		es = pTempDb->saveAs(path);
		if (es != ErrorStatus::eOk) {
			delete pTempDb;
			pTempDb=NULL;
			return false;
		}
		delete pTempDb;
		pTempDb=NULL;
		return true;




	}

	static	bool CopyTextStyleIdInfo(AcDbDatabase *pFromDataSrc/*in*/, AcDbDatabase *pToDataDes/*in*/)
	{

		if (pFromDataSrc == NULL || pToDataDes == NULL)
			return false;

		AcDbTextStyleTable *pStyleTable = NULL, *pNewSt = NULL;
		Acad::ErrorStatus es = Acad::eOk;
		es = pFromDataSrc->getSymbolTable(pStyleTable, AcDb::kForRead);
		if (es != Acad::eOk)
			return false;

		es = pToDataDes->getSymbolTable(pNewSt, AcDb::kForWrite);
		if (es != Acad::eOk)
			return false;




		AcDbTextStyleTableIterator *pIterator = NULL;
		es = pStyleTable->newIterator(pIterator);
		if (es != Acad::eOk)
		{
			pStyleTable->close();
			pStyleTable = NULL;
			return false;
		}

		AcDbObjectId stdId, anoId;
		pStyleTable->getAt(L"Standard", stdId);
		pStyleTable->getAt(L"Annotative", anoId);

		for (pIterator->start(); !pIterator->done(); pIterator->step())
		{
			AcDbObjectId styleId = AcDbObjectId::kNull;
			if ((es = pIterator->getRecordId(styleId)) == Acad::eOk)
			{
				AcDbTextStyleTableRecord* pNewRec = NULL;
				if (styleId == stdId) {

					pNewSt->getAt(L"Standard", pNewRec, AcDb::kForWrite);

				}
				else if (styleId == anoId) {
					pNewSt->getAt(L"Annotative", pNewRec, AcDb::kForWrite);
				}
				else {
					pNewRec = new AcDbTextStyleTableRecord;
				}

				AcGiTextStyle *pTextStyle = new AcGiTextStyle(pToDataDes);
				if ((es = fromAcDbTextStyle(*pTextStyle, styleId)) == Acad::eOk)
				{
					ACHAR * pTypeface = NULL;
					Adesk::Boolean bold;
					Adesk::Boolean italic;
					int  charset;
					int  pitchAndFamily;
					//Autodesk::AutoCAD::PAL::FontUtils::FontFamily fontFamily;



					if (styleId != stdId&&styleId != anoId) {
						setSymbolName(pNewRec, pTextStyle->styleName());
						pNewRec->setFileName(pTextStyle->fileName());
						pNewRec->setBigFontFileName(_T(""));
					}


					/*待完善*/

					es = pTextStyle->font(pTypeface, bold, italic, charset, pitchAndFamily);
					if (es == Acad::eOk)
						pNewRec->setFont(pTypeface, bold, italic, charset, pitchAndFamily);

					// must explicitly set to ""
					if (styleId != stdId&&styleId != anoId)
					{
						pNewRec->setTextSize(pTextStyle->textSize());

						pNewRec->setXScale(pTextStyle->xScale());
					}
					pNewRec->setObliquingAngle(pTextStyle->obliquingAngle());

					if (styleId == stdId || styleId == anoId) {
						pNewRec->close();

					}
					else {
						addToSymbolTableAndClose(pNewRec, pToDataDes);
					}
				}
				if (pTextStyle != NULL)
				{
					delete pTextStyle;
					pTextStyle = NULL;
				}
			}
		}
		pNewSt->close();
		if (pIterator != NULL)
		{
			delete pIterator;
			pIterator = NULL;
			pStyleTable->close();
			pStyleTable = NULL;
		}
		return true;
	}

	static BOOL setSymbolName(AcDbSymbolTableRecord* newRec, LPCTSTR newName)
	{
		Acad::ErrorStatus es;
		es = newRec->setName(newName);
		if (es != Acad::eOk) {
			newRec->close();
		}
		return(es);
	}
	//添加文字样式块表记录
	static BOOL addToSymbolTableAndClose(AcDbSymbolTableRecord* systemTextRec/*in*/, AcDbDatabase *pDataBase/*in*/)
	{
		if (pDataBase == NULL || systemTextRec == NULL)
			return FALSE;

		AcDbTextStyleTable* symTextTbl = NULL;
		Acad::ErrorStatus es = Acad::eOk;
		es = pDataBase->getTextStyleTable(symTextTbl, AcDb::kForWrite);
		if (es != Acad::eOk)
		{
			if (systemTextRec != NULL)
			{
				delete systemTextRec;
				systemTextRec = NULL;
			}
			return FALSE;
		}
		AcDbSymbolTable *pSysRec = AcDbTextStyleTable::cast(symTextTbl);
		//覆盖字体样式
		es = pSysRec->add(systemTextRec);
		if (es != Acad::eOk)
		{
			symTextTbl->close();
			systemTextRec->close();
			return FALSE;
		}
		else
		{
			systemTextRec->close();
			symTextTbl->close();
		}
		return TRUE;
	}

	static AcDbObjectId PostToModelSpace(AcDbEntity *pEnt, AcDbDatabase *pDb = acdbHostApplicationServices()->workingDatabase()) {

		if (pEnt == NULL) {
			return AcDbObjectId::kNull;
		}
		AcDbBlockTable *pTable;

		AcDbBlockTableRecord *blkTblRec;

		AcDbObjectId objId;

		pDb->getBlockTable(pTable, AcDb::OpenMode::kForRead);
		pTable->getAt(ACDB_MODEL_SPACE, blkTblRec, AcDb::OpenMode::kForWrite);

		pTable->close();

		Acad::ErrorStatus status = blkTblRec->appendAcDbEntity(objId, pEnt);

		if (status != Acad::eOk) {

			blkTblRec->close();

			delete pEnt;
			pEnt = NULL;

			return AcDbObjectId::kNull;

		}

		blkTblRec->close();
		pEnt->close();

		return objId;
	}

	static CString ReadAnsi(CFile& file)//读取方式1：ANSI //传对象传引用
	{
		file.Seek(0, CFile::begin);//回到头开始
		char buff[1024];
		UINT nRet = 0;//三种文件：ANSI:窄字符集 Unicode:宽字符集也叫UTF-16 UTF-8:窄字符集：属于Unicode 
		CString str;
		while (nRet = file.Read(buff, sizeof(buff) - 1))//nRet != 0
		{
			buff[nRet] = _T('\0');
			str += buff;
		}
		return str;
	}

	static CString ReadUTF8(CFile& file)//UTF-8文件编码读取
	{
		AfxMessageBox(L"UTF8");

		file.Seek(3, CFile::begin);//向后移动三个字节
		LONGLONG nLen = file.GetLength();//64位操作系统
		char* p = new char[nLen + 1];
		nLen = file.Read(p, nLen);
		p[nLen] = '\0';
		TCHAR* pText = new TCHAR[nLen / 2 + 2];//防止不够 + 2
		nLen = MultiByteToWideChar(CP_UTF8, NULL, p, -1, pText, nLen / 2 + 2);//代入p 传出 pText
		CString str(pText);

		
		delete[] p;
		delete[] pText;
		return str;
	}

	static CString ReadUnicode(CFile& file)//Unicode文件编码获取
	{
		
		AfxMessageBox(L"ReadUnicode");
		file.Seek(2, CFile::begin);//向后移动三个字节
		LONGLONG nLen = file.GetLength();//64位操作系统
		TCHAR* pText = new TCHAR[nLen / 2 + 1];//防止不够 + 1
		nLen = file.Read(pText, nLen);
		pText[nLen / 2] = _T('\0');
		//SetDlgItemText(IDC_EDIT1, pText);

		CString str(pText);
		delete[] pText;
		return str;
	}

};

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CEcdMirrorApp)

ACED_ARXCOMMAND_ENTRY_AUTO(CEcdMirrorApp, ECDMyGroup, EcdMR, EcdMR, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CEcdMirrorApp, ECDMyGroup, EcdRR, EcdRR, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CEcdMirrorApp, ECDMyGroup, EcdUpMR, EcdUpMR, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CEcdMirrorApp, ECDMyGroup, EcdCC, EcdCC, ACRX_CMD_MODAL, NULL)
