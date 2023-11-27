
#include "GraphicsFramework.h"
#include "ImguiHelper.h"
#include "MatrixUtils.h"
#include "Model.h"
#include "RenderCall.h"
#include "ShaderBuffer.h"
#include "Shaders.h"
#include "VertexBuffer.h"
#include "Window.h"

#include "tools/Logger.h"

#include "imgui/imgui.h"

#include <vector>
#include <chrono>

int main(int argc, char** argv)
{
	fisk::Vertex::Validate();

	fisk::tools::SetFilter(fisk::tools::Type::All);
	fisk::tools::SetHalting(fisk::tools::Type::AnyError);

	

	fisk::Window window(fisk::tools::V2ui(108,902));
	fisk::GraphicsFramework graphicsFramework(window);

	fisk::ImguiHelper imguiHelper(graphicsFramework, window);

	fisk::Shaders shaders(graphicsFramework);

	fisk::COMObject<ID3D11PixelShader> pixelShader = shaders.GetPixelShaderByPath("shaders/pixel/Depth.hlsl");
	fisk::COMObject<ID3D11VertexShader> vertexShader = shaders.GetVertexShaderByPath("shaders/vertex/SimpleTransform.hlsl");
	fisk::COMObject<ID3D11InputLayout> inputLayout = shaders.GetInputLayout("shaders/vertex/SimpleTransform.hlsl", fisk::VertexView::Layout());

	fisk::COMObject<ID3D11Buffer> shaderBuffer = shaders.GetShaderBuffer(fisk::ShaderBuffer::DataSize);

	std::optional<fisk::Model> model = fisk::Model::FromFile(graphicsFramework, "models/Menger_sponge_sample.stl");

	assert(model);

	float scale = 0.f;
	float speed = 0.f;
	fisk::tools::V4f backgroundColor;
	fisk::tools::V3f offset;
	fisk::tools::V3f rotation;

	fisk::tools::V2f fov(1,1);
	float nearPlane = 1.f;
	float farPlane = 100.f;
	float perpectivness = 0.f;

	fisk::tools::EventReg imguiDragRegistration = imguiHelper.DrawImgui.Register([&scale, &speed, &backgroundColor, &offset, &rotation, &fov, &nearPlane, &farPlane, &perpectivness]() {
		ImGui::Begin("Test");

		ImGui::ColorPicker4("Background color", backgroundColor.Raw());
		ImGui::Separator();
		ImGui::DragFloat("Scale (log)", &scale, 0.001f, -3.f, 3.f);
		ImGui::DragFloat("Speed", &speed, 0.001f, -3.f, 3.f);
		ImGui::DragFloat3("Position", offset.Raw(), 0.01f);
		ImGui::DragFloat2("Fov", fov.Raw(), 0.01f);
		ImGui::DragFloat("near", &nearPlane);
		ImGui::DragFloat("far", &farPlane);
		ImGui::DragFloat("perspectiveness", &perpectivness, 0.0001f, 0.f, 1.f);

		ImGui::End();
	});

	using clock = std::chrono::steady_clock;

	clock::time_point lastFrame = clock::now();

	while (window.ProcessEvents())
	{
		clock::time_point now = clock::now();
		float dt = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1,1>>>(now - lastFrame).count();

		lastFrame = now;

		rotation[0] += dt * 0.5f * speed;
		rotation[1] += dt * 1.f * speed;
		rotation[2] += dt * 1.5f * speed;

		fisk::COMObject<ID3D11RenderTargetView> renderTarget = graphicsFramework.GetBackBufferRenderTarget();

		fisk::ShaderBuffer data;
		fisk::ShaderBufferView view = data.Structure();

		*view.myTransform =
			fisk::Matrix44FUtils::Lerp(
				fisk::Matrix44FUtils::Identity(),
				fisk::Matrix44FUtils::PerspectiveProjection(fov[0], fov[1], nearPlane, farPlane),
				perpectivness) *
			fisk::Matrix44FUtils::Translate(offset) *
			fisk::Matrix44FUtils::Scale(static_cast<float>(pow(10, scale))) *
			fisk::Matrix44FUtils::Rotate(rotation);

		fisk::ClearTexture clearTexture(renderTarget, backgroundColor);
		fisk::ClearDepth clearDepth;
		fisk::RenderModel modelRenderCall(vertexShader, pixelShader, *model, inputLayout, renderTarget, shaderBuffer, data.AsGeneric());

		clearTexture.Render(graphicsFramework);
		clearDepth.Render(graphicsFramework);
		modelRenderCall.Render(graphicsFramework);

		graphicsFramework.Present(fisk::GraphicsFramework::VSyncState::OnVerticalBlank);
	}

	system("pause");

}