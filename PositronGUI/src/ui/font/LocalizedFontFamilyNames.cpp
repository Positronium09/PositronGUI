#include "ui/font/LocalizedFontFamilyNames.hpp"

#include "core/Exceptions.hpp"
#include "core/Logger.hpp"
#include "ui/font/FontFamily.hpp"


namespace PGUI::UI::Font
{
	LocalizedFontFamilyNames::LocalizedFontFamilyNames(const FontFamily& fontFamily) noexcept
	{
		HRESULT hr = fontFamily->GetFamilyNames(GetHeldPtrAddress()); HR_L(hr);
	}

	UINT32 LocalizedFontFamilyNames::GetCount() const noexcept
	{
		return GetHeldComPtr()->GetCount();
	}

	std::optional<UINT32> LocalizedFontFamilyNames::FindLocaleIndex(std::wstring_view localeName) const noexcept
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

	std::optional<std::wstring> LocalizedFontFamilyNames::GetLocaleName(UINT index) const noexcept
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
		catch (Core::ErrorHandling::HresultException& error)
		{
			HR_L(error.GetErrorCode());
			return std::nullopt;
		}
	}

	std::optional<std::wstring> LocalizedFontFamilyNames::GetFontFamilyName() const noexcept
	{
		return GetFontFamilyName(GetUserLocaleName());
	}

	std::optional<std::wstring> LocalizedFontFamilyNames::GetFontFamilyName(std::wstring_view localeName) const noexcept
	{
		return GetFontFamilyName(FindLocaleIndex(localeName).value_or(0));
	}

	std::optional<std::wstring> LocalizedFontFamilyNames::GetFontFamilyName(UINT index) const noexcept
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
		catch (Core::ErrorHandling::HresultException& error)
		{
			HR_L(error.GetErrorCode());
			return std::nullopt;
		}
	}
}
