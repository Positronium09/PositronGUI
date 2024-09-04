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

		[[nodiscard]] auto GetMetadata(std::wstring_view name) const noexcept -> PropVariant;
		[[nodiscard]] auto Location() const noexcept -> std::wstring;

		[[nodiscard]] auto operator[](std::wstring_view name) const noexcept -> PropVariant;
	};
}
