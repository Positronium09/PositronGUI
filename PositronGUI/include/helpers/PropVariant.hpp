#pragma once

#include <variant>
#include <vector>
#include <Windows.h>

#include "helpers/EnumFlag.hpp"

namespace PGUI
{
	enum class PropVariantType
	{
		Empty = 0,
		Null = 1,
		I1 = 16,
		Ui1 = 17,
		I2 = 2,
		Ui2 = 18,
		I4 = 3,
		Ui4 = 19,
		Int = 22,
		Uint = 23,
		I8 = 20,
		Ui8 = 21,
		R4 = 4,
		R8 = 5,
		Bool = 11,
		Error = 10,
		Cy = 6,
		Date = 7,
		Filetime = 64,
		Clsid = 72,
		Cf = 71,
		Bstr = 8,
		Blob = 65,
		BlobObject = 70,
		Lpstr = 30,
		Lpwstr = 31,
		Unknown = 13,
		Dispatch = 9,
		Stream = 66,
		StreamedObject = 68,
		Storage = 67,
		StoredObject = 69,
		VersionedStream = 73,
		Decimal = 14,
		Vector = 0x1000,
		Array = 0x2000,
		Byref = 0x4000,
		Variant = 12,
		TypeMask = 0xfff
	};

	using PropVariantValue = 
		std::variant<
		std::nullptr_t,
		CHAR, UCHAR,
		SHORT, USHORT, INT, UINT, 
		LARGE_INTEGER, ULARGE_INTEGER,
		FLOAT, DOUBLE, // DATE is DOUBLE
		bool, SCODE,
		CY, FILETIME,
		CLSID*, CLIPDATA*,
		BLOB, LPSTR, LPWSTR, // BSTR is LPWSTR
		IUnknown*, IDispatch*,
		IStream*, IStorage*, 
		LPVERSIONEDSTREAM,
		DECIMAL, LPSAFEARRAY,
		UCHAR*,
		SHORT*, USHORT*, INT*, UINT*,
		LARGE_INTEGER*, ULARGE_INTEGER*,
		FLOAT*, DOUBLE*,
		bool*, SCODE*,
		CY*, FILETIME*, BSTR*,
		IUnknown**, IDispatch**,
		DECIMAL*, LPSAFEARRAY*,
		PROPVARIANT*,
		std::vector<CHAR>, std::vector<UCHAR>,
		std::vector<SHORT>, std::vector<USHORT>,
		std::vector<LONG>, std::vector<ULONG>,
		std::vector<FLOAT>, std::vector<DOUBLE>,
		std::vector<CY>, std::vector<CLSID>,
		std::vector<bool>,
		std::vector<PROPVARIANT>,
		std::vector<LARGE_INTEGER>, std::vector<ULARGE_INTEGER>,
		std::vector<LPSTR>, std::vector<LPWSTR>,
		std::vector<FILETIME>, std::vector<CLIPDATA>>;

	class PropVariant final
	{
		public:
		PropVariant() noexcept;
		explicit(false) PropVariant(PROPVARIANT var) noexcept;
		~PropVariant() noexcept;

		PROPVARIANT* operator&() noexcept;
		explicit(false) operator PROPVARIANT() const noexcept;
		explicit(false) operator PropVariantValue() const noexcept;

		[[nodiscard]] PropVariantType Type() const noexcept;

		[[nodiscard]] PropVariantValue GetValue() const noexcept;

		PROPVARIANT var{};
	};
}
EnableEnumFlag(PGUI::PropVariantType);