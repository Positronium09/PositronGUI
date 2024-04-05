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
		ComPtr<T> GetHeldComPtr() const noexcept
		{
			return heldPtr;
		}
		auto GetHeldComPtrAddress() noexcept
		{
			return &heldPtr;
		}
		auto GetHeldComPtrAddress() const noexcept
		{
			return &heldPtr;
		}
		T* GetHeldPtr() const noexcept
		{
			return heldPtr.Get();
		}
		T* const* GetHeldPtrAddress() const noexcept
		{
			return heldPtr.GetAddressOf();
		}
		T** GetHeldPtrAddress() noexcept
		{
			return heldPtr.GetAddressOf();
		}

		private:
		ComPtr<T> heldPtr;
	};
}
