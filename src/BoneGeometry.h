#ifndef _BONE_GEOMETRY
#define _BONE_GEOMETRY
//
// File: BoneGeometry.h
//
// Dependency Graph Node: boneGeometry
//
// Author: Benjamin H. Singleton
//

#include "BoneGeometryData.h"

#include <maya/MPxLocatorNode.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MArrayDataBuilder.h>
#include <maya/MObject.h>
#include <maya/MObjectArray.h>
#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MString.h>
#include <maya/MMatrix.h>
#include <maya/MMatrixArray.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MBoundingBox.h>
#include <maya/MDistance.h>
#include <maya/MEulerRotation.h>
#include <maya/MAngle.h>
#include <maya/MColor.h>
#include <maya/MDoubleArray.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>

#include <maya/MFnDependencyNode.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnTransform.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnTypedAttribute.h>

#include <maya/MEvaluationNode.h>
#include <maya/MNodeCacheDisablingInfo.h>
#include <maya/MNodeCacheSetupInfo.h>
#include <maya/MViewport2Renderer.h>
#include <maya/MDGContext.h>
#include <maya/M3dView.h>
#include <maya/MGlobal.h>
#include <maya/MTypeId.h>

#include <assert.h>


class BoneGeometry : public MPxLocatorNode
{

public:

								BoneGeometry();
	virtual						~BoneGeometry();

	virtual MStatus				compute(const MPlug& plug, MDataBlock& data);
	virtual	void				draw(M3dView& view, const MDagPath& dagPath, M3dView::DisplayStyle displayStyle, M3dView::DisplayStatus displayStatus) {};

	virtual	MStatus				preEvaluation(const MDGContext& context, const MEvaluationNode& evaluationNode);
	virtual	void				getCacheSetup(const MEvaluationNode& evaluationNode, MNodeCacheDisablingInfo& disablingInfo, MNodeCacheSetupInfo& cacheSetupInfo, MObjectArray& monitoredAttributes) const;

	virtual	bool				setInternalValue(const MPlug& plug, const MDataHandle& handle);
	virtual BoneGeometryData*	getUserData();

	virtual	bool				isBounded() const;
	virtual	MBoundingBox		boundingBox() const;

	static  void*				creator();
	static  MStatus				initialize();

public:
	
	static  MObject				localRotate;
	static  MObject				localRotateX;
	static  MObject				localRotateY;
	static  MObject				localRotateZ;

	static	MObject				width;
	static	MObject				height;
	static	MObject				length;
	static	MObject				taper;
	static	MObject				sideFins;
	static	MObject				sideFinsSize;
	static	MObject				sideFinsStartTaper;
	static	MObject				sideFinsEndTaper;
	static	MObject				frontFin;
	static	MObject				frontFinSize;
	static	MObject				frontFinStartTaper;
	static	MObject				frontFinEndTaper;
	static	MObject				backFin;
	static	MObject				backFinSize;
	static	MObject				backFinStartTaper;
	static	MObject				backFinEndTaper;

	static  MObject				objectMatrix;
	static	MObject				objectInverseMatrix;
	static  MObject				objectWorldMatrix;
	static  MObject				objectWorldInverseMatrix;

	static	MString				localPositionCategory;
	static	MString				localRotationCategory;
	static	MString				localScaleCategory;
	static	MString				sizeCategory;
	static	MString				sideFinsCategory;
	static	MString				frontFinCategory;
	static	MString				backFinCategory;

	static	MString				drawDbClassification;
	static	MString				drawRegistrantId;
	static	MTypeId				id;

protected:

			BoneGeometryData*	data;

};

#endif