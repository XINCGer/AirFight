/*
ProjectName: 雷霆战机
Author: 马三小伙儿
Blog: http://www.cnblogs.com/msxh/
Github:https://github.com/XINCGer
Date: 2016/04/01
*/

//预加载类，完成预加载背景音乐、音效、精灵表单等准备工作

#ifndef _PreLoadScene_H_
#define _PreLoadScene_H_

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "GameScene.h"

USING_NS_CC;
using namespace CocosDenshion;

class PreLoad : public Layer{
public:
	static Scene * createScene();
	virtual bool init();
	//当Node节点进入“舞台”时调用
	virtual void onEnterTransitionDidFinish();
	//加载背景音乐
	void loadMusic(ValueVector musicFiles);
	//加载游戏音效
	void loadEffect(ValueVector effectFiles);
	//加载精灵表单
	void loadSpriteSheets(ValueVector spriteSheets);
	//更新游戏进度条
	void progressUpdate();
	CREATE_FUNC(PreLoad);
protected:
	//用来保存游戏需要加载的资源总数
	int _sourceCount;
	//进度条
	ProgressTimer * _progress;
	//进度条更新的次数
	float _progressInterval;
};

#endif