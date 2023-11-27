
#pragma once 

#include "COMObject.h"
#include "ShaderInputMapping.h"
#include "StructuredData.h"

#include "tools/MathVector.h"

#include <vector>
#include <cassert>

#include <d3d11.h>

namespace fisk {

	struct VertexBuffer
	{
		COMObject<ID3D11Buffer> myRawBuffer = nullptr;
		size_t myStride = 0;
	};
}