#ifndef _POINT_HELPER_DRAWABLE
#define _POINT_HELPER_DRAWABLE
//
// File: Drawable.h
//
// Author: Ben Singleton
//

#include <maya/MObject.h>
#include <maya/MDagPath.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>
#include <maya/MQuaternion.h>
#include <maya/MEulerRotation.h>
#include <maya/MMatrix.h>

#include <maya/MFn.h>
#include <maya/MFnMesh.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnSingleIndexedComponent.h>

#include <maya/MItMeshVertex.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshEdge.h>

#include <map>
#include <vector>
#include <numeric>


namespace Drawable
{

	constexpr auto	PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286;
	constexpr auto	UNIT_SQUARE_RADIUS = 0.70710678118654752440084436210485;
	constexpr auto	MERGE_THRESHOLD = 1e-3;

	unsigned int	sum(const MIntArray& values);
	MIntArray		range(int start, int end, int increment);
	
	MPointArray		chain(const std::vector<MPointArray>& points);
	
	MPointArray		stagger(const MPointArray& points);
	MPointArray		stagger(const std::vector<MPointArray>& points);
	
	MMatrix			composeMatrix(const MVector& center, const MVector& normal, const MVector& up, const MVector& scale);
	MMatrix			composeMatrix(const MVector& center, const MEulerRotation& eulerRotation, const MVector& scale);
	
	void			decomposePosition(const MMatrix& matrix, MPoint& position);
	void			decomposeRotation(const MMatrix& matrix, MEulerRotation& eulerRotation);
	void			decomposeScale(const MMatrix& matrix, MVector& scale);

	void			decomposeMatrix(const MMatrix& matrix, MPoint& position, MEulerRotation& eulerRotation, MVector& scale);
	void			decomposeMatrix(const MMatrix& matrix, MVector& xAxis, MVector& yAxis, MVector& zAxis, MPoint& position);
	
	MMatrix			createPositionMatrix(const double x, const double y, const double z);
	MMatrix			createPositionMatrix(const MVector& vector);
	MMatrix			createPositionMatrix(const MPoint& point);

	MMatrix			createScaleMatrix(const double x, const double y, const double z);
	MMatrix			createScaleMatrix(const double scale);
	MMatrix			createScaleMatrix(const MVector& scale);
	
	void			transform(const MMatrix& matrix, MPointArray& points);
	MPointArray		transform(const MMatrix& matrix, const double points[][4], const int numPoints);
	
	MPointArray		line(const MPoint& start, const MPoint& end);
	MPointArray		arc(const MVector& center, const MVector& normal, const double radius, const double startAngle, const double endAngle, const int numPoints);
	MPointArray		square(const MVector& center, const MVector& normal);
	MPointArray		circle(const MVector& center, const MVector& normal, double radius, int numPoints);
	
	MObject			sphere(const MVector& center, const double radius, const int subdivisionAxis, const int subdivisionHeight, MObject& parent);
	MObject			cylinder(const MVector& center, const MVector& normal, const double radius, const double length, const int subdivisionAxis, MObject& parent);
	MObject			disc(const MVector& center, const MVector& normal, const double radius, const int subdivisionAxis, MObject& parent);
	MObject			sector(const MVector& center, const MVector& normal, const double radius, const double startAngle, const double endAngle, const int subdivisionAxis, MObject& parent);
	
	MObject			createMeshData(const MPointArray& points, const MIntArray& polygonCounts, const MIntArray& polygonConnects, MStatus* status);
	MObject			createMeshData(const MPointArray& points, const MIntArray& polygonCounts, const MIntArray& polygonConnects, const MIntArray& edgeSmoothings, MStatus* status);
	MObject			copyMeshData(const MObject& source, const MMatrix& matrix, MStatus* status);
	
	MObject			createComponent(const MFn::Type componentType, MIntArray& elements, MStatus* status);
	MObject			createComponent(const MFn::Type componentType, const int count, MStatus* status);
	
	MStatus			getPoints(const MObject& meshData, MPointArray& points);
	MStatus			getBoundary(MObject& meshData, MIntArray& boundary);
	MStatus			getLines(const MObject& meshData, MPointArray& lines);
	MStatus			getLines(const MObject& meshData, MIntArray& elements, MPointArray& lines);
	MStatus			getLines(const MObject& meshData, const MObject& component, MPointArray& lines);
	MStatus			getTriangles(const MObject& meshData, MPointArray& triangles, MVectorArray& normals);
	
	double			getFaceNormalDifference(const MObject& meshData, const int polygonIndex, const int otherPolygonIndex, MStatus* status);
	
	MStatus			autoSmoothEdges(MObject& meshData);

};
#endif