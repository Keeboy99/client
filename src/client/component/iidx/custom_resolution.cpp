#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "d3d9_proxy/interface_ex.hpp"

namespace iidx::custom_resolution
{
	int mode()
	{
		static auto mode = std::stoi(game::environment::get_param("IIDX_DISPLAY_MODE"));
		return mode;
	}

	int width()
	{
		static auto w = mode() == 1 ?
			GetSystemMetrics(SM_CXSCREEN) : std::stoi(game::environment::get_param("IIDX_RESOLTION_W"));
		w = w ? w : GetSystemMetrics(SM_CXSCREEN);
		return w;
	}

	int height()
	{
		static auto h = mode() == 1 ?
			GetSystemMetrics(SM_CYSCREEN) : std::stoi(game::environment::get_param("IIDX_RESOLTION_H"));
		h = h ? h : GetSystemMetrics(SM_CXSCREEN);

		return h;
	}

	int graphicsAPI() {
		static auto api = std::stoi(game::environment::get_param("IIDX_GRAPHICS_API"));
		return api;
	}

	namespace
	{
		HRESULT WINAPI create_d3d9ex(UINT SDKVersion, IDirect3D9Ex** ppD3D9Ex)
		{
			IDirect3D9Ex* d3d9ex = nullptr;
			HRESULT hr = 0;
			switch (graphicsAPI())
			{
			case 0:
			{
				hr = Direct3DCreate9Ex(SDKVersion, &d3d9ex);
				break;
			}
			case 1:
			{
				ID3D12Device* device = nullptr;
				auto d3d12hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_1, __uuidof(ID3D12Device), (void**)&device);
				if (!SUCCEEDED(d3d12hr))
				{
					return d3d12hr;
				}
				_D3D9ON12_ARGS arg;
				arg.Enable9On12 = TRUE;
				arg.pD3D12Device = device;
				arg.NumQueues = 0;
				hr = Direct3DCreate9On12Ex(SDKVersion, &arg, 1, &d3d9ex);
				break;
			}
			case 2:
			{
				auto dxvk = LoadLibraryW(L"dxvk.dll");
				if (!dxvk)
				{
					return -1;
				}
				auto dxvkDirect3DCreate9Ex = GetProcAddress(dxvk, "Direct3DCreate9Ex");
				hr = ((HRESULT(*)(UINT SDKVersion, IDirect3D9Ex * *ppD3D9Ex))dxvkDirect3DCreate9Ex)(SDKVersion, &d3d9ex);
				break;
			}
			default:
			{
				hr = Direct3DCreate9Ex(SDKVersion, &d3d9ex);
				break;
			}
			}
			if (SUCCEEDED(hr))
			{
				*ppD3D9Ex = new d3d9ex_proxy(d3d9ex);
			}
			else
			{
				*ppD3D9Ex = nullptr;
			}

			return hr;
		}

		BOOL __stdcall set_rect(LPRECT lprc, int xLeft, int yTop, int xRight, int yBottom)
		{
			lprc->left = 0;
			lprc->top = 0;
			lprc->right = width();
			lprc->bottom = height();

			return TRUE;
		}

		HWND WINAPI create_window_ex_a(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
		{
			if (mode() == 2)
			{
				dwStyle = WS_SYSMENU | WS_CAPTION | WS_VISIBLE;
				RECT rc;
				SetRect(&rc, 0, 0, width(), height());
				AdjustWindowRect(&rc, WS_VISIBLE | WS_CAPTION, false);

				nWidth = rc.right - rc.left;
				nHeight = rc.bottom - rc.top;
			}
			else
			{
				nWidth = width();
				nHeight = height();
			}

			return CreateWindowExA(dwExStyle ^ WS_EX_TOPMOST, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
		}

		BOOL WINAPI set_window_pos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
		{
			if (mode() == 2)
			{
				RECT rc;
				SetRect(&rc, 0, 0, width(), height());
				AdjustWindowRect(&rc, WS_VISIBLE | WS_CAPTION, false);

				cx = rc.right - rc.left;
				cy = rc.bottom - rc.top;
			}
			else
			{
				cx = width();
				cy = height();
			}

			return SetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
		}
	}

	class component final : public component_interface
	{
	public:

		void* load_import(const std::string& library, const std::string& function) override
		{
			if (function == "Direct3DCreate9Ex")
			{
				return create_d3d9ex;
			}
			else if (function == "SetRect")
			{
				return set_rect;
			}
			else if (function == "CreateWindowExA")
			{
				return create_window_ex_a;
			}
			else if (function == "SetWindowPos")
			{
				return set_window_pos;
			}

			return nullptr;
		}
	};
}

REGISTER_COMPONENT(iidx::custom_resolution::component, launcher::game::iidx)
