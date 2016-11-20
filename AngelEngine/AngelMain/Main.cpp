#include"../AngelSource/AngelCore/AngelWindow/Public/AngelWindowBase.h"
#include<memory>


class SimpleScene : public AngelCore::AngelWindow::AngelWindowBase
{
protected:

	virtual bool Initialize() override;

};
bool SimpleScene::Initialize()
{
	return true;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	std::shared_ptr<SimpleScene> m(new SimpleScene());
#ifdef _DEBUG
	m->SetHeight(768);
	m->SetWidth(1366);
	m->SetWindowed(true);
#endif
	m->InitializeWindow(hInstance, nShowCmd);
}