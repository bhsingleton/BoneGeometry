cmake_minimum_required(VERSION 3.21)
project(BoneGeometry)
include($ENV{DEVKIT_LOCATION}/cmake/pluginEntry.cmake)

set(
	SOURCE_FILES
	"pluginMain.cpp"
	"BoneGeometry.h"
	"BoneGeometry.cpp"
	"BoneGeometryDrawOverride.h"
	"BoneGeometryDrawOverride.cpp"
	"BoneGeometryData.h"
	"BoneGeometryData.cpp"
	"Drawable.h"
	"Drawable.cpp"
)

set(
	LIBRARIES
	OpenMaya
	OpenMayaAnim
	OpenMayaRender
	OpenMayaUI
	Foundation
)

build_plugin()