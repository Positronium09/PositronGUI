#pragma once

#include "Frame.hpp"
#include "helpers/ComPtrHolder.hpp"
#include "helpers/PropVariant.hpp"

#include <wincodecsdk.h>


namespace PGUI::UI::Img
{
	class ImageMetadataReader : public ComPtrHolder<IWICMetadataQueryReader>
	{
		public:
		explicit ImageMetadataReader(Frame frame) noexcept;
		explicit ImageMetadataReader(Image img) noexcept;
		explicit ImageMetadataReader(ComPtr<IWICMetadataQueryReader> reader) noexcept;

		[[nodiscard]] PropVariant GetMetadata(std::wstring_view name) const noexcept;
		[[nodiscard]] std::wstring Location() const noexcept;
	};
}
