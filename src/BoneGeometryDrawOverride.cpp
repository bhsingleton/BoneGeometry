//
// File: BoneGeometryDrawOverride.cpp
//
// Author: Benjamin H. Singleton
//

#include "BoneGeometryDrawOverride.h"

int BoneGeometryDrawOverride::POLYGON_CONNECTS[32] = { 0, 1, 2, 0, 2, 3, 0, 4, 1, 0, 3, 4, 2, 1, 5, 6, 3, 2, 6, 7, 1, 4, 8, 5, 4, 3, 7, 8, 6, 5, 8, 7 };
int BoneGeometryDrawOverride::POLYGON_COUNTS[9] = { 3, 3, 3, 3, 4, 4, 4, 4, 4 };


BoneGeometryDrawOverride::BoneGeometryDrawOverride(const MObject& node) : MPxDrawOverride(node, NULL, false)
/**
Constructor.

@param node: The Maya object this override draws.
@param callback: The callback function to be invoked at draw time.
@param isAlwaysDirty: If true, this override will always be updated via prepareForDraw() or addUIDrawables() without checking the dirty state of the Maya object.
*/
{

	MStatus status;

	// Create model editor callback
	//
	this->modelEditorChangedCallbackId = MEventMessage::addEventCallback("modelEditorChanged", BoneGeometryDrawOverride::onModelEditorChanged, this, &status);
	CHECK_MSTATUS(status);

	// Store pointer to MPxLocator
	// This will be useful for getting plug data to pass into our MPxUserData class
	//
	MFnDependencyNode fnNode(node, &status);
	CHECK_MSTATUS(status);

	this->boneGeometry = status ? dynamic_cast<BoneGeometry*>(fnNode.userNode()) : nullptr;

};


BoneGeometryDrawOverride::~BoneGeometryDrawOverride()
/**
Destructor.
*/
{

	// Clear pointer
	//
	this->boneGeometry = NULL;

	// Remove model editor callback
	//
	if (this->modelEditorChangedCallbackId != 0)
	{

		MMessage::removeCallback(this->modelEditorChangedCallbackId);
		this->modelEditorChangedCallbackId = 0;

	}

};


MHWRender::MPxDrawOverride* BoneGeometryDrawOverride::creator(const MObject& node)
/**
Static function used to create a new draw override instance.
This function is called via the MDrawRegistry::registerDrawOverrideCreator() method.

@param node: The Maya object this override draws.
@return: MPxDrawOverride*
*/
{

	return new BoneGeometryDrawOverride(node);

};


void BoneGeometryDrawOverride::onModelEditorChanged(void* clientData)
/**
Callback function used to mark this locator as dirty whenever a model editor change has occurred.
This includes things like toggling wireframe mode, etc.
To explicitly mark an object as being dirty the MRenderer::setGeometryDrawDirty() method can be used!

@param clientData: Pointer to the draw override instance.
@return: void
*/
{

	// Check if client data is null
	//
	if (clientData == NULL)
	{

		return;

	}

	// Check if point helper is null
	//
	BoneGeometryDrawOverride* drawOverride = static_cast<BoneGeometryDrawOverride*>(clientData);

	if (drawOverride->boneGeometry == NULL)
	{

		return;

	}

	// Mark the node as being dirty so that it can update when the display appearance switches between wireframe and shaded
	//
	MHWRender::MRenderer::setGeometryDrawDirty(drawOverride->boneGeometry->thisMObject());

};


MHWRender::DrawAPI BoneGeometryDrawOverride::supportedDrawAPIs() const
/**
Returns the draw API supported by this override.
The returned value may be formed as the bitwise or of MHWRender::DrawAPI elements to indicate that the override supports multiple draw APIs.
This method returns MHWRender::kOpenGL by default.

@return: MHWRender::DrawAPI
*/
{

	return (MHWRender::kOpenGL | MHWRender::kDirectX11 | MHWRender::kOpenGLCoreProfile);

};


bool BoneGeometryDrawOverride::isBounded(const MDagPath& objPath, const MDagPath& cameraPath) const
/**
Called by Maya to determine if the drawable object is bounded or not.
If the object is not bounded then it will never be culled by the current camera frustum used for drawing.
The default implementation will always return true. This method can be overridden in derived classes to customize the behaviour.
Note that if this method returns false then the boundingBox() method will not be called as no bounds are required in this case.

@param objPath: The path to the object being drawn.
@param cameraPath: The path to the camera that is being used to draw.
@return: bool
*/
{

	return this->boneGeometry->isBounded();

};


MBoundingBox BoneGeometryDrawOverride::boundingBox(const MDagPath& objPath, const MDagPath& cameraPath) const
/**
Called by Maya whenever the bounding box of the drawable object is needed.
This method should return the object space bounding box for the object to be drawn.
Note that this method will not be called if the isBounded() method returns a value of false.
Default implementation returns a huge bounding box which will never cull the object.

@param objPath: The path to the object being drawn.
@param cameraPath: The path to the camera that is being used to draw.
@return: MBoundingBox
*/
{

	return this->boneGeometry->boundingBox();

};


bool BoneGeometryDrawOverride::hasUIDrawables() const
/**
In order for any override for the addUIDrawables() method to be called this method must also be overridden to return true.
This method should not be overridden if addUIDrawables() has not also been overridden as there may be associated wasted overhead.

@return: bool
*/
{

	return true;

};


MUserData* BoneGeometryDrawOverride::prepareForDraw(const MDagPath& objPath, const MDagPath& cameraPath, const MHWRender::MFrameContext& frameContext, MUserData* userData)
/**
Called by Maya whenever the object is dirty and needs to update for draw.
Any data needed from the Maya dependency graph must be retrieved and cached in this stage.
It is invalid to pull data from the Maya dependency graph in the draw callback method and Maya may become unstable if that is attempted.

@param objPath: The path to the object being drawn.
@param cameraPath: The path to the camera that is being used to draw.
@param frameContext: Frame level context information.
@param userData: Data cached by the previous draw of the instance.
@return: MUserData
*/
{

	MStatus status;

	// Check if an instance of PointHelperData exists
	//
	BoneGeometryData* boneGeometryData = dynamic_cast<BoneGeometryData*>(userData);

	if (boneGeometryData == nullptr)
	{

		boneGeometryData = new BoneGeometryData();

	}

	// Cache internal values
	//
	*boneGeometryData = this->boneGeometry->getUserData();
	boneGeometryData->copyWireColor(objPath);
	boneGeometryData->copyDepthPriority(objPath);

	// Compute mesh points
	//
	double width = boneGeometryData->width;
	double height = boneGeometryData->height;
	double taper = boneGeometryData->taper;

	double minLength = (width > height) ? width : height;
	double length = boneGeometryData->length > minLength ? boneGeometryData->length : minLength;

	MMatrix baseMatrix = Drawable::createScaleMatrix(width, height, width);
	MMatrix taperMatrix = Drawable::createScaleMatrix((length * 2.0), (1.0 - taper), (1.0 - taper));
	MMatrix objectMatrix = boneGeometryData->objectMatrix;

	MPointArray points = MPointArray(9, MPoint::origin);
	points[0] = MPoint(0.0, 0.0, 0.0);
	points[1] = MPoint(0.5, -0.5, 0.5) * baseMatrix * objectMatrix;
	points[2] = MPoint(0.5, 0.5, 0.5) * baseMatrix * objectMatrix;
	points[3] = MPoint(0.5, 0.5, -0.5) * baseMatrix * objectMatrix;
	points[4] = MPoint(0.5, -0.5, -0.5) * baseMatrix * objectMatrix;
	points[5] = MPoint(0.5, -0.5, 0.5) * taperMatrix * objectMatrix;
	points[6] = MPoint(0.5, 0.5, 0.5) * taperMatrix * objectMatrix;
	points[7] = MPoint(0.5, 0.5, -0.5) * taperMatrix * objectMatrix;
	points[8] = MPoint(0.5, -0.5, -0.5) * taperMatrix * objectMatrix;

	// Create mesh data
	//
	MIntArray polygonCounts = MIntArray(BoneGeometryDrawOverride::POLYGON_COUNTS, 9);
	MIntArray polygonConnects = MIntArray(BoneGeometryDrawOverride::POLYGON_CONNECTS, 32);
	MIntArray edgeSmoothings = MIntArray(16, 0);

	this->meshData = Drawable::createMeshData(points, polygonCounts, polygonConnects, edgeSmoothings, &status);
	CHECK_MSTATUS_AND_RETURN(status, boneGeometryData);

	status = Drawable::getTriangles(this->meshData, this->triangles, this->normals);
	CHECK_MSTATUS_AND_RETURN(status, boneGeometryData);

	status = Drawable::getLines(this->meshData, this->lines);
	CHECK_MSTATUS_AND_RETURN(status, boneGeometryData);

	return boneGeometryData;

};


void BoneGeometryDrawOverride::addUIDrawables(const MDagPath& objPath, MHWRender::MUIDrawManager& drawManager, const MHWRender::MFrameContext& frameContext, const MUserData* userData)
/**
Provides access to the MUIDrawManager, which can be used to queue up operations to draw simple UI shapes like lines, circles, text, etc.
This method will only be called when this override is dirty and its hasUIDrawables() is overridden to return true.
This method is called after prepareForDraw() and carries the same restrictions on the sorts of operations it can perform.

@param objPath: The path to the object being drawn.
@param cameraPath: The path to the camera that is being used to draw.
@param drawManager: The UI draw manager, it can be used to draw some simple geometry including text.
@param frameContext: Frame level context information.
@param userData: Data cached by prepareForDraw().
@return: void
*/
{

	// Retrieve the cached data from the prepareForDraw() method
	//
	BoneGeometryData* boneGeometryData = (BoneGeometryData*)userData;

	if (!boneGeometryData)
	{

		return;

	}

	// Begin drawable
	//
	drawManager.beginDrawable();

	// Edit draw properties
	//
	drawManager.setColor(boneGeometryData->wireColor);
	drawManager.setDepthPriority(boneGeometryData->depthPriority);
	drawManager.setLineWidth(1.0);
	drawManager.setLineStyle(MHWRender::MUIDrawManager::kSolid);
	
	// Draw bone geometry
	//
	drawManager.setPaintStyle(MHWRender::MUIDrawManager::kShaded);
	drawManager.mesh(MHWRender::MUIDrawManager::kTriangles, this->triangles, &this->normals);
	drawManager.mesh(MHWRender::MUIDrawManager::kLines, this->lines);

	// End drawable
	//
	drawManager.endDrawable();

};


bool BoneGeometryDrawOverride::traceCallSequence() const
/**
This method allows a way for a plug-in to examine the basic call sequence for a draw override.
The default implementation returns false meaning no tracing will occur.

@return: bool
*/
{

	return false; // Toggle for debugging!!!

};


void BoneGeometryDrawOverride::handleTraceMessage(const MString& message) const
/**
When debug tracing is enabled via MPxDrawOverride::traceCallSequence(), this method will be called for each trace message.
The default implementation will print the message to stderr.

@param message: A string which will provide feedback on either an internal or plug-in call location.
@return: void
*/
{

	MGlobal::displayInfo("BoneGeometryDrawOverride::handleTraceMessage() : " + message);

}