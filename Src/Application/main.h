#pragma once

//============================================================
// アプリケーションクラス
//	APP.～ でどこからでもアクセス可能
//============================================================
class Menu;
class Application
{
// メンバ
public:

	// アプリケーション実行
	void Execute();

	// アプリケーション終了
	void End() { m_endFlag = true; }


	//=====================================================
	// グローバルデータ
	//=====================================================

	// ゲームウィンドウクラス
	KdGameWindow		m_window;

	// FPS制御
	int				m_fps = 0;					// 現在のFPS値
	int				m_maxFps = 60;				// 最大FPS


private:

	// アプリケーション初期化
	bool Init(int w, int h);

	bool InitMenus();
	bool InitImages();
	bool InitBackTex();
	// アプリケーション解放
	void Release();

	// ゲーム終了フラグ trueで終了する
	bool		m_endFlag = false;

	std::string m_programmPath;
	std::string m_documentPath;

	Menu* m_mainMenu;

	KdTexture m_mainMenuBackTex;

//=====================================================
// シングルトンパターン
//=====================================================
private:
	// 
	Application() {}
	
	bool CreateDataPath();
public:
	static Application &GetInstance(){
		static Application Instance;
		return Instance;
	}
};

// 簡単にアクセスできるようにするためのマクロ
#define APP Application::GetInstance()
