#pragma once

#include "Matrix.h"

Matrix4f CalculateLightSpaceMatrix(Vector3f CameraPos, Vector3f CameraFront, Vector3f LightDirection, float Near, float Far);