/*
ProjectName: 雷霆战机
Author: 马三小伙儿
Blog: http://www.cnblogs.com/msxh/
Github:https://github.com/XINCGer
Date: 2016/04/01
*/

#ifndef _GameScene_H_
#define _GameScene_H_
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "FKSprite.h"
using namespace cocos2d;
using namespace CocosDenshion;

namespace my{
	class Game : public Layer{
	private:
		//玩家飞机变量
		Sprite * _plane;
		//屏幕的宽度、高度变量
		int _screenWidth, _screenHeight;
		//游戏背景图
		Sprite *_bg1;
		Sprite *_bg2;
		//游戏帧计数器
		int _count;
		//敌机数组
		Vector<FKSprite *> _enemyVector;
		//子弹数组
		Vector<Sprite *> _bulletVector;
		//分数值标签
		Label * _scoreLabel;
		//分数
		int _scoreValue;
		//大BOSS
		FKSprite * _boss =NULL;
		//大BOSS炮弹精灵数组
		Vector <Sprite*> _bossBulletVecor;
		//是否启动大BOSS，大BOSS是否开始移动和大BOSS是否发射子弹
		bool isStart=false, _isMove=false, isShoot=false;

	public:
		static Scene *createScene();
		virtual bool init();
		//当节点所在的场景即将呈现的时候调用
		virtual void onEnter();
		// 获得动画函数
		Animation* getAnimationByName(std::string animName, float delay, int animNum);
		//回调更新函数，该函数每一帧都会被调用
		void update(float delta);
		//背景图片滚动函数
		void updateBackground();
		//更新敌机函数
		void enemyPlaneDive(float delta);
		//敌机离开屏幕删除函数
		void removeEnemy(float delta);
		//玩家飞机不断发射子弹的函数
		void shootBullet(float delta);
		//子弹离开屏幕删除的函数
		void removeBullet(float delta);
		//检测碰撞的函数
		void collisionDetection(float delta);
		//播放爆炸动画
		void playBombAnimate(std::string name, Vec2 position);
		//游戏结束
		void gameOver(std::string message);
		//重新开始
		void restart(int tag);
		//启动大BOSS
		void startBoss();
		//移动大BOSS
		void moveBoss();
		//大BOSS发射炮弹
		void shootBoss(float delta);
		//用来更新大Boss的血条
		void updateHUD(float delta);
		CREATE_FUNC(my::Game);
	};
}

#endif