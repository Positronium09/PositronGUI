#pragma once

#include "helpers/ComPtrHolder.hpp"

#include <string>
#include <vector>
#include <optional>
#include <wincodec.h>


namespace PGUI::UI::Img
{
	class Frame;
	class ImageMetadataReader;

	class Image : public ComPtrHolder<IWICBitmapDecoder>
	{
		public:
		Image(std::wstring_view filePath, bool readOnly = true, 
			std::optional<GUID> vendorGUID = std::nullopt) noexcept;
		Image(ULONG_PTR fileHandle,
			std::optional<GUID> vendorGUID = std::nullopt) noexcept;
		//Image(ComPtr<IStream> stream, 
		//	std::optional<GUID> vendorGUID = std::nullopt) noexcept;
		Image() noexcept;

		[[nodiscard]] Frame GetFrame(UINT frameIndex = 0) const noexcept;
		[[nodiscard]] std::vector<Frame> GetFrames() const noexcept;
		[[nodiscard]] UINT GetFrameCount() const noexcept;
		[[nodiscard]] ImageMetadataReader GetMetadata() const noexcept;

		[[nodiscard]] ComPtr<IWICBitmapSource> GetThumbnail() const noexcept;
	};
}
