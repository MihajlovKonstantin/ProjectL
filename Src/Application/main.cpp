#include "main.h"

#include "Scene.h"
#pragma comment(lib, "Shlwapi.lib")
//===================================================================
// メイン
//===================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszArgs, int nWinMode)
{
	// メモリリークを知らせる
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// COM初期化
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	// mbstowcs_s関数で日本語対応にするために呼ぶ
	setlocale(LC_ALL, "japanese");

	//===================================================================
	// 実行
	//===================================================================
	APP.Execute();

	// COM解放
	CoUninitialize();

	return 0;
}


// アプリケーション初期設定
bool Application::Init(int w, int h)
{

	//===================================================================
	// ウィンドウ作成
	//===================================================================
	if (m_window.Create(w, h, "ProjectL", "Window") == false) {
		MessageBoxA(nullptr, "ウィンドウ作成に失敗", "エラー", MB_OK);
		return false;
	}

	//===================================================================
	// フルスクリーン確認
	//===================================================================
	bool bFullScreen = false;
	/*if (MessageBoxA(m_window.GetWndHandle(), "フルスクリーンにしますか？", "確認", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES) {
		bFullScreen = true;
	}*/


	//===================================================================
	// Direct3D
	//===================================================================

	// デバイスのデバッグモードを有効にする
	bool deviceDebugMode = false;
	#ifdef _DEBUG
	deviceDebugMode = true;
	#endif

	// Direct3D初期化
	std::string errorMsg;
	if (D3D.Init(m_window.GetWndHandle(), w, h, deviceDebugMode, errorMsg) == false) {
		MessageBoxA(m_window.GetWndHandle(), errorMsg.c_str(), "Direct3D初期化失敗", MB_OK | MB_ICONSTOP);
		return false;
	}

	// フルスクリーン設定
	if (bFullScreen) {
		D3D.GetSwapChain()->SetFullscreenState(TRUE, 0);
	}

	// シェーダー初期化
	SHADER.Init();

	//===================================================================
	// XAudio2
	//===================================================================
	if (AUDIO.Init() == false)
	{
		MessageBoxA(m_window.GetWndHandle(), errorMsg.c_str(), "サウンド初期化失敗", MB_OK | MB_ICONSTOP);
		return false;
	}

	//===================================================================
	// imgui初期設定
	//===================================================================
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// Setup Dear ImGui style
	ImGui::StyleColorsClassic();
	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(m_window.GetWndHandle());
	ImGui_ImplDX11_Init(D3D.GetDev(), D3D.GetDevContext());

	{
		// 日本語対応
		#include "imgui/ja_glyph_ranges.h"
		ImFontConfig config;
		config.MergeMode = true;
		io.Fonts->AddFontDefault();
		io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msgothic.ttc", 13.0f, &config, glyphRangesJapanese);
	}
	InitMenus();

	return true;
}

bool Application::InitMenus()
{
	if (!m_mainMenu)
	{
		Menu* _menu = new Menu("MainMenu");
		m_mainMenu = _menu;
		m_mainMenu->SetBackTexture(&m_mainMenuBackTex);
	}
	return false;
}

bool Application::InitImages()
{
	bool dataLeak = false;
	if(InitBackTex()!=false)dataLeak = true;

	if (dataLeak)
	{
		MessageBoxA(m_window.GetWndHandle(), "TextureLoadのときDataLeakがはっせした", "画像初期化失敗", MB_OK | MB_ICONSTOP);
	}
	return dataLeak;
}

bool Application::InitBackTex()
{
	bool dataLeak = false;
	std::string errorMsg;
	if (!m_mainMenuBackTex.Load("Data/Texture/BackGrounds/MainMenuBackTex.png"))
	{
		errorMsg = "MainMenuBackTexture";
		MessageBoxA(m_window.GetWndHandle(), errorMsg.c_str(), "画像初期化失敗", MB_OK | MB_ICONSTOP);
		dataLeak = true;
	}
	return dataLeak;
}

// アプリケーション終了
void Application::Release()
{
	D3D.GetSwapChain()->SetFullscreenState(FALSE, 0);

	// imgui解放
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// シェーダ解放
	SHADER.Release();

	// Direct3D解放
	D3D.Release();

	// XAudio2解放
	AUDIO.Release();

	// ウィンドウ削除
	m_window.Release();

	{
		delete m_mainMenu;
		m_mainMenu = nullptr;
	}
}

bool Application::CreateDataPath()
{
	char buffer[1024];
	if (getcwd(buffer, sizeof(buffer)) != nullptr) {
		std::string currentDir(buffer);
		m_programmPath = currentDir;
	}

	const char* _homePath = std::getenv("USERPROFILE");
	if (!_homePath)
	{
		std::cerr << "Dont have access to Home directory";
		return false;
	}
	char _documentPath[512];
	std::strcpy(_documentPath, _homePath);
	std::strcat(_documentPath, "\\Documents");

	char _result[512];
	std::strcpy(_result, _documentPath);
	std::strcat(_result, "\\ProjectL");

	if (!PathFileExistsA(_result)) {
		// �D�y���u�{�������y�� �~�u �������u�����r���u��, �����x�t�p�u�} �u�v
		if (SHCreateDirectoryExA(NULL, _result, NULL) != ERROR_SUCCESS) {
			std::cerr << "Unable to create directory." << std::endl;
			return false;
		}
	}
	m_documentPath = std::string(_result);
	return true;
}

// アプリケーション実行
void Application::Execute()
{
	//===================================================================
	// 初期設定(ウィンドウ作成、Direct3D初期化など)
	//===================================================================
	if (APP.Init(1280, 720) == false) {
		return;
	}
	CreateDataPath();
	InitImages();
	InitMenus();
	//===================================================================
	// ゲームループ
	//===================================================================

	// ゲームインスタンス
	SCENE.Init(m_mainMenu);

	// 時間
	DWORD baseTime = timeGetTime();
	int count = 0;

	// ループ
	while (1)
	{

		// 処理開始時間Get
		DWORD st = timeGetTime();

		// ゲーム終了指定があるときはループ終了
		if (m_endFlag)
		{ 
			break;
		}

		//=========================================
		//
		// ウィンドウ関係の処理
		//
		//=========================================

		// ウィンドウのメッセージを処理する
		m_window.ProcessMessage();

		// ウィンドウが破棄されてるならループ終了
		if (m_window.IsCreated() == false)
		{
			break;
		}

		//=========================================
		//
		// オーディオ処理
		//
		//=========================================
		AUDIO.Update();

		//=========================================
		//
		// ゲーム処理
		//
		//=========================================

		// バックバッファクリア
		D3D.GetDevContext()->ClearRenderTargetView(D3D.GetBackBuffer(), Math::Color(0.3f, 0.3f, 0.5f, 1));
		// Zバッファクリア
		D3D.GetDevContext()->ClearDepthStencilView(D3D.GetZBuffer(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

		// ゲーム更新処理
		SCENE.Update();

		// ゲーム描画処理
		SHADER.m_spriteShader.Begin();
		SCENE.Draw2D();
		SHADER.m_spriteShader.End();


		//リリース時はImGuiの部分は通らないようにする
		// ImGui開始
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		// ImGui処理
		SCENE.ImGuiUpdate();
		// GUI描画実行
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		//---------------------


		// BackBuffer -> FrontBuffer
		D3D.GetSwapChain()->Present(0, 0);


		//=========================================
		//
		// フレームレート制御
		//
		//=========================================
		// 処理終了時間Get
		DWORD et = timeGetTime();
		// Fps制御
		DWORD ms = 1000 / m_maxFps;
		if (et - st < ms)
		{
			Sleep(ms - (et - st));	// 速すぎたら待つ
		}

		// FPS計測
		count++;
		if (st - baseTime >= 1000)
		{
			m_fps = (count * 1000) / (st - baseTime);
			baseTime = st;
			count = 0;
		}

	}

	// ゲーム解放
	SCENE.Release();


	//===================================================================
	// アプリケーション解放
	//===================================================================
	Release();
}