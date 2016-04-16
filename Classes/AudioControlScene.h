/*
ProjectName: 雷霆战机
Author: 马三小伙儿
Blog: http://www.cnblogs.com/msxh/
Github:https://github.com/XINCGer
Date: 2016/04/01
*/

#ifndef _AudioControlScene_H_
#define _AudioControlScene_H_

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "GUI\CCControlExtension\CCControlSlider.h"
#include "cocos-ext.h"
#include "MenuScene.h"

using namespace cocos2d;
using namespace CocosDenshion;
USING_NS_CC;

//AudioControl类用来设置背景音乐和声音特效
class AudioControl : public Layer{
public:
	static Scene *createScene();
	virtual bool init();
	CREATE_FUNC(AudioControl);
};

#endif