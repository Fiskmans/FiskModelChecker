#include "MatrixUtils.h"


namespace fisk
{
	tools::M44F Matrix44FUtils::Identity()
	{
		return tools::M44F(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}

	tools::M44F Matrix44FUtils::Translate(tools::V3f aVector)
	{
		return tools::M44F(
			1, 0, 0, aVector[0],
			0, 1, 0, aVector[1],
			0, 0, 1, aVector[2],
			0, 0, 0, 1);
	}

	tools::M44F Matrix44FUtils::Scale(float aUniformScale)
	{
		return tools::M44F(
			aUniformScale, 0, 0, 0,
			0, aUniformScale, 0, 0,
			0, 0, aUniformScale, 0,
			0, 0, 0, 1);
	}

	tools::M44F Matrix44FUtils::Scale(tools::V3f aScale)
	{
		return tools::M44F(
			aScale[0], 0, 0, 0,
			0, aScale[1], 0, 0,
			0, 0, aScale[2], 0,
			0, 0, 0, 1);
	}

	tools::M44F Matrix44FUtils::Rotate(tools::V3f aRotation)
	{
		return 
			RotateX(aRotation[0]) *
			RotateY(aRotation[1]) *
			RotateZ(aRotation[2]);
	}

	tools::M44F Matrix44FUtils::RotateX(float aAmount)
	{
		float s = sin(aAmount);
		float c = cos(aAmount);

		return tools::M44F(
				c, 0, s, 0,
				0, 1, 0, 0,
				-s, 0, c, 0,
				0, 0, 0, 1);
	}

	tools::M44F Matrix44FUtils::RotateY(float aAmount)
	{
		float s = sin(aAmount);
		float c = cos(aAmount);

		return tools::M44F(
			c, -s, 0, 0,
			s, c, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}

	tools::M44F Matrix44FUtils::RotateZ(float aAmount)
	{
		float s = sin(aAmount);
		float c = cos(aAmount);

		return tools::M44F(
			1, 0, 0, 0,
			0, c, -s, 0,
			0, s, c, 0,
			0, 0, 0, 1);
	}

	tools::M44F Matrix44FUtils::PerspectiveProjection(float aXFov, float aYFov, float aNearPlane, float aFarPlane)
	{
		float xScale = 1.f / tan(aXFov / 2.f);
		float yScale = 1.f / tan(aYFov / 2.f);
		float weightedFar = aFarPlane / (aNearPlane + aFarPlane);
		float weightedNear = weightedFar * aNearPlane;

		return tools::M44F(
			xScale, 0, 0, 0,
			0, yScale, 0, 0,
			0, 0, -weightedFar, -weightedNear,
			0, 0, -1, 0);
	}

	tools::M44F Matrix44FUtils::Lerp(tools::M44F aFrom, tools::M44F aTo, float aValue)
	{
		tools::M44F out;

		for (size_t y = 0; y < 4; y++)
			for (size_t x = 0; x < 4; x++)
				out.GetElement(x, y) = aFrom.GetElement(x, y) * (1.f - aValue) + aTo.GetElement(x, y) * aValue;

		return out;
	}
}