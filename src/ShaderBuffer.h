
#pragma once 

#include "StructuredData.h"

#include "tools/Matrix.h"

namespace fisk {

	struct ShaderBufferView
	{
		ShaderBufferView(unsigned char* aData, size_t aDataSize)
			: myTransform(aData, aDataSize)
		{
		}

		StructuredItem<tools::M44F, 0> myTransform;

		void Paint()
		{
			myTransform.Paint();
		}
	};

	using ShaderBuffer = StructuredData<ShaderBufferView, 64>;
}