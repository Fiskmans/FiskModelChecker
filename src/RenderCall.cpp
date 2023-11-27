
#include "RenderCall.h"

namespace fisk {
	
	RenderModel::RenderModel(ID3D11VertexShader* aVertexShader, ID3D11PixelShader* aPixelShader, Model& aModel, ID3D11InputLayout* aLayout, ID3D11RenderTargetView* aTarget, ID3D11Buffer* aShaderBuffer, GenericData aShaderData)
		: myVertexShader(aVertexShader)
		, myPixelShader(aPixelShader)
		, myModel(aModel)
		, myLayout(aLayout)
		, myTarget(aTarget)
		, myShaderBuffer(aShaderBuffer)
		, myShaderData(aShaderData.myData, aShaderData.myData + aShaderData.mySize)
	{
	}

	void RenderModel::Render(GraphicsFramework& aFramework)
	{
		ContextUtility context = aFramework.Context();

		context.SetTopology(ContextUtility::Topology::TriangleList);
		context.SetCulling(ContextUtility::Culling::Backface);
		context.SetBlendmode(ContextUtility::BlendMode::Opaque);

		context.SetPixelShader(myPixelShader);
		context.SetVertex(
			myVertexShader,
			myLayout,
			{ &myModel.myVertexBuffer }, 
			myModel.myIndexBuffer);

		context.SetShaderData(myShaderBuffer, myShaderData);

		context.SetRenderTarget(myTarget);

		context.Render(myModel.myIndexCount);
	}

	ClearTexture::ClearTexture(ID3D11RenderTargetView* aTarget, tools::V4f aColor)
		: myTarget(aTarget)
		, myColor(aColor)
	{
	}

	void ClearTexture::Render(GraphicsFramework& aFramework)
	{
		ContextUtility context = aFramework.Context();

		context.ClearRenderTarget(myTarget, myColor);
	}

	ClearDepth::ClearDepth()
	{
	}

	void ClearDepth::Render(GraphicsFramework& aFramework)
	{
		ContextUtility context = aFramework.Context();

		context.ClearDepth(0.f);
	}
}
