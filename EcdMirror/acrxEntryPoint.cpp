#include "StdAfx.h"
#include "resource.h"
#include "acdocman.h"
#include"acedads.h"
#include "acedCmdNF.h"
#include<vector>
#include"MoveJig.h"
#include <shlwapi.h>
#include"StringUtil.h"
#include "algorithm" //sort函数、交并补函数
#include "iterator" //求交并补使用到的迭代器

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

	/*static void ECDMyGroupEcdRR()
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

	}*/

	static void  ECDMyGroupEcdUpMR() {

		CString path = acDocManager->curDocument()->fileName();

		int lastInt = path.ReverseFind('\\');


		CString fileName = path.Mid(lastInt + 1);
		int lastDot = fileName.ReverseFind('.');

		CString newFileName;
		newFileName.Format(L"%s_M", fileName.Mid(0, lastDot));

		CString copyFileName;
		copyFileName.Format(L"%s_M_C", fileName.Mid(0, lastDot));

		CString  newPath;
		newPath.Format(L"%s\\%s.dwg", path.Mid(0, lastInt), newFileName);

		CString aixPath;
		aixPath.Format(L"%s\\%s.txt", path.Mid(0, lastInt), newFileName);

		CString  cPath;
		cPath.Format(L"%s\\%s.dwg", path.Mid(0, lastInt), copyFileName);


		AcDbObjectIdArray oIds;
		GetSel(oIds);

		for (int i = 0; i < oIds.length(); i++)
		{
			AcDbEntity * l = NULL;

			if (acdbOpenObject(l, oIds[i], AcDb::kForRead) == Acad::eOk) {

				if (l->isA() == AcDbBlockReference::desc()) {

					l->close();

					acutPrintf(L"所选集合不能有块\n");

					return;
				}
			}
		}

		vector<CString>vecHex;
		for (int i = 0; i < oIds.length(); i++){
		
			AcDbHandle h=oIds[i].handle();

			Adesk::UInt32 high=h.high();
			Adesk::UInt32 low=h.low();

			CString strNum;
			strNum.Format(L"%d%d",high,low);

			int num=_ttoi(strNum);

			strNum="";
			strNum.Format(L"%X",num);

			vecHex.push_back(strNum);
		}

	


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
			vector<CString>vecStr;

			CFile file;
			CFileException pError;
			if(file.Open(aixPath,CFile::modeReadWrite,&pError)!=0){

				CString allStr=ReadUnicode(file);
				
				CStringUtil::Split(allStr,L"\r\n",vecStr,false);
				
			}else{
				//AfxMessageBox(pError.m_cause);
				acutPrintf(L"不存在对称轴数据文件\n");
				file.Close();
				return;
			}
			file.Close();
			
			if(vecStr.size()==0){
				acutPrintf(L"称轴数据文件沒有记录\n");
				return;

			}

			CString find;
			for (int i=0;i<(int)vecStr.size();i++)
			{
				CString strOne=vecStr[i];

				vector<CString>vecO;

				CStringUtil::Split(strOne,L"&",vecO,false);

				vector<CString> v;
				sort(vecO.begin(),vecO.end());   
				sort(vecHex.begin(),vecHex.end());   
				set_intersection(vecO.begin(),vecO.end(),vecHex.begin(),vecHex.end(),back_inserter(v));

				SIZE_T max=(int)vecO.size()-1>vecHex.size()?vecHex.size():vecO.size()-1;

				int stand=(int)max/2;

				if(stand<1)
					stand=1;

				if(v.size()>=stand){

					find=strOne;
					break;

				}

			}

			if(find.IsEmpty()==true){

				acutPrintf(L"称轴数据文件中不存在对应的记录\n");
				return;

			}

			vector<CString>vecF;
			vector<CString>vecPt;
			CStringUtil::Split(find,L"|",vecF,false);

			CString s1=vecF[1];
			CString s2=vecF[2];

			CStringUtil::Split(s1,L",",vecPt,false);

			formPt.x=_wtof(vecPt[0]);
			formPt.y=_wtof(vecPt[1]);
			formPt.z=_wtof(vecPt[2]);
		
			vecPt.clear();


			CStringUtil::Split(s2,L",",vecPt,false);

			toPt.x=_wtof(vecPt[0]);
			toPt.y=_wtof(vecPt[1]);
			toPt.z=_wtof(vecPt[2]);

		}

		CMoveJig jig(formPt, toPt);

		jig.UpdateDoIt(oIds, true);


		CopyFile(newPath,cPath,FALSE);


		if(DeepClone(jig.m_idsC,newPath)){

			DeleteFile(cPath);
			acutPrintf(L"更新成功");

		}
		else{

			DeleteFile(newPath);
			CopyFile(cPath,newPath,FALSE);
			acutPrintf(L"更新文件不成功！");

		}

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

		

		CString copyFileName;
		copyFileName.Format(L"%s_M_C", fileName.Mid(0, lastDot));

		CString aixPath;
		aixPath.Format(L"%s\\%s.txt", path.Mid(0, lastInt), newFileName);

		CString  cPath;
		cPath.Format(L"%s\\%s.dwg", path.Mid(0, lastInt), copyFileName);

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

			Adesk::UInt32 high=h.high();
			Adesk::UInt32 low=h.low();

			CString strNum;
			strNum.Format(L"%d%d",high,low);

			int num=_ttoi(strNum);

			strNum="";
			strNum.Format(L"%X",num);

			strHdl+=strNum+"&";


			
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

			f.SeekToBegin();

			char ww[2] ={0xFF,0xFE};

			f.Write(ww,2);

			f.Seek(2,CFile::begin);

			f.Write(wStr,wStr.GetLength()*sizeof(wchar_t));
			f.Flush();
			f.Close();
		}
	}
	else{

		CString strE;
		if (f.Open(aixPath, CFile::modeReadWrite, NULL)) {
			
			strE=ReadUnicode(f);
			f.Close();
				
		}
		else {
			AfxMessageBox(_T("打开记录轴的文件失败！"));
			f.Close();
			return;
			
		}
	
		
	wStr+=L"\r\n"+strE;
	
		
	

		if(f.Open(aixPath,CFile::modeNoTruncate|CFile::modeWrite)!=0){

			f.SeekToBegin();

			char ww[2] ={0xFF,0xFE};

			f.Write(ww,2);

			f.Seek(2,CFile::begin);

			f.Write(wStr,wStr.GetLength()*sizeof(wchar_t));
			f.Flush();
			f.Close();

		}
	}
	DeepClone(jig.m_idsC,newPath);
		
	}
	/*static void ECDMyGroupEcdCC(){
	
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
	}*/
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

	static CString ReadUnicode(CFile& file)//Unicode文件编码获取
	{
		
		file.Seek(2, CFile::begin);//向后移动三个字节
		LONGLONG nLen = file.GetLength();//64位操作系统
		TCHAR* pText = new TCHAR[nLen / 2 + 1];//防止不够 + 1
		nLen = file.Read(pText, nLen);
		pText[nLen / 2] = _T('\0');
		
		CString str(pText);
		delete[] pText;
		
		return str;
	}

};

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CEcdMirrorApp)

ACED_ARXCOMMAND_ENTRY_AUTO(CEcdMirrorApp, ECDMyGroup, EcdMR, EcdMR, ACRX_CMD_MODAL, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CEcdMirrorApp, ECDMyGroup, EcdRR, EcdRR, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CEcdMirrorApp, ECDMyGroup, EcdUpMR, EcdUpMR, ACRX_CMD_MODAL, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CEcdMirrorApp, ECDMyGroup, EcdCC, EcdCC, ACRX_CMD_MODAL, NULL)
