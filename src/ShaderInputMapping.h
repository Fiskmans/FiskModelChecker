
#pragma once 

#include <string>

#include <d3d11.h>

namespace fisk {

	struct ShaderInputMapping
	{
		ShaderInputMapping(const std::string aName, size_t aOffset, DXGI_FORMAT aFormat);

		std::string myName;
		size_t myOffset;
		DXGI_FORMAT myFormat;
	};
}