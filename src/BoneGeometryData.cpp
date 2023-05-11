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
	this->localRotate = MVector(0.0, 0.0, 0.0);
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


BoneGeometryData& BoneGeometryData::operator=(const BoneGeometryData* src)
/**
Assignment operator.

@param src: Point helper data to be copied.
@return: Self.
*/
{

	this->localPosition = src->localPosition;
	this->localRotate = src->localRotate;
	this->localScale = src->localScale;

	this->width = src->width;
	this->height = src->height;
	this->length = src->length;
	this->taper = src->taper;

	this->sideFins = src->sideFins;
	this->sideFinsSize = src->sideFinsSize;
	this->sideFinsStartTaper = src->sideFinsStartTaper;
	this->sideFinsEndTaper = src->sideFinsEndTaper;

	this->frontFin = src->frontFin;
	this->frontFinSize = src->frontFinSize;
	this->frontFinStartTaper = src->frontFinStartTaper;
	this->frontFinEndTaper = src->frontFinEndTaper;

	this->backFin = src->backFin;
	this->backFinSize = src->backFinSize;
	this->backFinStartTaper = src->backFinStartTaper;
	this->backFinEndTaper = src->backFinEndTaper;

	this->wireColor = src->wireColor;
	this->depthPriority = src->depthPriority;

	return *this;

};


MStatus BoneGeometryData::copyWireColor(const MDagPath& dagPath)
/**
Caches the wire-colour from the supplied dag path.

@param dagPath: A path to the point helper.
@return: Return status.
*/
{

	MStatus status;

	// Check if path is valid
	//
	bool isValid = dagPath.isValid(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (!isValid)
	{

		return MS::kFailure;

	}

	// Evaluate wire color
	//
	this->wireColor = MHWRender::MGeometryUtilities::wireframeColor(dagPath);
	return MS::kSuccess;

};


MStatus BoneGeometryData::copyDepthPriority(const MDagPath& dagPath)
/**
Caches the depth priority from the supplied dag path.

@param dagPath: A path to the point helper.
@return: Return status.
*/
{

	MStatus status;

	// Check if path is valid
	//
	bool isValid = dagPath.isValid(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (!isValid)
	{

		return MS::kFailure;

	}

	// Evaluate display status
	//
	MHWRender::DisplayStatus displayStatus = MHWRender::MGeometryUtilities::displayStatus(dagPath);

	switch (displayStatus)
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
	MMatrix rotateMatrix = Drawable::createRotationMatrix(this->localRotate);
	MMatrix scaleMatrix = Drawable::createScaleMatrix(this->localScale);

	this->objectMatrix = scaleMatrix * rotateMatrix * positionMatrix;

};