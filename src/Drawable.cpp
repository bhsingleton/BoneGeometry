//
// File: Drawable.cpp
//
// Author: Ben Singleton
//

#include "Drawable.h"


unsigned int Drawable::sum(const MIntArray& values)
/**
Function similar to the builtin method in python used to add up all of the values inside an array.
Would be nice to expand support for this but this is all we really need right now.

@param values: Sequence of integers.
@return: Total sum of all values inside array.
*/
{

	// Iterate through values
	//
	unsigned int total = 0;

	for (unsigned int i = 0; i < values.length(); i++)
	{

		total += values[i];

	}

	return total;

};


MIntArray Drawable::range(int start, int end, int increment)
/**
Function similar to the builtin method in python used to construct an array of integers.
This is not a 1:1 copy, there is no error checking, so please check your arguments!

@param start: Value to start at.
@param end: Value to end at.
@param increment: Value to increment by.
@return: A squence of integers.
*/
{

	// Calculate length of array
	//
	unsigned int length = (end - start) / increment;
	MIntArray values(length, 0);

	for (unsigned int i = 0; i < length; i += increment)
	{

		values[i] = start + i;

	}

	return values;

};


MPointArray Drawable::chain(const std::vector<MPointArray>& points)
/**
Function used to take a 2-dimensional array of points and collapse it into a single array.
If you're planning on merging seperate line lists make sure to stagger them before combining!

@param points: A 2D array of points.
@return: A flattened array.
*/
{

	// Get sum of arrays
	//
	int length = 0;

	for (size_t i = 0; i < points.size(); i++)
	{

		length += points[i].length();

	}

	// Pre-allocate space for all points
	//
	MPointArray newPoints(length, MPoint::origin);
	int counter = 0;

	for (size_t i = 0; i < points.size(); i++)
	{

		// Assign points to array
		//
		for (unsigned int j = 0; j < points[i].length(); j++)
		{

			newPoints[counter + j] = points[i][j];

		}

		counter += points[i].length();

	}

	return newPoints;

};


MPointArray Drawable::stagger(const MPointArray& points)
/**
Static function used to take a series of points and stagger them to form a line list.
For example: x[4] = {a, b, c, d}; becomes y[6] = {a, b, b, c, c, d};

@param points: A series of sequential points.
@return: An array of points suitable to pass as a line list.
*/
{

	// Calculate new length
	//
	unsigned int length = points.length();
	unsigned int newLength = (length - 1) * 2;

	MPointArray newPoints(newLength, MPoint::origin);

	// Iterate through points
	//
	unsigned int i, j;

	for (i = 0, j = 0; i < (length - 1); i++, j += 2)
	{

		newPoints[j] = points[i];
		newPoints[j + 1] = points[i + 1];

	}

	return newPoints;

};


MPointArray Drawable::stagger(const std::vector<MPointArray>& points)
/**
Static function used to take a series of 2-Dimensional arrays and stagger them to form a line list.
For example: x[4] = {a, b, c, d}; becomes y[6] = {a, b, b, c, c, d};

@param points: A series of sequential points.
@return: An array of points suitable to pass as a line list.
*/
{

	// Calculate new length
	//
	size_t size = points.size();
	std::vector<MPointArray> newPoints(size);

	for (size_t i = 0; i < points.size(); i++)
	{

		newPoints[i] = Drawable::stagger(points[i]);

	}

	return Drawable::chain(newPoints);

};


MMatrix Drawable::composeMatrix(const MVector& center, const MVector& normal, const MVector& up, const MVector& scale)
/**
Returns a transformation matrix using a position, forward/up vector and scale.

@param center: Center of transform.
@param normal: Vector assigned to x-axis.
@param up: Secondary vector used in cross product.
@param scale: Local space scale.
@return: MMatrix
*/
{

	// Compose matrix list from arguments
	//
	MVector xAxis = normal.normal() * scale[0];
	MVector zAxis = (normal ^ up).normal() * scale[1];
	MVector yAxis = (zAxis ^ xAxis).normal() * scale[2];

	double matrixList[4][4] =
	{
		{ xAxis.x, xAxis.y, xAxis.z, 0.0 },
		{ yAxis.x, yAxis.y, yAxis.z, 0.0 },
		{ zAxis.x, zAxis.y, zAxis.z, 0.0 },
		{ center.x, center.y, center.z, 1.0 }
	};

	return MMatrix(matrixList);

};


MMatrix Drawable::composeMatrix(const MVector& center, const MEulerRotation& eulerRotation, const MVector& scale)
/**
Function used to compose a transformation matrix from a position, rotation and scale.

@param center: Center of transform.
@param rotation: Euler rotation angles.
@param rotateOrder: Order of rotation.
@param scale: Local space scale.
@return: MMatrix
*/
{

	MMatrix positionMatrix = Drawable::createPositionMatrix(center);
	MMatrix rotationMatrix = eulerRotation.asMatrix();
	MMatrix scaleMatrix = Drawable::createScaleMatrix(scale);

	return scaleMatrix * rotationMatrix * positionMatrix;

};


void Drawable::decomposePosition(const MMatrix& matrix, MPoint& position)
/**
Decomposes the supplied transform matrix into the passed point.

@param matrix: The matrix to decompose.
@param scale: The passed point to populate.
@return: Null.
*/
{

	position.x = matrix(3, 0);
	position.y = matrix(3, 1);
	position.z = matrix(3, 2);
	position.w = matrix(3, 3);

};


void Drawable::decomposeRotation(const MMatrix& matrix, MEulerRotation& eulerRotation)
/**
Decomposes the supplied transform matrix into the passed euler rotation.

@param matrix: The matrix to decompose.
@param scale: The passed euler rotation to populate.
@return: Null.
*/
{

	eulerRotation = matrix;

};


void Drawable::decomposeScale(const MMatrix& matrix, MVector& scale)
/**
Decomposes the supplied transform matrix into the passed scale vector.

@param matrix: The matrix to decompose.
@param scale: The passed scale vector to populate.
@return: Null.
*/
{

	scale.x = MVector(matrix(0, 0), matrix(0, 1), matrix(0, 2)).length();
	scale.y = MVector(matrix(1, 0), matrix(1, 1), matrix(1, 2)).length();
	scale.z = MVector(matrix(2, 0), matrix(2, 1), matrix(2, 2)).length();

};


void Drawable::decomposeMatrix(const MMatrix& matrix, MPoint& position, MEulerRotation& eulerRotation, MVector& scale)
/**
Decomposes a matrix into it's translate, rotate and scale components.

@param matrix: The matrix to decompose.
@param translation: The passed point to populate.
@param eulerRotation: The passed euler rotation to populate.
@param scale: The passed scale vector to populate.
@return: Null.
*/
{

	Drawable::decomposePosition(matrix, position);
	Drawable::decomposeRotation(matrix, eulerRotation);
	Drawable::decomposeScale(matrix, scale);

};


void Drawable::decomposeMatrix(const MMatrix& matrix, MVector& xAxis, MVector& yAxis, MVector& zAxis, MPoint& position)
/**
Decomposes a matrix into it's axis vectors and position.

@param matrix: The matrix to decompose.
@param xAxis: The passed X axis vector to populate.
@param yAxis: The passed Y axis vector to populate.
@param zAxis: The passed Z axis vector to populate.
@param position: The passed point to populate
@return: Null.
*/
{

	xAxis = MVector(matrix(0, 0), matrix(0, 1), matrix(0, 2));
	yAxis = MVector(matrix(1, 0), matrix(1, 1), matrix(1, 2));
	zAxis = MVector(matrix(2, 0), matrix(2, 1), matrix(2, 2));
	position = MPoint(matrix(3, 0), matrix(3, 1), matrix(3, 2), 1.0);

};


MMatrix Drawable::createPositionMatrix(const double x, const double y, const double z)
/**
Returns a position matrix from the supplied x, y and z values.

@param x: The x value.
@param y: The y value.
@param z: The z value.
@return: The position matrix.
*/
{

	double rows[4][4] =
	{
		{ 1.0, 0.0, 0.0, 0.0 },
		{ 0.0, 1.0, 0.0, 0.0 },
		{ 0.0, 0.0, 1.0, 0.0 },
		{ x, y, z, 1.0 }
	};

	return MMatrix(rows);

};


MMatrix Drawable::createPositionMatrix(const MVector& vector)
/**
Returns a position matrix from the supplied vector.

@param vector: The position vector.
@return: The position matrix.
*/
{

	return Drawable::createPositionMatrix(vector.x, vector.y, vector.z);

};


MMatrix Drawable::createPositionMatrix(const MPoint& point)
/**
Returns a position matrix from the supplied point.

@param point: The position point.
@return: The position matrix.
*/
{

	return Drawable::createPositionMatrix(point.x, point.y, point.z);

};


MMatrix Drawable::createScaleMatrix(const double x, const double y, const double z)
/**
Returns a scale matrix from the supplied x, y and z values.

@param x: The x value.
@param y: The y value.
@param z: The z value.
@return: The scale matrix.
*/
{

	double rows[4][4] =
	{
		{ x, 0.0, 0.0, 0.0 },
		{ 0.0, y, 0.0, 0.0 },
		{ 0.0, 0.0, z, 0.0 },
		{ 0.0, 0.0, 0.0, 1.0 }
	};

	return MMatrix(rows);

};


MMatrix Drawable::createScaleMatrix(const double scale)
/**
Returns a uniform scale matrix from the supplied global scale value.

@param scale: The global scale value.
@return: The scale matrix.
*/
{

	return Drawable::createScaleMatrix(scale, scale, scale);

};


MMatrix Drawable::createScaleMatrix(const MVector& scale)
/**
Returns a scale matrix from the supplied vector.

@param scale: The scale vector.
@return: The scale matrix.
*/
{

	return Drawable::createScaleMatrix(scale.x, scale.y, scale.z);

};


void Drawable::transform(const MMatrix& matrix, MPointArray& points)
/**
Function used to transform an array of points using the supplied transform matrix.
This is an in place multiplication performed on a passed array so the original values will be lost!

@param matrix: Transform matrix.
@param points: A passed array of points to be multiplied.
@return: void
*/
{

	// Iterate through points
	//
	unsigned int numPoints = points.length();

	for (unsigned int i = 0; i < numPoints; i++)
	{

		points[i] *= matrix;

	}

};


MPointArray Drawable::transform(const MMatrix& matrix, const double points[][4], const int numPoints)
/**
Function used to transform an array of points using the supplied transform matrix.
Unlike the other function, this method will return a brand new array.

@param matrix: Transform matrix.
@param points: An array of points to be multiplied.
@param points: Number of points inside the array.
@return: MPointArray
*/
{

	// Initialize new point array
	//
	MPointArray newPoints(numPoints, MPoint::origin);

	for (int i = 0; i < numPoints; i++)
	{

		newPoints[i] = MPoint(points[i]) * matrix;

	}

	return newPoints;

};


MPointArray Drawable::line(const MPoint& start, const MPoint& end)
/**
A convenience function used to quickly create a point array for a set of line points.

@param start: The start point on the line.
@param end: The end point on the line.
@return: An array of points that make up a line.
*/
{

	// Define point array
	//
	MPointArray points(2, MPoint::origin);
	points[0] = start;
	points[1] = end;

	return points;

};


MPointArray	Drawable::arc(const MVector& center, const MVector& normal, const double radius, const double startAngle, const double endAngle, const int numPoints)
/**
Static function used to quickly generate an arc that can be used as a line strip for drawables.
By default all points are calculated on the yz plane using x as the normal vector.
The number of divisions dictates the number of points returned.

@param center: The center of this arc.
@param normal: The forward vector for this arc.
@param startAngle: The angle to start iterating from in degrees.
@param endAngle: The angle to stop iterating at in degrees.
@param steps: The number of points that make up this arc.
@return: An array of points in object space.
*/
{

	// Derive transform matrix from normal
	//
	MQuaternion quat = MVector::xAxis.rotateTo(normal);
	MMatrix matrix = quat.asMatrix();

	// Iterate through angle range
	//
	MPointArray points(numPoints, MPoint::origin);

	double fraction = 1.0 / (numPoints - 1);
	double step = (endAngle - startAngle) * fraction;
	double y, z, angle;
	MPoint point;

	for (int i = 0; i < numPoints; i++)
	{

		// Calculate 2D co-ordinates
		//
		angle = startAngle + (step * static_cast<double>(i));

		y = radius * sin(angle * (PI / 180.0));
		z = radius * cos(angle * (PI / 180.0));

		// Convert co-ordinates into 3D space
		//
		point = MPoint(0.0, y, z, 1.0);
		point *= matrix;
		point += center;

		points[i] = point;

	}

	return points;

};


MPointArray Drawable::square(const MVector& center, const MVector& normal)
/**
Static function used to generate a square for drawables.
The diagonal of a unit square is expressed through the square root of 2.

@param center: The center of this square.
@param normal: The forward vector for this square.
@return: An array of points in object space.
*/
{

	return Drawable::arc(center, normal, UNIT_SQUARE_RADIUS, 45, 405, 5);

};


MPointArray	Drawable::circle(const MVector& center, const MVector& normal, const double radius, const int numPoints)
/**
Static function used to quickly generate a list of points that make up a circle.

@param center: The center of this circle.
@param normal: The forward vector for this circle.
@param radius: The radius of this circle.
@param numPoints: The number of vertices for this circle.
@return: An array of points in object space.
*/
{

	return Drawable::arc(center, normal, radius, 0, 360, numPoints);

};


MObject Drawable::sphere(const MVector& center, const double radius, const int subdivisionAxis, const int subdivisionHeight, MObject& parent)
/**
Creates a sphere primitive in the form of a mesh data object.

@param center: The center of the sphere.
@param radius: The radius of the sphere.
@param subdivisionAxis: The number of faces around the x-axis.
@param subdivisionHeight: The number of faces along the x-axis.
@param parent: The parent container to store this data.
@return: MObject
*/
{

	MStatus status;

	// Check if parent has been initialized
	//
	if (parent.apiType() != MFn::kMeshData)
	{

		MFnMeshData fnMeshData;
		parent = fnMeshData.create();

	}

	// Assign mesh data to function set
	//
	MFnMesh fnMesh;

	status = fnMesh.setObject(parent);
	CHECK_MSTATUS(status);

	// Add polygons to mesh data
	//
	double fraction = 1.0 / static_cast<double>(subdivisionHeight);
	double endAngle = 360.0 - (360.0 / static_cast<double>(subdivisionAxis));

	double x1, x2, y1, y2;
	int startIndex, endIndex;
	MPointArray p1, p2;
	MPointArray polygon;

	for (int i = 0; i < subdivisionHeight; i++)
	{

		// Get center and radius of edge loops
		//
		x1 = radius * cos((fraction * static_cast<double>(i)) * Drawable::PI);
		y1 = radius * sin((fraction * static_cast<double>(i)) * Drawable::PI);

		x2 = radius * cos((fraction * static_cast<double>(i + 1)) * Drawable::PI);
		y2 = radius * sin((fraction * static_cast<double>(i + 1)) * Drawable::PI);

		// Get edge loop points
		//
		p1 = Drawable::arc(MVector(center.x + x1, center.y, center.z), MVector::xAxis, y1, 0.0, endAngle, subdivisionAxis);
		p2 = Drawable::arc(MVector(center.x + x2, center.y, center.z), MVector::xAxis, y2, 0.0, endAngle, subdivisionAxis);

		for (int j = 0; j < subdivisionAxis; j++)
		{

			// Determine start and end index
			//
			startIndex = j;
			endIndex = (j < (subdivisionAxis - 1)) ? (startIndex + 1) : 0;

			if (i == 0)
			{

				polygon = MPointArray(3);
				polygon[0] = p1[startIndex];
				polygon[1] = p2[endIndex];
				polygon[2] = p2[startIndex];

			}
			else if (i < (subdivisionHeight - 1))
			{

				polygon = MPointArray(4);
				polygon[0] = p1[startIndex];
				polygon[1] = p1[endIndex];
				polygon[2] = p2[endIndex];
				polygon[3] = p2[startIndex];

			}
			else
			{

				polygon = MPointArray(3);
				polygon[0] = p1[startIndex];
				polygon[1] = p1[endIndex];
				polygon[2] = p2[startIndex];

			}

			// Add polygon to mesh
			//
			fnMesh.addPolygon(polygon, true, MERGE_THRESHOLD, parent, &status);
			CHECK_MSTATUS(status);

		}

	}

	return parent;

};


MObject Drawable::disc(const MVector& center, const MVector& normal, const double radius, const int subdivisionAxis, MObject& parent)
/**
Static function used to generate a disc for drawables.

@param center: The center of this disc.
@param normal: The forward vector for this sector.
@param radius: The radius of this disc.
@param subdivisionAxis: The number of faces around the x-axis.
@param parent: The parent container to store this data.
@return: An array of points in object space.
*/
{

	return Drawable::sector(center, normal, radius, 0, 360, subdivisionAxis, parent);

};


MObject Drawable::sector(const MVector& center, const MVector& normal, const double radius, const double startAngle, const double endAngle, const int subdivisionAxis, MObject& parent)
/**
Static function used to generate a sector for drawables.
This is essentially a disc but with a start and end angle.

@param center: The center of this sector.
@param normal: The forward vector for this sector.
@param radius: The radius of this sector.
@param startAngle: The start angle of this sector.
@param endAngle: The end angle of this sector.
@param subdivisionAxis: The number of faces around the x-axis.
@param parent: The parent container to store this data.
@return: An array of points in object space.
*/
{

	MStatus status;

	// Check if parent has been initialized
	//
	if (parent.apiType() != MFn::kMeshData)
	{

		MFnMeshData fnMeshData;
		parent = fnMeshData.create();

	}

	// Initialize function set
	//
	MFnMesh fnMesh(parent, &status);
	CHECK_MSTATUS(status);

	// Create point array
	//
	MPointArray points = Drawable::arc(center, normal, radius, startAngle, endAngle, subdivisionAxis + 1);
	unsigned int numPoints = points.length();

	MPointArray polygon(3, MPoint::origin);

	for (unsigned int i = 0; i < (numPoints - 1); i++)
	{

		// Add points to polygon
		//
		polygon[0] = points[i];
		polygon[1] = points[i + 1];
		polygon[2] = center;

		// Create polygon
		//
		fnMesh.addPolygon(polygon, true, Drawable::MERGE_THRESHOLD, parent, &status);
		CHECK_MSTATUS(status);

	}

	return parent;

};


MObject Drawable::cylinder(const MVector& center, const MVector& normal, const double radius, const double length, const int subdivisionAxis, MObject& parent)
/**
Static function used to generate a cylinder for drawables.

@param center: The center of this cylinder.
@param normal: The forward vector for this cylinder.
@param radius: The radius of this cylinder.
@param length: The length of this cylinder.
@param subdivisionAxis: The number of faces around the x-axis.
@param parent: The parent container to store this data.
@return: An array of points in object space.
*/
{

	MStatus status;

	// Check if parent has been initialized
	//
	if (parent.apiType() != MFn::kMeshData)
	{

		MFnMeshData fnMeshData;
		parent = fnMeshData.create();

	}

	// Initialize function set
	//
	MFnMesh fnMesh(parent, &status);
	CHECK_MSTATUS(status);

	// Create point array
	//
	MPointArray points = Drawable::circle(center, normal, radius, subdivisionAxis + 1);
	unsigned int numPoints = points.length();

	MPointArray frontCap(3, MPoint::origin);
	MPointArray backCap(3, MPoint::origin);
	MPointArray bridge(4, MPoint::origin);

	double halfLength = length * 0.5;

	for (unsigned int i = 0; i < (numPoints - 1); i++)
	{

		// Add fron cap segment
		//
		frontCap[0] = points[i] + (normal * halfLength);
		frontCap[1] = points[i + 1] + (normal * halfLength);
		frontCap[2] = center + (normal * halfLength);

		fnMesh.addPolygon(frontCap, true, MERGE_THRESHOLD, parent, &status);
		CHECK_MSTATUS(status);

		// Add back cap segment
		//
		backCap[0] = points[i] + (-normal * halfLength);
		backCap[1] = points[i + 1] + (-normal * halfLength);
		backCap[2] = center + (-normal * halfLength);

		fnMesh.addPolygon(backCap, true, MERGE_THRESHOLD, parent, &status);
		CHECK_MSTATUS(status);

		// Bridge caps
		//
		bridge[0] = points[i] + (normal * halfLength);
		bridge[1] = points[i + 1] + (normal * halfLength);
		bridge[2] = points[i + 1] + (-normal * halfLength);;
		bridge[3] = points[i] + (-normal * halfLength);

		fnMesh.addPolygon(bridge, true, MERGE_THRESHOLD, parent, &status);
		CHECK_MSTATUS(status);

	}

	// Auto smooth primitive
	//
	status = Drawable::autoSmoothEdges(parent);
	CHECK_MSTATUS(status);

	return parent;

};


MObject Drawable::createMeshData(const MPointArray& points, const MIntArray& polygonCounts, const MIntArray& polygonConnects, MStatus* status)
/**
Static function used to create a new mesh data object based on the supplied constructors.
No error checking is performed to test if the incoming data is valid!

@param points: List of vertex points.
@param polygonCounts: List of number of vertices per polygon face.
@param polygonConnects: List of vertices that make up face vertices.
@param status: Return status.
@return: MObject
*/
{

	// Create new mesh data object
	//
	MFnMeshData fnMeshData;

	MObject meshData = fnMeshData.create(status);
	CHECK_MSTATUS_AND_RETURN(*status, meshData);

	// Create mesh and assign to data object
	//
	MFnMesh fnMesh;

	fnMesh.create(points.length(), polygonCounts.length(), points, polygonCounts, polygonConnects, meshData, status);
	CHECK_MSTATUS_AND_RETURN(*status, meshData);

	return meshData;

};


MObject Drawable::createMeshData(const MPointArray& points, const MIntArray& polygonCounts, const MIntArray& polygonConnects, const MIntArray& smoothings, MStatus* status)
/**
Overload function used to create a new mesh data object based on the supplied constructors.
No error checking is performed to test if the incoming data is valid!

@param points: List of vertex points.
@param polygonCounts: List of number of vertices per polygon face.
@param polygonConnects: List of vertices that make up face vertices.
@param edgeSmoothings: List of smoothing values per edge.
@param status: Return status.
@return: MObject
*/
{

	// Call base function
	//
	MObject meshData = Drawable::createMeshData(points, polygonCounts, polygonConnects, status);
	CHECK_MSTATUS_AND_RETURN(*status, meshData);

	// Initialize function set
	//
	MFnMesh fnMesh(meshData, status);
	CHECK_MSTATUS_AND_RETURN(*status, meshData);

	// Define element and smoothing arrays
	//
	int numEdges = smoothings.length();
	MIntArray elements = Drawable::range(0, numEdges, 1);

	// Assign edge smoothing values
	//
	*status = fnMesh.setEdgeSmoothings(elements, smoothings);
	CHECK_MSTATUS_AND_RETURN(*status, meshData);

	// Cleanup edge smoothings
	//
	*status = fnMesh.cleanupEdgeSmoothing();
	CHECK_MSTATUS_AND_RETURN(*status, meshData);

	return meshData;

};


MObject Drawable::copyMeshData(const MObject& source, const MMatrix& matrix, MStatus* status)
/**
Copies the supplied mesh data while simultaneously transforming the control points.

@param source: The source mesh data to copy.
@param matrix: The transformation matrix.
@param status: Return status.
@return: The copied mesh data.
*/
{

	// Create new mesh data object
	//
	MFnMeshData fnMeshData;

	MObject meshData = fnMeshData.create(status);
	CHECK_MSTATUS_AND_RETURN(*status, meshData);

	// Copy source mesh to data object
	//
	MFnMesh fnMesh;

	fnMesh.copy(source, meshData, status);
	CHECK_MSTATUS_AND_RETURN(*status, meshData);

	// Get control points
	//
	unsigned int numVertices = fnMesh.numVertices();
	MPointArray points = MPointArray(numVertices);

	*status = fnMesh.getPoints(points);
	CHECK_MSTATUS_AND_RETURN(*status, meshData);

	// Modify control points
	//
	Drawable::transform(matrix, points);

	*status = fnMesh.setPoints(points);
	CHECK_MSTATUS_AND_RETURN(*status, meshData);

	return meshData;

};


MObject Drawable::createComponent(const MFn::Type componentType, MIntArray& elements, MStatus* status)
/**
Static function used to create a component object from a list of elements.

@param type: Component type to create.
@param elements: List of indexed elements to add to component.
@param status: Return status.
@return: MObject
*/
{

	// Initialize component object
	//
	MFnSingleIndexedComponent fnSingleIndexedComponent;

	MObject component = fnSingleIndexedComponent.create(componentType, status);
	CHECK_MSTATUS_AND_RETURN(*status, component);

	// Add elements to component
	//
	*status = fnSingleIndexedComponent.addElements(elements);
	CHECK_MSTATUS_AND_RETURN(*status, component);

	return component;

};


MObject Drawable::createComponent(const MFn::Type componentType, const int count, MStatus* status)
/**
Static function used to create a component object from a list of elements.

@param type: Component type to create.
@param elements: List of indexed elements to add to component.
@param status: Return status.
@return: MObject
*/
{

	// Initialize component object
	//
	MFnSingleIndexedComponent fnSingleIndexedComponent;

	MObject component = fnSingleIndexedComponent.create(componentType, status);
	CHECK_MSTATUS_AND_RETURN(*status, component);

	// Add elements to component
	//
	MIntArray elements = Drawable::range(0, count, 1);

	*status = fnSingleIndexedComponent.addElements(elements);
	CHECK_MSTATUS_AND_RETURN(*status, component);

	return component;

};


MStatus Drawable::getPoints(const MObject& meshData, MPointArray& points)
/**
Static function used to populate a point array from a mesh data object.

@param meshData: Mesh data object to copy points from.
@param matrix: Point array to copy points to.
@return: Status.
*/
{

	MStatus status;

	// Initialize function set
	//
	MFnMesh fnMesh(meshData, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Copy point data over to array
	//
	int numVertices = fnMesh.numVertices(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = fnMesh.getPoints(points);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return MS::kSuccess;

};


MStatus Drawable::getLines(const MObject& meshData, MPointArray& lines)
/**
Static function used to populate a point array from a mesh data object.

@param meshData: Mesh data object to copy points from.
@param matrix: Point array to copy points to.
@return: Status.
*/
{

	MStatus status;

	// Initialize function set
	//
	MFnMesh fnMesh(meshData, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Create complete component
	//
	int numEdges = fnMesh.numEdges(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MObject component = Drawable::createComponent(MFn::kMeshEdgeComponent, numEdges, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Call overload function
	//
	return Drawable::getLines(meshData, component, lines);

};


MStatus Drawable::getLines(const MObject& meshData, const MObject& component, MPointArray& lines)
/**
Static function used to populate a point array from a mesh data and component pair.

@param meshData: Mesh data object to copy points from.
@param matrix: Point array to copy points to.
@return: Status.
*/
{

	MStatus status;

	// Initialize function sets
	//
	MFnMesh fnMesh(meshData, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MFnSingleIndexedComponent fnComponent(component, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Resize array to fit points
	//
	int numEdges = fnComponent.elementCount(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	int length = numEdges * 2;

	status = lines.setLength(length);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Iterate through edges
	//
	int element, insertAt = 0;
	int2 edgeVertexIndices;

	for (int i = 0; i < numEdges; i++)
	{

		// Get start and end points
		//
		element = fnComponent.element(i, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		status = fnMesh.getEdgeVertices(element, edgeVertexIndices);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Add points to array
		//
		status = fnMesh.getPoint(edgeVertexIndices[0], lines[insertAt]);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		status = fnMesh.getPoint(edgeVertexIndices[1], lines[insertAt + 1]);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		insertAt += 2;

	}

	return MS::kSuccess;

};


MStatus Drawable::getLines(const MObject& meshData, MIntArray& elements, MPointArray& lines)
/**
Static function used to populate a point array from a mesh data and edge elements pair.

@param meshData: Mesh data object to copy points from.
@param matrix: Point array to copy points to.
@return: Status.
*/
{

	MStatus status;

	// Create edge component
	//
	MObject component = Drawable::createComponent(MFn::kMeshEdgeComponent, elements, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Call overload
	//
	return Drawable::getLines(meshData, component, lines);

}


MStatus Drawable::getTriangles(const MObject& meshData, MPointArray& triangles, MVectorArray& normals)
/**
Static function used to populate a point array from a mesh data object.

@param meshData: Mesh data object to copy points from.
@param matrix: Point array to copy points to.
@return: Status.
*/
{

	MStatus status;

	// Initialize function set
	//
	MFnMesh fnMesh(meshData, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Get triangles from mesh data
	//
	MIntArray triangleCounts, triangleVertices;

	status = fnMesh.getTriangles(triangleCounts, triangleVertices);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MPointArray points;

	status = fnMesh.getPoints(points);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Resize trianlge arrays
	//
	int triangleSum = Drawable::sum(triangleCounts);
	int length = triangleSum * 3;

	status = triangles.setLength(length);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = normals.setLength(length);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Iterate through polygons
	//
	MItMeshPolygon iterPolygons(meshData, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	int polygonIndex, triangleCount;
	int i0, i1, i2, counter = 0;

	for (iterPolygons.reset(); !iterPolygons.isDone(); iterPolygons.next())
	{

		// Iterate through triangles
		//
		polygonIndex = iterPolygons.index();
		triangleCount = triangleCounts[polygonIndex];

		for (int triangleIndex = 0; triangleIndex < triangleCount; triangleIndex++) {

			// Update triangle points
			//
			i0 = counter * 3;
			i1 = i0 + 1;
			i2 = i0 + 2;

			triangles[i0] = points[triangleVertices[i0]];
			triangles[i1] = points[triangleVertices[i1]];
			triangles[i2] = points[triangleVertices[i2]];

			// Update triangle normals
			//
			status = fnMesh.getFaceVertexNormal(polygonIndex, triangleVertices[i0], normals[i0], MSpace::kObject);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			status = fnMesh.getFaceVertexNormal(polygonIndex, triangleVertices[i1], normals[i1], MSpace::kObject);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			status = fnMesh.getFaceVertexNormal(polygonIndex, triangleVertices[i2], normals[i2], MSpace::kObject);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Increment insertion index
			//
			counter += 1;

		}

	}

	return MS::kSuccess;

};


MStatus Drawable::getBoundary(MObject& meshData, MIntArray& boundary)
/**
Static function used to populate an element array from a mesh data object.
Since passed arrays can be reused this function has been structured to only resize arrays when needed.

@param meshData: Mesh data object to copy points from.
@param matrix: Point array to copy points to.
@return: Return status.
*/
{

	MStatus status;

	// Initialize iterator
	//
	MItMeshEdge iterEdges(meshData, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Set length to maximum possible number of edges
	//
	int count = iterEdges.count(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = boundary.setLength(count);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Iterate through elements
	//
	bool onBoundary;
	MPoint start, end;

	int insertAt = 0;

	for (iterEdges.reset(); !iterEdges.isDone(); iterEdges.next())
	{

		// Check if edge is on boundary
		//
		onBoundary = iterEdges.onBoundary(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (onBoundary)
		{

			boundary[insertAt] = iterEdges.index(&status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			insertAt += 1;

		}

	}

	// Check if array requires pruning
	//
	if (insertAt < count) {

		status = boundary.setLength(insertAt);
		CHECK_MSTATUS_AND_RETURN_IT(status);

	}

	return MS::kSuccess;

};


double Drawable::getFaceNormalDifference(const MObject& meshData, const int polygonIndex, const int otherPolygonIndex, MStatus* status)
/**
Gets the angle between two face normals.

@param meshData: Mesh data object.
@param polygonIndex: A polygon index.
@param otherPolygonIndex: The other polygon index.
@param status: MStatus
@return: Angle in degrees.
*/
{

	// Initialize function set
	//
	double angle = 0.0;

	MFnMesh fnMesh(meshData, status);
	CHECK_MSTATUS_AND_RETURN(*status, angle);

	// Get polygon normal
	//
	MVector normal;

	*status = fnMesh.getPolygonNormal(polygonIndex, normal, MSpace::kObject);
	CHECK_MSTATUS_AND_RETURN(*status, angle);

	// Get other polygon normal
	//
	MVector otherNormal;

	*status = fnMesh.getPolygonNormal(otherPolygonIndex, otherNormal, MSpace::kObject);
	CHECK_MSTATUS_AND_RETURN(*status, angle);

	// Return angle in degrees
	//
	return normal.angle(otherNormal) * (180.0 / PI);

};


MStatus Drawable::autoSmoothEdges(MObject& meshData)
/**
Auto smooths the edges on the supplied mesh based on a 45 degree angle.

@param meshData: The mesh data object to auto smooth.
@return: MStatus
*/
{

	MStatus status;

	// Initialize iterator
	//
	MItMeshEdge iterEdges(meshData, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Iterate through edges
	//
	int numConnectedFaces;
	MIntArray connectedFaces;

	double normalAngle;

	for (iterEdges.reset(); !iterEdges.isDone(); iterEdges.next())
	{

		// Get connected faces
		//
		numConnectedFaces = iterEdges.getConnectedFaces(connectedFaces, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		switch (numConnectedFaces)
		{

			case 1:
			{

				// Assume hard edge for boundaries
				//
				iterEdges.setSmoothing(false);
				break;

			}

			case 2:
			{

				// Calculate normal difference to determine edge smoothing
				//
				normalAngle = Drawable::getFaceNormalDifference(meshData, connectedFaces[0], connectedFaces[1], &status);
				CHECK_MSTATUS_AND_RETURN_IT(status);

				if (normalAngle > 45.0)
				{

					iterEdges.setSmoothing(false);

				}
				else
				{

					iterEdges.setSmoothing(true);

				}

				break;

			}

			default:
			{

				break;

			}

		}

	}

	return MS::kSuccess;

};