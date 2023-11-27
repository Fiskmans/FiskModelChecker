
#include "Model.h"

#include "tools/Utility.h"
#include "tools/Stream.h"
#include "tools/StreamReader.h"

#include <comdef.h>
#include <limits>
#include <fstream>

namespace fisk {

	template<class Type>
	bool CreateShaderBuffer(GraphicsFramework& aFramework, const std::vector<Type>& aItems, COMObject<ID3D11Buffer>& aOutBuffer)
	{
		constexpr size_t itemSize = sizeof(Type);

		if (aItems.size() * itemSize > (std::numeric_limits<UINT>::max)()) // fuck windows
		{
			LOG_ERROR("Buffer is too large");
			return false;
		}

		UINT bytewidth = static_cast<UINT>(aItems.size() * itemSize);

		D3D11_BUFFER_DESC bufferDesc{};

		bufferDesc.ByteWidth = bytewidth;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = itemSize;

		D3D11_SUBRESOURCE_DATA data{};
		data.pSysMem = aItems.data();
		data.SysMemPitch = itemSize;
		data.SysMemSlicePitch = bytewidth;

		HRESULT result = aFramework.Device().CreateBuffer(&bufferDesc, &data, &static_cast<ID3D11Buffer*&>(aOutBuffer));

		if (FAILED(result))
		{
			_com_error err(result);
			LOG_ERROR("failed to get create vertexbuffer for model", err.ErrorMessage());
			return false;
		}

		return true;
	}


	Model::Model(GraphicsFramework& aFramework, const std::vector<Vertex>& aVertexes, const std::vector<UINT>& aIndexes, WindingOrder aWindingOrder)
	{
		myVertexes = aVertexes;
		myIndexes = aIndexes;

		if (!CreateShaderBuffer(aFramework, myVertexes, myVertexBuffer.myRawBuffer))
			return;

		myVertexBuffer.myStride = sizeof(Vertex);

		if (myIndexes.size() % 3 != 0)
		{
			LOG_ERROR("Model: Index list does not form triangles");
			return;
		}

		myIndexCount = myIndexes.size();

		if (aWindingOrder == WindingOrder::AntiClockwise)
		{
			for (size_t i = 0; i < myIndexCount; i += 3)
			{
				std::swap(myIndexes[i], myIndexes[i + 1]);
			}
		}

		if (!CreateShaderBuffer(aFramework, myIndexes, myIndexBuffer))
			return;
		

		myIsValid = true;
	}

	struct ReorderedVertex
	{
		tools::V4f myPosition;
		size_t myStartIndex;

		bool operator<(const ReorderedVertex& aOther)
		{
			return myPosition[0] < aOther.myPosition[0];
		}
	};

	void DeduplicateVertexes(std::vector<tools::V4f>& aVertex, std::vector<UINT>& aOutIndex)
	{
		std::vector<ReorderedVertex> reordered;
		
		size_t flag = aVertex.size() + 1;

		aOutIndex.reserve(aVertex.size());
		reordered.reserve(aVertex.size());

		for (size_t i = 0; i < aVertex.size(); i++)
		{
			ReorderedVertex v;

			v.myPosition = aVertex[i];
			v.myStartIndex = i;

			reordered.push_back(v);
			aOutIndex.push_back(flag);
		}

		std::sort(reordered.begin(), reordered.end());

		constexpr float mergeDistance = 1.e-20;
		constexpr float mergeDistanceSqr = mergeDistance * mergeDistance;

		std::vector<size_t> toErase;
		toErase.reserve(aVertex.size());

		for (size_t i = 0; i < reordered.size(); i++)
		{
			ReorderedVertex& v1 = reordered[i];

			if (aOutIndex[v1.myStartIndex] != flag)
				continue;

			aOutIndex[v1.myStartIndex] = v1.myStartIndex;

			for (size_t j = i + 1; j < reordered.size(); j++)
			{
				ReorderedVertex& v2 = reordered[j];

				if (v1.myPosition[0] + mergeDistance < v2.myPosition[0])
					break;

				if (aOutIndex[v2.myStartIndex] != flag)
					continue;

				if (v1.myPosition.DistanceSqr(v2.myPosition) > mergeDistanceSqr)
					continue;
				
				aOutIndex[v2.myStartIndex] = v1.myStartIndex;

				toErase.push_back(v2.myStartIndex);
			}
		}

		std::sort(toErase.begin(), toErase.end());

		std::vector<tools::V4f>::iterator readHead = aVertex.begin();
		std::vector<tools::V4f>::iterator writeHead = aVertex.begin();
		std::vector<size_t>::iterator eraseHead = toErase.begin();

		std::vector<size_t> offsets;

		size_t erased = 0;
		size_t amount = aVertex.size();
		for (size_t i = 0; i < amount; i++)
		{
			offsets.push_back(erased);

			if (eraseHead != toErase.end() && *eraseHead == i)
			{
				eraseHead++;
				readHead++;
				erased++;
				continue;
			}

			*writeHead = *readHead;
			
			writeHead++;
			readHead++;
		}

		aVertex.resize(aVertex.size() - erased);

		for (UINT& index : aOutIndex)
			index = index - offsets[index];
	}

	std::optional<Model> ModelFromAsciiStl(GraphicsFramework& aFramework, std::istream& aStream)
	{

		std::string line;
		std::string name;

		if (!std::getline(aStream, line))
		{
			std::string solid;

			std::stringstream ss(line);

			ss >> solid >> name;

			if (solid != "solid")
				return {};
		}


		std::vector<tools::V4f> vertex;

		std::string word;
		while (aStream >> word && word == "facet")
		{
			std::string normal;

			tools::V4f normalVector;

			if (!(aStream >> normal >> normalVector[0] >> normalVector[1] >> normalVector[2]))
				return {};

			if (normal != "normal")
				return {};

			normalVector[3] = 0.f;

			std::string outer;
			std::string loop;

			aStream >> outer >> loop;

			if (outer != "outer")
				return {};

			if (loop != "loop")
				return {};

			size_t indexes[3] = { 0, 0, 0 };

			for (size_t i = 0; i < 3; i++)
			{
				tools::V4f pos;

				if (!(aStream >> pos[0] >> pos[1] >> pos[2]))
					return {};

				pos[3] = 1.f;

				vertex.push_back(pos);
			}

			std::string endloop;
			std::string endfacet;

			if (!(aStream >> endloop >> endfacet))
				return {};

		}

		if (word != "endsolid")
			return {};

		std::vector<UINT> indexList;

		DeduplicateVertexes(vertex, indexList);

		std::vector<Vertex> translatedVertexes;

		for (tools::V4f pos : vertex)
			translatedVertexes.push_back(Vertex(pos));

		return Model(aFramework, translatedVertexes, indexList, Model::WindingOrder::AntiClockwise);
	}

	std::optional<Model> ModelFromBinaryStl(GraphicsFramework& aFramework, std::istream& aStream)
	{
		constexpr float mergeDistance = 0.000001f;

		tools::ReadStream readStream;
		
		if (!aStream)
			return {};

		aStream.seekg(0, std::ios_base::end);

		if (!aStream)
			return {};

		size_t amount = aStream.tellg();
		aStream.clear();
		aStream.seekg(0);

		bool eof = aStream.eof();

		if (!aStream)
			return {};

		while (amount > 0)
		{
			size_t thisPass = (std::min)(amount, tools::StreamSegment::CHUNK_SIZE);
			std::shared_ptr<tools::StreamSegment> segment = std::make_shared<tools::StreamSegment>();

			aStream.read(reinterpret_cast<char*>(segment->myData), thisPass);

			if (!aStream.good())
				return {};

			segment->mySize = thisPass;

			readStream.AppendData(segment);

			amount -= thisPass;
		}

		uint8_t header[80] = {};
		if (!readStream.Read(header, 80))
			return {};

		uint32_t triCount = 0;
		if (!readStream.Read(reinterpret_cast<uint8_t*>(&triCount), sizeof(triCount)))
			return {};

		std::vector<tools::V4f> vertex;

		for (size_t i = 0; i < triCount; i++)
		{
			tools::V4f normal;

			if (!readStream.Read(reinterpret_cast<uint8_t*>(normal.Raw()), sizeof(float) * 3))
				return {};

			normal[3] = 0.f;

			for (size_t i = 0; i < 3; i++)
			{
				tools::V4f pos;

				if (!readStream.Read(reinterpret_cast<uint8_t*>(pos.Raw()), sizeof(float) * 3))
					return {};

				pos[3] = 1.f;

				vertex.push_back(pos);

			}

			uint16_t attribute;

			if (!readStream.Read(reinterpret_cast<uint8_t*>(&attribute), sizeof(attribute)))
				return {};

			readStream.CommitRead();
		}

		std::vector<Vertex> translatedVertexes;
		std::vector<UINT> indexList;

		DeduplicateVertexes(vertex, indexList);

		for (tools::V4f pos : vertex)
			translatedVertexes.push_back(Vertex(pos));

		return Model(aFramework, translatedVertexes, indexList, Model::WindingOrder::AntiClockwise);
	}

	std::optional<Model> ModelFromStl(GraphicsFramework& aFramework, std::istream& aStream)
	{
		char header[6];
		aStream.read(header, 6);

		if (!aStream)
			return {};

		aStream.seekg(0);
		
		if (std::string(header) == "solid")
			return ModelFromAsciiStl(aFramework, aStream);

		return ModelFromBinaryStl(aFramework, aStream);
	}

	std::optional<Model> Model::FromFile(GraphicsFramework& aFramework, const std::filesystem::path& aPath)
	{
		if (!aPath.has_extension())
			return {};

		std::ifstream file(aPath, std::ios_base::binary);
		if (!file)
			return {};

		std::string extension = aPath.extension().string();
		if (extension == ".stl")
			return ModelFromStl(aFramework, file);

		return {};
	}
}