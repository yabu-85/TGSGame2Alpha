#pragma once

class Screen;
class UIBase;

namespace ScreenManager
{
	//Screen�ǉ�
	void AddScreen(Screen* screen);
	
	//Screen�폜
	void DeleteScreen(Screen* screen);
	
	//Screen������
	void AllDeleteScreen();

	//Screen���X�g��UIList�Ɍ���������UI�폜
	void DeleteUI(UIBase* ui);

	//Screen���X�g�̍X�V
	void Update();

	//Screen���X�g�̕`��
	void Draw();

};