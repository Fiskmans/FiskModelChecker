
#include "Vertex.h"

#include <algorithm>

#include <cassert>

namespace fisk {

	void VertexView::Paint()
	{
		myPosition.Paint();
	}

	std::vector<ShaderInputMapping> VertexView::Layout()
	{
		std::vector<ShaderInputMapping> out;

		out.push_back(ShaderInputMapping("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT));

		return out;
	}
}