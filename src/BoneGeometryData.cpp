//
// File: BoneGeometryData.cpp
//
// Author: Ben Singleton
//

#include "BoneGeometryData.h"


BoneGeometryData::BoneGeometryData() : MUserData(false)
/**
Constructor.
*/
{

	this->localPosition = MVector(0.0, 0.0, 0.0);
	this->localRotate = MEulerRotation(0.0, 0.0, 0.0);
	this->localScale = MVector(1.0, 1.0, 1.0);
	this->objectMatrix = MMatrix::identity;

	this->width = 1.0;
	this->height = 1.0;
	this->length = 1.0;
	this->taper = 0.9;

	this->sideFins = false;
	this->sideFinsSize = 1.0;
	this->sideFinsStartTaper = 0.1;
	this->sideFinsEndTaper = 0.1;

	this->frontFin = false;
	this->frontFinSize = 1.0;
	this->frontFinStartTaper = 0.1;
	this->frontFinEndTaper = 0.1;

	this->backFin = false;
	this->backFinSize = 1.0;
	this->backFinStartTaper = 0.1;
	this->backFinEndTaper = 0.1;

	this->wireColor = MColor();
	this->depthPriority = 0;

};


BoneGeometryData::~BoneGeometryData() {};


MStatus BoneGeometryData::cacheWireColor(const MDagPath& dagPath)
/**
Caches the wire-colour from the supplied dag path.

@param dagPath: A path to the point helper.
@return: Return status.
*/
{

	this->wireColor = MHWRender::MGeometryUtilities::wireframeColor(dagPath);
	return MS::kSuccess;

};


MStatus BoneGeometryData::cacheDepthPriority(const MDagPath& dagPath)
/**
Caches the depth priority from the supplied dag path.

@param dagPath: A path to the point helper.
@return: Return status.
*/
{


	// Get correct depth priority
	//
	MHWRender::DisplayStatus status = MHWRender::MGeometryUtilities::displayStatus(dagPath);

	switch (status)
	{

	case MHWRender::DisplayStatus::kActiveComponent:

		this->depthPriority = MHWRender::MRenderItem::sActiveWireDepthPriority;
		break;

	default:

		this->depthPriority = MHWRender::MRenderItem::sDormantFilledDepthPriority;
		break;

	}

	return MS::kSuccess;

};


void BoneGeometryData::dirtyObjectMatrix()
/**
Updates the internal object-matrix.

@return: Null.
*/
{

	MMatrix positionMatrix = Drawable::createPositionMatrix(this->localPosition);
	MMatrix rotateMatrix = this->localRotate.asMatrix();
	MMatrix scaleMatrix = Drawable::createScaleMatrix(this->localScale);

	this->objectMatrix = scaleMatrix * rotateMatrix * positionMatrix;

};