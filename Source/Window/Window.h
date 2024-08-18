/**
* @file Window.h
*/

#ifndef WINDOW_H_INCLUDE
#define WINDOW_H_INCLUDE

#include <Windows.h>
#include <string>
#include <unordered_map>

// ウィンドウ管理
namespace WindowManagement
{
/**
* ウィンドウ管理クラス
*/
class WindowManager
{
public:
	~WindowManager() = default;

	static bool Initialize();
	static void Finalize();
	static WindowManager& Instance();

	HWND& CraeteWindow(const unsigned int width, const unsigned int height, std::string titleName);
	bool ShowWindowByName(std::string windowName);

	WNDCLASSEX& GetWindowClass(std::string windowName);
	HWND& GetWindow(std::string widowName);

private:
	WindowManager() = default;
	WindowManager(const WindowManager&) = delete;
	WindowManager& operator=(const WindowManager&) = delete;

	std::unordered_map<std::string, WNDCLASSEX> windowClasses;
	std::unordered_map<std::string, HWND> hwnds;

};

} // namespace WindowManagement

#endif // WINDOW_H_INCLUDE