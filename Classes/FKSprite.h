/*
ProjectName: 雷霆战机
Author: 马三小伙儿
Blog: http://www.cnblogs.com/msxh/
Github:https://github.com/XINCGer
Date: 2016/04/01
*/

#ifndef _FKSprite_H_
#define _FKSprite_H_

#include "cocos2d.h"
#include "ui\CocosGUI.h"
USING_NS_CC;
using namespace ui;

//自定义精灵类
class FKSprite :public Sprite{
private:
	//生命值
	int _lifeValue;
	//血条
	LoadingBar * _HP;
	//血条的更新量(每被子弹击中一次减少的血量)
	float _HPInterval;
public:
	//静态的create函数
	static FKSprite *createWithSpriteFrameName(const std::string &filename){
		FKSprite *sprite = new FKSprite();
		if (sprite &&sprite->initWithSpriteFrameName(filename)){
			sprite->autorelease();
			return sprite;
		}
		CC_SAFE_DELETE(sprite);
		return nullptr;
	}
	void setLifeValue(int lifeValue){
		_lifeValue = lifeValue;
	}
	int getLifeValue(){
		return _lifeValue;
	}
	void setHP(LoadingBar *HP){
		_HP = HP;
	}
	LoadingBar *getHP(){
		return _HP;
	}
	void setHPInterval(float HPInterval){
		_HPInterval = HPInterval;
	}
	float getHPInterval(){
		return _HPInterval;
	}
};
#endif