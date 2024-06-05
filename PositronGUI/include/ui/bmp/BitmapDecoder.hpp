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

		[[nodiscard]] Frame GetFrame(UINT frameIndex = 0) const noexcept;
		[[nodiscard]] std::vector<Frame> GetFrames() const noexcept;
		[[nodiscard]] UINT GetFrameCount() const noexcept;
		[[nodiscard]] MetadataReader GetMetadata() const noexcept;

		[[nodiscard]] ComPtr<IWICBitmapSource> GetThumbnail() const noexcept;
	};
}
