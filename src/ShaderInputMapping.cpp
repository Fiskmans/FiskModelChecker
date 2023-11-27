
#include "ShaderInputMapping.h"

namespace fisk {

	ShaderInputMapping::ShaderInputMapping(const std::string aName, size_t aOffset, DXGI_FORMAT aFormat)
		: myName(aName)
		, myOffset(aOffset)
		, myFormat(aFormat)
	{
	}

}