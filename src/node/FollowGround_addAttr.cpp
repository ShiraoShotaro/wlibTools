#include "FollowGround.hpp"
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MVector.h>
#include "exception/MStatusException.hpp"

////////////////////////////////////////
// _addAttr_inMesh
////////////////////////////////////////
void cma::FollowGround::_addAttr_inMesh(void) {
	MStatus stat;
	MFnTypedAttribute inMesh_attr;
	inMesh = inMesh_attr.create("inMesh", "i", MFnData::kMesh, &stat);
	MStatusException::throwIfError(stat, "inMeshアトリビュートの生成に失敗しました", "FollowGround::_addAttr_inMesh");

	inMesh_attr.setWritable(true);
	inMesh_attr.setReadable(true);
	inMesh_attr.setStorable(false);
	inMesh_attr.setConnectable(true);
	inMesh_attr.setInternal(false);
	inMesh_attr.setChannelBox(false);
	inMesh_attr.addToCategory("Node Behaving");
	
	stat = addAttribute(inMesh);
	MStatusException::throwIfError(stat, "inMeshアトリビュートの追加に失敗しました", "FollowGround::_addAttr_inMesh");
	
	stat = attributeAffects(inMesh, outPoint);
	MStatusException::throwIfError(stat, "inMeshアトリビュートにおいて、アトリビュート影響の登録に失敗しました", "FollowGround::_addAttr_inMesh");
}


////////////////////////////////////////
// _addAttr_inVector
////////////////////////////////////////
void cma::FollowGround::_addAttr_rayVector(void){
	MStatus stat;
	MFnNumericAttribute rayVector_attr;
	rayVector = rayVector_attr.createPoint("rayVector", "ray", &stat);
	MStatusException::throwIfError(stat, "rayVectorアトリビュートの生成に失敗しました", "FollowGround::_addAttr_rayVector");

	rayVector_attr.setWritable(true);
	rayVector_attr.setReadable(true);
	rayVector_attr.setStorable(false);
	rayVector_attr.setConnectable(true);
	rayVector_attr.setInternal(false);
	rayVector_attr.setChannelBox(true);	
	rayVector_attr.setDefault(1.0, 0.0, 0.0);
	rayVector_attr.addToCategory("Ray Attribute");

	stat = addAttribute(rayVector);
	MStatusException::throwIfError(stat, "rayVectorアトリビュートの追加に失敗しました", "FollowGround::_addAttr_rayVector");

	stat = attributeAffects(rayVector, outPoint);
	MStatusException::throwIfError(stat, "rayVectorアトリビュートにおいて、アトリビュート影響の登録に失敗しました", "FollowGround::_addAttr_rayVector");
}


////////////////////////////////////////
// _addAttr_outPoint
////////////////////////////////////////
void cma::FollowGround::_addAttr_outPoint(void) {
	MStatus stat;
	MFnNumericAttribute outPoint_attr;
	outPoint = outPoint_attr.createPoint("outPoint", "point", &stat);
	MStatusException::throwIfError(stat, "rayVectorアトリビュートの生成に失敗しました", "FollowGround::_addAttr_outPoint");

	outPoint_attr.setWritable(false);
	outPoint_attr.setReadable(true);
	outPoint_attr.setStorable(true);
	outPoint_attr.setConnectable(true);
	outPoint_attr.setInternal(false);
	outPoint_attr.setChannelBox(true);	
	outPoint_attr.setDefault(0.0, 0.0, 0.0);

	stat = addAttribute(outPoint);
	MStatusException::throwIfError(stat, "rayVectorアトリビュートの追加に失敗しました", "FollowGround::_addAttr_outPoint");
}

void cma::FollowGround::_addAttr_rayPoint(void) {
	MStatus stat;
	MFnNumericAttribute rayPoint_attr;
	rayPoint = rayPoint_attr.createPoint("rayPoint", "from", &stat);
	MStatusException::throwIfError(stat, "rayPointアトリビュートの生成に失敗しました", "FollowGround::_addAttr_rayPoint");

	rayPoint_attr.setWritable(true);
	rayPoint_attr.setReadable(true);
	rayPoint_attr.setStorable(false);
	rayPoint_attr.setConnectable(true);
	rayPoint_attr.setInternal(false);
	rayPoint_attr.setChannelBox(true);	
	rayPoint_attr.setDefault(0.0, 10.0, 0.0);
	rayPoint_attr.addToCategory("Ray Attribute");

	stat = addAttribute(rayPoint);
	MStatusException::throwIfError(stat, "rayPointアトリビュートの追加に失敗しました", "FollowGround::_addAttr_rayPoint");

	stat = attributeAffects(rayPoint, outPoint);
	MStatusException::throwIfError(stat, "rayPointアトリビュートにおいて、アトリビュート影響の登録に失敗しました", "FollowGround::_addAttr_rayPoint");
}

void cma::FollowGround::_addAttr_distance(void) {

	MStatus stat;
	MFnNumericAttribute distance_attr, maxDistance_attr;
	distance = distance_attr.create("distance", "distance",  MFnNumericData::kDouble, 0.0, &stat);
	MStatusException::throwIfError(stat, "distanceアトリビュートの生成に失敗しました", "FollowGround::_addAttr_distance");
	maxDistance = maxDistance_attr.create("maxDistance", "maxDistance",  MFnNumericData::kDouble, 100000.0, &stat);
	MStatusException::throwIfError(stat, "maxDistanceアトリビュートの生成に失敗しました", "FollowGround::_addAttr_distance");
	
	distance_attr.setWritable(false);
	distance_attr.setReadable(true);
	distance_attr.setStorable(true);
	distance_attr.setConnectable(true);
	distance_attr.setInternal(false);
	distance_attr.setChannelBox(true);

	maxDistance_attr.setWritable(true);
	maxDistance_attr.setReadable(true);
	maxDistance_attr.setStorable(true);
	maxDistance_attr.setConnectable(true);
	maxDistance_attr.setInternal(false);
	maxDistance_attr.setChannelBox(true);
	
	stat = addAttribute(distance);
	MStatusException::throwIfError(stat, "distanceアトリビュートの追加に失敗しました", "FollowGround::_addAttr_distance");
	stat = addAttribute(maxDistance);
	MStatusException::throwIfError(stat, "maxDistanceアトリビュートの追加に失敗しました", "FollowGround::_addAttr_distance");

	stat = attributeAffects(maxDistance, outPoint);
	MStatusException::throwIfError(stat, "maxDistanceアトリビュートにおいて、アトリビュート影響の登録に失敗しました", "FollowGround::_addAttr_distance");

}
