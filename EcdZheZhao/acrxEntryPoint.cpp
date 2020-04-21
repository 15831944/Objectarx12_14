#include "StdAfx.h"
#include "resource.h"
#include"dbwipe.h"
#define  PI 3.1415926
//-----------------------------------------------------------------------------
#define szRDS _RXST("ECD")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CEcdZheZhaoApp : public AcRxArxApp {

public:
	CEcdZheZhaoApp () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		
		// TODO: Add your initialization code here

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
	static void ECDMyGroupEcdCC() {

		ads_point pt;
		int nums;
		double radius;

		if (acedGetPoint(NULL, L"\n圆心：\n", pt) != RTNORM) {
			return;
		}
		if (acedGetReal(L"\n半径：\n", &radius) != RTNORM) {
			return;
		}
		if (acedGetInt(L"\n多边形边数：\n", &nums) != RTNORM) {
			return;
		}

		AcGePoint3d ptc = asPnt3d(pt);

		AcDbObjectId polyId =CreatePolygon(AcGePoint2d(ptc.x, ptc.y), nums, radius, 0, 0);


		AcDbPolyline *pl = NULL;
	
		AcGePoint2dArray ptArr;
		if (acdbOpenObject(pl, polyId, AcDb::OpenMode::kForWrite) == ErrorStatus::eOk) {

			AcGePoint2d ptStart;
			for (int i = 0; i < (int)pl->numVerts(); i++)
			{

				AcGePoint2d pt;

				pl->getPointAt(i, pt);

				ptArr.append(pt);

			}
			pl->getPointAt(0, ptStart);
			ptArr.append(ptStart);
			pl->erase();
			pl->close();
			pl = NULL;
		}
		AcDbWipeout *pWipeout = new AcDbWipeout();


		ErrorStatus es;// = wipeOut->fitPointsToImage(ptArr, minPt, scale);
		pWipeout->setDatabaseDefaults();
		AcGePoint3d originPnt(AcGePoint3d::kOrigin);
		AcGeVector3d Udirection(1, 0, 0);
		AcGeVector3d Vdirection(0, -1, 0);
		pWipeout->setOrientation(originPnt, Udirection, Vdirection);
		pWipeout->setDisplayOpt(AcDbRasterImage::kTransparent, Adesk::kTrue);
		pWipeout->setDisplayOpt(AcDbRasterImage::kShow, Adesk::kTrue);
		es = pWipeout->setClipBoundaryToWholeImage();
		es = pWipeout->setClipBoundary(AcDbRasterImage::kPoly, ptArr);

		if (es == ErrorStatus::eOk) {

			PostToModelSpace(pWipeout);
			pWipeout->close();
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
	static AcDbObjectId CreatePolygon(AcGePoint2d ptCenter, int number,
		double radius, double rotation, double width)
	{
		AcGePoint2dArray points;
		double angle = 2 *PI / (double)number;
		for (int i = 0; i < number; i++)
		{
			AcGePoint2d pt;
			pt.x = ptCenter.x + radius * cos(i * angle);
			pt.y = ptCenter.y + radius * sin(i * angle);
			points.append(pt);
		}

		AcDbObjectId polyId = CreatePolyline(points, width);

		// 将其闭合
		AcDbEntity *pEnt;
		acdbOpenAcDbEntity(pEnt, polyId, AcDb::kForWrite);
		AcDbPolyline *pPoly = AcDbPolyline::cast(pEnt);
		if (pPoly != NULL)
		{
			pPoly->setClosed(Adesk::kTrue);
		}

		pEnt->close();

		return polyId;
	}

	static AcDbObjectId CreatePolyline(AcGePoint2dArray points, double width)
	{
		int numVertices = points.length();
		AcDbPolyline *pPoly = new AcDbPolyline(numVertices);
		for (int i = 0; i < numVertices; i++)
		{
			pPoly->addVertexAt(i, points.at(i), 0, width, width);
		}
		AcDbObjectId polyId;
		polyId =PostToModelSpace(pPoly);
		return polyId;
	}
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CEcdZheZhaoApp)
ACED_ARXCOMMAND_ENTRY_AUTO(CEcdZheZhaoApp, ECDMyGroup, EcdCC, EcdCC, ACRX_CMD_MODAL, NULL)
