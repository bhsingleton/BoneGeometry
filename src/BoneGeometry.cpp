//
// File: Bone.cpp
//
// Dependency Graph Node: boneGeometry
//
// Author: Benjamin H. Singleton
//

#include "BoneGeometry.h"

MObject	BoneGeometry::localRotate;
MObject	BoneGeometry::localRotateX;
MObject	BoneGeometry::localRotateY;
MObject	BoneGeometry::localRotateZ;

MObject	BoneGeometry::width;
MObject	BoneGeometry::height;
MObject	BoneGeometry::length;
MObject	BoneGeometry::taper;
MObject	BoneGeometry::sideFins;
MObject	BoneGeometry::sideFinsSize;
MObject	BoneGeometry::sideFinsStartTaper;
MObject	BoneGeometry::sideFinsEndTaper;
MObject	BoneGeometry::frontFin;
MObject	BoneGeometry::frontFinSize;
MObject	BoneGeometry::frontFinStartTaper;
MObject	BoneGeometry::frontFinEndTaper;
MObject	BoneGeometry::backFin;
MObject	BoneGeometry::backFinSize;
MObject	BoneGeometry::backFinStartTaper;
MObject	BoneGeometry::backFinEndTaper;

MObject	BoneGeometry::objectMatrix;
MObject	BoneGeometry::objectInverseMatrix;
MObject	BoneGeometry::objectWorldMatrix;
MObject	BoneGeometry::objectWorldInverseMatrix;

MString	BoneGeometry::localPositionCategory("LocalPosition");
MString	BoneGeometry::localRotationCategory("LocalRotation");
MString	BoneGeometry::localScaleCategory("LocalScale");
MString	BoneGeometry::sizeCategory("Size");
MString	BoneGeometry::sideFinsCategory("SideFins");
MString	BoneGeometry::frontFinCategory("FrontFin");
MString	BoneGeometry::backFinCategory("BackFin");

MString	BoneGeometry::drawDbClassification("drawdb/geometry/BoneGeometry");
MString	BoneGeometry::drawRegistrantId("BoneGeometryPlugin");
MTypeId	BoneGeometry::id(0x0013b1d2);


BoneGeometry::BoneGeometry()
/**
Constructor.
*/
{
	
	this->data = new BoneGeometryData();

};


BoneGeometry::~BoneGeometry()
/**
Destructor.
*/
{

	delete this->data;

};


MStatus BoneGeometry::compute(const MPlug& plug, MDataBlock& data)
/**
This method should be overridden in user defined nodes.
Recompute the given output based on the nodes inputs.
The plug represents the data value that needs to be recomputed, and the data block holds the storage for all of the node's attributes.
The MDataBlock will provide smart handles for reading and writing this node's attribute values.
Only these values should be used when performing computations!

@param plug: Plug representing the attribute that needs to be recomputed.
@param data: Data block containing storage for the node's attributes.
@return: Return status.
*/
{

	MStatus status;

	// Check requested attribute
	//
	if (plug == BoneGeometry::objectMatrix || plug == BoneGeometry::objectInverseMatrix)
	{

		// Get input data handles
		//
		MDataHandle localPositionHandle = data.inputValue(BoneGeometry::localPosition, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle localRotateHandle = data.inputValue(BoneGeometry::localRotate, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle localScaleHandle = data.inputValue(BoneGeometry::localScale, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Get values from handles
		//
		MVector position = localPositionHandle.asVector();
		MVector rotation = localRotateHandle.asVector();
		MVector scale = localScaleHandle.asVector();

		MMatrix positionMatrix = Drawable::createPositionMatrix(position);
		MMatrix rotateMatrix = Drawable::createRotationMatrix(rotation);
		MMatrix scaleMatrix = Drawable::createScaleMatrix(scale);

		MMatrix objectMatrix = scaleMatrix * rotateMatrix * positionMatrix;
		MMatrix objectInverseMatrix = objectMatrix.inverse();

		// Get output data handles
		//
		MDataHandle objectMatrixHandle = data.outputValue(BoneGeometry::objectMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MDataHandle objectInverseMatrixHandle = data.outputValue(BoneGeometry::objectInverseMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Commit values to data handle
		//
		objectMatrixHandle.setMMatrix(objectMatrix);
		objectInverseMatrixHandle.setMMatrix(objectInverseMatrix);

		// Mark handles as clean
		//
		objectMatrixHandle.setClean();
		objectInverseMatrixHandle.setClean();

		// Mark plug as clean
		//
		status = data.setClean(plug);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		return MS::kSuccess;

	}
	else if (plug == BoneGeometry::objectWorldMatrix || plug == BoneGeometry::objectWorldInverseMatrix)
	{

		// Get object matrix
		//
		MDataHandle objectMatrixHandle = data.inputValue(BoneGeometry::objectMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MMatrix objectMatrix = objectMatrixHandle.asMatrix();

		// Get all dag paths to this node
		//
		MObject node = this->thisMObject();

		MDagPathArray dagPaths;

		status = MDagPath::getAllPathsTo(node, dagPaths);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		unsigned int numPaths = dagPaths.length();

		// Define data builders
		//
		MArrayDataHandle objectWorldMatrixArrayHandle = data.outputArrayValue(BoneGeometry::objectWorldMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MArrayDataBuilder objectWorldMatrixBuilder(&data, BoneGeometry::objectWorldMatrix, numPaths, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MArrayDataHandle objectWorldInverseMatrixArrayHandle = data.outputArrayValue(BoneGeometry::objectWorldInverseMatrix, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MArrayDataBuilder objectWorldInverseMatrixBuilder(&data, BoneGeometry::objectWorldInverseMatrix, numPaths, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		for (unsigned int i = 0; i < numPaths; i++)
		{

			// Create new data handles for elements
			//
			MDataHandle hObjectWorldMatrix = objectWorldMatrixBuilder.addElement(i, &status);
			MDataHandle hObjectWorldInverseMatrix = objectWorldInverseMatrixBuilder.addElement(i, &status);

			// Get inclusive matrix to dag path
			//
			MDagPath dagPath = dagPaths[i];
			MMatrix inclusiveMatrix = dagPath.inclusiveMatrix();

			MMatrix objectWorldMatrix = objectMatrix * inclusiveMatrix;
			MMatrix objectWorldInverseMatrix = objectWorldMatrix.inverse();

			// Assign matrices to elements
			//
			hObjectWorldMatrix.setMMatrix(objectWorldMatrix);
			hObjectWorldInverseMatrix.setMMatrix(objectWorldInverseMatrix);

			// Mark handles as clean
			//
			hObjectWorldMatrix.setClean();
			hObjectWorldInverseMatrix.setClean();

		}

		// Assign builders to data handles
		//
		objectWorldMatrixArrayHandle.set(objectWorldMatrixBuilder);
		objectWorldInverseMatrixArrayHandle.set(objectWorldInverseMatrixBuilder);

		// Mark data handles as clean
		//
		objectWorldMatrixArrayHandle.setAllClean();
		objectWorldInverseMatrixArrayHandle.setAllClean();

		// Mark data block as clean
		//
		status = data.setClean(plug);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		return MS::kSuccess;

	}
	else
	{

		return MS::kUnknownParameter;

	}

};


MStatus BoneGeometry::preEvaluation(const MDGContext& context, const MEvaluationNode& evaluationNode)
/**
Prepare a node's internal state for threaded evaluation.
During the evaluation graph execution each node gets a chance to reset its internal states just before being evaluated.
This code has to be thread safe, non-blocking and work only on data owned by the node.
The timing of this callback is at the discretion of evaluation graph dependencies and individual evaluators. This means, it should be used purely to prepare this node for evaluation and no particular order should be assumed.
This call will most likely happen from a worker thread.
When using Evaluation Caching or VP2 Custom Caching, preEvaluation() is called as part of the evaluation process. This function is not called as part of the cache restore process because no evaluation takes place in that case.

@param context: Context in which the evaluation is happening. This should be respected and only internal state information pertaining to it should be modified.
@param evaluationNode: Evaluation node which contains information about the dirty plugs that are about to be evaluated for the context. Should be only used to query information.
@return: Return status.
*/
{

	MStatus status;

	if (context.isNormal())
	{

		if (evaluationNode.dirtyPlugExists(BoneGeometry::width, &status) && status ||
			evaluationNode.dirtyPlugExists(BoneGeometry::height, &status) && status ||
			evaluationNode.dirtyPlugExists(BoneGeometry::length, &status) && status ||
			evaluationNode.dirtyPlugExists(BoneGeometry::localPosition, &status) && status ||
			evaluationNode.dirtyPlugExists(BoneGeometry::localRotate, &status) && status ||
			evaluationNode.dirtyPlugExists(BoneGeometry::localScale, &status) && status)
		{

			MHWRender::MRenderer::setGeometryDrawDirty(this->thisMObject(), true);

		}

	}

	return MPxLocatorNode::preEvaluation(context, evaluationNode);

};


void BoneGeometry::getCacheSetup(const MEvaluationNode& evaluationNode, MNodeCacheDisablingInfo& disablingInfo, MNodeCacheSetupInfo& cacheSetupInfo, MObjectArray& monitoredAttributes) const
/**
Provide node-specific setup info for the Cached Playback system.

@param evaluationNode: This node's evaluation node, contains animated plug information.
@param disablingInfo: Information about why the node disables Cached Playback to be reported to the user.
@param cacheSetupInfo: Preferences and requirements this node has for Cached Playback.
@param monitoredAttributes: Attributes impacting the behavior of this method that will be monitored for change.
@return: Void.
*/
{

	// Call parent function
	//
	MPxLocatorNode::getCacheSetup(evaluationNode, disablingInfo, cacheSetupInfo, monitoredAttributes);
	assert(!disablingInfo.getCacheDisabled());

	// Update caching preference
	//
	cacheSetupInfo.setPreference(MNodeCacheSetupInfo::kWantToCacheByDefault, true);

};


bool BoneGeometry::getInternalValue(const MPlug& plug, MDataHandle& handle)
/**
This method is overridden by nodes that store attribute data in some internal format.
The internal state of attributes can be set or queried using the setInternal and internal methods of MFnAttribute.
When internal attribute values are queried via getAttr or MPlug::getValue this method is called.

@param plug: The attribute that is being queried.
@param handle: The data handle to store the attribute value.
@return: The attribute was placed in the datablock.
*/
{

	MStatus status;

	// Evaluate attribute category
	//
	MObject attribute = plug.attribute(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MFnAttribute fnAttribute(attribute, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	bool isLocalPosition = fnAttribute.hasCategory(BoneGeometry::localPositionCategory);
	bool isLocalRotation = fnAttribute.hasCategory(BoneGeometry::localRotationCategory);
	bool isLocalScale = fnAttribute.hasCategory(BoneGeometry::localScaleCategory);
	bool isSize = fnAttribute.hasCategory(BoneGeometry::sizeCategory);
	bool isSideFins = fnAttribute.hasCategory(BoneGeometry::sideFinsCategory);
	bool isFrontFin = fnAttribute.hasCategory(BoneGeometry::frontFinCategory);
	bool isBackFin = fnAttribute.hasCategory(BoneGeometry::backFinCategory);

	if (isLocalPosition)
	{

		if (attribute == BoneGeometry::localPosition)
		{

			handle.setMVector(this->data->localPosition);

		}
		else if (attribute == BoneGeometry::localPositionX)
		{

			handle.setMDistance(MDistance(this->data->localPosition.x, MDistance::kCentimeters));

		}
		else if (attribute == BoneGeometry::localPositionY)
		{

			handle.setMDistance(MDistance(this->data->localPosition.y, MDistance::kCentimeters));

		}
		else if (attribute == BoneGeometry::localPositionZ)
		{

			handle.setMDistance(MDistance(this->data->localPosition.z, MDistance::kCentimeters));

		}
		else;

		return true;

	}
	else if (isLocalRotation)
	{

		if (attribute == BoneGeometry::localRotate)
		{

			handle.setMVector(this->data->localRotate);

		}
		else if (attribute == BoneGeometry::localRotateX)
		{

			handle.setMAngle(MAngle(this->data->localRotate.x, MAngle::kRadians));

		}
		else if (attribute == BoneGeometry::localRotateY)
		{

			handle.setMAngle(MAngle(this->data->localRotate.y, MAngle::kRadians));

		}
		else if (attribute == BoneGeometry::localRotateZ)
		{

			handle.setMAngle(MAngle(this->data->localRotate.z, MAngle::kRadians));

		}
		else;

		return true;

	}
	else if (isLocalScale)
	{

		if (attribute == BoneGeometry::localScale)
		{

			handle.setMVector(this->data->localScale);

		}
		else if (attribute == BoneGeometry::localScaleX)
		{

			handle.setMDistance(MDistance(this->data->localScale.x, MDistance::kCentimeters));

		}
		else if (attribute == BoneGeometry::localScaleY)
		{

			handle.setMDistance(MDistance(this->data->localScale.y, MDistance::kCentimeters));

		}
		else if (attribute == BoneGeometry::localScaleZ)
		{

			handle.setMDistance(MDistance(this->data->localScale.z, MDistance::kCentimeters));

		}
		else;

		return true;

	}
	else if (isSize)
	{

		if (attribute == BoneGeometry::length)
		{

			handle.setDouble(this->data->length);

		}
		else if (attribute == BoneGeometry::width)
		{

			handle.setDouble(this->data->width);

		}
		else if (attribute == BoneGeometry::height)
		{

			handle.setDouble(this->data->height);

		}
		else if (attribute == BoneGeometry::taper)
		{

			handle.setDouble(this->data->taper);

		}
		else;

		return true;

	}
	else if (isSideFins)
	{

		if (attribute == BoneGeometry::sideFins)
		{

			handle.setBool(this->data->sideFins);

		}
		else if (attribute == BoneGeometry::sideFinsSize)
		{

			handle.setDouble(this->data->sideFinsSize);

		}
		else if (attribute == BoneGeometry::sideFinsStartTaper)
		{

			handle.setDouble(this->data->sideFinsStartTaper);

		}
		else if (attribute == BoneGeometry::sideFinsEndTaper)
		{

			handle.setDouble(this->data->sideFinsEndTaper);

		}
		else;

		return true;

	}
	else if (isFrontFin)
	{

		if (attribute == BoneGeometry::frontFin)
		{

			handle.setBool(this->data->frontFin);

		}
		else if (attribute == BoneGeometry::frontFinSize)
		{

			handle.setDouble(this->data->frontFinSize);

		}
		else if (attribute == BoneGeometry::frontFinStartTaper)
		{

			handle.setDouble(this->data->frontFinStartTaper);

		}
		else if (attribute == BoneGeometry::frontFinEndTaper)
		{

			handle.setDouble(this->data->frontFinEndTaper);

		}
		else;

		return true;

	}
	else if (isBackFin)
	{

		if (attribute == BoneGeometry::backFin)
		{

			handle.setBool(this->data->backFin);

		}
		else if (attribute == BoneGeometry::backFinSize)
		{

			handle.setDouble(this->data->backFinSize);

		}
		else if (attribute == BoneGeometry::backFinStartTaper)
		{

			handle.setDouble(this->data->backFinStartTaper);

		}
		else if (attribute == BoneGeometry::backFinEndTaper)
		{

			handle.setDouble(this->data->backFinEndTaper);

		}
		else;

		return true;

	}
	else;

	return MPxLocatorNode::getInternalValue(plug, handle);

};


bool BoneGeometry::setInternalValue(const MPlug& plug, const MDataHandle& handle)
/**
This method is overridden by nodes that store attribute specs in some internal format.
The internal state of attributes can be set or queried using the setInternal and internal methods of MFnAttribute.
When internal attribute values are set via setAttr or MPlug::setValue this method is called.
Another use for this method is to impose attribute limits.

@param plug: The attribute that is being set.
@param handle: The data handle containing the value to set.
@return: The attribute was handled internally.
*/
{

	MStatus status;

	// Inspect attribute
	//
	MObject attribute = plug.attribute(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MFnAttribute fnAttribute(attribute, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	bool isLocalPosition = fnAttribute.hasCategory(BoneGeometry::localPositionCategory);
	bool isLocalRotation = fnAttribute.hasCategory(BoneGeometry::localRotationCategory);
	bool isLocalScale = fnAttribute.hasCategory(BoneGeometry::localScaleCategory);
	bool isSize = fnAttribute.hasCategory(BoneGeometry::sizeCategory);
	bool isSideFins = fnAttribute.hasCategory(BoneGeometry::sideFinsCategory);
	bool isFrontFin = fnAttribute.hasCategory(BoneGeometry::frontFinCategory);
	bool isBackFin = fnAttribute.hasCategory(BoneGeometry::backFinCategory);
	
	if (isLocalPosition)
	{

		if (attribute == BoneGeometry::localPosition)
		{

			this->data->localPosition = handle.asVector();

		}
		else if (attribute == BoneGeometry::localPositionX)
		{

			this->data->localPosition.x = handle.asDistance().asCentimeters();

		}
		else if (attribute == BoneGeometry::localPositionY)
		{

			this->data->localPosition.y = handle.asDistance().asCentimeters();

		}
		else if (attribute == BoneGeometry::localPositionZ)
		{

			this->data->localPosition.z = handle.asDistance().asCentimeters();

		}
		else;

		this->data->dirtyObjectMatrix();
		return true;

	}
	else if (isLocalRotation)
	{

		if (attribute == BoneGeometry::localRotate)
		{

			this->data->localRotate = handle.asVector();

		}
		else if (attribute == BoneGeometry::localRotateX)
		{

			this->data->localRotate.x = handle.asAngle().asRadians();

		}
		else if (attribute == BoneGeometry::localRotateY)
		{

			this->data->localRotate.y = handle.asAngle().asRadians();

		}
		else if (attribute == BoneGeometry::localRotateZ)
		{

			this->data->localRotate.z = handle.asAngle().asRadians();

		}
		else;

		this->data->dirtyObjectMatrix();
		return true;

	}
	else if (isLocalScale)
	{

		if (attribute == BoneGeometry::localScale)
		{

			this->data->localScale = handle.asVector();

		}
		else if (attribute == BoneGeometry::localScaleX)
		{

			this->data->localScale.x = handle.asDistance().asCentimeters();

		}
		else if (attribute == BoneGeometry::localScaleY)
		{

			this->data->localScale.y = handle.asDistance().asCentimeters();

		}
		else if (attribute == BoneGeometry::localScaleZ)
		{

			this->data->localScale.z = handle.asDistance().asCentimeters();

		}
		else;

		this->data->dirtyObjectMatrix();
		return true;

	}
	else if (isSize)
	{

		if (attribute == BoneGeometry::length)
		{

			this->data->length = handle.asDouble();

		}
		else if (attribute == BoneGeometry::width)
		{

			this->data->width = handle.asDouble();

		}
		else if (attribute == BoneGeometry::height)
		{

			this->data->height = handle.asDouble();

		}
		else if (attribute == BoneGeometry::taper)
		{

			this->data->taper = handle.asDouble();

		}
		else;

		return true;

	}
	else if (isSideFins)
	{

		if (attribute == BoneGeometry::sideFins)
		{

			this->data->sideFins = handle.asBool();

		}
		else if (attribute == BoneGeometry::sideFinsSize)
		{

			this->data->sideFinsSize = handle.asDouble();

		}
		else if (attribute == BoneGeometry::sideFinsStartTaper)
		{

			this->data->sideFinsStartTaper = handle.asDouble();

		}
		else if (attribute == BoneGeometry::sideFinsEndTaper)
		{

			this->data->sideFinsEndTaper = handle.asDouble();

		}
		else;

		return true;

	}
	else if (isFrontFin)
	{

		if (attribute == BoneGeometry::frontFin)
		{

			this->data->frontFin = handle.asBool();

		}
		else if (attribute == BoneGeometry::frontFinSize)
		{

			this->data->frontFinSize = handle.asDouble();

		}
		else if (attribute == BoneGeometry::frontFinStartTaper)
		{

			this->data->frontFinStartTaper = handle.asDouble();

		}
		else if (attribute == BoneGeometry::frontFinEndTaper)
		{

			this->data->frontFinEndTaper = handle.asDouble();

		}
		else;

		return true;

	}
	else if (isBackFin)
	{

		if (attribute == BoneGeometry::backFin)
		{

			this->data->backFin = handle.asBool();

		}
		else if (attribute == BoneGeometry::backFinSize)
		{

			this->data->backFinSize = handle.asDouble();

		}
		else if (attribute == BoneGeometry::backFinStartTaper)
		{

			this->data->backFinStartTaper = handle.asDouble();

		}
		else if (attribute == BoneGeometry::backFinEndTaper)
		{

			this->data->backFinEndTaper = handle.asDouble();

		}
		else;

		return true;

	}
	else;

	return MPxLocatorNode::setInternalValue(plug, handle);

};


void BoneGeometry::copyInternalData(MPxNode* node)
/**
This method is overridden by nodes that store attribute data in some internal format.
On duplication this method is called on the duplicated node with the node being duplicated passed as the parameter.
Overriding this method gives your node a chance to duplicate any internal data you've been storing and manipulating outside of normal attribute data.

@param node: The node that is being duplicated.
@return: Void.
*/
{

	BoneGeometry* boneGeometry = static_cast<BoneGeometry*>(node);
	this->data = boneGeometry->getUserData();

};


BoneGeometryData* BoneGeometry::getUserData()
/**
Returns a pointer to the internal bone geometry data.

@return: The bone geometry data pointer.
*/
{

	return this->data;

};


bool BoneGeometry::isBounded() const
/**
This function indicates if the bounding method will be overrided by the user.
Supplying a bounding box will make selection calculation more efficient!

@return: bool
*/
{

	return true;

};


MBoundingBox BoneGeometry::boundingBox() const
/**
This function is used to calculate a bounding box based on the object transform.
Supplying a bounding box will make selection calculation more efficient!

@return: MBoundingBox
*/
{

	MPoint corner1 = MPoint(0.0, -this->data->height * 0.5, -this->data->width * 0.5);  // x = length, y = height, z = width
	MPoint corner2 = MPoint(this->data->length, this->data->height * 0.5, this->data->width * 0.5);  // x = length, y = height, z = width

	MBoundingBox boundingBox = MBoundingBox(corner1, corner2);
	boundingBox.transformUsing(this->data->objectMatrix);

	return boundingBox;

};


void* BoneGeometry::creator()
/**
This function is called by Maya when a new instance is requested.
See pluginMain.cpp for details.

@return: BoneGeometry
*/
{

	return new BoneGeometry();

};


MStatus BoneGeometry::initialize()
/**
This function is called by Maya after a plugin has been loaded.
Use this function to define any static attributes.

@return: MStatus
*/
{

	MStatus status;

	// Initialize function sets
	//
	MFnNumericAttribute fnNumericAttr;
	MFnTypedAttribute fnTypedAttr;
	MFnUnitAttribute fnUnitAttr;
	MFnMatrixAttribute fnMatrixAttr;
	
	// Input attributes:
	// Edit ".localPositionX" attribute
	//
	status = fnUnitAttr.setObject(BoneGeometry::localPositionX);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setInternal(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(BoneGeometry::localPositionCategory));

	// Edit ".localPositionY" attribute
	//
	status = fnUnitAttr.setObject(BoneGeometry::localPositionY);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setInternal(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(BoneGeometry::localPositionCategory));

	// Edit ".localPositionZ" attribute
	//
	status = fnUnitAttr.setObject(BoneGeometry::localPositionZ);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setInternal(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(BoneGeometry::localPositionCategory));

	// Edit ".localPosition" attribute
	//
	status = fnNumericAttr.setObject(BoneGeometry::localPosition);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setInternal(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(BoneGeometry::localPositionCategory));
	
	// Define ".localRotateX" attribute
	//
	BoneGeometry::localRotateX = fnUnitAttr.create("localRotateX", "lorx", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setInternal(true));
	CHECK_MSTATUS(fnUnitAttr.setChannelBox(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(BoneGeometry::localRotationCategory));

	// Define ".localRotateY" attribute
	//
	BoneGeometry::localRotateY = fnUnitAttr.create("localRotateY", "lory", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setInternal(true));
	CHECK_MSTATUS(fnUnitAttr.setChannelBox(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(BoneGeometry::localRotationCategory));

	// Define ".localRotateZ" attribute
	//
	BoneGeometry::localRotateZ = fnUnitAttr.create("localRotateZ", "lorz", MFnUnitAttribute::kAngle, 0.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setInternal(true));
	CHECK_MSTATUS(fnUnitAttr.setChannelBox(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(BoneGeometry::localRotationCategory));

	// Define ".localRotate"
	//
	BoneGeometry::localRotate = fnNumericAttr.create("localRotate", "lor", BoneGeometry::localRotateX, BoneGeometry::localRotateY, BoneGeometry::localRotateZ, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setInternal(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(BoneGeometry::localRotationCategory));

	// Edit ".localScaleX" attribute
	//
	status = fnUnitAttr.setObject(BoneGeometry::localScaleX);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setInternal(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(BoneGeometry::localScaleCategory));

	// Edit ".localScaleY" attribute
	//
	status = fnUnitAttr.setObject(BoneGeometry::localScaleY);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setInternal(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(BoneGeometry::localScaleCategory));

	// Edit ".localScaleZ" attribute
	//
	status = fnUnitAttr.setObject(BoneGeometry::localScaleZ);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnUnitAttr.setInternal(true));
	CHECK_MSTATUS(fnUnitAttr.addToCategory(BoneGeometry::localScaleCategory));

	// Edit ".localScale" attribute
	//
	status = fnNumericAttr.setObject(BoneGeometry::localScale);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setInternal(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(BoneGeometry::localScaleCategory));
	
	// ".width" attribute
	//
	BoneGeometry::width = fnNumericAttr.create("width", "w", MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setInternal(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(BoneGeometry::sizeCategory));

	// ".height" attribute
	//
	BoneGeometry::height = fnNumericAttr.create("height", "h", MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setInternal(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(BoneGeometry::sizeCategory));

	// ".length" attribute
	//
	BoneGeometry::length = fnNumericAttr.create("length", "l", MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setInternal(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(BoneGeometry::sizeCategory));

	// ".taper" attribute
	//
	BoneGeometry::taper = fnNumericAttr.create("taper", "t", MFnNumericData::kDouble, 0.9, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setMin(0.0));
	CHECK_MSTATUS(fnNumericAttr.setMax(1.0));
	CHECK_MSTATUS(fnNumericAttr.setInternal(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(BoneGeometry::sizeCategory));

	// ".sideFins" attribute
	//
	BoneGeometry::sideFins = fnNumericAttr.create("sideFins", "sf", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setInternal(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(BoneGeometry::sideFinsCategory));

	// ".sideFinsSize" attribute
	//
	BoneGeometry::sideFinsSize = fnNumericAttr.create("sideFinsSize", "sfs", MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setInternal(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(BoneGeometry::sideFinsCategory));

	// ".sideFinsStartTaper" attribute
	//
	BoneGeometry::sideFinsStartTaper = fnNumericAttr.create("sideFinsStartTaper", "sfst", MFnNumericData::kDouble, 0.1, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setMin(0.0));
	CHECK_MSTATUS(fnNumericAttr.setMax(1.0));
	CHECK_MSTATUS(fnNumericAttr.setInternal(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(BoneGeometry::sideFinsCategory));

	// ".sideFinsEndTaper" attribute
	//
	BoneGeometry::sideFinsEndTaper = fnNumericAttr.create("sideFinsEndTaper", "sfet", MFnNumericData::kDouble, 0.1, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setMin(0.0));
	CHECK_MSTATUS(fnNumericAttr.setMax(1.0));
	CHECK_MSTATUS(fnNumericAttr.setInternal(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(BoneGeometry::sideFinsCategory));

	// ".frontFin" attribute
	//
	BoneGeometry::frontFin = fnNumericAttr.create("frontFin", "ff", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setInternal(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(BoneGeometry::frontFinCategory));

	// ".frontFinSize" attribute
	//
	BoneGeometry::frontFinSize = fnNumericAttr.create("frontFinSize", "ffs", MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setInternal(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(BoneGeometry::frontFinCategory));

	// ".frontFinStartTaper" attribute
	//
	BoneGeometry::frontFinStartTaper = fnNumericAttr.create("frontFinStartTaper", "ffst", MFnNumericData::kDouble, 0.1, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setMin(0.0));
	CHECK_MSTATUS(fnNumericAttr.setMax(1.0));
	CHECK_MSTATUS(fnNumericAttr.setInternal(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(BoneGeometry::frontFinCategory));

	// ".frontFinEndTaper" attribute
	//
	BoneGeometry::frontFinEndTaper = fnNumericAttr.create("frontFinEndTaper", "ffet", MFnNumericData::kDouble, 0.1, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setMin(0.0));
	CHECK_MSTATUS(fnNumericAttr.setMax(1.0));
	CHECK_MSTATUS(fnNumericAttr.setInternal(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(BoneGeometry::frontFinCategory));

	// ".backFin" attribute
	//
	BoneGeometry::backFin = fnNumericAttr.create("backFin", "bf", MFnNumericData::kBoolean, false, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setInternal(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(BoneGeometry::backFinCategory));

	// ".backFinSize" attribute
	//
	BoneGeometry::backFinSize = fnNumericAttr.create("backFinSize", "bfs", MFnNumericData::kDouble, 1.0, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setInternal(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(BoneGeometry::backFinCategory));

	// ".backFinStartTaper" attribute
	//
	BoneGeometry::backFinStartTaper = fnNumericAttr.create("backFinStartTaper", "bfst", MFnNumericData::kDouble, 0.1, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setMin(0.0));
	CHECK_MSTATUS(fnNumericAttr.setMax(1.0));
	CHECK_MSTATUS(fnNumericAttr.setInternal(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(BoneGeometry::backFinCategory));

	// ".backFinEndTaper" attribute
	//
	BoneGeometry::backFinEndTaper = fnNumericAttr.create("backFinEndTaper", "bfet", MFnNumericData::kDouble, 0.1, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnNumericAttr.setMin(0.0));
	CHECK_MSTATUS(fnNumericAttr.setMax(1.0));
	CHECK_MSTATUS(fnNumericAttr.setInternal(true));
	CHECK_MSTATUS(fnNumericAttr.addToCategory(BoneGeometry::backFinCategory));

	// Output attributes:
	// Define ".objectMatrix" attribute
	//
	BoneGeometry::objectMatrix = fnMatrixAttr.create("objectMatrix", "om", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.setCached(false));

	// Define ".objectInverseMatrix" attribute
	//
	BoneGeometry::objectInverseMatrix = fnMatrixAttr.create("objectInverseMatrix", "oim", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.setCached(false));

	// Define ".objectWorldMatrix" attribute
	//
	BoneGeometry::objectWorldMatrix = fnMatrixAttr.create("objectWorldMatrix", "owm", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.setCached(false));
	CHECK_MSTATUS(fnMatrixAttr.setArray(true));
	CHECK_MSTATUS(fnMatrixAttr.setUsesArrayDataBuilder(true));
	CHECK_MSTATUS(fnMatrixAttr.setWorldSpace(true));

	// Define ".objectWorldInverseMatrix" attribute
	//
	BoneGeometry::objectWorldInverseMatrix = fnMatrixAttr.create("objectWorldInverseMatrix", "owim", MFnMatrixAttribute::kDouble, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	CHECK_MSTATUS(fnMatrixAttr.setWritable(false));
	CHECK_MSTATUS(fnMatrixAttr.setStorable(false));
	CHECK_MSTATUS(fnMatrixAttr.setCached(false));
	CHECK_MSTATUS(fnMatrixAttr.setArray(true));
	CHECK_MSTATUS(fnMatrixAttr.setUsesArrayDataBuilder(true));
	CHECK_MSTATUS(fnMatrixAttr.setWorldSpace(true));

	// Add attributes to node
	//
	CHECK_MSTATUS(BoneGeometry::addAttribute(BoneGeometry::localRotate));
	CHECK_MSTATUS(BoneGeometry::addAttribute(BoneGeometry::width));
	CHECK_MSTATUS(BoneGeometry::addAttribute(BoneGeometry::height));
	CHECK_MSTATUS(BoneGeometry::addAttribute(BoneGeometry::length));
	CHECK_MSTATUS(BoneGeometry::addAttribute(BoneGeometry::taper));
	CHECK_MSTATUS(BoneGeometry::addAttribute(BoneGeometry::sideFins));
	CHECK_MSTATUS(BoneGeometry::addAttribute(BoneGeometry::sideFinsSize));
	CHECK_MSTATUS(BoneGeometry::addAttribute(BoneGeometry::sideFinsStartTaper));
	CHECK_MSTATUS(BoneGeometry::addAttribute(BoneGeometry::sideFinsEndTaper));
	CHECK_MSTATUS(BoneGeometry::addAttribute(BoneGeometry::frontFin));
	CHECK_MSTATUS(BoneGeometry::addAttribute(BoneGeometry::frontFinSize));
	CHECK_MSTATUS(BoneGeometry::addAttribute(BoneGeometry::frontFinStartTaper));
	CHECK_MSTATUS(BoneGeometry::addAttribute(BoneGeometry::frontFinEndTaper));
	CHECK_MSTATUS(BoneGeometry::addAttribute(BoneGeometry::backFin));
	CHECK_MSTATUS(BoneGeometry::addAttribute(BoneGeometry::backFinSize));
	CHECK_MSTATUS(BoneGeometry::addAttribute(BoneGeometry::backFinStartTaper));
	CHECK_MSTATUS(BoneGeometry::addAttribute(BoneGeometry::backFinEndTaper));

	CHECK_MSTATUS(BoneGeometry::addAttribute(BoneGeometry::objectMatrix));
	CHECK_MSTATUS(BoneGeometry::addAttribute(BoneGeometry::objectInverseMatrix));
	CHECK_MSTATUS(BoneGeometry::addAttribute(BoneGeometry::objectWorldMatrix));
	CHECK_MSTATUS(BoneGeometry::addAttribute(BoneGeometry::objectWorldInverseMatrix));

	// Define attribute relationships
	//
	CHECK_MSTATUS(BoneGeometry::attributeAffects(BoneGeometry::localPosition, BoneGeometry::objectMatrix));
	CHECK_MSTATUS(BoneGeometry::attributeAffects(BoneGeometry::localRotate, BoneGeometry::objectMatrix));
	CHECK_MSTATUS(BoneGeometry::attributeAffects(BoneGeometry::localScale, BoneGeometry::objectMatrix));

	CHECK_MSTATUS(BoneGeometry::attributeAffects(BoneGeometry::localPosition, BoneGeometry::objectInverseMatrix));
	CHECK_MSTATUS(BoneGeometry::attributeAffects(BoneGeometry::localRotate, BoneGeometry::objectInverseMatrix));
	CHECK_MSTATUS(BoneGeometry::attributeAffects(BoneGeometry::localScale, BoneGeometry::objectInverseMatrix));

	CHECK_MSTATUS(BoneGeometry::attributeAffects(BoneGeometry::localPosition, BoneGeometry::objectWorldMatrix));
	CHECK_MSTATUS(BoneGeometry::attributeAffects(BoneGeometry::localRotate, BoneGeometry::objectWorldMatrix));
	CHECK_MSTATUS(BoneGeometry::attributeAffects(BoneGeometry::localScale, BoneGeometry::objectWorldMatrix));

	CHECK_MSTATUS(BoneGeometry::attributeAffects(BoneGeometry::localPosition, BoneGeometry::objectWorldInverseMatrix));
	CHECK_MSTATUS(BoneGeometry::attributeAffects(BoneGeometry::localRotate, BoneGeometry::objectWorldInverseMatrix));
	CHECK_MSTATUS(BoneGeometry::attributeAffects(BoneGeometry::localScale, BoneGeometry::objectWorldInverseMatrix));

	return status;

};