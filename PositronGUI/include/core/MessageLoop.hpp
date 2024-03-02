#pragma once


namespace PGUI::Core
{
	class IMessageLoop
	{
		public:
		virtual ~IMessageLoop() = default;

		virtual int Run() = 0;
	};

	class GetMessageLoop : public IMessageLoop
	{
		public:
		[[nodiscard]] int Run() override;
	};

	class PeekMessageLoop : public IMessageLoop
	{
		public:
		[[nodiscard]] int Run() override;
	};
}
