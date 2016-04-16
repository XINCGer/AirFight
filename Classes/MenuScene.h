/*
ProjectName: 雷霆战机
Author: 马三小伙儿
Blog: http://www.cnblogs.com/msxh/
Github:https://github.com/XINCGer
Date: 2016/04/01
*/

//GameMenu类的作用是显示一个菜单场景，让用户选择“开始游戏”或者“游戏设置”
#ifndef _GameMenu_H_
#define _GameMenu_H_

#include "cocos2d.h"
#include "AudioControlScene.h"
using namespace cocos2d;
USING_NS_CC;

class GameMenu : public Layer{
public:
	static cocos2d::Scene *createScene();
	virtual bool init();
	CREATE_FUNC(GameMenu);
};

#endif