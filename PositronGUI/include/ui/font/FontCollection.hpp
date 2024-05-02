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
		[[nodiscard]] static FontCollection GetSystemFontCollection();
		[[nodiscard]] static FontCollection LoadFontFile(std::wstring_view filePath);

		FontCollection() noexcept;
		explicit FontCollection(ComPtr<IDWriteFontCollection3> fontCollection) noexcept;

		[[nodiscard]] FontSet GetFontSet() const noexcept;
		[[nodiscard]] FontFamily GetFontFamily(UINT32 index) const noexcept;
		[[nodiscard]] UINT32 GetFontFamilyCount() const noexcept;
	};
}
