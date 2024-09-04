#include "ui/font/LocalizedFontFamilyNames.hpp"

#include "core/Exceptions.hpp"
#include "helpers/HelperFunctions.hpp"
#include "ui/font/FontFamily.hpp"


namespace PGUI::UI::Font
{
	LocalizedFontFamilyNames::LocalizedFontFamilyNames(const FontFamily& fontFamily) noexcept
	{
		HRESULT hr = fontFamily->GetFamilyNames(GetHeldPtrAddress()); HR_L(hr);
	}

	auto LocalizedFontFamilyNames::GetCount() const noexcept -> UINT32
	{
		return GetHeldComPtr()->GetCount();
	}

	auto LocalizedFontFamilyNames::FindLocaleIndex(std::wstring_view localeName) const noexcept -> std::optional<UINT32>
	{
		UINT32 index = 0;
		BOOL exists = false;

		HRESULT hr = GetHeldComPtr()->FindLocaleName(localeName.data(), &index, &exists); HR_L(hr);

		if (!exists)
		{
			return std::nullopt;
		}
		return index;
	}

	auto LocalizedFontFamilyNames::GetLocaleName(UINT index) const noexcept -> std::optional<std::wstring>
	{
		try
		{
			UINT32 length = 0;
			HRESULT hr = GetHeldComPtr()->GetLocaleNameLength(index, &length); HR_T(hr);
			length++; // For null character

			std::wstring localeName(length, L'\0');

			hr = GetHeldComPtr()->GetLocaleName(index, localeName.data(), length); HR_T(hr);

			return localeName;
		}
		catch (Core::HresultException& error)
		{
			HR_L(error.GetErrorCode());
			return std::nullopt;
		}
	}

	auto LocalizedFontFamilyNames::GetFontFamilyName() const noexcept -> std::optional<std::wstring>
	{
		return GetFontFamilyName(GetUserLocaleName());
	}

	auto LocalizedFontFamilyNames::GetFontFamilyName(std::wstring_view localeName) const noexcept -> std::optional<std::wstring>
	{
		return GetFontFamilyName(FindLocaleIndex(localeName).value_or(0));
	}

	auto LocalizedFontFamilyNames::GetFontFamilyName(UINT index) const noexcept -> std::optional<std::wstring>
	{
		try
		{
			UINT32 length = 0;
			HRESULT hr = GetHeldComPtr()->GetStringLength(index, &length); HR_T(hr);
			length++; // For null character

			std::wstring familyName(length, L'\0');
			GetHeldComPtr()->GetString(index, familyName.data(), length); HR_T(hr);

			return familyName;

		}
		catch (Core::HresultException& error)
		{
			HR_L(error.GetErrorCode());
			return std::nullopt;
		}
	}
}
