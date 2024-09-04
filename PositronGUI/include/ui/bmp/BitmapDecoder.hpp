#pragma once

#include "helpers/ComPtrHolder.hpp"

#include <string>
#include <vector>
#include <optional>
#include <wincodec.h>


namespace PGUI::UI::Bmp
{
	class Frame;
	class MetadataReader;
	
	class BitmapDecoder : public ComPtrHolder<IWICBitmapDecoder>
	{
		public:
		BitmapDecoder(std::wstring_view filePath, bool readOnly = true, 
			std::optional<GUID> vendorGUID = std::nullopt) noexcept;
		BitmapDecoder(ULONG_PTR fileHandle,
			std::optional<GUID> vendorGUID = std::nullopt) noexcept;
		BitmapDecoder() noexcept;

		[[nodiscard]] auto GetFrame(UINT frameIndex = 0) const noexcept -> Frame;
		[[nodiscard]] auto GetFrames() const noexcept -> std::vector<Frame>;
		[[nodiscard]] auto GetFrameCount() const noexcept -> UINT;
		[[nodiscard]] auto GetMetadata() const noexcept -> MetadataReader;

		[[nodiscard]] auto GetThumbnail() const noexcept -> ComPtr<IWICBitmapSource>;
	};
}
