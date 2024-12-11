#pragma once

#include "ComPtr.hpp"


namespace PGUI
{
	template <typename T>
	class ComPtrHolder
	{
		public:
		ComPtrHolder() = default;
		explicit ComPtrHolder(ComPtr<T> ptr) noexcept : heldPtr(ptr) { }

		[[nodiscard]] auto IsInitialized() const noexcept -> bool
		{
			return heldPtr;
		}

		void Reset() noexcept
		{
			heldPtr = nullptr;
		}

		[[nodiscard]] auto operator->() const noexcept { return GetHeldPtr(); }
		[[nodiscard]] explicit(false) operator T* () const noexcept { return GetHeldPtr(); }
		[[nodiscard]] explicit operator bool () const noexcept { return IsInitialized(); }

		protected:
		[[nodiscard]] auto GetHeldComPtr() const noexcept
		{
			return heldPtr;
		}
		[[nodiscard]] auto GetHeldComPtrAddress() noexcept
		{
			return &heldPtr;
		}
		[[nodiscard]] auto GetHeldComPtrAddress() const noexcept
		{
			return &heldPtr;
		}
		[[nodiscard]] auto GetHeldPtr() const noexcept
		{
			return heldPtr.Get();
		}
		[[nodiscard]] auto GetHeldPtrAddress() const noexcept
		{
			return heldPtr.GetAddressOf();
		}
		[[nodiscard]] auto GetHeldPtrAddress() noexcept
		{
			return heldPtr.GetAddressOf();
		}

		private:
		ComPtr<T> heldPtr;
	};
}
