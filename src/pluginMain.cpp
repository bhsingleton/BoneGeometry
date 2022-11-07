//
// File: pluginMain.cpp
//
// Author: Ben Singleton
//

#include "BoneGeometry.h"
#include "BoneGeometryDrawOverride.h"

#include <maya/MFnPlugin.h>
#include <maya/MDrawRegistry.h>


MStatus initializePlugin(MObject obj) 
{ 

	MStatus   status;

	MFnPlugin plugin(obj, "Ben Singleton", "2017", "Any");
	status = plugin.registerNode("boneGeometry", BoneGeometry::id, &BoneGeometry::creator, &BoneGeometry::initialize, MPxNode::kLocatorNode, &BoneGeometry::drawDbClassification);
	
	if (!status) 
	{

		status.perror("registerNode");
		return status;

	}

	status = MHWRender::MDrawRegistry::registerDrawOverrideCreator(BoneGeometry::drawDbClassification, BoneGeometry::drawRegistrantId, BoneGeometryDrawOverride::creator);

	if (!status) 
	{

		status.perror("registerDrawOverrideCreator");
		return status;

	}

	return status;

}

MStatus uninitializePlugin(MObject obj) 
{

	MStatus   status;

	status = MHWRender::MDrawRegistry::deregisterDrawOverrideCreator(BoneGeometry::drawDbClassification, BoneGeometry::drawRegistrantId);

	if (!status) 
	{

		status.perror("deregisterDrawOverrideCreator");
		return status;

	}

	MFnPlugin plugin(obj);
	status = plugin.deregisterNode(BoneGeometry::id);

	if (!status) 
	{

		status.perror("deregisterNode");
		return status;

	}

	return status;

}
