#pragma once

#include "Frame.hpp"
#include "helpers/ComPtrHolder.hpp"
#include "helpers/PropVariant.hpp"

#include <wincodecsdk.h>


namespace PGUI::UI::Bmp
{
	class MetadataReader : public ComPtrHolder<IWICMetadataQueryReader>
	{
		public:
		explicit MetadataReader(Frame frame) noexcept;
		explicit MetadataReader(BitmapDecoder img) noexcept;
		explicit MetadataReader(ComPtr<IWICMetadataQueryReader> reader) noexcept;

		[[nodiscard]] PropVariant GetMetadata(std::wstring_view name) const noexcept;
		[[nodiscard]] std::wstring Location() const noexcept;

		[[nodiscard]] PropVariant operator[](std::wstring_view name) const noexcept;
	};
}
