// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣


#include "./heads/d3dhook.h"


//ԭDetour��Ҫ��lib�ļ���ͷ�ļ�
//#include "detours.h"
//#if defined _M_X64
//#pragma comment(lib, "detours.X64/detours.lib")
//#elif defined _M_IX86
//#pragma comment(lib, "detours.X86/detours.lib")
//#endif

//����hook����ָ��
unique_ptr<PLH::x64Detour> presentHook;
unique_ptr<PLH::x64Detour> resizeBuffersHook;
unique_ptr<PLH::x64Detour> pSSetShaderResourcesHook;
unique_ptr<PLH::x64Detour> drawHook;
unique_ptr<PLH::x64Detour> drawIndexedHook;
unique_ptr<PLH::x64Detour> drawIndexedInstancedHook;

//��ʼ��
D3D11PresentHook phookD3D11Present = NULL;
D3D11ResizeBuffersHook phookD3D11ResizeBuffers = NULL;
D3D11PSSetShaderResourcesHook phookD3D11PSSetShaderResources = NULL;
D3D11DrawHook phookD3D11Draw = NULL;
D3D11DrawIndexedHook phookD3D11DrawIndexed = NULL;
D3D11DrawIndexedInstancedHook phookD3D11DrawIndexedInstanced = NULL;
D3D11CreateQueryHook phookD3D11CreateQuery = NULL;

ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;

DWORD_PTR* pSwapChainVtable = NULL;
DWORD_PTR* pContextVTable = NULL;
DWORD_PTR* pDeviceVTable = NULL;

//����hook����ԭʼ��ַ
uint64_t presentHookRedirectOld;
uint64_t resizeBuffersHookRedirectOld;
uint64_t pSSetShaderResourcesHookRedirectOld;
uint64_t drawHookRedirectOld;
uint64_t drawIndexedHookRedirectOld;
uint64_t drawIndexedInstancedHookRedirectOld;

//uint64_t createQueryHookRedirectOld;

//�����������hook��Ҫ������
unique_ptr<PLH::x64Detour> detour;
//�����������ַ
ULONG64 tzmAdress;
//������Ҫ��ת���Լ�����ĵ�ַ
EXTERN_C void jmpFunction();
//�Զ�������
EXTERN_C int changeMoney = 333333;
//ִ�����Զ����߼�Ҫ����ִ�еĵ�ַ
ULONG64 jmpAdress = 0;
//ԭʼ��ַ������Ҫ��ָ��
uint64_t oldAdress;
//��������
ULONG64 ScanTZM(PCHAR tzm);

// ����Ϊ1���ö����
const int MultisampleCount = 1;

// �˵�Ĭ������

bool ModelrecFinder = true;
int Wallhack = 1;
bool DeleteTexture = true;

//�Ƿ��һ�γ�ʼ��
bool firstTime = true;

// ���ģ��״̬
ID3D11DepthStencilState* DepthStencilState_FALSE = NULL; //depth off
ID3D11DepthStencilState* DepthStencilState_ORIG = NULL; //depth on
// ��դ��״̬
ID3D11RasterizerState* DEPTHBIASState_FALSE;
ID3D11RasterizerState* DEPTHBIASState_TRUE;
ID3D11RasterizerState* DEPTHBIASState_ORIG;
#define DEPTH_BIAS_D32_FLOAT(d) (d/(1/pow(2,23)))

// ��ͼ��
UINT vps = 1;
D3D11_VIEWPORT viewport;
float ScreenCenterX;
float ScreenCenterY;

// RenderTargetView �ӿڱ�ʶ���ڳ����ڼ���ʵĳ���Ŀ������Դ��
ID3D11RenderTargetView* RenderTargetView = NULL;

//vertex
ID3D11Buffer* veBuffer;
UINT Stride;
UINT veBufferOffset;
D3D11_BUFFER_DESC vedesc;

//index
ID3D11Buffer* inBuffer;
DXGI_FORMAT inFormat;
UINT        inOffset;
D3D11_BUFFER_DESC indesc;

//psgetConstantbuffers
UINT pscStartSlot;
ID3D11Buffer* pscBuffer;
D3D11_BUFFER_DESC pscdesc;

//vsgetconstantbuffers
UINT vscStartSlot;
ID3D11Buffer* vscBuffer;
D3D11_BUFFER_DESC vscdesc;

//pssetshaderresources
UINT pssrStartSlot;
ID3D11Resource* Resource;
D3D11_SHADER_RESOURCE_VIEW_DESC Descr;
D3D11_TEXTURE2D_DESC texdesc;

// window��Ϣ����
HWND window = nullptr;
// �ж�imgui״̬�Ƿ���ʾ
bool ShowMenu = false;
// ԭʼwindows��Ϣ����
static WNDPROC OriginalWndProcHandler = nullptr;

// ��־,������
bool logger = false;
int countnum = -1;
int countStride = -1;
int countIndexCount = -1;
int countpscdescByteWidth = -1;
int countindescByteWidth = -1;
int countvedescByteWidth = -1;

wchar_t reportValue[256];
#define SAFE_RELEASE(x) if (x) { x->Release(); x = NULL; }
HRESULT hr;

// �Ƿ񵯳�imgui���ص���
bool greetings = true;
bool f9 = false;
bool f10 = false;
bool f11 = false;
//ULONG64 tzmAdress = 0;
//ULONG64 healthAdress = 0;

//==========================================================================================================================
#include <string>
#include <fstream>
// Ŀ¼
char dlldir[320];
// �����ļ�����ȡĿ¼
char* GetDirectoryFile(char* filename)
{
	static char path[320];
	strcpy_s(path, dlldir);
	strcat_s(path, filename);
	return path;
}

// ��¼��־
void Log(const char* fmt, ...)
{
	if (!fmt)	return;

	char		text[4096];
	va_list		ap;
	va_start(ap, fmt);
	vsprintf_s(text, fmt, ap);
	va_end(ap);

	ofstream logfile(GetDirectoryFile("log.txt"), ios::app);
	if (logfile.is_open() && text)	logfile << text << endl;
	logfile.close();
}

//==========================================================================================================================


// ����cfg
void SaveCfg()
{
	ofstream fout;
	fout.open(GetDirectoryFile("d3dwh.ini"), ios::trunc);
	fout << "Wallhack " << Wallhack << endl;
	fout << "ModelrecFinder " << ModelrecFinder << endl;
	fout.close();
}

// ���� cfg
void LoadCfg()
{
	ifstream fin;
	string Word = "";
	fin.open(GetDirectoryFile("d3dwh.ini"), ifstream::in);
	fin >> Word >> Wallhack;
	fin >> Word >> ModelrecFinder;
	fin.close();
}

//�ص�����
LRESULT CALLBACK DXGIMsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// �����¼��Ļص�����
LRESULT CALLBACK hWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ImGuiIO& io = ImGui::GetIO();
	POINT mPos;
	GetCursorPos(&mPos);
	ScreenToClient(window, &mPos);
	ImGui::GetIO().MousePos.x = mPos.x;
	ImGui::GetIO().MousePos.y = mPos.y;
	//���ͷŷ�ϵͳ��ʱ�����������м��̽���Ĵ��ڡ�
	if (uMsg == WM_KEYUP)
	{
		//�ж��Ƿ�ΪINSERT��
		if (wParam == VK_INSERT)
		{
			//���imgui״̬Ϊ��ʾ
			if (ShowMenu)
				io.MouseDrawCursor = true;
			else
				io.MouseDrawCursor = false;
		}
	}

	if (ShowMenu)
	{
		// ��imgui������Ϣ
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		//return true;
		return CallWindowProc(OriginalWndProcHandler, hWnd, uMsg, wParam, lParam);
	}
	// ����Ϣ���ݸ���һ����
	return CallWindowProc(OriginalWndProcHandler, hWnd, uMsg, wParam, lParam);
}

//��ʼ����Ҫ��TZM����
void initTZM()
{
	//mov ecx,[rcx+00000180]
	tzmAdress = ScanTZM("8B 89 80 01 00 00 81 F9 A0");
	if (tzmAdress == 0) {
		sprintf_s(str, 100, "δɨ�赽������\n");
	}
	else if (tzmAdress == 1) {
		sprintf_s(str, 100, "�����벻Ψһ\n");
	}
	else
		sprintf_s(str, 100, "��Ч������\n%016I64X\n", tzmAdress);
	MessageBox(0, str, "��ʾ", MB_SYSTEMMODAL);
}

//==========================================================================================================================
// HOOK��Ⱦ����
HRESULT __stdcall hookD3D11Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	//Log("hookD3D11Present\n");
	// �ж��Ƿ��һ�γ�ʼ��
	if (firstTime)
	{
		Log("��ʼ��\n");
		firstTime = false; //only once




		// ��ȡָ�򴴽��˽ӿڵ��豸��ָ�롣
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
		{
			// SwapChain = pSwapChain;
			// ��ȡָ�򴴽��˽ӿڵ��豸��ָ�롣
			pSwapChain->GetDevice(__uuidof(pDevice), (void**)&pDevice);
			// GetImmediateContext��ȡ�ɻط������б�ļ�ʱ�����ġ�GetImmediateContext��������һ��ID3D11DeviceContext���󣬸ö����ʾ����ִ��Ҫ�����ύ���豸�ĳ��ֵļ�ʱ�����ġ�
			// ���ڴ����Ӧ�ó���ֱ�������������ڻ��Ƴ�������Ҫ����
			// GetImmediateContext������ֱ�������ĵ����ü������� 1����ˣ���������ʹ���귵�صĽӿ�ָ��ʱ����Release�Ա����ڴ�й©��
			pDevice->GetImmediateContext(&pContext);
		}

		//imgui
		// ������������
		DXGI_SWAP_CHAIN_DESC sd;
		// ��ȡ������ (����Ƶ��) �� DXGI 1.0 ˵����
		pSwapChain->GetDesc(&sd);
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		// ������������
		io.Fonts->AddFontFromFileTTF("c:/windows/fonts/simhei.ttf", 13.0f, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());

		// �������Ʋ˵�
		ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantTextInput || ImGui::GetIO().WantCaptureKeyboard;

		// ���ü��̿���
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		window = sd.OutputWindow;

		// SetWindowLongPtr����ָ�����ڵ����ԡ� �ú��������ڶ��ⴰ���ڴ��е�ָ��ƫ��������ֵ��
		OriginalWndProcHandler = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)hWndProc);

		ImGui_ImplWin32_Init(window);
		ImGui_ImplDX11_Init(pDevice, pContext);
		ImGui::GetIO().ImeWindowHandle = window;

		// D3D11_DEPTH_STENCIL_DESC�������ģ��״̬��
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		// �Ƿ�������Ȳ��ԡ�
		depthStencilDesc.DepthEnable = TRUE;
		// ���ֵд������
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		// ��ȱȽϺ�����
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		// �Ƿ�����ģ�߲��ԡ�
		depthStencilDesc.StencilEnable = FALSE;
		// ģ��ֵ��ȡ����
		depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		// ģ��ֵд������
		depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		// FrontFace���ýṹ��ָ���˲�ͬ���Խ���¶�ģ��ֵӦ��ʲô���ĸ��£��������泯��������Σ�
		// ��ģ����Բ�ͨ�������/ģ�建������ģ��ֵ���ֵĲ���
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		// ��ģ�����ͨ��������Ȳ��Բ�ͨ�������/ģ�建������ģ��ֵ���ֵĲ���
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		// ��ģ��/��Ȳ���ͨ�������/ģ�建������ģ��ֵ���ֵĲ���
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		// ģ��������õıȽϺ���
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		// BackFace���ýṹ��ָ���˲�ͬ���Խ���¶�ģ��ֵӦ��ʲô���ĸ��£����ڱ��泯��������Σ�
		// ��ģ����Բ�ͨ�������/ģ�建������ģ��ֵ���ֵĲ���
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		// ��ģ�����ͨ��������Ȳ��Բ�ͨ�������/ģ�建������ģ��ֵ���ֵĲ���
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		// ��ģ��/��Ȳ���ͨ�������/ģ�建������ģ��ֵ���ֵĲ���
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		// ģ��������õıȽϺ���
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		// CreateDepthStencilState�������ģ��״̬���󣬸ö����װ����ϲ��׶ε����ģ�߲�����Ϣ��
		pDevice->CreateDepthStencilState(&depthStencilDesc, &DepthStencilState_FALSE);

		// D3D11_RASTERIZER_DESC������դ��״̬��
		D3D11_RASTERIZER_DESC rasterizer_desc;
		//  ZeroMemory() ������Ϊ���㺯������Ҳ������ΪRtlZeroMemory��  ��0�����һ���ڴ�����
		ZeroMemory(&rasterizer_desc, sizeof(rasterizer_desc));
		// ȷ������ (���� D3D11_FILL_MODE) ʱҪʹ�õ����ģʽ��
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		// ָʾ����������ָ������������Σ� (���� D3D11_CULL_MODE) ��
		rasterizer_desc.CullMode = D3D11_CULL_NONE; //D3D11_CULL_FRONT;
		// ȷ�������������滹�Ǻ��� ����˲���Ϊ TRUE��������������ڳ���Ŀ������ʱ�룬���������������˳ʱ��ģ��������ν�����Ϊ���档 ����˲���Ϊ FALSE�����෴Ϊ true��
		rasterizer_desc.FrontCounterClockwise = false;
		float bias = 1000.0f;
		float bias_float = static_cast<float>(-bias);
		bias_float /= 10000.0f;
		// ��ӵ��������ص����ֵ
		rasterizer_desc.DepthBias = DEPTH_BIAS_D32_FLOAT(*(DWORD*)&bias_float);
		// ��������б���ϵı����� 
		rasterizer_desc.SlopeScaledDepthBias = 0.0f;
		// ���ص�������ƫ� 
		rasterizer_desc.DepthBiasClamp = 0.0f;
		// ���ݾ������ü�����
		rasterizer_desc.DepthClipEnable = true;
		// ���ü��������޳��� ���������֮����������ض��ᱻ�޳���
		rasterizer_desc.ScissorEnable = false;
		// ָ���ڶ��������� (MSAA) ����Ŀ����ʹ���ı߻� alpha �з�����㷨�� ����Ϊ TRUE ��ʹ���ı��߿�����㷨��������Ϊ FALSE ��ʹ�� alpha �з������㷨��
		rasterizer_desc.MultisampleEnable = false;
		// ָ���Ƿ������п����;����ִ���������ƺ� MultisampleEnable Ϊ FALSE ʱ���á�
		rasterizer_desc.AntialiasedLineEnable = false;
		// CreateRasterizerState����һ����դ��״̬���󣬸ö����֪��դ���׶ε���Ϊ��ʽ��
		pDevice->CreateRasterizerState(&rasterizer_desc, &DEPTHBIASState_FALSE);

		// D3D11_RASTERIZER_DESC������դ��״̬��
		D3D11_RASTERIZER_DESC nrasterizer_desc;
		ZeroMemory(&nrasterizer_desc, sizeof(nrasterizer_desc));
		nrasterizer_desc.FillMode = D3D11_FILL_SOLID;
		//nrasterizer_desc.CullMode = D3D11_CULL_BACK; //flickering
		nrasterizer_desc.CullMode = D3D11_CULL_NONE;
		nrasterizer_desc.FrontCounterClockwise = false;
		nrasterizer_desc.DepthBias = 0.0f;
		nrasterizer_desc.SlopeScaledDepthBias = 0.0f;
		nrasterizer_desc.DepthBiasClamp = 0.0f;
		nrasterizer_desc.DepthClipEnable = true;
		nrasterizer_desc.ScissorEnable = false;
		nrasterizer_desc.MultisampleEnable = false;
		nrasterizer_desc.AntialiasedLineEnable = false;
		pDevice->CreateRasterizerState(&nrasterizer_desc, &DEPTHBIASState_TRUE);

		// ���� cfg ����
		LoadCfg();
	}

	// render-target-view �ӿڱ�ʶ���ڳ����ڼ���ʵĳ���Ŀ������Դ��
	if (RenderTargetView == NULL)
	{
		// RSGetViewports��ȡ�󶨵���դ���׶ε��������顣
		pContext->RSGetViewports(&vps, &viewport);
		ScreenCenterX = viewport.Width / 2.0f;
		ScreenCenterY = viewport.Height / 2.0f;

		// ID3D11Texture2D 2D ����ӿڹ���ṹ���ڴ���������ݡ�
		ID3D11Texture2D* backbuffer = NULL;
		hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer);
		if (FAILED(hr)) {
			Log("Failed to get BackBuffer");
			return hr;
		}

		// CreateRenderTargetView�������ڷ�����Դ���ݵĳ���Ŀ����ͼ��
		hr = pDevice->CreateRenderTargetView(backbuffer, NULL, &RenderTargetView);
		backbuffer->Release();
		if (FAILED(hr)) {
			Log("Failed to get RenderTarget");
			return hr;
		}
	}
	else
		// OMSetRenderTargets��ԭ�ӷ�ʽ��һ����������Ŀ��󶨵�����ϲ��׶Σ��������ģ�߻������󶨵� ����ϲ��׶Ρ�
		pContext->OMSetRenderTargets(1, &RenderTargetView, NULL);


	//imgui
	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX11_NewFrame();
	ImGui::NewFrame();

	// imgui���ص���
	if (greetings)
	{
		ImVec4 Bgcol = ImColor(0.0f, 0.4f, 0.28f, 0.8f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, Bgcol);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 0.8f));

		ImGui::Begin(u8"����", &greetings, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);
		ImGui::Text(u8"�Ѽ���,ͨ��INSERT���ز˵�");
		ImGui::End();

		static DWORD lastTime = timeGetTime();
		DWORD timePassed = timeGetTime() - lastTime;
		// ��ʾʱ��
		if (timePassed > 6000)
		{
			greetings = false;
			lastTime = timeGetTime();
		}
	}

	// �����Ƿ���ʾ�˵��������Ƿ���������
	if (ShowMenu)
		ImGui::GetIO().MouseDrawCursor = 1;
	else
		ImGui::GetIO().MouseDrawCursor = 0;

	// �Ƿ���ʾ�˵�
	if (ShowMenu)
	{
		//ImGui::SetNextWindowPos(ImVec2(50.0f, 400.0f)); //pos
		// ������һ�����ڴ�С
		ImGui::SetNextWindowSize(ImVec2(510.0f, 400.0f)); //size
		ImVec4 Bgcol = ImColor(0.0f, 0.4f, 0.28f, 0.8f); //bg color
		ImGui::PushStyleColor(ImGuiCol_WindowBg, Bgcol);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 0.8f)); //frame color

		ImGui::Begin(u8"�˵�");
		//ImGui::Checkbox("Wallhack Texture", &Wallhack);

		const char* Wallhack_Options[] = { u8"�ر�", u8"���ģ��", u8"���ƫ��" };
		ImGui::Text(u8"͸��������");
		ImGui::SameLine();
		ImGui::Combo(u8"##͸��������", (int*)&Wallhack, Wallhack_Options, IM_ARRAYSIZE(Wallhack_Options));

		ImGui::Checkbox(u8"ɾ������", &DeleteTexture); //the point is to highlight textures to see which we are logging
		ImGui::Checkbox(u8"����ģ��", &ModelrecFinder);

		if (ModelrecFinder)
		{
			ImGui::SliderInt(u8"Ѱ��Stride", &countStride, -1, 148);

			if (countIndexCount >= -1 && countIndexCount <= 147)
			{
				ImGui::SliderInt(u8"Ѱ��IndexCount", &countIndexCount, -1, 148);
			}
			else if (countIndexCount >= 148 && countIndexCount <= 295)
			{
				ImGui::SliderInt(u8"Ѱ��IndexCount", &countIndexCount, 149, 296);
			}
			else if (countIndexCount >= 296 && countIndexCount <= 443)
			{
				ImGui::SliderInt(u8"Ѱ��IndexCount", &countIndexCount, 297, 444);
			}
			else if (countIndexCount >= 444 && countIndexCount <= 591)
			{
				ImGui::SliderInt(u8"Ѱ��IndexCount", &countIndexCount, 445, 592);
			}
			else if (countIndexCount >= 592 && countIndexCount <= 739)
			{
				ImGui::SliderInt(u8"Ѱ��IndexCount", &countIndexCount, 593, 740);
			}
			else if (countIndexCount >= 740 && countIndexCount <= 887)
			{
				ImGui::SliderInt(u8"Ѱ��IndexCount", &countIndexCount, 741, 888);
			}
			else if (countIndexCount >= 888 && countIndexCount <= 1035)
			{
				ImGui::SliderInt(u8"Ѱ��IndexCount", &countIndexCount, 889, 1036);
				if (countIndexCount == 1036)
					countIndexCount = -1;
			}

			ImGui::SliderInt(u8"Ѱ��pscdesc.ByteWidth", &countpscdescByteWidth, -1, 148);
			ImGui::SliderInt(u8"Ѱ��indesc.ByteWidth", &countindescByteWidth, -1, 148);
			ImGui::SliderInt(u8"Ѱ��vedesc.ByteWidth", &countvedescByteWidth, -1, 148);

			ImGui::Text(u8"�˵�˵��");
			ImGui::Text(u8"ͨ��TAB�������ѡ��,�ո�ȷ��");
			ImGui::Text(u8"ͨ��F9��¼���Ʒ�����log�ļ�");
			ImGui::Text(u8"ͨ��END��¼ɾ������log�ļ�");
			ImGui::Spacing();
			ImGui::Text(u8"��ݼ�:");
			ImGui::Text(u8"ALT + F1 �л� ����ģʽ");
			ImGui::Text(u8"ALT + F2 �л� ɾ������");
			ImGui::Text(u8"ALT + F3 �л� ����ģ��");
			ImGui::Text(u8"ͨ�� Page Up/Down ���� Stride");
			ImGui::Text(u8"ͨ�� 7/8 ���� IndexCount");
		}
		ImGui::End();
	}

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());



	return PLH::FnCast(presentHookRedirectOld, phookD3D11Present)(pSwapChain, SyncInterval, Flags);
	//return phookD3D11Present(pSwapChain, SyncInterval, Flags);
}

//==========================================================================================================================
// HOOK�������ڴ�С����
HRESULT __stdcall hookD3D11ResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
	//Log("hookD3D11ResizeBuffers\n");
	// ��֤�豸����
	ImGui_ImplDX11_InvalidateDeviceObjects();
	if (nullptr != RenderTargetView) { RenderTargetView->Release(); RenderTargetView = nullptr; }

	// ����ԭʼ�ķ���
	//HRESULT toReturn = phookD3D11ResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
	HRESULT toReturn = PLH::FnCast(resizeBuffersHookRedirectOld, phookD3D11ResizeBuffers)(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
	// �����豸����
	ImGui_ImplDX11_CreateDeviceObjects();
	return toReturn;
}

//==========================================================================================================================
// HOOK����ɫ����Դ������󶨵�������ɫ������(������ͼ)
void __stdcall hookD3D11PSSetShaderResources(ID3D11DeviceContext* pContext, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews)
{
	//Log("hookD3D11PSSetShaderResources\n");
	pssrStartSlot = StartSlot;

	// ���WndProc������û�й��ܣ�ʹ�˵���Ȼ���� 
	if (GetAsyncKeyState(VK_INSERT) & 1)
	{
		SaveCfg();
		ShowMenu = !ShowMenu;
	}

	// �ȼ�
	if (ShowMenu)
	{
		// alt + f1 to �л� ����ģʽ
		//if (GetAsyncKeyState(VK_MENU) && GetAsyncKeyState(VK_F1) & 1)
			//Wallhack = !Wallhack;

		// alt + f1 to �л� ����ģʽ
		if (GetAsyncKeyState(VK_MENU) && GetAsyncKeyState(VK_F1) & 1)
		{
			Wallhack++;
			if (Wallhack > 2) Wallhack = 0;
		}

		// alt + f2 to �л� ɾ������
		if (GetAsyncKeyState(VK_MENU) && GetAsyncKeyState(VK_F2) & 1)
			DeleteTexture = !DeleteTexture;

		// alt + f3 to �л� ģ��ʶ��
		if (GetAsyncKeyState(VK_MENU) && GetAsyncKeyState(VK_F3) & 1)
			ModelrecFinder = !ModelrecFinder;

		// ��סpage down/up��ֱ���������仯
		if (GetAsyncKeyState(VK_NEXT) & 1) //page down
			countStride--;
		if (GetAsyncKeyState(VK_PRIOR) & 1) //page up
			countStride++;

		if (GetAsyncKeyState(0x37) & 1) //7-
			countIndexCount--;
		if (GetAsyncKeyState(0x38) & 1) //8+
			countIndexCount++;

		if (GetAsyncKeyState(0x35) & 1) //5-
			countpscdescByteWidth--;
		if (GetAsyncKeyState(0x36) & 1) //6+
			countpscdescByteWidth++;
		if (GetAsyncKeyState(0x33) & 1) //3-
			countindescByteWidth--;
		if (GetAsyncKeyState(0x34) & 1) //4+
			countindescByteWidth++;
	}

	// ʹ��alt��tab
	if (GetAsyncKeyState(VK_MENU) && GetAsyncKeyState(VK_TAB) & 1)
		ShowMenu = false;
	if (GetAsyncKeyState(VK_TAB) && GetAsyncKeyState(VK_MENU) & 1)
		ShowMenu = false;
	/*
	//texture stuff (usually not needed)
	for (UINT j = 0; j < NumViews; j++)
	{
		ID3D11ShaderResourceView* pShaderResView = ppShaderResourceViews[j];
		if (pShaderResView)
		{
			pShaderResView->GetDesc(&Descr);
			ID3D11Resource *Resource;
			pShaderResView->GetResource(&Resource);
			ID3D11Texture2D *Texture = (ID3D11Texture2D *)Resource;
			Texture->GetDesc(&texdesc);

			SAFE_RELEASE(Resource);
			SAFE_RELEASE(Texture);
		}
	}
	*/
	return PLH::FnCast(pSSetShaderResourcesHookRedirectOld, phookD3D11PSSetShaderResources)(pContext, StartSlot, NumViews, ppShaderResourceViews);
	//return phookD3D11PSSetShaderResources(pContext, StartSlot, NumViews, ppShaderResourceViews);
}

//==========================================================================================================================
// HOOK���Ʒ���������ʵ����ͼԪ����
void __stdcall hookD3D11Draw(ID3D11DeviceContext* pContext, UINT VertexCount, UINT StartVertexLocation)
{
	//Log("hookD3D11Draw\n");

	if (GetAsyncKeyState(VK_F9) & 1) {
		Log("Draw called");

	}


	return PLH::FnCast(drawHookRedirectOld, phookD3D11Draw)(pContext, VertexCount, StartVertexLocation);
	//return phookD3D11Draw(pContext, VertexCount, StartVertexLocation);
}

//==========================================================================================================================
// HOOK���������ġ���ʵ������ͼԪ����
void __stdcall hookD3D11DrawIndexed(ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
	//Log("hookD3D11DrawIndexed\n");
	//// ����F9����
	//if (GetAsyncKeyState(VK_F9) & 1)
	//{
	//	MessageBox(0, "F9", "��ʾ", MB_SYSTEMMODAL);
	//	Log("DrawIndexed called");
	//}
	// ����F9����
	if (GetAsyncKeyState(VK_F9) & 1)
	{
		f9 = true;
		MessageBox(0, "F9", "��ʾ", MB_SYSTEMMODAL);
		//if (healthAdress == 0) {
		//initTZM();
		//}
		//�����߳̽��г�ʼ��
		//CreateThread(NULL, 0, startCPUHook, NULL, 0, NULL);
	}
	//	// ����F10����
	//	if (GetAsyncKeyState(VK_F10) & 1)
	//	{
	//		f10 = true;
	//		if (tzmAdress == 0) {
	//			tzmAdress = ScanTZM("8B 81 80 01 00 00 F7 D8 05 FF FF FF 7F");// 8B 81 80 01 00 00 mov eax,[rcx+00000180]
	//			if (tzmAdress == 0) {
	//				return phookD3D11DrawIndexed(pContext, IndexCount, StartIndexLocation, BaseVertexLocation);
	//			}
	//		}
	//#ifdef _WIN64
	//#else
	//#endif // _WIN64
	//
	//
	//	}
	//	// ����F11����
	//	if (GetAsyncKeyState(VK_F11) & 1)
	//	{
	//		//if (nowMoeny == 0) {
	//		//ָ������ת��
	//		//nowMoeny = reinterpret_cast <uintptr_t*>(playerAdress + 0x180);
	//		//}
	//		//if (nowHp == 0) {
	//		//nowHp = reinterpret_cast <uintptr_t*>(playerAdress + 0x174);
	//		//}
	//		//if (nowLev == 0) {
	//		//nowLev = reinterpret_cast <uintptr_t*>(playerAdress + 0x164);
	//		//}
	//		//ֱ���޸��ڴ���ָ���Ӧ��ֵ
	//		//*nowMoeny = 99999999;
	//		//*nowHp = 654321;
	//		//*nowLev = 1;
	//	}

		// �����Ϸ��DrawIndexed�л������ģ�ͣ���ô�����������е�����(������Ĵ���)
		// ��ȡ stride & vedesc.ByteWidth
	pContext->IAGetVertexBuffers(0, 1, &veBuffer, &Stride, &veBufferOffset);
	if (veBuffer != NULL)
		veBuffer->GetDesc(&vedesc);
	if (veBuffer != NULL) { veBuffer->Release(); veBuffer = NULL; }

	// ��ȡ indesc.ByteWidth (comment out if not used)
	pContext->IAGetIndexBuffer(&inBuffer, &inFormat, &inOffset);
	if (inBuffer != NULL)
		inBuffer->GetDesc(&indesc);
	if (inBuffer != NULL) { inBuffer->Release(); inBuffer = NULL; }

	// ��ȡ pscdesc.ByteWidth (comment out if not used)
	pContext->PSGetConstantBuffers(pscStartSlot, 1, &pscBuffer);
	if (pscBuffer != NULL)
		pscBuffer->GetDesc(&pscdesc);
	if (pscBuffer != NULL) { pscBuffer->Release(); pscBuffer = NULL; }

	// ��ȡ vscdesc.ByteWidth (comment out if not used)
	pContext->VSGetConstantBuffers(vscStartSlot, 1, &vscBuffer);
	if (vscBuffer != NULL)
		vscBuffer->GetDesc(&vscdesc);
	if (vscBuffer != NULL) { vscBuffer->Release(); vscBuffer = NULL; }


	// �ж�ģʽ�Ƿ�����
	if (Wallhack == 1 || Wallhack == 2) //if wallhack option is enabled in menu
	//
	//ut4 model recognition example
	//if ((Stride == 32 && IndexCount == 10155)||(Stride == 44 && IndexCount == 11097)||(Stride == 40 && IndexCount == 11412)||(Stride == 40 && IndexCount == 11487)||(Stride == 44 && IndexCount == 83262)||(Stride == 40 && IndexCount == 23283))
	//if (Stride == 40 && pscdesc.ByteWidth == 256 && vscdesc.ByteWidth == 4096 && pssrStartSlot == 0) //swbf2 incomplete
	//_____________________________________________________________________________________________________________________________________________________________
	// ģ��ʶ��������鿴log.txt�Ի����ȷ��Stride�ȡ��������Ҫ�������飬�����ĸ�ֵ����Ч
		if ((countnum == pssrStartSlot || countStride == Stride || countIndexCount == IndexCount / 100 || countpscdescByteWidth == pscdesc.ByteWidth / 10 ||
			countindescByteWidth == indesc.ByteWidth / 1000 || countvedescByteWidth == vedesc.ByteWidth / 10000))
			//_____________________________________________________________________________________________________________________________________________________________
			//			
		{
			// ��ȡ����ϲ��׶ε����ģ��״̬��
			if (Wallhack == 1)
				pContext->OMGetDepthStencilState(&DepthStencilState_ORIG, 0); //get original

			// ��������ϲ��׶ε����ģ��״̬��
			if (Wallhack == 1)
				pContext->OMSetDepthStencilState(DepthStencilState_FALSE, 0); //depthstencil off

			// Ϊ�ܵ��Ĺ�դ���׶����ù�դ��״̬ ��
			if (Wallhack == 2)
				pContext->RSSetState(DEPTHBIASState_FALSE); //depthbias off

			// ���»���
			phookD3D11DrawIndexed(pContext, IndexCount, StartIndexLocation, BaseVertexLocation); //redraw

			// ��ԭ���ģ��״̬
			if (Wallhack == 1)
				pContext->OMSetDepthStencilState(DepthStencilState_ORIG, 0); //depthstencil on

			// ���ù�դ��״̬ (����Ϊtrue�������ǻָ�ԭʼ���Ի����һ��wallhackЧ��)
			if (Wallhack == 2)
				pContext->RSSetState(DEPTHBIASState_TRUE); //depthbias true

			// �ͷ�
			if (Wallhack == 1)
				SAFE_RELEASE(DepthStencilState_ORIG); //release
		}

	// ģ��д��log�ļ�
	if (ShowMenu)
	{
		// ��μ�¼ģ��:
		// ������Ϸ��ע��dll���򿪲˵�
		//0. ��F9�鿴�ĸ���ͼ��������Ϸ����
		//1. ѡ��ɾ������
		//2. ѡ��Stride��ʹ�û���ֱ������ģ��/������ʧ
		//3. ��END����ģ��/�����ֵ��¼��log.txt
		//4. ��Stride������ӵ�����ģ��ʶ���У�����if(Stride == 32)
		//5. ��ģ�͵���һ����־IndexCount
		//6. ���IndexCount�����ģ�͹�������if(Stride == 32 && IndexCount == 10155)
		//7. �ȵ�

		if ((countnum == pssrStartSlot || countStride == Stride || countIndexCount == IndexCount / 100 || countpscdescByteWidth == pscdesc.ByteWidth / 10 ||
			countindescByteWidth == indesc.ByteWidth / 1000 || countvedescByteWidth == vedesc.ByteWidth / 10000))
			if (GetAsyncKeyState(VK_END) & 1)
				Log("Stride == %d && IndexCount == %d && indesc.ByteWidth == %d && vedesc.ByteWidth == %d && pscdesc.ByteWidth == %d && vscdesc.ByteWidth == %d && pssrStartSlot == %d && vscStartSlot == %d",
					Stride, IndexCount, indesc.ByteWidth, vedesc.ByteWidth, pscdesc.ByteWidth, vscdesc.ByteWidth, pssrStartSlot, vscStartSlot);

		//log specific model
		//if (Stride == 40 && pscdesc.ByteWidth == 256 && vscdesc.ByteWidth == 4096 && pssrStartSlot == 0)
		//if (GetAsyncKeyState(VK_F10) & 1)
		//Log("Stride == %d && IndexCount == %d && indesc.ByteWidth == %d && vedesc.ByteWidth == %d && pscdesc.ByteWidth == %d && vscdesc.ByteWidth == %d && pssrStartSlot == %d && vscStartSlot == %d && Descr.Format == %d && Descr.Buffer.NumElements == %d && texdesc.Format == %d && texdesc.Height == %d && texdesc.Width == %d",
			//Stride, IndexCount, indesc.ByteWidth, vedesc.ByteWidth, pscdesc.ByteWidth, vscdesc.ByteWidth, pssrStartSlot, vscStartSlot, Descr.Format, Descr.Buffer.NumElements, texdesc.Format, texdesc.Height, texdesc.Width);

		// ����˵�������ɾ������
		if (DeleteTexture)
			if ((countnum == pssrStartSlot || countStride == Stride || countIndexCount == IndexCount / 100 || countpscdescByteWidth == pscdesc.ByteWidth / 10 ||
				countindescByteWidth == indesc.ByteWidth / 1000 || countvedescByteWidth == vedesc.ByteWidth / 100000))
				// ɾ������
				return;
	}
	//return phookD3D11DrawIndexed(pContext, IndexCount, StartIndexLocation, BaseVertexLocation);
	return PLH::FnCast(drawIndexedHookRedirectOld, phookD3D11DrawIndexed)(pContext, IndexCount, StartIndexLocation, BaseVertexLocation);
}

//==========================================================================================================================
// HOOK����������ʵ����ͼԪ����
void __stdcall hookD3D11DrawIndexedInstanced(ID3D11DeviceContext* pContext, UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation)
{
	//Log("hookD3D11DrawIndexedInstanced\n");
	// ����F9����
	if (GetAsyncKeyState(VK_F9) & 1) {
		MessageBox(0, "f9�ɹ�", 0, MB_SYSTEMMODAL);
		//hook���ɶ�
		initTZM();
		MessageBox(0, "�������ʼ���ɹ�", 0, MB_SYSTEMMODAL);
		jmpAdress = tzmAdress + 6;
		detour = make_unique<PLH::x64Detour>((uint64_t)(tzmAdress), (uint64_t)&jmpFunction, &oldAdress);
		if (detour->hook())
		{
			sprintf_s(str, 100, "hook�ɹ�\n");
			MessageBox(0, str, "��ʾ", MB_SYSTEMMODAL);
		}
		else {
			sprintf_s(str, 100, "hookʧ��\n");
			MessageBox(0, str, "��ʾ", MB_SYSTEMMODAL);
		}

		//mov ecx,[rcx+00000180]
	}
	if (GetAsyncKeyState(VK_F10) & 1) {
		MessageBox(0, "�޸Ľ��", 0, MB_SYSTEMMODAL);
		changeMoney = 55555;
	}

	//�����Ϸ��DrawIndexedInstanced�л������ģ�ͣ���ô�����������е�����(������Ĵ���)


	// ��ȡ stride & vedesc.ByteWidth
	pContext->IAGetVertexBuffers(0, 1, &veBuffer, &Stride, &veBufferOffset);
	if (veBuffer != NULL)
		veBuffer->GetDesc(&vedesc);
	if (veBuffer != NULL) { veBuffer->Release(); veBuffer = NULL; }

	// ��ȡ indesc.ByteWidth (comment out if not used)
	pContext->IAGetIndexBuffer(&inBuffer, &inFormat, &inOffset);
	if (inBuffer != NULL)
		inBuffer->GetDesc(&indesc);
	if (inBuffer != NULL) { inBuffer->Release(); inBuffer = NULL; }

	// ��ȡ pscdesc.ByteWidth (comment out if not used)
	pContext->PSGetConstantBuffers(pscStartSlot, 1, &pscBuffer);
	if (pscBuffer != NULL)
		pscBuffer->GetDesc(&pscdesc);
	if (pscBuffer != NULL) { pscBuffer->Release(); pscBuffer = NULL; }

	// ��ȡ vscdesc.ByteWidth (comment out if not used)
	pContext->VSGetConstantBuffers(vscStartSlot, 1, &vscBuffer);
	if (vscBuffer != NULL)
		vscBuffer->GetDesc(&vscdesc);
	if (vscBuffer != NULL) { vscBuffer->Release(); vscBuffer = NULL; }


	// �ж�ģʽ�Ƿ�����
	if (Wallhack == 1 || Wallhack == 2) //if wallhack option is enabled in menu
	//
	//ut4 model recognition example
	//if ((Stride == 32 && IndexCount == 10155)||(Stride == 44 && IndexCount == 11097)||(Stride == 40 && IndexCount == 11412)||(Stride == 40 && IndexCount == 11487)||(Stride == 44 && IndexCount == 83262)||(Stride == 40 && IndexCount == 23283))
	//if (Stride == 40 && pscdesc.ByteWidth == 256 && vscdesc.ByteWidth == 4096 && pssrStartSlot == 0) //swbf2 incomplete
	//_____________________________________________________________________________________________________________________________________________________________
	// ģ��ʶ��������鿴log.txt�Ի����ȷ��Stride�ȡ��������Ҫ�������飬�����ĸ�ֵ����Ч
		if ((countnum == pssrStartSlot || countStride == Stride || countIndexCount == IndexCountPerInstance / 100 || countpscdescByteWidth == pscdesc.ByteWidth / 10 ||
			countindescByteWidth == indesc.ByteWidth / 1000 || countvedescByteWidth == vedesc.ByteWidth / 10000))
			//_____________________________________________________________________________________________________________________________________________________________
			//			
		{
			// ��ȡ����ϲ��׶ε����ģ��״̬��
			if (Wallhack == 1)
				pContext->OMGetDepthStencilState(&DepthStencilState_ORIG, 0); //get original

			// ��������ϲ��׶ε����ģ��״̬
			if (Wallhack == 1)
				pContext->OMSetDepthStencilState(DepthStencilState_FALSE, 0); //depthstencil off

			// Ϊ�ܵ��Ĺ�դ���׶����ù�դ��״̬ 
			if (Wallhack == 2)
				pContext->RSSetState(DEPTHBIASState_FALSE); //depthbias off

			// ���»���
			phookD3D11DrawIndexedInstanced(pContext, IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation); //redraw

			// ��ԭ���ģ��״̬
			if (Wallhack == 1)
				pContext->OMSetDepthStencilState(DepthStencilState_ORIG, 0); //depthstencil on

			// ���ù�դ��״̬ (����Ϊtrue�������ǻָ�ԭʼ���Ի����һ��wallhackЧ��)
			if (Wallhack == 2)
				pContext->RSSetState(DEPTHBIASState_TRUE); //depthbias true

			// �ͷ�
			if (Wallhack == 1)
				SAFE_RELEASE(DepthStencilState_ORIG); //release
		}

	// ģ��д��log�ļ�
	if (ShowMenu)
	{
		// ��μ�¼ģ��:
		// ������Ϸ��ע��dll���򿪲˵�
		//0. ��F9�鿴�ĸ���ͼ��������Ϸ����
		//1. ѡ��ɾ������
		//2. ѡ��Stride��ʹ�û���ֱ������ģ��/������ʧ
		//3. ��END����ģ��/�����ֵ��¼��log.txt
		//4. ��Stride������ӵ�����ģ��ʶ���У�����if(Stride == 32)
		//5. ��ģ�͵���һ����־IndexCount
		//6. ���IndexCount�����ģ�͹�������if(Stride == 32 && IndexCount == 10155)
		//7. �ȵ�

		if ((countnum == pssrStartSlot || countStride == Stride || countIndexCount == IndexCountPerInstance / 100 || countpscdescByteWidth == pscdesc.ByteWidth / 10 ||
			countindescByteWidth == indesc.ByteWidth / 1000 || countvedescByteWidth == vedesc.ByteWidth / 10000))
			if (GetAsyncKeyState(VK_END) & 1)
				Log("Stride == %d && IndexCountPerInstance == %d && indesc.ByteWidth == %d && vedesc.ByteWidth == %d && pscdesc.ByteWidth == %d && vscdesc.ByteWidth == %d && pssrStartSlot == %d && vscStartSlot == %d",
					Stride, IndexCountPerInstance, indesc.ByteWidth, vedesc.ByteWidth, pscdesc.ByteWidth, vscdesc.ByteWidth, pssrStartSlot, vscStartSlot);

		//log specific model
		//if (Stride == 40 && pscdesc.ByteWidth == 256 && vscdesc.ByteWidth == 4096 && pssrStartSlot == 0)
		//if (GetAsyncKeyState(VK_F10) & 1)
		//Log("Stride == %d && IndexCountPerInstance == %d && indesc.ByteWidth == %d && vedesc.ByteWidth == %d && pscdesc.ByteWidth == %d && vscdesc.ByteWidth == %d && pssrStartSlot == %d && vscStartSlot == %d && Descr.Format == %d && Descr.Buffer.NumElements == %d && texdesc.Format == %d && texdesc.Height == %d && texdesc.Width == %d",
			//Stride, IndexCountPerInstance, indesc.ByteWidth, vedesc.ByteWidth, pscdesc.ByteWidth, vscdesc.ByteWidth, pssrStartSlot, vscStartSlot, Descr.Format, Descr.Buffer.NumElements, texdesc.Format, texdesc.Height, texdesc.Width);

		// ����˵�������ɾ������
		if (DeleteTexture)
			if ((countnum == pssrStartSlot || countStride == Stride || countIndexCount == IndexCountPerInstance / 100 || countpscdescByteWidth == pscdesc.ByteWidth / 10 ||
				countindescByteWidth == indesc.ByteWidth / 1000 || countvedescByteWidth == vedesc.ByteWidth / 100000))
				// ɾ������
				return;
	}

	return PLH::FnCast(drawIndexedInstancedHookRedirectOld, phookD3D11DrawIndexedInstanced)(pContext, IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
	//return phookD3D11DrawIndexedInstanced(pContext, IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
}

//==========================================================================================================================
//HOOK�� GPU ��ѯ��Ϣ�ĺ���
void __stdcall hookD3D11CreateQuery(ID3D11Device* pDevice, const D3D11_QUERY_DESC* pQueryDesc, ID3D11Query** ppQuery)
{
	/*
	// �����ڵ�����ֹͨ��ĳЩ������Ⱦ���ģ��
	// ����FPS�����Ƽ���ֻ�����ڿͻ����ڵ������
	if (pQueryDesc->Query == D3D11_QUERY_OCCLUSION)
	{
		D3D11_QUERY_DESC oqueryDesc = CD3D11_QUERY_DESC();
		(&oqueryDesc)->MiscFlags = pQueryDesc->MiscFlags;
		(&oqueryDesc)->Query = D3D11_QUERY_TIMESTAMP;

		return phookD3D11CreateQuery(pDevice, &oqueryDesc, ppQuery);
	}
	*/
	return phookD3D11CreateQuery(pDevice, pQueryDesc, ppQuery);
}

//������Ҫ���ҵ�dll���ƺͺ��������غ�����ַ
/**
ʹ��ʾ��:
std::unordered_map<std::string, std::string> functionNames = {
		{"Present", "Present"},
		{"ResizeBuffers", "ResizeBuffers"},
		// ����������...
	};

	std::unordered_map<std::string, FARPROC> functionAddresses = GetFunctionAddresses("dxgi.dll", functionNames);

	// ʹ�ú�����ַ
	Present_t pPresent = reinterpret_cast<Present_t>(functionAddresses["Present"]);
	ResizeBuffers_t pResizeBuffers = reinterpret_cast<ResizeBuffers_t>(functionAddresses["ResizeBuffers"]);
	// ��������...

	// ��ӡ������ַ
	std::cout << "Present address: " << pPresent << std::endl;
	std::cout << "ResizeBuffers address: " << pResizeBuffers << std::endl;
*/
std::unordered_map<std::string, FARPROC> GetFunctionAddresses(const std::string& moduleName, const std::unordered_map<std::string, std::string>& functionNames)
{
	std::unordered_map<std::string, FARPROC> functionAddresses;

	HMODULE hModule = GetModuleHandleA(moduleName.c_str());
	if (hModule != nullptr)
	{
		for (const auto& functionName : functionNames)
		{
			FARPROC pFunction = GetProcAddress(hModule, functionName.second.c_str());
			if (pFunction != nullptr)
			{
				functionAddresses[functionName.first] = pFunction;
				Log("��ֵ��ַ%p:\n", pFunction);
			}
			else
			{
				std::cout << "Failed to get address of function: " << functionName.first << std::endl;
			}
		}
	}
	else
	{
		std::cout << "Failed to get module handle for: " << moduleName << std::endl;
	}

	return functionAddresses;
}

// ��ʼ��HOOK
DWORD __stdcall InitHooks(LPVOID)
{
	HMODULE hDXGIDLL = 0;
	do
	{
		hDXGIDLL = GetModuleHandle("dxgi.dll");
		Sleep(4000);
	} while (!hDXGIDLL);
	Sleep(100);

	IDXGISwapChain* pSwapChain;

	WNDCLASSEXA wc = { sizeof(WNDCLASSEX), CS_CLASSDC, DXGIMsgProc, 0L, 0L, GetModuleHandleA(NULL), NULL, NULL, NULL, NULL, "DX", NULL };
	RegisterClassExA(&wc);
	HWND hWnd = CreateWindowA("DX", NULL, WS_OVERLAPPEDWINDOW, 100, 100, 300, 300, NULL, NULL, wc.hInstance, NULL);

	D3D_FEATURE_LEVEL requestedLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1 };
	D3D_FEATURE_LEVEL obtainedLevel;

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(scd));
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = MultisampleCount;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Windowed = ((GetWindowLongPtr(hWnd, GWL_STYLE) & WS_POPUP) != 0) ? false : true;

	scd.BufferDesc.Width = 1;
	scd.BufferDesc.Height = 1;
	scd.BufferDesc.RefreshRate.Numerator = 0;
	scd.BufferDesc.RefreshRate.Denominator = 1;

	UINT createFlags = 0;
#ifdef _DEBUG
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	if (FAILED(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createFlags,
		requestedLevels,
		sizeof(requestedLevels) / sizeof(D3D_FEATURE_LEVEL),
		D3D11_SDK_VERSION,
		&scd,
		&pSwapChain,
		&pDevice,
		&obtainedLevel,
		&pContext)))
	{
		MessageBox(hWnd, "Failed to create directX device and swapchain!", "Error", MB_ICONERROR);
		return NULL;
	}

	pSwapChainVtable = (DWORD_PTR*)pSwapChain;
	pSwapChainVtable = (DWORD_PTR*)pSwapChainVtable[0];

	pContextVTable = (DWORD_PTR*)pContext;
	pContextVTable = (DWORD_PTR*)pContextVTable[0];

	pDeviceVTable = (DWORD_PTR*)pDevice;
	pDeviceVTable = (DWORD_PTR*)pDeviceVTable[0];

	phookD3D11Present = (D3D11PresentHook)(DWORD_PTR*)pSwapChainVtable[8];
	phookD3D11ResizeBuffers = (D3D11ResizeBuffersHook)(DWORD_PTR*)pSwapChainVtable[13];
	phookD3D11PSSetShaderResources = (D3D11PSSetShaderResourcesHook)(DWORD_PTR*)pContextVTable[8];
	phookD3D11Draw = (D3D11DrawHook)(DWORD_PTR*)pContextVTable[13];
	phookD3D11DrawIndexed = (D3D11DrawIndexedHook)(DWORD_PTR*)pContextVTable[12];
	phookD3D11DrawIndexedInstanced = (D3D11DrawIndexedInstancedHook)(DWORD_PTR*)pContextVTable[20];
	//phookD3D11CreateQuery = (D3D11CreateQueryHook)(DWORD_PTR*)pDeviceVTable[24];

	//ԭDetour��hook����
	// 
	// ��ʼ����
	//DetourTransactionBegin();
	//DetourUpdateThread(GetCurrentThread());
	// �趨hook����
	//DetourAttach(&(LPVOID&)phookD3D11Present, (PBYTE)hookD3D11Present);
	//DetourAttach(&(LPVOID&)phookD3D11ResizeBuffers, (PBYTE)hookD3D11ResizeBuffers);
	//DetourAttach(&(LPVOID&)phookD3D11PSSetShaderResources, (PBYTE)hookD3D11PSSetShaderResources);
	//DetourAttach(&(LPVOID&)phookD3D11Draw, (PBYTE)hookD3D11Draw);
	//DetourAttach(&(LPVOID&)phookD3D11DrawIndexed, (PBYTE)hookD3D11DrawIndexed);
	//DetourAttach(&(LPVOID&)phookD3D11DrawIndexedInstanced, (PBYTE)hookD3D11DrawIndexedInstanced);
	//DetourAttach(&(LPVOID&)phookD3D11CreateQuery, (PBYTE)hookD3D11CreateQuery);
	// �ύhook
	//DetourTransactionCommit();


	//polyhook2��hook����
	// 
	//ָ��hook����
	presentHook = make_unique<PLH::x64Detour>(reinterpret_cast<std::uint64_t>(phookD3D11Present), reinterpret_cast<std::uint64_t>(&hookD3D11Present), &presentHookRedirectOld);
	resizeBuffersHook = make_unique<PLH::x64Detour>(reinterpret_cast<std::uint64_t>(phookD3D11ResizeBuffers), reinterpret_cast<std::uint64_t>(&hookD3D11ResizeBuffers), &resizeBuffersHookRedirectOld);
	pSSetShaderResourcesHook = make_unique<PLH::x64Detour>(reinterpret_cast<std::uint64_t>(phookD3D11PSSetShaderResources), reinterpret_cast<std::uint64_t>(&hookD3D11PSSetShaderResources), &pSSetShaderResourcesHookRedirectOld);
	drawHook = make_unique<PLH::x64Detour>(reinterpret_cast<std::uint64_t>(phookD3D11Draw), reinterpret_cast<std::uint64_t>(&hookD3D11Draw), &drawHookRedirectOld);
	drawIndexedHook = make_unique<PLH::x64Detour>(reinterpret_cast<std::uint64_t>(phookD3D11DrawIndexed), reinterpret_cast<std::uint64_t>(&hookD3D11DrawIndexed), &drawIndexedHookRedirectOld);
	drawIndexedInstancedHook = make_unique<PLH::x64Detour>(reinterpret_cast<std::uint64_t>(phookD3D11DrawIndexedInstanced), reinterpret_cast<std::uint64_t>(&hookD3D11DrawIndexedInstanced), &drawIndexedInstancedHookRedirectOld);

	//��ʼhook
	presentHook->hook();
	resizeBuffersHook->hook();
	pSSetShaderResourcesHook->hook();
	drawHook->hook();
	drawIndexedHook->hook();
	drawIndexedInstancedHook->hook();

	//��ԭ�ڴ���

	DWORD dwOld;
	VirtualProtect(phookD3D11Present, 2, PAGE_EXECUTE_READWRITE, &dwOld);

	while (true) {
		Sleep(10);
	}

	pDevice->Release();
	pContext->Release();
	pSwapChain->Release();

	return NULL;
}


//BOOL APIENTRY DllMain(HMODULE hModule,
//	DWORD  ul_reason_for_call,
//	LPVOID lpReserved
//)
//{
//	switch (ul_reason_for_call) {
//		// ����dll
//	case DLL_PROCESS_ATTACH:
//		// ����ָ����DLL��DLL_THREAD_ATTACH��DLL_THREAD_DETACH֪ͨ����СĳЩ����Ĺ�������С
//		DisableThreadLibraryCalls(hModule);
//#ifdef _WIN64
//		////��ȡ��ǰ�����Ѽ���ģ����ļ�������·������ģ������ɵ�ǰ���̼���
//		//GetModuleFileName(hModule, dlldir, 512);
//		//// ����·��
//		//for (size_t i = strlen(dlldir); i > 0; i--) { if (dlldir[i] == '\\') { dlldir[i + 1] = 0; break; } }
//		////��ʼ������ת��
//		//for (INT i = 0; i < sizeof(m_dwReturn) / sizeof(DWORD); i++)
//		//{
//		//	m_dwReturn[i] = TlsAlloc();
//		//}
//		//InitAddr();
//#else
//		//Load();
//#endif
//		MessageBox(0, "DLL�ɹ�", 0, MB_SYSTEMMODAL);
//		// �����߳�ִ��HOOK��ʼ��
//		CreateThread(NULL, 0, InitHooks, NULL, 0, NULL);
//		break;
//	case DLL_THREAD_ATTACH:break;
//	case DLL_THREAD_DETACH:break;
//		// ж��dll
//	case DLL_PROCESS_DETACH:
//		// ȡ��hook
//		//DetourTransactionBegin();
//		//DetourUpdateThread(GetCurrentThread());
//		//DetourDetach(&(LPVOID&)phookD3D11Present, (PBYTE)hookD3D11Present);
//		//DetourDetach(&(LPVOID&)phookD3D11ResizeBuffers, (PBYTE)hookD3D11ResizeBuffers);
//		//DetourDetach(&(LPVOID&)phookD3D11PSSetShaderResources, (PBYTE)hookD3D11PSSetShaderResources);
//		//DetourDetach(&(LPVOID&)phookD3D11Draw, (PBYTE)hookD3D11Draw);
//		//DetourDetach(&(LPVOID&)phookD3D11DrawIndexed, (PBYTE)hookD3D11DrawIndexed);
//		//DetourDetach(&(LPVOID&)phookD3D11DrawIndexedInstanced, (PBYTE)hookD3D11DrawIndexedInstanced);
//		////DetourDetach(&(LPVOID&)phookD3D11CreateQuery, (PBYTE)hookD3D11CreateQuery);
//		//DetourTransactionCommit();
//
//		presentHook->unHook();
//		resizeBuffersHook->unHook();
//		pSSetShaderResourcesHook->unHook();
//		drawHook->unHook();
//		drawIndexedHook->unHook();
//		drawIndexedInstancedHook->unHook();
//		//createQueryHook->unHook();
//
//#ifdef _WIN64
//		//�ͷ��ڴ�
//		/*for (INT i = 0; i < sizeof(m_dwReturn) / sizeof(DWORD); i++)
//		{
//			TlsFree(m_dwReturn[i]);
//		}
//		Free();*/
//#else
//		Free();
//#endif
//		break;
//	}
//return TRUE;
//}

