#pragma once
#include "matrix.h"
#include"matrixfunction.h"
#include"VectorFunction.h"
#include <math.h>
#include <vector>
#include "ViewProjection.h"

Vector3 CatmullRom(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t);
Vector3 CatmullRomComplement(std::vector<Vector3> controlPoints, float t);
void DrawCurve(std::vector<Vector3> controlPoints, ViewProjection&);
