#pragma once

#include "helpers/ComPtrHolder.hpp"

#include <string>
#include <dwrite_3.h>


namespace PGUI::UI::Font
{
	class FontFamily;
	class FontSet;

	class FontCollection : public ComPtrHolder<IDWriteFontCollection3>
	{
		public:
		[[nodiscard]] static auto GetSystemFontCollection() -> FontCollection;
		[[nodiscard]] static auto LoadFontFile(std::wstring_view filePath) -> FontCollection;

		FontCollection() noexcept;
		explicit FontCollection(ComPtr<IDWriteFontCollection3> fontCollection) noexcept;

		[[nodiscard]] auto GetFontSet() const noexcept -> FontSet;
		[[nodiscard]] auto GetFontFamily(UINT32 index) const noexcept -> FontFamily;
		[[nodiscard]] auto GetFontFamilyCount() const noexcept -> UINT32;
	};
}
