
#include "Shaders.h"

#include "tools/File.h"

#include <comdef.h>
#include <d3dcompiler.h>

namespace fisk
{
	Shaders::Shaders(GraphicsFramework& aFramework)
		: myFramework(aFramework)
	{

	}

	COMObject<ID3D11Buffer> Shaders::GetShaderBuffer(size_t aSize)
	{
		ID3D11Buffer* buffer;

		D3D11_BUFFER_DESC bufferDesc{};

		bufferDesc.ByteWidth = aSize;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = aSize;

		HRESULT result = myFramework.Device().CreateBuffer(&bufferDesc, nullptr, &buffer);

		if (FAILED(result))
		{
			_com_error err(result);
			LOG_ERROR("Failed to create shader buffer", err.ErrorMessage());
			return nullptr;
		}

		return buffer;
	}

	COMObject<ID3D11InputLayout> Shaders::GetInputLayout(const std::filesystem::path& aPath, const std::vector<ShaderInputMapping>& aLayout)
	{
		COMObject<ID3D11InputLayout> layout = nullptr;

		std::optional<std::vector<char>> byteCode = GetShaderByteCode(ShaderKey(aPath, ShaderType::Vertex));

		if (!byteCode)
			return std::move(layout);

		std::vector<D3D11_INPUT_ELEMENT_DESC> descriptions;

		for (const ShaderInputMapping& mapping : aLayout)
		{
			D3D11_INPUT_ELEMENT_DESC desc;

			desc.SemanticName = mapping.myName.c_str();
			desc.SemanticIndex = 0;
			desc.Format = mapping.myFormat;
			desc.InputSlot = 0;
			desc.AlignedByteOffset = (UINT)mapping.myOffset;
			desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			desc.InstanceDataStepRate = 0;

			descriptions.push_back(desc);
		}

		HRESULT result = myFramework.Device().CreateInputLayout(descriptions.data(), (UINT)descriptions.size(), byteCode->data(), byteCode->size(), &static_cast<ID3D11InputLayout*&>(layout));

		if (FAILED(result))
		{
			_com_error err(result);
			LOG_SYS_CRASH("failed to create InputLayout", err.ErrorMessage());
			return std::move(layout);
		}
		
		return std::move(layout);
	}

	COMObject<ID3D11PixelShader> Shaders::GetPixelShaderByPath(const std::filesystem::path& aPath)
	{
		COMObject<ID3D11PixelShader> shader = nullptr;

		std::optional<std::vector<char>> byteCode = GetShaderByteCode(ShaderKey(aPath, ShaderType::Pixel));

		if (!byteCode)
			return std::move(shader);

		HRESULT result = myFramework.Device().CreatePixelShader(byteCode->data(), byteCode->size(), nullptr, &static_cast<ID3D11PixelShader*&>(shader));

		if (FAILED(result))
		{
			_com_error err(result);
			LOG_SYS_CRASH("failed to create pixel shader", err.ErrorMessage());
			return std::move(shader);
		}

		return std::move(shader);
	}

	COMObject<ID3D11VertexShader> Shaders::GetVertexShaderByPath(const std::filesystem::path& aPath)
	{
		COMObject<ID3D11VertexShader> shader = nullptr;

		std::optional<std::vector<char>> byteCode = GetShaderByteCode(ShaderKey(aPath, ShaderType::Vertex));

		if (!byteCode)
			return std::move(shader);

		HRESULT result = myFramework.Device().CreateVertexShader(byteCode->data(), byteCode->size(), nullptr, &static_cast<ID3D11VertexShader*&>(shader));

		if (FAILED(result))
		{
			_com_error err(result);
			LOG_SYS_CRASH("failed to create vertex shader", err.ErrorMessage());
			return std::move(shader);
		}

		return std::move(shader);
	}

	std::optional<std::vector<char>> Shaders::GetShaderByteCode(const ShaderKey& aShaderKey)
	{
		auto cachedIt = myCachedByteCode.find(aShaderKey);
		if (cachedIt != myCachedByteCode.end())
			return cachedIt->second;

		return CompileShader(aShaderKey);
	}

	std::optional<std::vector<char>> Shaders::CompileShader(const ShaderKey& aShaderKey)
	{
		std::optional<std::string> fileContent = PreCompileFile(aShaderKey.myPath);

		if (!fileContent)
			return {};

		const char* entryPoint = "";
		const char* target = "";
		switch (aShaderKey.myType)
		{
		case ShaderType::Pixel:
			entryPoint = "pixelShader";
			target = "ps_5_0";
			break;
		case ShaderType::Vertex:
			entryPoint = "vertexShader";
			target = "vs_5_0";
			break;
		case ShaderType::Geometry:
			entryPoint = "geometryShader";
			target = "gs_5_0";
			break;
		}

		UINT flags = 0;

		// TODO enable D3DCOMPILE_SKIP_OPTIMIZATION && D3DCOMPILE_DEBUG

#ifdef _DEBUG
		flags |= D3DCOMPILE_OPTIMIZATION_LEVEL0;
#else
		flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

		COMObject<ID3DBlob> compiled = nullptr;
		COMObject<ID3DBlob> errors = nullptr;

		HRESULT result = D3DCompile(
			fileContent->data(), 
			fileContent->size(), 
			aShaderKey.myPath.string().c_str(), 
			nullptr,
			nullptr, 
			entryPoint,
			target,
			flags,
			0,
			&static_cast<ID3DBlob*&>(compiled),
			&static_cast<ID3DBlob*&>(errors));

		if (FAILED(result))
		{
			_com_error err(result);
			std::string errorMessage(static_cast<const char*>(errors->GetBufferPointer()), errors->GetBufferSize());

			LOG_SYS_CRASH("failed to compile shader", err.ErrorMessage(), errorMessage);
			return {};
		}

		const char* start = static_cast<const char*>(compiled->GetBufferPointer());
		const char* end = start + compiled->GetBufferSize();

		return std::vector<char>(start, end);
	}

	std::optional<std::string> Shaders::PreCompileFile(const std::filesystem::path& aPath)
	{
		std::optional<std::string> fileContent = tools::ReadWholeFile(aPath);

		if (!fileContent)
		{
			LOG_SYS_CRASH("Failed to read shader file ", aPath.string());
			return {};
		}

		const std::string includePattern = "#include \"";

		size_t at = 0;
		size_t includedLines = 0;
		while (true)
		{
			size_t includeStart = fileContent->find(includePattern, at);

			if (includeStart == std::string::npos)
				break;

			size_t includePathBegin = includeStart + includePattern.length();
			
			size_t includePathEnd = fileContent->find('"', includePathBegin);

			if (includePathEnd == std::string::npos)
			{
				LOG_SYS_CRASH("Shader file has malformed #include", aPath.string());
				return {};
			}

			std::filesystem::path includedFilePath = aPath.parent_path() / fileContent->substr(includePathBegin, includePathEnd - includePathBegin);

			std::optional<std::string> includedFileContent = PreCompileFile(includedFilePath);
			if (!includedFileContent)
				return {};

			std::string_view startContent = std::string_view(*fileContent).substr(0, includeStart);

			size_t lines = std::ranges::count_if(startContent, [](char c) { return c == '\n'; });

			fileContent = 
				fileContent->substr(0, includeStart) +
				"\n#line 0 \"" + includedFilePath.string() + "\"\n" +
				*includedFileContent +
				"\n#line " + std::to_string(lines - includedLines) + " \"" + aPath.string() + "\"\n" +
				fileContent->substr(includePathEnd + 1);

			includedLines += std::ranges::count_if(*includedFileContent, [](char c) { return c == '\n'; });
			includedLines += 4;

			at = includePathEnd + 1;
		}

		return fileContent;
	}

}