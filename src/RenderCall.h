
#include "tools/Matrix.h"

#include "COMObject.h"
#include "GraphicsFramework.h"
#include "Model.h"
#include "StructuredData.h"

#include <d3d11.h>

namespace fisk 
{
	class RenderCall
	{
	public:
		virtual ~RenderCall() = default;
		virtual void Render(GraphicsFramework& aFramework) = 0;

	};

	class RenderModel : public RenderCall
	{
	public:
		RenderModel(
			ID3D11VertexShader* aVertexShader, 
			ID3D11PixelShader* aPixelShader, 
			Model& aModel, 
			ID3D11InputLayout* aLayout, 
			ID3D11RenderTargetView* aTarget,
			ID3D11Buffer* aShaderBuffer,
			GenericData aShaderData);

		void Render(GraphicsFramework& aFramework) override;

	private:
		ID3D11VertexShader* myVertexShader;
		ID3D11PixelShader* myPixelShader;
		Model& myModel;
		ID3D11InputLayout* myLayout;
		ID3D11RenderTargetView* myTarget;

		ID3D11Buffer* myShaderBuffer;
		std::vector<char> myShaderData;
	};

	class ClearTexture : public RenderCall
	{
	public:
		ClearTexture(ID3D11RenderTargetView* aTarget, tools::V4f aColor = tools::V4f(0,0,0,0));

		void Render(GraphicsFramework& aFramework) override;

	private:
		ID3D11RenderTargetView* myTarget;
		tools::V4f myColor;
	};

	class ClearDepth: public RenderCall
	{
	public:
		ClearDepth();

		void Render(GraphicsFramework& aFramework) override;

	private:
	};
}