#include "ui/dialogs/MessageBoxDialog.hpp"
#include "ui/Colors.hpp"

#include "factories/DWriteFactory.hpp"


namespace PGUI::UI::Dialogs
{
	using namespace PGUI::UI::Controls;

	MessageBoxDialog::MessageBoxDialog(std::wstring_view _text, 
		MessageBoxButtonSet _buttonSet, MessageBoxIcon __icon) noexcept : 
		ModalDialog{ Core::WindowClass::Create(L"MessageBoxDialog_Dialog") },
		text(_text), buttonSet(_buttonSet), _icon(__icon)
	{
		RegisterMessageHandler(WM_CREATE, &MessageBoxDialog::OnCreate);
		RegisterMessageHandler(WM_PAINT, &MessageBoxDialog::OnPaint);

		if (UIColors::IsDarkMode())
		{
			backgroundColor = 0x111111;
			textBrush = Colors::Aliceblue;
			buttonHighlightBrush = UIColors::GetBackgroundColor();
		}
		else
		{
			backgroundColor = 0xFFFFFF;
			textBrush = UIColors::GetForegroundColor();
			buttonHighlightBrush = 0xDDDDDD;
		}
	}

	void MessageBoxDialog::CreateDeviceResources()
	{
		auto g = GetGraphics();

		if (!buttonHighlightBrush)
		{
			g.CreateBrush(buttonHighlightBrush);
		}
		if (!textBrush)
		{
			g.CreateBrush(textBrush);
		}
	}

	void MessageBoxDialog::DiscardDeviceResources()
	{
		buttonHighlightBrush.ReleaseBrush();
		textBrush.ReleaseBrush();
	}

	MessageBoxChoice MessageBoxDialog::Display() noexcept
	{
		RunModal();
		return choice;
	}

	void MessageBoxDialog::CreateButtons()
	{
		auto size = GetClientSize();

		auto clipParams = RoundedRectangleClipParameters{ RoundedRect{ RectF{ }, 5.0f, 5.0f } };

		const auto createButton = [this, &size, &clipParams](std::size_t id, 
			std::wstring_view buttonText, MessageBoxChoice buttonChoice)
		{
			auto button = AddChildWindow<TextButton>(
				Core::WindowCreateParams{ buttonText,
				{ size.cx - static_cast<int>(id) * (buttonSize.cx + 20), 
				size.cy - buttonSize.cy - 20 },
				buttonSize,
				NULL
				});
			button->ClickedEvent().Subscribe(
				std::bind_front(BindMemberFunc(&MessageBoxDialog::ButtonHandler, this), buttonChoice)
			);
			button->Show();
			button->SetClip(clipParams);
			buttons.push_back(button);
		};

		switch (buttonSet)
		{
			case PGUI::UI::Dialogs::MessageBoxButtonSet::Ok:
			{
				createButton(1, L"Ok", MessageBoxChoice::Ok);
				break;
			}
			case PGUI::UI::Dialogs::MessageBoxButtonSet::OkCancel:
			{
				createButton(1, L"Cancel", MessageBoxChoice::Cancel);
				createButton(2, L"Ok", MessageBoxChoice::Ok);
				break;
			}
			case PGUI::UI::Dialogs::MessageBoxButtonSet::RetryCancel:
			{
				createButton(1, L"Cancel", MessageBoxChoice::Cancel);
				createButton(2, L"Retry", MessageBoxChoice::Retry);
				break;
			}
			case PGUI::UI::Dialogs::MessageBoxButtonSet::YesNo:
			{
				createButton(1, L"No", MessageBoxChoice::No);
				createButton(2, L"Yes", MessageBoxChoice::Yes);
				break;
			}
			case PGUI::UI::Dialogs::MessageBoxButtonSet::YesNoCancel:
			{
				createButton(1, L"Cancel", MessageBoxChoice::Cancel);
				createButton(2, L"No", MessageBoxChoice::No);
				createButton(3, L"Yes", MessageBoxChoice::Yes);
				break;
			}
			case PGUI::UI::Dialogs::MessageBoxButtonSet::CancelTryContinue:
			{
				createButton(1, L"Cancel", MessageBoxChoice::Cancel);
				createButton(2, L"Try Again", MessageBoxChoice::TryAgain);
				createButton(3, L"Continue", MessageBoxChoice::Continue);
				break;
			}
			case PGUI::UI::Dialogs::MessageBoxButtonSet::AbortRetryIgnore:
			{
				createButton(1, L"Abort", MessageBoxChoice::Abort);
				createButton(2, L"Retry", MessageBoxChoice::Retry);
				createButton(3, L"Ignore", MessageBoxChoice::Ignore);
				break;
			}
			case PGUI::UI::Dialogs::MessageBoxButtonSet::Help:
			{
				createButton(buttons.size() + 1, L"Help", MessageBoxChoice::Continue);
				break;
			}
			default:
			{
				std::unreachable();
			}
		}
	}

	void MessageBoxDialog::SetToRequiredSize() const noexcept
	{
		AdjustForClientSize(CalculateSize());
		CenterAroundParent();
	}

	void MessageBoxDialog::ButtonHandler(MessageBoxChoice _choice) noexcept
	{
		choice = _choice;
		SetRunning(false);
	}

	SizeI MessageBoxDialog::CalculateSize() const noexcept
	{
		auto metrics = textLayout.GetMetrics();
		
		SizeI size{ 
			static_cast<int>(std::ceilf(metrics.width)), 
			static_cast<int>(std::ceilf(metrics.height + metrics.top)) };

		SizeI monitorSize_5_8{  };

		if (HMONITOR monitor = MonitorFromWindow(ParentHwnd(), MONITOR_DEFAULTTONEAREST);
			monitor != nullptr)
		{
			MONITORINFOEXW monitorInfo{ };
			monitorInfo.cbSize = sizeof(monitorInfo);
			if (BOOL ret = GetMonitorInfoW(monitor, &monitorInfo);
				ret != 0)
			{
				SizeI monitorSize = RectI{ monitorInfo.rcMonitor }.Size();
				monitorSize_5_8 = 5 * monitorSize / 8;
			}
		}

		SizeI totalButtonSize{ };
		totalButtonSize = { margin.right, buttonSize.cy + margin.top + margin.bottom };
		switch (buttonSet)
		{
			using enum PGUI::UI::Dialogs::MessageBoxButtonSet;
			case Ok:
				totalButtonSize.cx += (buttonSize.cx + 20);
				break;
			case OkCancel:
			case RetryCancel:
			case YesNo:
				totalButtonSize.cx += 2 * (buttonSize.cx + 20);
				break;
			case YesNoCancel:
			case CancelTryContinue:
			case AbortRetryIgnore:
				totalButtonSize.cx += 3 * (buttonSize.cx + 20);
				break;
			default:
				break;
		}

		if (size.cx > monitorSize_5_8.cx)
		{
			size.cx = monitorSize_5_8.cx;
		}
		if (size.cy > monitorSize_5_8.cy)
		{
			size.cy = monitorSize_5_8.cy;
		}

		if (size.cx < totalButtonSize.cx)
		{
			size.cx = totalButtonSize.cx;
		}
		size.cy += totalButtonSize.cy;
		size.cy += margin.top + margin.bottom;

		if (iconBmp)
		{
			size.cx += icon.GetSize().cx + margin.left * 2;
		}
		size.cx += margin.left + margin.right;

		return size;
	}

	Core::HandlerResult MessageBoxDialog::OnCreate(UINT, WPARAM, LPARAM)
	{
		if (UIColors::IsDarkMode())
		{
			EnableDarkTitleBar(Hwnd());
		}

		maxSize = ScaleByDPI(SizeF{ maxSize });

		textFormat = TextFormat::GetDefTextFormat(ScaleByDPI(16.0f));
		textFormat.SetParagraphAlignment(Font::ParagraphAlignments::Near);
		textFormat.SetTextAlignment(Font::TextAlignments::Leading);

		auto layoutSize = GetClientSize();
		layoutSize.cx -= margin.left + margin.right;
		layoutSize.cy -= (margin.top + margin.bottom) * 2 + buttonSize.cy;

		margin.left = ScaleByDPI(margin.left);
		margin.top = ScaleByDPI(margin.top);
		margin.right = ScaleByDPI(margin.right);
		margin.bottom = ScaleByDPI(margin.bottom);

		iconSize = ScaleByDPI(SizeF{ iconSize });

		switch (_icon)
		{
			using enum MessageBoxIcon;

			#pragma warning (push)
			#pragma warning (disable : 4302)

			case Error: // Stop, Hand
			{
				auto hIcon = static_cast<HICON>(
					LoadImageW(nullptr, 
						MAKEINTRESOURCEW(IDI_ERROR), IMAGE_ICON, iconSize.cx, iconSize.cy, LR_SHARED)
					);
				icon = Bmp::Bitmap{ hIcon };
				break;
			}
			case Question:
			{
				auto hIcon = static_cast<HICON>(
					LoadImageW(nullptr, 
						MAKEINTRESOURCEW(IDI_QUESTION), IMAGE_ICON, iconSize.cx, iconSize.cy, LR_SHARED)
					);
				icon = Bmp::Bitmap{ hIcon };
				break;
			}
			case Warning: // Exclamation
			{
				auto hIcon = static_cast<HICON>(
					LoadImageW(nullptr, 
						MAKEINTRESOURCEW(IDI_WARNING), IMAGE_ICON, iconSize.cx, iconSize.cy, LR_SHARED)
					);
				icon = Bmp::Bitmap{ hIcon };
				break;
			}
			case Information: // Asterisk
			{
				auto hIcon = static_cast<HICON>(
					LoadImageW(nullptr,
						MAKEINTRESOURCEW(IDI_INFORMATION), IMAGE_ICON, iconSize.cx, iconSize.cy, LR_SHARED)
					);
				icon = Bmp::Bitmap{ hIcon };
				break;
			}
			#pragma warning (pop)
			
			default:
				break;
		}

		if (icon.ComPtrHolder<IWICBitmap>::IsInitialized())
		{
			iconBmp = GetGraphics().CreateBitmap(icon);
			layoutSize.cx -= iconSize.cx;
		}

		textLayout = TextLayout{ text, textFormat, layoutSize };

		PointL textPos{ 0, 0 };
		if (iconBmp)
		{
			textPos.x = iconBmp.GetPixelSize().cx + 10;
		}

		SetToRequiredSize();
		CreateButtons();
		AdjustForClientSize(ScaleByDPI(SizeF{ GetClientSize() }));

		buttonSize = ScaleByDPI(SizeF{ buttonSize });
		return 0;
	}

	Core::HandlerResult MessageBoxDialog::OnPaint(UINT, WPARAM, LPARAM)
	{
		BeginDraw();

		SizeF size = GetClientSize();

		auto g = GetGraphics();
		g.Clear(backgroundColor);

		g.FillRect(RectF{ 0, size.cy - 
			static_cast<float>(buttonSize.cy + margin.top + margin.bottom), 
			size.cx, size.cy },
			buttonHighlightBrush);

		PointF textOrigin{ static_cast<float>(margin.left), static_cast<float>(margin.top) };
		if (iconBmp)
		{
			float middleY = (size.cy - 
				static_cast<float>(buttonSize.cy + margin.top + margin.bottom)) / 2.f;

			g.DrawBitmap(iconBmp, RectF{
				static_cast<float>(margin.left), 
				middleY - static_cast<float>(iconSize.cy) / 2.f,
				static_cast<float>(margin.left + iconSize.cx),
				middleY + 
				static_cast<float>(iconSize.cy) / 2.f
				});
			textOrigin.x += static_cast<float>(margin.left + iconSize.cx);
		}

		g.DrawTextLayout(textOrigin, textLayout, textBrush);

		EndDraw();

		return 0;
	}

	MessageBoxChoice ShowMessageBox(HWND parentHwnd,
		std::wstring_view caption, std::wstring_view text,
		MessageBoxButtonSet buttonSet, MessageBoxIcon icon)
	{
		auto msgBox = Dialog::Create<MessageBoxDialog>(
			DialogCreateParams{ parentHwnd, DS_MODALFRAME },
			Core::WindowCreateParams{ caption, PointI{ }, SizeI{ 200, 200 }, NULL },
			text, buttonSet, icon
		);

		return msgBox->Display();
	}
}
