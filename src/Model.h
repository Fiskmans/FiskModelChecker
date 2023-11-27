
#pragma once

#include "tools/MathVector.h"

#include "Shaders.h"
#include "COMObject.h"
#include "GraphicsFramework.h"
#include "VertexBuffer.h"
#include "Vertex.h"

#include <vector>
#include <d3d11.h>
#include <algorithm>
#include <filesystem>

namespace fisk {


	struct Model
	{
		// TODO figure out which is which
		enum class WindingOrder
		{
			Clockwise,
			AntiClockwise
		};

		Model(GraphicsFramework& aFramework, const std::vector<Vertex>& aVertexes, const std::vector<UINT>& aIndexes, WindingOrder aWindingOrder);

		static std::optional<Model> FromFile(GraphicsFramework& aFramework, const std::filesystem::path& aPath);

		VertexBuffer myVertexBuffer;
		COMObject<ID3D11Buffer> myIndexBuffer = nullptr;

		size_t myIndexCount;
		bool myIsValid = false;

		std::vector<Vertex> myVertexes;
		std::vector<UINT> myIndexes;
	};
}