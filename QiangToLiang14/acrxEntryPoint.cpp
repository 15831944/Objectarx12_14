

//-----------------------------------------------------------------------------
//----- acrxEntryPoint.cpp
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include<vector>
#include<algorithm>
#include <iostream>
#include <fstream>
#include<istream>
#include <cassert>
#include <string> 
#define PI 3.1415926
using namespace std;
//-----------------------------------------------------------------------------
#define szRDS _RXST("ECD")
int queKou = 201;
int queKou2 = 301;
int quekou3 = 351;
//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CQiangToLiangApp : public AcRxArxApp {

public:
	CQiangToLiangApp () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		
		// TODO: Add your initialization code here
		
		ifstream infile; 
		infile.open(L"自定义插件命令1.txt");   //将文件流对象与文件连接起来 
		
		string s;
		getline(infile,s);
		


		infile.close();             //关闭文件输入流 

		TCHAR strNewCmdName[256];

		if(s.empty()){
			acedRegCmds->addCommand(_T("CQiangToLiangApp"),L"ee",L"ee",ACRX_CMD_TRANSPARENT, ECDQiangToLiangee);
		}
		else{
				#ifdef UNICODE
							_stprintf_s(strNewCmdName, MAX_PATH, _T("%S"), s.c_str());//%S宽字符
				#else
				_stprintf_s(strNewCmdName, MAX_PATH, _T("%s"), s.c_str());//%s单字符
				#endif

				acedRegCmds->addCommand(_T("CQiangToLiangApp"),strNewCmdName,strNewCmdName,ACRX_CMD_TRANSPARENT, ECDQiangToLiangee);

		}


		//TCHAR strNewCmdName[256];
		//if (acedGetString(Adesk::kFalse, _T("\n输入新的命令名称："), strNewCmdName) != RTNORM) 
		//{ 
		//	acedRegCmds->addCommand(_T("CQiangToLiangApp"),L"ee",L"ee",ACRX_CMD_TRANSPARENT, ECDQiangToLiangee);;
		//	
		//}
		//else
		//{
		//	acedRegCmds->addCommand(_T("CQiangToLiangApp"),strNewCmdName,strNewCmdName,ACRX_CMD_TRANSPARENT, ECDQiangToLiangee);
		//}
		/*if (acedGetString(Adesk::kFalse, _T("\n移除命令名："), strNewCmdName) != RTNORM) 
		{ 
			return; 
		}
		acedRegCmds->removeCmd(_T("myNewCmdGroup"),strNewCmdName);*/



		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

		// TODO: Unload dependencies here

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}


	// - ECDQiangToLiang.EcdTest command (do not rename)
	static void ECDQiangToLiangee(void) {


		ErrorStatus es = ErrorStatus::eOk;

		vector<AcDbLine*>vecLines;

		ads_name aName;
		if (acedSSGet(NULL, NULL, NULL, NULL, aName) != RTNORM) {
			return;
		}

		long len = 0;
		acedSSLength(aName, &len);

		AcDbObjectIdArray ids;

		for (int i = 0; i < len; i++)
		{
			ads_name temp;

			acedSSName(aName, i, temp);

			AcDbObjectId tempId;
			acdbGetObjectId(tempId, temp);

			ids.append(tempId);

		}
		acedSSFree(aName);

		for (int i = 0; i < ids.length(); i++)
		{
			AcDbLine * l = NULL;

			if (acdbOpenObject(l, ids[i], AcDb::kForWrite) == Acad::eOk) {

				double len = 0;
				l->getDistAtPoint(l->endPoint(), len);

				if (len <queKou||len<queKou2||len<quekou3) {
					l->erase();
					l->close();
					l = NULL;
				}
				else
				{
					vecLines.push_back(l);
				}
			}
		}
		acutPrintf(L"before=%d", vecLines.size());
		
		AcGePoint3dArray temp1;
		AcGePoint3dArray temp2;
		temp1.append(AcGePoint3d::kOrigin);
		temp2.append(AcGePoint3d::kOrigin);
		temp1.removeAll();
		temp2.removeAll();
		//联通
		for (int ww = 0; ww < (int)vecLines.size(); ww++)
		{
			AcDbLine * l1 = vecLines[ww];
			for (int j = ww + 1; j < (int)vecLines.size(); j++)
			{
				AcDbLine *l2 = vecLines[j];

				l1->intersectWith(l2, AcDb::kOnBothOperands, temp1, 0, 0);

				if (1 == temp1.length()) {


					temp1.removeAll();

				}
				else {
					l1->intersectWith(l2, AcDb::kExtendBoth, temp2, 0, 0);
				
					if (temp2.length() > 0) {
						ExtendLine(l1, temp2[0]);
						ExtendLine(l2, temp2[0]);
						temp2.removeAll();
					}
				}
			}
		}
		
		
		
		//削减
		for (int ww = 0; ww < (int)vecLines.size(); ww++)
		{
			AcDbLine * l1 = vecLines[ww];

			for (int j = ww + 1; j < (int)vecLines.size(); j++)
			{
				AcDbLine *l2 = vecLines[j];
				//l1->intersectWith(l2, AcDb::kExtendBoth, temp2, 0, 0);
				l1->intersectWith(l2, AcDb::kOnBothOperands, temp1, 0, 0);
				
				if (1 == temp1.length()) {
					
					CutLine(l1, temp1[0]);			
					CutLine(l2, temp1[0]);
					temp1.removeAll();
				}
			}
		}
		

		//補充
		temp1.removeAll();
		temp2.removeAll();
		AcGePoint3dArray ptArrAll;
		ptArrAll.append(AcGePoint3d::kOrigin);
		ptArrAll.removeAll();

		for (int i = 0; i < (int)vecLines.size(); i++)
		{
			AcDbLine * l1 = vecLines[i];

			ptArrAll.append(l1->startPoint());
			ptArrAll.append(l1->endPoint());


		}

		//acutPrintf(L"prePt=%d", ptArrAll.length());
		double r = (queKou + queKou2 + quekou3) / 3*2;
		for (int i=0;i<ptArrAll.length();i++)
		{
			AcGePoint3d ptCenter=ptArrAll[i];

			AcDbCircle *cir = new AcDbCircle(ptCenter, AcGeVector3d::kZAxis, r);

			for (int j = i+1; j < ptArrAll.length(); j++) {

				AcGePoint3d pt2 = ptArrAll[j];

				double dis = pt2.distanceTo(ptCenter);

				if (dis <= r) {

					ptArrAll.removeAt(j);

				}

			}
			delete cir;
			cir = NULL;
		}
		//acutPrintf(L"afterPt=%d", ptArrAll.length());

		for (int i = 0; i < ptArrAll.length(); i++)
		{
			AcGePoint3d ptCenter = ptArrAll[i];

			AcDbCircle *cir = new AcDbCircle(ptCenter, AcGeVector3d::kZAxis, r);
			vector<AcDbLine*>vecLL;
			for (int ww = 0; ww < (int)vecLines.size(); ww++)
			{

				AcDbLine * l1 = vecLines[ww];

				l1->intersectWith(cir, AcDb::kOnBothOperands, temp1, 0, 0);

				if (temp1.length() > 0) {

					vecLL.push_back(l1);
					temp1.removeAll();

				}

			}

			delete cir;
			cir = NULL;

			AcGeIntArray intArr;

			for (int m = 0; m < (int)vecLL.size(); m++)
			{
				AcDbLine * lT1 = vecLL[m];

				for (int s = 0; s < (int)vecLL.size(); s++)
				{
					AcDbLine * lT2 = vecLL[s];

					lT1->intersectWith(lT2, AcDb::kOnBothOperands, temp2, 0, 0);

					if (temp2.length() > 0) {

						intArr.append(m);
						intArr.append(s);

						temp2.removeAll();


					}

				}

			}

			for (int m = 0; m < (int)vecLL.size(); m++)
			{
				if (intArr.contains(m))
				{
					continue;
				}

				AcDbLine * lT1 = vecLL[m];

				for (int s = 0; s < (int)vecLL.size() ; s++)
				{
					if (intArr.contains(s)) {

						continue;
					}

					AcDbLine * lT2 = vecLL[s];


					lT1->intersectWith(lT2, AcDb::kExtendBoth, temp2, 0, 0);
					//ptNJdAll.append(temp2[0]);
					if (temp2.length() > 0) {
						AcGePoint3d pt11 = lT1->startPoint();
						AcGePoint3d pt12 = lT1->endPoint();
						double disO = pt11.distanceTo(pt12);

						double dis1 = pt11.distanceTo(temp2[0]);
						double dis2 = pt12.distanceTo(temp2[0]);

						double max1 = (dis2 > dis1 ? dis2 : dis1);

						AcGePoint3d pt21 = lT2->startPoint();
						AcGePoint3d pt22 = lT2->endPoint();
						double disO2 = pt21.distanceTo(pt22);

						double dis21 = pt21.distanceTo(temp2[0]);
						double dis22 = pt22.distanceTo(temp2[0]);

						double max21 = (dis22 > dis21 ? dis22 : dis21);

						if (max1 < disO + 0.5*(queKou + queKou2 + quekou3))
						{
							if (max1 == dis2) {

								lT1->setStartPoint(temp2[0]);
							}
							else {
								lT1->setEndPoint(temp2[0]);
							}
						}
						if (max21 < disO2 + 0.5*(queKou + queKou2 + quekou3)) {
							if (max21 == dis22) {

								lT2->setStartPoint(temp2[0]);
							}
							else {
								lT2->setEndPoint(temp2[0]);
							}
						}

						temp2.removeAll();
					}



				}

			}
			intArr.removeAll();

			vecLL.clear();
		}
		
		for (int i = 0; i < (int)vecLines.size(); i++)
		{

			vecLines[i]->setColorIndex(1);

			vecLines[i]->close();

		}
		
		//补充
	}	
	static void ECDQiangToLiangEcdEE2(void) {
		ErrorStatus es = ErrorStatus::eOk;

		vector<AcDbLine*>vecLines;

		ads_name aName;
		if (acedSSGet(NULL, NULL, NULL, NULL, aName) != RTNORM) {
			return;
		}

		long len = 0;
		acedSSLength(aName, &len);

		AcDbObjectIdArray ids;

		for (int i = 0; i < len; i++)
		{
			ads_name temp;

			acedSSName(aName, i, temp);

			AcDbObjectId tempId;
			acdbGetObjectId(tempId, temp);

			ids.append(tempId);

		}
		acedSSFree(aName);

		for (int i = 0; i < ids.length(); i++)
		{
			AcDbLine * l = NULL;

			if (acdbOpenObject(l, ids[i], AcDb::kForWrite) == Acad::eOk) {

				double len = 0;
				l->getDistAtPoint(l->endPoint(), len);

				if (len <queKou || len<queKou2 || len<quekou3) {
					l->erase();
					l->close();
					l = NULL;
				}
				else
				{
					vecLines.push_back(l);
				}
			}
		}
		//acutPrintf(L"before=%d", vecLines.size());
		AcGePoint3dArray temp1;
		AcGePoint3dArray temp2;
		temp1.append(AcGePoint3d::kOrigin);
		temp2.append(AcGePoint3d::kOrigin);
		temp1.removeAll();
		temp2.removeAll();

		AcGePoint3dArray ptArrAll;
		ptArrAll.append(AcGePoint3d::kOrigin);
		ptArrAll.removeAll();

		for (int i = 0; i < (int)vecLines.size(); i++)
		{
			AcDbLine * l1 = vecLines[i];

			ptArrAll.append(l1->startPoint());
			ptArrAll.append(l1->endPoint());


		}

		//acutPrintf(L"prePt=%d", ptArrAll.length());
		double r = (queKou + queKou2 + quekou3) / 3*2;
		for (int i=0;i<ptArrAll.length();i++)
		{
			AcGePoint3d ptCenter=ptArrAll[i];

			AcDbCircle *cir = new AcDbCircle(ptCenter, AcGeVector3d::kZAxis, r);

			for (int j = i+1; j < ptArrAll.length(); j++) {

				AcGePoint3d pt2 = ptArrAll[j];

				double dis = pt2.distanceTo(ptCenter);

				if (dis <= r) {

					ptArrAll.removeAt(j);

				}

			}
			delete cir;
			cir = NULL;
		}
		//acutPrintf(L"afterPt=%d", ptArrAll.length());

		for (int i = 0; i < ptArrAll.length(); i++)
		{
			AcGePoint3d ptCenter = ptArrAll[i];

			AcDbCircle *cir = new AcDbCircle(ptCenter, AcGeVector3d::kZAxis, r);
			vector<AcDbLine*>vecLL;
			for (int ww = 0; ww < (int)vecLines.size(); ww++)
			{

				AcDbLine * l1 = vecLines[ww];

				l1->intersectWith(cir, AcDb::kOnBothOperands, temp1, 0, 0);

				if (temp1.length() > 0) {

					vecLL.push_back(l1);
					temp1.removeAll();

				}

			}

			delete cir;
			cir = NULL;

			AcGeIntArray intArr;

			for (int m = 0; m < (int)vecLL.size(); m++)
			{
				AcDbLine * lT1 = vecLL[m];

				for (int s = 0; s < (int)vecLL.size(); s++)
				{
					AcDbLine * lT2 = vecLL[s];

					lT1->intersectWith(lT2, AcDb::kOnBothOperands, temp2, 0, 0);

					if (temp2.length() > 0) {

						intArr.append(m);
						intArr.append(s);

						temp2.removeAll();


					}

				}

			}

			for (int m = 0; m < (int)vecLL.size(); m++)
			{
				if (intArr.contains(m))
				{
					continue;
				}

				AcDbLine * lT1 = vecLL[m];

				for (int s = 0; s < (int)vecLL.size() ; s++)
				{
					if (intArr.contains(s)) {

						continue;
					}

					AcDbLine * lT2 = vecLL[s];


					lT1->intersectWith(lT2, AcDb::kExtendBoth, temp2, 0, 0);
					//ptNJdAll.append(temp2[0]);
					if (temp2.length() > 0) {
						AcGePoint3d pt11 = lT1->startPoint();
						AcGePoint3d pt12 = lT1->endPoint();
						double disO = pt11.distanceTo(pt12);

						double dis1 = pt11.distanceTo(temp2[0]);
						double dis2 = pt12.distanceTo(temp2[0]);

						double max1 = (dis2 > dis1 ? dis2 : dis1);

						AcGePoint3d pt21 = lT2->startPoint();
						AcGePoint3d pt22 = lT2->endPoint();
						double disO2 = pt21.distanceTo(pt22);

						double dis21 = pt21.distanceTo(temp2[0]);
						double dis22 = pt22.distanceTo(temp2[0]);

						double max21 = (dis22 > dis21 ? dis22 : dis21);

						if (max1 < disO + 0.5*(queKou + queKou2 + quekou3))
						{
							if (max1 == dis2) {

								lT1->setStartPoint(temp2[0]);
							}
							else {
								lT1->setEndPoint(temp2[0]);
							}
						}
						if (max21 < disO2 + 0.5*(queKou + queKou2 + quekou3)) {
							if (max21 == dis22) {

								lT2->setStartPoint(temp2[0]);
							}
							else {
								lT2->setEndPoint(temp2[0]);
							}
						}

						temp2.removeAll();
					}



				}

			}
			intArr.removeAll();

			vecLL.clear();
		}
		for (int i = 0; i < (int)vecLines.size(); i++)
		{


			vecLines[i]->setColorIndex(1);

			vecLines[i]->close();

		}
	}
	static void CutLine(AcDbLine * l1, const  AcGePoint3d & pt) {
		AcGePoint3d pt11 = l1->startPoint();
		AcGePoint3d pt12 = l1->endPoint();
		double disO = pt11.distanceTo(pt12);

		double dis1 = pt11.distanceTo(pt);
		double dis2 = pt12.distanceTo(pt);

		if (disO > dis1&&disO > dis2) {

			if (dis1 <= (queKou + queKou2 + quekou3)/3&&dis2 <= (queKou + queKou2 + quekou3) / 3) {

				return;
			
			}
			else {

				if (dis1 < dis2) {

					l1->setStartPoint(pt);

				}
				else {
					l1->setEndPoint(pt);
				}

			}

		}

	}
	static void ExtendLine(AcDbLine * l1, const  AcGePoint3d & pt) {
		AcGePoint3d pt11 = l1->startPoint();
		AcGePoint3d pt12 = l1->endPoint();
		double disO = pt11.distanceTo(pt12);

		double dis1 = pt11.distanceTo(pt);
		double dis2 = pt12.distanceTo(pt);

		double max = (dis2 > dis1 ? dis2 : dis1);

		if (max > disO + 1.5*(queKou + queKou2 + quekou3) / 3 || max < disO) {

			return;
		}
		else {

			if (max==dis2) {

				l1->setStartPoint(pt);

			}
			else {
				l1->setEndPoint(pt);
			}

		}
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

	static bool comp(AcGePoint3d pt1,AcGePoint3d pt2){

		if(fabs(pt1.x-pt2.x)<=0.001){
			return pt1.y<pt2.y;
		}
		else{
			return pt1.x<pt2.x;
		}
	}
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CQiangToLiangApp)

	//ACED_ARXCOMMAND_ENTRY_AUTO(CQiangToLiangApp, ECDQiangToLiang, ee, ee, ACRX_CMD_TRANSPARENT, NULL)
