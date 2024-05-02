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

			case Vector | Variant:
				return var.capropvar;

			case Vector | I1:
				return var.cac;

			case Vector | Ui1:
				return var.caub;

			case Vector | I2:
				return var.cai;

			case Vector | Ui2:
				return var.caui;

			case Vector | Bool:
				return var.cabool;

			case Vector | I4:
				return var.cal;

			case Vector | Ui4:
				return var.caul;

			case Vector | R4:
				return var.caflt;

			case Vector | R8:
				return var.cadbl;

			case Vector | Error:
				return var.cascode;

			case Vector | I8:
				return var.cah;

			case Vector | Ui8:
				return var.cauh;

			case Vector | Cy:
				return var.cacy;

			case Vector | Date:
				return var.cadate;

			case Vector | Filetime:
				return var.cafiletime;

			case Vector | Clsid:
				return var.cauuid;

			case Vector | Cf:
				return var.caclipdata;

			case Vector | Bstr:
				return var.cabstr;

			case Vector | Lpstr:
				return var.calpstr;

			case Vector | Lpwstr:
				return var.calpwstr;

			default:
				return nullptr;
		}
	}
}
