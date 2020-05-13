#include "StdAfx.h"
#include "resource.h"
#include "acdocman.h"
#include"acedads.h"
#include "acedCmdNF.h"
#include<vector>
#include"MoveJig.h"
//#include <shlwapi.h>
#include"StringUtil.h"
#include "algorithm" //sort����������������
#include "iterator" //�󽻲���ʹ�õ��ĵ�����

//#pragma comment(lib,"Shlwapi.lib")

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

	static void  ECDMyGroupMR() {

		CString path = acDocManager->curDocument()->fileName();

		int lastInt = path.ReverseFind('\\');
		ErrorStatus es2=ErrorStatus::eOk;

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


		vector<AcDbEntity*>vecEnt;

		AcDbObjectIdArray oIds;
		//GetSel(oIds);
		AcDbBlockTable *pBlkTbl;
		es2=acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlkTbl, AcDb::kForRead);
		
		//��ȡ����¼
		AcDbBlockTableRecord *pBlkRcd;
		es2=pBlkTbl->getAt(ACDB_MODEL_SPACE,pBlkRcd,AcDb::kForWrite);
		pBlkTbl->close();

		

		//����������׼������ѭ��
		AcDbBlockTableRecordIterator *pBlkTblRcdltr;
		es2=pBlkRcd->newIterator(pBlkTblRcdltr);
		
	
		//����ʵ��ָ��
		AcDbEntity *pEnt=NULL;
		//�ڶ���Ŀ���¼�н��е���
		

		for (pBlkTblRcdltr->start();!pBlkTblRcdltr->done();pBlkTblRcdltr->step())
		{
			es2=pBlkTblRcdltr->getEntity(pEnt,AcDb::kForWrite);
			
			AcDbObjectId tempId;
			pBlkTblRcdltr->getEntityId(tempId);
			
			if(es2!=ErrorStatus::eOk){
				continue;
				es2=pEnt->upgradeOpen();
				if(es2==ErrorStatus::eOk){
				pEnt->erase();
				pEnt->close();
				}else{
					CString text;
					text.Format(L"es2=%d",es2);

					AfxMessageBox(text);
				}
				continue;
			}
			//��ȡʵ��
			AcDbEntity * cL=AcDbEntity::cast(pEnt->clone());

			vecEnt.push_back(cL);

			oIds.append(pEnt->objectId());

			pEnt->close();

		}
		//�رտ��
		pBlkRcd->close();
		

		//ɾ������¼������
		delete pBlkTblRcdltr;

		/*CString test;

		test.Format(L"Oids=%d,vecEnts=%d",oIds.length(),vecEnt.size());

		AfxMessageBox(test);*/




		AcGePoint3d formPt, toPt;
		bool findPt = false;
		

		if (!findPt) {
			
			CString allStr;
			CFile file;
			CFileException pError;
			if(file.Open(aixPath,CFile::modeReadWrite,&pError)!=0){

				allStr=ReadUnicode(file);
			}else
			{
				acutPrintf(L"�����ڶԳ��������ļ�\n");
				file.Close();
				return;
			}
			file.Close();
			
			if(allStr.IsEmpty()){
				acutPrintf(L"�Գ��������ļ��]�м�¼\n");
				return;

			}

			vector<CString>vecF;
			vector<CString>vecPt;
			//AfxMessageBox(allStr);
			CStringUtil::Split(allStr,L"|",vecF,false);

			CString s1=vecF[0];
			CString s2=vecF[1];

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


		//CopyFile(newPath,cPath,FALSE);

		

//ɾ��ԭʵ��
		for (int i=0;i<oIds.length();i++)
		{
			AcDbEntity * l = NULL;

			if (acdbOpenObject(l, oIds[i], AcDb::kForWrite) == Acad::eOk) {

				l->erase();

				l->close();
				l=NULL;

			}

		}

		if(DeepClone(jig.m_idsC,newPath)){
			ErrorStatus es =ErrorStatus::eOk;
			AcDbLine * aixLine=new AcDbLine(formPt,jig.m_ToPoint);

			
			acutPrintf(L"���³ɹ�");
			AcDbExtents ext;
			AcGePoint3d ptO;
			for (int i=0;i<(int)vecEnt.size();i++){

				AcDbEntity * ent1=vecEnt[i];

				es=ent1->getGeomExtents(ext);
				if(es==ErrorStatus::eOk){
					ptO=ext.maxPoint();
					break;
				}
			}

			AcGePoint3d ptOnL;

			aixLine->getClosestPointTo(ptO,ptOnL,Adesk::kTrue);

			AcGeVector3d vec=(ptO-ptOnL).normal();


			DeleteOnDesDb(vec,newPath,aixLine);

		}
		else{

			//DeleteFile(newPath);
			//CopyFile(cPath,newPath,FALSE);
			acutPrintf(L"�����ļ����ɹ���");

		}

		for (int i=0;i<jig.m_idsC.length();i++)
		{
			AcDbEntity * l = NULL;

			if (acdbOpenObject(l, jig.m_idsC[i], AcDb::kForWrite) == Acad::eOk) {

				l->erase();

				l->close();
				l=NULL;

			}

		}

		for (int i=0;i<(int)vecEnt.size();i++)
		{

			PostToModelSpace(vecEnt[i]);
			vecEnt[i]->close();
		}
	}

	static void ECDMyGroupMM() {
		ErrorStatus es =ErrorStatus::eOk;
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

		if (acedGetPoint(NULL, L"���ߵ�һ���㣺\n", pt1) != RTNORM) {
			return;
		}


		fPt = asPnt3d(pt1);

		vector<AcDbEntity*>vecEnt;

		CMoveJig jig(fPt);

		jig.doIt(oIds, true);

		if (jig.m_idsC.length() == 0) {
			return;
		}
		CString tt;
		
		/*tt.Format(L"m_idsC=%d",jig.m_idsC.length());
		AfxMessageBox(tt);*/


	//CString strHdl;

//дxData
	//for (int i = 0; i < oIds.length(); i++)
	//	{
	//		AcDbEntity * l = NULL;

	//		if (acdbOpenObject(l, oIds[i], AcDb::kForWrite) == Acad::eOk) {

	//			CString appName = "XDataDCZ";

	//			resbuf * rb = l->xData(appName);

	//			if (rb == NULL) {

	//				acdbRegApp(appName);

	//			}
	//			else {
	//				rb = NULL;
	//				l->setXData(rb);
	//			}

	//			rb = acutBuildList(AcDb::kDxfRegAppName, appName,
	//				AcDb::kDxfXdInteger16, 1,
	//				AcDb::kDxfXdWorldXCoord, pt1,
	//				AcDb::kDxfXdWorldXCoord, asDblArray(jig.m_ToPoint),
	//				RTNONE);

	//			l->setXData(rb);

	//			acutRelRb(rb);
	//			l->close();

	//		}
	//	
	//		/*AcDbHandle h=oIds[i].handle();

	//		Adesk::UInt32 high=h.high();
	//		Adesk::UInt32 low=h.low();

	//		CString strNum;
	//		strNum.Format(L"%d%d",high,low);

	//		int num=_ttoi(strNum);

	//		strNum="";
	//		strNum.Format(L"%X",num);

	//		strHdl+=strNum+"&";*/


	//		
	//	}
//��¡ԭʵ��		
	for (int i = 0; i < oIds.length(); i++)
	{
		AcDbEntity * l = NULL;

		if (acdbOpenObject(l, oIds[i], AcDb::kForWrite) == Acad::eOk) {

			AcDbEntity * cL=AcDbEntity::cast(l->clone());

			vecEnt.push_back(cL);
			l->close();
		}
	}
//ԭʵ��ɾ��
	for (int i=0;i<oIds.length();i++)
	{
		AcDbEntity * l = NULL;

		if (acdbOpenObject(l, oIds[i], AcDb::kForWrite) == Acad::eOk) {

			l->erase();

			l->close();
			l=NULL;

		}

	}

	CString strFpt;
	strFpt.Format(L"%f,%f,%f",fPt.x,fPt.y,fPt.z);

	CString strEpt;
	strEpt.Format(L"%f,%f,%f",jig.m_ToPoint.x,jig.m_ToPoint.y,jig.m_ToPoint.z);

	CString wStr;

	wStr.Format(L"%s|%s",strFpt,strEpt);

	CFile f;

	if(true){
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
		if (f.Open(aixPath, CFile::modeReadWrite, NULL)!=0) {
			
			strE=ReadUnicode(f);
			f.Close();
				
		}
		else {
			AfxMessageBox(_T("�򿪼�¼����ļ�ʧ�ܣ�"));
			f.Close();
			return;
			
		}
	
		
	wStr+=L"\r\n"+strE;
	//д��������
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
	
	AcDbLine * aixLine=new AcDbLine(fPt,jig.m_ToPoint);
	
	AcDbExtents ext;
	AcGePoint3d ptO;
	bool hasPt=false;
	for (int i=0;i<(int)vecEnt.size();i++){

		AcDbEntity * ent1=vecEnt[i];
		
		es=ent1->getGeomExtents(ext);
		if(es==ErrorStatus::eOk){
			ptO=ext.maxPoint();
			hasPt=true;
			break;
		}
	}
	
	if(hasPt){

		AcGePoint3d ptOnL;

		aixLine->getClosestPointTo(ptO,ptOnL,Adesk::kTrue);

		AcGeVector3d vec=(ptO-ptOnL).normal();

		DeleteOnDesDb(vec,newPath,aixLine);
	}else{
		delete aixLine;
		aixLine=NULL;
	}


	

//����ʵ��ɾ��
	for (int i=0;i<jig.m_idsC.length();i++)
	{
		AcDbEntity * l = NULL;

		if (acdbOpenObject(l, jig.m_idsC[i], AcDb::kForWrite) == Acad::eOk) {

			l->erase();

			l->close();
			l=NULL;

		}

	}

	for (int i=0;i<(int)vecEnt.size();i++)
	{

		PostToModelSpace(vecEnt[i]);

		//vecEnt[i]->close();

	}

}

	static void DeleteOnDesDb(const AcGeVector3d&vec,CString path,AcDbLine*aixLine){
		ErrorStatus es=ErrorStatus::eOk;
		AcDbDatabase *pDb=new AcDbDatabase(Adesk::kFalse);
		pDb->readDwgFile(path,AcDbDatabase::OpenMode::kForReadAndWriteNoShare);
		//��ȡ���
		AcDbBlockTable *pBlkTbl;
		pDb->getSymbolTable(pBlkTbl,AcDb::kForRead);
		//��ȡ����¼
		AcDbBlockTableRecord *pBlkRcd;
		pBlkTbl->getAt(ACDB_MODEL_SPACE,pBlkRcd,AcDb::kForWrite);
		pBlkTbl->close();
		//����������׼������ѭ��
		AcDbBlockTableRecordIterator *pBlkTblRcdltr;
		pBlkRcd->newIterator(pBlkTblRcdltr);
		//����ʵ��ָ��
		AcDbEntity *pEnt=NULL;
		//�ڶ���Ŀ���¼�н��е���
		AcDbExtents ext;

		for (pBlkTblRcdltr->start();!pBlkTblRcdltr->done();pBlkTblRcdltr->step())
		{
			

			//��ȡʵ��
			pBlkTblRcdltr->getEntity(pEnt,AcDb::kForWrite);
			AcGePoint3d ptO;
			es=pEnt->getGeomExtents(ext);
			if(es!=ErrorStatus::eOk){
				 pEnt->close();
				continue;
			}
			ptO=ext.maxPoint();

			AcGePoint3d ptOnL;

			aixLine->getClosestPointTo(ptO,ptOnL,Adesk::kTrue);

			AcGeVector3d vec2=(ptO-ptOnL).normal();

			if(vec2.x*vec.x>=0&&vec2.y*vec.y>=0){

				pEnt->erase();

				pEnt->close();
				

			}
			else{
				pEnt->close();
				
			}
			
		}
		//�رտ��
		pBlkRcd->close();
		es=pDb->saveAs(path);

		
		//ɾ������¼������
		delete pBlkTblRcdltr;
		//ɾ��ͼ��
		delete pDb;

		delete aixLine;

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
		
		es=pTempDb->setLtscale(acdbHostApplicationServices()->workingDatabase()->ltscale());
		es=pTempDb->setLineWeightDisplay(acdbHostApplicationServices()->workingDatabase()->lineWeightDisplay());
		es=pTempDb->setFillmode(acdbHostApplicationServices()->workingDatabase()->fillmode());
		
		AcDbIdMapping idMap;
		es = idMap.setDestDb(pTempDb);
		
		if (!CopyTextStyleIdInfo(acdbHostApplicationServices()->workingDatabase(), pTempDb)) {
			AfxMessageBox(L"CopyTextStyleIdInfo false");
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
		
		AcDbTextStyleTable *pStyleTable = NULL;
		AcDbTextStyleTable *pNewSt = NULL;
		Acad::ErrorStatus es = Acad::eOk;
		es = pFromDataSrc->getSymbolTable(pStyleTable, AcDb::kForRead);
		if (es != Acad::eOk)
		{
			CString ttt;
			ttt.Format(L"pStyleTable open false=%d",es);
			AfxMessageBox(ttt);

			return false;
		}
		es = pToDataDes->getSymbolTable(pNewSt, AcDb::kForWrite);
		if (es != Acad::eOk)
		{
			CString ttt;
			ttt.Format(L"pNewSt open false=%d",es);
			AfxMessageBox(ttt);

			pStyleTable->close();
			pStyleTable=NULL;
			return false;

		}



		AcDbTextStyleTableIterator *pIterator = NULL;
		es = pStyleTable->newIterator(pIterator);
		if (es != Acad::eOk)
		{
			CString ttt;
			ttt.Format(L"pStyleTable->newIterator false=%d",es);
			AfxMessageBox(ttt);

			pStyleTable->close();
			pStyleTable = NULL;

			pNewSt->close();
			pNewSt=NULL;
			return false;
		}

		

		AcDbObjectId stdId, anoId;
		pStyleTable->getAt(L"Standard", stdId);
		
		pStyleTable->getAt(L"Annotative", anoId);
		
		AcDbTextStyleTableRecord *txtRec=NULL;
		for (pIterator->start(); !pIterator->done(); pIterator->step())
		{
			AcDbObjectId styleId = AcDbObjectId::kNull;
			
			
				if ((es = pIterator->getRecord(txtRec,AcDb::kForRead)) != Acad::eOk){
					CString ttt;
					ttt.Format(L"pIterator->getRecord(txtRec,AcDb::kForRead))=%d",es);
					AfxMessageBox(ttt);

					if(txtRec!=NULL){
						txtRec->close();
					}
					continue;
				}

			if ((es = pIterator->getRecordId(styleId)) == Acad::eOk)
			{
				AcDbTextStyleTableRecord* pNewRec = NULL;
				if (styleId == stdId) {

					es=pNewSt->getAt(L"Standard", pNewRec, AcDb::kForWrite);
				}
				else if (styleId == anoId) {
					es=pNewSt->getAt(L"Annotative", pNewRec, AcDb::kForWrite);
					
				}
				else {
					pNewRec = new AcDbTextStyleTableRecord;
				}

				//AcGiTextStyle *pTextStyle = new AcGiTextStyle(pToDataDes);
				if (true/*(es = fromAcDbTextStyle(*pTextStyle, styleId)) == Acad::eOk*/)
				{
					ACHAR * pTypeface = NULL;
					Adesk::Boolean bold;
					Adesk::Boolean italic;
					int  charset;
					int  pitchAndFamily;
					//Autodesk::AutoCAD::PAL::FontUtils::FontFamily fontFamily;
					ACHAR *na,*na1;					
					txtRec->getName(na);
					txtRec->fileName(na1);
					if (styleId != stdId&&styleId != anoId) {
						setSymbolName(pNewRec,na );
						es=pNewRec->setFileName(na1);
						es=pNewRec->setBigFontFileName(_T(""));
					}
					    es =   txtRec->font(pTypeface, bold, italic, charset, pitchAndFamily);  
					
					if (es == Acad::eOk)
						es=pNewRec->setFont(pTypeface, bold, italic, charset, pitchAndFamily);
					if (styleId != stdId&&styleId != anoId)
					{
						es=pNewRec->setTextSize(pNewRec->textSize());

						es=pNewRec->setXScale(pNewRec->xScale());
					}
					es=pNewRec->setObliquingAngle(pNewRec->obliquingAngle());

					if (styleId == stdId || styleId == anoId) {
						es=pNewRec->close();
						
					}
					else {
						es=pNewSt->close();
						pNewSt=NULL;
						bool flag=false;
						
						flag=addToSymbolTableAndClose(pNewRec, pToDataDes);
						
						pToDataDes->getSymbolTable(pNewSt, AcDb::kForWrite);
					}
			
				}
				if(txtRec!=NULL){
					es=txtRec->close();

					txtRec=NULL;
					
					
				}
				
			}
		}
		
		if(pNewSt!=NULL){
			es=pNewSt->close();
			pNewSt=NULL;
			
		}
		if (pIterator != NULL)
		{
			delete pIterator;
			pIterator = NULL;
			es=pStyleTable->close();

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
	//���������ʽ����¼
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
		//����������ʽ
		es = pSysRec->add(systemTextRec);
		if (es != Acad::eOk)
		{
			symTextTbl->close();
			es=systemTextRec->close();
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

	static CString ReadUnicode(CFile& file)//Unicode�ļ������ȡ
	{
		
		file.Seek(2, CFile::begin);//����ƶ������ֽ�
		LONGLONG nLen = file.GetLength();//64λ����ϵͳ
		TCHAR* pText = new TCHAR[nLen / 2 + 1];//��ֹ���� + 1
		nLen = file.Read(pText, nLen);
		pText[nLen / 2] = _T('\0');
		
		CString str(pText);
		delete[] pText;
		
		return str;
	}

};

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CEcdMirrorApp)

ACED_ARXCOMMAND_ENTRY_AUTO(CEcdMirrorApp, ECDMyGroup, MR, MR, ACRX_CMD_MODAL, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CEcdMirrorApp, ECDMyGroup, EcdRR, EcdRR, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CEcdMirrorApp, ECDMyGroup, MM, MM, ACRX_CMD_MODAL, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CEcdMirrorApp, ECDMyGroup, EcdCC, EcdCC, ACRX_CMD_MODAL, NULL)
