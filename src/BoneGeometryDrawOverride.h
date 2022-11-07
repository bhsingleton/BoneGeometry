#ifndef _BONE_GEOMETRY_DRAW_OVERRIDE
#define _BONE_GEOMETRY_DRAW_OVERRIDE
//
// File: BoneGeometryDrawOverride.h
//
// Author: Benjamin H. Singleton
//

#include "BoneGeometry.h"
#include "BoneGeometryData.h"
#include "Drawable.h"

#include <maya/MPxDrawOverride.h>
#include <maya/MObject.h>
#include <maya/MDagPath.h>
#include <maya/MEventMessage.h>
#include <maya/MCallbackIdArray.h>
#include <maya/MFnDependencyNode.h>

#include <maya/MViewport2Renderer.h>
#include <maya/MUIDrawManager.h>
#include <maya/MFrameContext.h>
#include <maya/MHWGeometry.h>
#include <maya/MHWGeometryUtilities.h>

#include <map>
#include <string>


class BoneGeometryDrawOverride : public MHWRender::MPxDrawOverride
{

public:

								BoneGeometryDrawOverride(const MObject& node);
	virtual						~BoneGeometryDrawOverride();

	static	MPxDrawOverride*	creator(const MObject& node);

	virtual	MHWRender::DrawAPI	supportedDrawAPIs() const;

	virtual bool				isBounded(const MDagPath& objPath, const MDagPath& cameraPath) const;
	virtual	MBoundingBox		boundingBox(const MDagPath& objPath, const MDagPath& cameraPath) const;

	virtual MUserData*			prepareForDraw(const MDagPath& objPath, const MDagPath& cameraPath, const MHWRender::MFrameContext& frameContext, MUserData* userData);

	virtual bool				hasUIDrawables() const;
	virtual void				addUIDrawables(const MDagPath& objPath, MHWRender::MUIDrawManager& drawManager, const MHWRender::MFrameContext& frameContext, const MUserData* userData);

	virtual	bool				traceCallSequence() const;
	virtual	void				handleTraceMessage(const MString& message) const;

protected:

	static	int					POLYGON_CONNECTS[32];
	static	int					POLYGON_COUNTS[9];

protected:

			BoneGeometry*		boneGeometry;

			MObject				meshData;
			MPointArray			triangles;
			MVectorArray		normals;
			MPointArray			lines;

			MCallbackId			modelEditorChangedCallbackId;
			static	void		onModelEditorChanged(void* clientData);

};

#endif