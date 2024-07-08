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

		[[nodiscard]] bool IsInitialized() const noexcept
		{
			return heldPtr;
		}

		void Reset() noexcept
		{
			heldPtr = nullptr;
		}

		[[nodiscard]] T* operator->() const noexcept { return GetHeldPtr(); }
		[[nodiscard]] explicit(false) operator T* () const noexcept { return GetHeldPtr(); }
		[[nodiscard]] explicit operator bool () const noexcept { return IsInitialized(); }

		protected:
		[[nodiscard]] ComPtr<T> GetHeldComPtr() const noexcept
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
		[[nodiscard]] T* GetHeldPtr() const noexcept
		{
			return heldPtr.Get();
		}
		[[nodiscard]] T* const* GetHeldPtrAddress() const noexcept
		{
			return heldPtr.GetAddressOf();
		}
		[[nodiscard]] T** GetHeldPtrAddress() noexcept
		{
			return heldPtr.GetAddressOf();
		}

		private:
		ComPtr<T> heldPtr;
	};
}
