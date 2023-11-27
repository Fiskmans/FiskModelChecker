
#pragma once

#include "ShaderInputMapping.h"
#include "GraphicsFramework.h"
#include "COMObject.h"

#include <filesystem>
#include <string>
#include <unordered_map>

#include <d3d11.h>

namespace fisk
{
	enum class ShaderType
	{
		Pixel,
		Vertex,
		Geometry
	};

	struct ShaderKey
	{
		ShaderKey(const std::filesystem::path& aPath, ShaderType aType)
			: myPath(aPath)
			, myType(aType)
		{
		}

		bool operator==(const ShaderKey& aOther) const
		{
			return myType == aOther.myType &&
				myPath == aOther.myPath;
		}

		std::filesystem::path myPath;
		ShaderType myType;
	};
}

namespace std
{
	template<>
	class hash<fisk::ShaderKey>
	{
	public:
		size_t operator()(const fisk::ShaderKey& aKey) const 
		{
			return 
					hash<filesystem::path>()(aKey.myPath)
				^ ( hash<int>()(static_cast<int>(aKey.myType)) << 1);
		}
	};
}

namespace fisk
{
	class Shaders
	{
	public:
		Shaders(GraphicsFramework& aFramework);
		~Shaders() = default;

		Shaders(const Shaders&) = delete;
		Shaders& operator=(const Shaders&) = delete;

		COMObject<ID3D11Buffer> GetShaderBuffer(size_t aSize);

		COMObject<ID3D11InputLayout> GetInputLayout(const std::filesystem::path& aPath,const std::vector<ShaderInputMapping>& aLayout);
		COMObject<ID3D11PixelShader> GetPixelShaderByPath(const std::filesystem::path& aPath);
		COMObject<ID3D11VertexShader> GetVertexShaderByPath(const std::filesystem::path& aPath);

	private:

		std::optional<std::vector<char>> GetShaderByteCode(const ShaderKey& aShaderKey);
		std::optional<std::vector<char>> CompileShader(const ShaderKey& aShaderKey);
		std::optional<std::string> PreCompileFile(const std::filesystem::path& aPath);

		std::unordered_map<ShaderKey, std::vector<char>> myCachedByteCode;

		GraphicsFramework& myFramework;
	};
}
