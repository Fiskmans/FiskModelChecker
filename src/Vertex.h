
#pragma once

#include "StructuredData.h"
#include "ShaderInputMapping.h"

#include "tools/MathVector.h"

#include <vector>

#include <d3d11.h>

namespace fisk {

	struct VertexView
	{
		VertexView(unsigned char* aData, size_t aDataSize)
			: myPosition(aData, aDataSize)
		{
		}

		VertexView(unsigned char* aData, size_t aDataSize, tools::V4f aPosition)
			: myPosition(aData, aDataSize)
		{
			*myPosition = aPosition;
		}

		StructuredItem<tools::V4f, 0> myPosition;


		void Paint();
		static std::vector<ShaderInputMapping> Layout();
	};

	using Vertex = StructuredData<VertexView, 16>;
}