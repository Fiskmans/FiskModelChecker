
#pragma once

#include <vector>
#include <cassert>
#include <algorithm>

namespace fisk
{
	template<class T, size_t offset>
	class StructuredItem
	{
	public:
		StructuredItem(unsigned char* aData, size_t aDataSize)
		{
			assert(offset + sizeof(T) <= aDataSize);
			myObject = reinterpret_cast<T*>(aData + offset);
		}

		void operator=(const T& aOther)
		{
			*myObject = aOther;
		}

		T* operator-> ()
		{
			return myObject;
		}

		T& operator * ()
		{
			return *myObject;
		}

		void Paint()
		{
			unsigned char* start = reinterpret_cast<unsigned char*>(myObject);

			for (size_t i = 0; i < sizeof(T); i++)
				start[i]++;
		}

	private:
		T* myObject;
	};

	struct GenericData
	{
		unsigned char* myData;
		size_t mySize;
	};

	template<class View, size_t Size>
	class StructuredData
	{
	public:
		static constexpr size_t DataSize = Size;

		template<typename... Args>
		StructuredData(Args... aArgs)
		{
			View v(myData, Size, aArgs...);
		}

		View Structure()
		{
			return View(myData, Size);
		}

		static void Validate()
		{
			StructuredData d;

			memset(d.myData, 0, Size);

			d.Structure().Paint();

			assert(std::all_of(d.myData, d.myData + Size, [](unsigned char c) { return c == 1; }));
		}

		GenericData AsGeneric()
		{
			GenericData out;
			out.myData = myData;
			out.mySize = Size;
			return out;
		}


	private:
		unsigned char myData[Size];
	};
}