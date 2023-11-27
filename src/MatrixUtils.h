
#pragma once

#include "tools/Matrix.h"

namespace fisk
{
	class Matrix44FUtils
	{
	public:
		static tools::M44F Identity();
		static tools::M44F Translate(tools::V3f aVector);
		static tools::M44F Scale(float aUniformScale);
		static tools::M44F Scale(tools::V3f aScale);
		static tools::M44F Rotate(tools::V3f aRotation);
		static tools::M44F RotateX(float aAmount);
		static tools::M44F RotateY(float aAmount);
		static tools::M44F RotateZ(float aAmount);
		static tools::M44F PerspectiveProjection(float aXFov, float aYFov, float aNearPlane, float aFarPlane);
		static tools::M44F Lerp(tools::M44F aFrom, tools::M44F aTo, float aValue);
	};
}