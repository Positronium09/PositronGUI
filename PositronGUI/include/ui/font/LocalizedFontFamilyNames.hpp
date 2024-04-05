#pragma once

#include "helpers/ComPtrHolder.hpp"

#include <string>
#include <optional>
#include <dwrite_3.h>


namespace PGUI::UI::Font
{
	class FontFamily;

	class LocalizedFontFamilyNames : public ComPtrHolder<IDWriteLocalizedStrings>
	{
		public:
		explicit LocalizedFontFamilyNames(const FontFamily& fontFamily) noexcept;

		[[nodiscard]] UINT32 GetCount() const noexcept;
		[[nodiscard]] std::optional<UINT32> FindLocaleIndex(std::wstring_view localeName) const noexcept;
		[[nodiscard]] std::optional<std::wstring> GetLocaleName(UINT index) const noexcept;
		[[nodiscard]] std::optional<std::wstring> GetFontFamilyName() const noexcept;
		[[nodiscard]] std::optional<std::wstring> GetFontFamilyName(std::wstring_view localeName) const noexcept;
		[[nodiscard]] std::optional<std::wstring> GetFontFamilyName(UINT index) const noexcept;
	};
}
