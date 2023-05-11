#ifndef _BONE_GEOMETRY_DATA
#define _BONE_GEOMETRY_DATA
//
// File: BoneGeometryData.h
//
// Author: Ben Singleton
//

#include "Drawable.h"

#include <maya/MUserData.h>
#include <maya/MPlug.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>
#include <maya/MEulerRotation.h>
#include <maya/MMatrix.h>
#include <maya/MFnMatrixData.h>
#include <maya/MColor.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MStatus.h>

#include <maya/MViewport2Renderer.h>
#include <maya/MUIDrawManager.h>
#include <maya/MFrameContext.h>
#include <maya/MHWGeometry.h>
#include <maya/MHWGeometryUtilities.h>

#include <maya/MViewport2Renderer.h>
#include <maya/MUIDrawManager.h>

#include <string>
#include <map>


class BoneGeometryData : public MUserData
{

public:

								BoneGeometryData();
	virtual						~BoneGeometryData();

	virtual	BoneGeometryData&	operator=(const BoneGeometryData* src);

	virtual	MStatus				copyWireColor(const MDagPath& dagPath);
	virtual	MStatus				copyDepthPriority(const MDagPath& dagPath);

	virtual	void				dirtyObjectMatrix();

public:
			
			MVector				localPosition;
			MVector				localRotate;
			MVector				localScale;
			MMatrix				objectMatrix;

			double				width;
			double				height;
			double				length;
			double				taper;

			bool				sideFins;
			double				sideFinsSize;
			double				sideFinsStartTaper;
			double				sideFinsEndTaper;

			bool				frontFin;
			double				frontFinSize;
			double				frontFinStartTaper;
			double				frontFinEndTaper;

			bool				backFin;
			double				backFinSize;
			double				backFinStartTaper;
			double				backFinEndTaper;

			MColor				wireColor;
			unsigned int		depthPriority;

};

#endif