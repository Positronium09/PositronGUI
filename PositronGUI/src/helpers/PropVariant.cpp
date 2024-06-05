#include "helpers/PropVariant.hpp"


namespace PGUI
{
	PropVariant::PropVariant() noexcept
	{
		PropVariantInit(&var);
	}

	PropVariant::PropVariant(PROPVARIANT var) noexcept : 
		var{ var }
	{
	}

	PropVariant::~PropVariant() noexcept
	{
		PropVariantClear(&var);
	}

	PROPVARIANT* PropVariant::operator&() noexcept
	{
		PropVariantClear(&var);

		return &var;
	}
	PropVariant::operator PROPVARIANT() const noexcept
	{
		return var;
	}
	PropVariant::operator PropVariantValue() const noexcept
	{
		return GetValue();
	}
	PropVariantType PropVariant::Type() const noexcept
	{
		return var.vt;
	}
	PropVariantValue PropVariant::GetValue() const noexcept
	{
		switch (Type())
		{
			using enum PropVariantType::EnumValues;

			case Empty:
			case Null:
				return nullptr;

			case I1:
				return var.cVal;

			case Ui1:
				return var.bVal;

			case I2:
				return var.iVal;

			case Ui2:
				return var.uiVal;

			case Int:
			case I4:
				return var.lVal;

			case Uint:
			case Ui4:
				return var.ulVal;

			case I8:
				return var.hVal;

			case Ui8:
				return var.uhVal;

			case R4:
				return var.fltVal;

			case R8:
				return var.dblVal;

			case Bool:
				return (bool)var.boolVal;

			case Error:
				return var.scode;

			case Cy:
				return var.cyVal;

			case Date:
				return var.date;

			case Filetime:
				return var.filetime;

			case Clsid:
				return var.puuid;

			case Cf:
				return var.pclipdata;

			case Bstr:
				return var.bstrVal;

			case Blob:
			case BlobObject:
				return var.blob;

			case Lpstr:
				return var.pszVal;

			case Lpwstr:
				return var.pwszVal;

			case Unknown:
				return var.punkVal;

			case Dispatch:
				return var.pdispVal;

			case Stream:
			case StreamedObject:
				return var.pStream;

			case Storage:
			case StoredObject:
				return var.pStorage;

			case VersionedStream:
				return var.pVersionedStream;

			case Decimal:
				return var.decVal;

			case Array:
				return var.parray;

#pragma region Byref

			case Byref | I1:
				return var.pcVal;

			case Byref | Ui1:
				return var.bVal;

			case Byref | I2:
				return var.piVal;

			case Byref | Ui2:
				return var.puiVal;

			case Byref | Bool:
				return var.pboolVal;

			case Byref | Int:
			case Byref | I4:
				return var.pintVal;

			case Byref | Uint:
			case Byref | Ui4:
				return var.puintVal;

			case Byref | R4:
				return var.pfltVal;

			case Byref | R8:
				return var.pdblVal;

			case Byref | Error:
				return var.pscode;

			case Byref | Cy:
				return var.cyVal;

			case Byref | Date:
				return var.pdate;

			case Byref | Bstr:
				return var.pbstrVal;

			case Byref | Variant:
				return var.pvarVal;

			case Byref | Unknown:
				return var.ppunkVal;

			case Byref | Dispatch:
				return var.ppdispVal;

			case Byref | Array:
				return var.pparray;

#pragma endregion

#pragma region Vector
			case Vector | Variant:
				return std::vector<PROPVARIANT>{ 
					var.capropvar.pElems, 
					var.capropvar.pElems + var.capropvar.cElems 
				};

			case Vector | I1:
				return std::vector<CHAR>{
					var.cac.pElems,
						var.cac.pElems + var.cac.cElems
				};

			case Vector | Ui1:
				return std::vector<UCHAR>{
					var.caub.pElems,
						var.caub.pElems + var.caub.cElems
				};

			case Vector | I2:
				return std::vector<SHORT>{
					var.cai.pElems,
						var.cai.pElems + var.cai.cElems
				};

			case Vector | Ui2:
				return std::vector<USHORT>{
					var.caui.pElems,
						var.caui.pElems + var.caui.cElems
				};

			case Vector | Bool:
			{
				std::vector<bool> vec(var.cabool.cElems);
				for (ULONG i = 0; i < var.cabool.cElems; i++)
				{
					vec.push_back(var.cabool.pElems[i]);
				}
				return vec;
			}

			case Vector | Int:
			case Vector | I4:
			case Vector | Error:
				return std::vector<LONG>{
					var.cal.pElems,
						var.cal.pElems + var.cal.cElems
				};

			case Vector | Uint:
			case Vector | Ui4:
				return std::vector<ULONG>{
					var.caul.pElems,
						var.caul.pElems + var.caul.cElems
				};

			case Vector | R4:
				return std::vector<FLOAT>{
					var.caflt.pElems,
						var.caflt.pElems + var.caflt.cElems
				};

			case Vector | Date:
			case Vector | R8:
				return std::vector<DOUBLE>{
					var.cadbl.pElems,
						var.cadbl.pElems + var.cadbl.cElems
				};

			case Vector | I8:
				return std::vector<LARGE_INTEGER>{
					var.cah.pElems,
						var.cah.pElems + var.cah.cElems
				};

			case Vector | Ui8:
				return std::vector<ULARGE_INTEGER>{
					var.cauh.pElems,
						var.cauh.pElems + var.cauh.cElems
				};

			case Vector | Cy:
				return std::vector<CY>{
					var.cacy.pElems,
						var.cacy.pElems + var.cacy.cElems
				};

			case Vector | Filetime:
				return std::vector<FILETIME>{
					var.cafiletime.pElems,
						var.cafiletime.pElems + var.cafiletime.cElems
				};

			case Vector | Clsid:
				return std::vector<CLSID>{
					var.cauuid.pElems,
						var.cauuid.pElems + var.cauuid.cElems
				};

			case Vector | Cf:
				return std::vector<CLIPDATA>{
					var.caclipdata.pElems,
						var.caclipdata.pElems + var.caclipdata.cElems
				};

			case Vector | Lpstr:
				return std::vector<LPSTR>{
					var.calpstr.pElems,
						var.calpstr.pElems + var.calpstr.cElems
				};

			case Vector | Bstr:
			case Vector | Lpwstr:
				return std::vector<LPWSTR>{
					var.calpwstr.pElems,
						var.calpwstr.pElems + var.calpwstr.cElems
				};

#pragma endregion

			default:
				return nullptr;
		}
	}
}
