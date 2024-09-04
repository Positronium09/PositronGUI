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

		[[nodiscard]] auto GetCount() const noexcept -> UINT32;
		[[nodiscard]] auto FindLocaleIndex(std::wstring_view localeName) const noexcept -> std::optional<UINT32>;
		[[nodiscard]] auto GetLocaleName(UINT index) const noexcept -> std::optional<std::wstring>;
		[[nodiscard]] auto GetFontFamilyName() const noexcept -> std::optional<std::wstring>;
		[[nodiscard]] auto GetFontFamilyName(std::wstring_view localeName) const noexcept -> std::optional<std::wstring>;
		[[nodiscard]] auto GetFontFamilyName(UINT index) const noexcept -> std::optional<std::wstring>;
	};
}
