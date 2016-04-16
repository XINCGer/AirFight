/*
ProjectName: 雷霆战机
Author: 马三小伙儿
Blog: http://www.cnblogs.com/msxh/
Github:https://github.com/XINCGer
Date: 2016/04/01
*/

#include "GameScene.h"
#include "MenuScene.h"
#include "ReadJson.h"
USING_NS_CC;
using namespace ui;
using namespace CocosDenshion;

//不同敌机的tag
static int E0TAG = 10;
static int E1TAG = 11;
static int E2TAG = 12;
static int BOSSTAG = 100;

Scene * my::Game::createScene(){
	// 创建一个场景对象，该对象将会由自动释放池管理内存的释放
	auto scene = Scene::create();
	// 创建层对象，该对象将会由自动释放池管理内存的释放
	auto layer = Game::create();
	// 将Game层作为子节点添加到场景
	scene->addChild(layer);
	// 返回场景对象
	return scene;
}

bool my::Game::init(){
	if (!Layer::init()){
		return false;
	}
	auto visibleSize = Director::getInstance()->getVisibleSize();
	//获取屏幕的宽度与高度
	_screenWidth = visibleSize.width;
	_screenHeight = visibleSize.height;
	//将动画添加到动画帧缓存
	AnimationCache::getInstance()->addAnimation(getAnimationByName("plane", 0.08, 2), "fly");
	AnimationCache::getInstance()->addAnimation(getAnimationByName("blast", 0.09, 4), "blast");
	AnimationCache::getInstance()->addAnimation(getAnimationByName("bomb", 0.1, 4), "bomb");
	return true;
}

void my::Game::onEnter(){
	Layer::onEnter();
	//游戏背景图
	//Sprite *bg1 = Sprite::create("bg1.png");
	//bg1->setAnchorPoint(Vec2(0, 0));
	//this->addChild(bg1, -1);

	//初始化随机种子
	srand(time(NULL));
	//添加连续滚动的背景，代替之前的静态背景
	std::string bgName;
	//自适应屏幕大小
	if (_screenHeight == 960){
		bgName = "bg1.png";
	}
	else{
		bgName = "bg2.png";
	}
	//第一张背景图
	_bg1 = Sprite::create(bgName);
	_bg1->setAnchorPoint(Vec2::ZERO);
	//setAliasTexParameters用于解决拼接的地图在连接滚动时容易形成黑色缝隙的问题
	_bg1->getTexture()->setAliasTexParameters();
	this->addChild(_bg1);

	//第二张背景图
	_bg2 = Sprite::create(bgName);
	_bg2->setAnchorPoint(Vec2::ZERO);
	_bg2->getTexture()->setAliasTexParameters();
	_bg2->setPosition(Vec2(0, _bg1->getContentSize().height));
	this->addChild(_bg2);

	//添加玩家飞机精灵
	_plane = Sprite::createWithSpriteFrameName("plane0.png");
	_plane->setPosition(Vec2(_screenWidth / 2, _plane->getContentSize().height/2+5));
	this->addChild(_plane);

	//玩家飞机动画（尾部喷火）
	auto planeFlyAnimation = AnimationCache::getInstance()->getAnimation("fly");
	//创建动画动作
	auto animate = Animate::create(planeFlyAnimation);
	//重复动作
	auto planeFlyAction = RepeatForever::create(animate);
	//执行动作，实现飞机尾部喷火效果
	_plane->runAction(planeFlyAction);

	//创建事件监听器
	auto planeListener = EventListenerTouchOneByOne::create();
	//响应触摸事件函数
	planeListener->onTouchBegan = [](Touch *touch, Event *event){
		//获得事件对象
		auto target = event->getCurrentTarget();
		//获得触摸的Opengl坐标
		Vec2 location = touch->getLocation();
		//当前相对Event(_plane)的节点坐标
		Vec2 locationInNode = target->convertToNodeSpace(location);
		Size s = target->getContentSize();
		//获取该对象的矩形区域
		Rect rect = Rect(0, 0, s.width, s.height);
		//如果矩形区域包含触摸点
		if (rect.containsPoint(locationInNode)){
			return true;
		}
		return false;
	};

	//添加背景音乐
	SimpleAudioEngine::getInstance()->playBackgroundMusic("s3.wav", true);

	//初始化分数标签
	_scoreLabel = Label::createWithSystemFont("00", "Arial", 32);
	_scoreLabel->setPosition(50, _screenHeight*0.98);
	this->addChild(_scoreLabel);
	//初始化分数值
	_scoreValue = 0;

	planeListener->onTouchMoved = [](Touch *touch, Event *event){
		//获得事件对象
		auto target = event->getCurrentTarget();
		//移动触摸的精灵位置
		target->setPosition(target->getPosition() + touch->getDelta());
	};
	//添加场景优先事件监听器
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(planeListener, _plane);
	//游戏主循环，每帧都调用的函数
	this->scheduleUpdate();
	//指定每帧执行自定义的函数
	this->schedule(schedule_selector(my::Game::shootBullet)); //发射子弹
	this->schedule(schedule_selector(my::Game::collisionDetection)); //检测碰撞
	this->schedule(schedule_selector(my::Game::shootBoss)); //大Boss发射子弹
}

Animation* my::Game::getAnimationByName(std::string animName, float delay, int animNum){
	/*
	获得动画函数
	animName :动画帧的名字
	delay :动画帧和帧之间的间隔时间
	animNum :动画帧的数量
	*/
	auto animation = Animation::create();
	//循环从精灵帧缓存中获取与图片名称相对应的精灵帧组成动画
	for (unsigned int i = 0; i < animNum; i++){
		//获取动画图片名称，例如plane0.png
		std::string frameName = animName;
		frameName.append(StringUtils::format("%d", i)).append(".png");
		//将单张图片添加为精灵帧（即动画帧）
		animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName.c_str()));
	}
	//设置动画播放的属性
	animation->setDelayPerUnit(delay);
	//让精灵对象在动画执行完后恢复到最初状态
	animation->setRestoreOriginalFrame(true);
	//返回动画对象
	return animation;
}

//背景图片滚动函数
void my::Game::updateBackground(){
	//获取背景1图片的位置
	Vec2 p1 = _bg1->getPosition();
	//背景1图片的y轴每帧自减5
	p1.y = p1.y - 5;
	//如果背景1图片的y轴小于等于负背景1图片自身的高度（即背景1图片移除屏幕），则背景1图片的y轴
	//重新等于背景1图片的自身的高度
	if (p1.y <= -_bg1->getContentSize().height){
		p1.y = _bg1->getContentSize().height;
	}
	//重新设置背景1图片的位置
	_bg1->setPosition(p1);
	//背景2图片的操作与背景1图片的操作一致
	Vec2 p2 = _bg2->getPosition();
	p2.y = p2.y - 5;
	if (p2.y <= -_bg2->getContentSize().height){
		p2.y = _bg2->getContentSize().height;
	}
	_bg2->setPosition(p2);
}

void my::Game::update(float delta){
	_count++;
	updateBackground(); //背景图片滚动
	enemyPlaneDive(delta); //敌机俯冲
	removeEnemy(delta); //删除离开屏幕的敌机
	//shootBullet(delta); //发射子弹
	removeBullet(delta); //删除离开屏幕的子弹
	//collisionDetection(delta); //检测碰撞
	updateHUD(delta);  //更新HUD
}

//enemyPlaneDive函数用于控制敌机的创建、俯冲
void my::Game::enemyPlaneDive(float delta){
	bool flag = false;
	int lifeValue;
	//自定义精灵类FKSprite
	FKSprite *enemyPlane;
	//使用随机数来设置敌机的x坐标
	int randX = cocos2d::random()%(_screenWidth - 40) + 20;
	//获取随机时间（敌机掉落的时间）
	float durationTime = cocos2d::random() % 2 + 2;
	//count为20的倍数时添加一架敌机
	if (_count % 30 == 0 && !isStart){
		//根据cocos2d::random随机数田间不同的敌机
		int rand = cocos2d::random() % 2;
		switch (rand)
		{
		case 0:
			enemyPlane = FKSprite::createWithSpriteFrameName("e0.png");
			enemyPlane->setTag(E0TAG);
			break;
		case 1:
			enemyPlane = FKSprite::createWithSpriteFrameName("e2.png");
			enemyPlane->setTag(E2TAG);
			break;
		}
		//设置敌机生命值、敌机图片比例
		lifeValue = 1;
		enemyPlane->setLifeValue(lifeValue);
		enemyPlane->setScale(0.6);
		flag = true;
	}
	else{
		if (_count % 200 == 0 && !isStart){
			enemyPlane = FKSprite::createWithSpriteFrameName("e1.png");
			enemyPlane->setTag(E1TAG);
			enemyPlane->setScale(0.8);
			lifeValue = 10;
			enemyPlane->setLifeValue(lifeValue);
			flag = true;
		}
	}
	if (flag){
		//设置敌机的位置
		enemyPlane->setPosition(randX, _screenHeight + enemyPlane->getContentSize().height);
		//MoveBy 动作,高度为屏幕高度加上飞机自身的高度
		auto moveBy = MoveBy::create(durationTime, Vec2(0, -enemyPlane->getPosition().y - enemyPlane->getContentSize().height));
		//敌机俯冲动作
		enemyPlane->runAction(moveBy);
		//将敌机精灵添加到敌机数组
		_enemyVector.pushBack(enemyPlane);
		//将敌机添加为当前层的子节点
		this->addChild(enemyPlane);
		//如果敌机是一个飞碟添加一个血条
		if (enemyPlane->getTag() == E1TAG){
			//创建一个LoadingBar
			auto loadingBar = LoadingBar::create("planeHP.png");
			loadingBar->setScale(0.15);
			//设置进度条的方向
			loadingBar->setDirection(LoadingBar::Direction::LEFT);
			//设置进度条的初始值为100，满血
			loadingBar->setPercent(100);
			Point pos = enemyPlane->getPosition();
			//设置坐标位置
			loadingBar->setPosition(Vec2(pos.x, pos.y + 48));
			//将LoadingBar添加为当前层的子节点
			this->addChild(loadingBar, 1);
			//进度条执行moveby动作
			auto moveBy2 = MoveBy::create(durationTime, Vec2(0, -pos.y - enemyPlane->getContentSize().height));
			loadingBar->runAction(moveBy2);
			//将该进度条设置为敌机的血条
			enemyPlane->setHP(loadingBar);
			//设置血条更新量
			enemyPlane->setHPInterval(100.0f / enemyPlane->getLifeValue());
		}
	}
}

//当敌机已经移出屏幕外时删除敌机精灵
void my::Game::removeEnemy(float delta){
	//遍历所有的敌机精灵
	for (unsigned int i = 0; i < _enemyVector.size(); i++){
		auto enemy = _enemyVector.at(i);
		//如果敌机已经移除屏幕外，删除敌机精灵
		if (enemy->getPosition().y <= -enemy->getContentSize().height){
			//从当前层删除该敌机精灵
			this->removeChild(enemy, true);
			//从_enemyVector集合中删除敌机精灵
			_enemyVector.eraseObject(enemy);
		}
	}
}

//shootBullet函数用于不断发射子弹
void my::Game::shootBullet(float delta){
	//获得飞机的坐标
	Vec2 pos = _plane->getPosition();
	//控制_count为8的倍数时发射一颗子弹
	if (_count % 8 == 0){
		//创建代表子弹的精灵
		auto bullet = Sprite::createWithSpriteFrameName("bullet.png");
		//设置子弹的坐标
		bullet->setPosition(pos.x, pos.y + _plane->getContentSize().height / 2 + bullet->getContentSize().height);
		//子弹移动时间为0.4秒，移动距离为屏幕高度-子弹的y轴
		auto moveBy = MoveBy::create(0.4f, Vec2(0, _screenHeight - bullet->getPosition().y));
		bullet->runAction(moveBy);
		//将子弹精灵添加到当前层
		this->addChild(bullet, 4);
		//将子弹精灵添加到子弹精灵数组
		_bulletVector.pushBack(bullet);
	}
}

void my::Game::removeBullet(float delta){
	//遍历子弹数组
	for (unsigned int i = 0; i < _bulletVector.size(); i++){
		auto bullet = _bulletVector.at(i);
		//如果子弹已经移除屏幕外，则删除子弹
		if (bullet->getPosition().y >= _screenHeight){
			//从当前层中删除该子弹精灵
			this->removeChild(bullet, true);
			//从子弹精灵数组中删除子弹精灵
			_bulletVector.eraseObject(bullet);
		}
	}
}

void my::Game::collisionDetection(float delta){
	//遍历敌机数组
	for (unsigned int i = 0; i < _enemyVector.size(); i++){
		FKSprite *enemy = _enemyVector.at(i);
		//判断玩家飞机和敌机发生碰撞
		if (_plane->getBoundingBox().intersectsRect(enemy->getBoundingBox())){
			//播放爆炸动画
			playBombAnimate("blast", enemy->getPosition());
			playBombAnimate("blast", _plane->getPosition());
			//如果不是大boss,删除敌机精灵
			if (enemy->getTag() != BOSSTAG){
				_enemyVector.eraseObject(enemy);
				this->removeChild(enemy, true);
			}
			//玩家飞机停止所有动作
			_plane->stopAllActions();
			//玩家精灵不可见
			_plane->setVisible(false);
			gameOver(ReadJson::getString("restart"));
		}
		else{
			//遍历子弹数组
			for (unsigned int i = 0; i < _bulletVector.size(); i++){
				auto bullet = _bulletVector.at(i);
				//如果敌机与子弹发生碰撞
				if (enemy->getBoundingBox().intersectsRect(bullet->getBoundingBox())){
					//播放子弹音效
					SimpleAudioEngine::getInstance()->playEffect("bullet.wav");
					//删除子弹精灵
					_bulletVector.eraseObject(bullet);
					this->removeChild(bullet, true);
					//敌机生命值减1
					enemy->setLifeValue(enemy->getLifeValue() - 1);
					//飞碟和大boss血量减少
					if (enemy->getTag() == E1TAG || enemy->getTag() ==BOSSTAG){
						if (enemy->getHP() != nullptr){
							enemy->getHP()->setPercent(enemy->getHPInterval()*enemy->getLifeValue());
						}
					}
					//判断敌机生命值
					if (enemy->getLifeValue() <= 0){
						//如果是大boss，停止大boss的所有动作
						if (enemy->getTag() == BOSSTAG){
							_boss->stopAllActions();
							//播放boss爆炸动画
							playBombAnimate("bomb", enemy->getPosition());
							SimpleAudioEngine::getInstance()->playEffect("b1.wav");
							//更新分数
							_scoreLabel->setString(StringUtils::format("%d", _scoreValue + 10000));
							gameOver(ReadJson::getString("win"));
						}
						else{
							//播放爆炸动画
							playBombAnimate("blast", enemy->getPosition());
							SimpleAudioEngine::getInstance()->playEffect("b0.wav");
							//判断飞机得分
							if (enemy->getTag() == E1TAG){
								_scoreLabel->setString(StringUtils::format("%d", _scoreValue += 500));
							}
							else{
								_scoreLabel->setString(StringUtils::format("%d", _scoreValue += 100));
							}
						}

						//删除敌机精灵
						_enemyVector.eraseObject(enemy);
						this->removeChild(enemy, true);
						//10000分时出动大Boss
						if (_scoreValue >= 10000 && ! isStart){
							startBoss();
							//大Boss启动，可以通过设置该变量停止飞机俯冲
							isStart = true;
							break;
						}
					}
					break;
				}
			}
			//判断大Boss的子弹是否和玩家飞机碰撞
			if (isShoot){
				for (unsigned int i = 0; i < _bossBulletVecor.size(); i++){
					auto bossBullet = _bossBulletVecor.at(i);
					if (_plane->getBoundingBox().intersectsCircle(Vec2(bossBullet->getPositionX(), bossBullet->getPositionY()),bossBullet->getContentSize().height/2)){
						_plane->stopAllActions();
						_plane->setVisible(false);
						playBombAnimate("blast", _plane->getPosition());
						SimpleAudioEngine::getInstance()->playEffect("b0.wav");
						gameOver(ReadJson::getString("restart"));
					}
				}
			}
		}
	}
}
/* 播放爆炸动画
name: 爆炸动画的名称
position：爆炸动画播放的位置 */
void my::Game::playBombAnimate(std::string name, Vec2 position){
	//获得爆炸动画
	Animation *blastAnimation;
	if (name.compare("bomb") == 0){
		blastAnimation = AnimationCache::getInstance()->getAnimation("bomb");
	}
	else{
		blastAnimation = AnimationCache::getInstance()->getAnimation("blast");
	}
	std::string bombName = name + "0.png";
	//创建一个精灵
	auto blast = Sprite::createWithSpriteFrameName(bombName);
	blast->setPosition(position);
	//组合动作：1.播放动画2.播放完爆炸动画之后删除动画
	auto animate = Animate::create(blastAnimation);
	auto callFunc = CallFunc::create([=]{
		this->removeChild(blast, true);
	});
	auto action = Sequence::create(animate, callFunc, NULL);
	//执行爆炸动画
	blast->runAction(action);
	//将精灵添加到精灵表单
	this->addChild(blast);

}

//gameOver函数用于游戏结束时清除精灵对象并提示游戏信息

void my::Game::gameOver(std::string message){
	//停止回调更新函数
	//this->unscheduleUpdate();
	//停止指定每帧执行自定义的函数，即不再发射子弹，检测碰撞和发射大Boss的炮弹
	this->unschedule(schedule_selector(my::Game::shootBullet));
	this->unschedule(schedule_selector(my::Game::collisionDetection));
	this->unschedule(schedule_selector(my::Game::shootBoss));
	//重新开始按钮
	auto restart_button = Button::create("button.png");
	restart_button->setScale(2);
	restart_button->setTitleText(message);
	restart_button->setTitleFontName("微软雅黑");
	restart_button->setTitleFontSize(16);
	restart_button->setPosition(Vec2(_screenWidth / 2, _screenHeight*0.6));
	restart_button->addTouchEventListener([=](Ref *pSender, Widget::TouchEventType type){
		if (type == Widget::TouchEventType::ENDED){
			this->restart(1);
		}
	});
	this->addChild(restart_button, 1);
	//返回主菜单按钮
	auto back_button = Button::create("button.png");
	back_button->setScale(2);
	back_button->setTitleText(ReadJson::getString("returnMenu"));
	back_button->setTitleFontName("微软雅黑");
	back_button->setTitleFontSize(16);
	back_button->setPosition(Vec2(_screenWidth / 2, _screenHeight*0.4));
	back_button->addTouchEventListener([=](Ref *pSender, Widget::TouchEventType type){
		if (type == Widget::TouchEventType::ENDED){
			this->restart(2);
		}
	});
	this->addChild(back_button, 1);
}

//restart函数用于切换场景
void my::Game::restart(int tag){
	SimpleAudioEngine::getInstance()->stopBackgroundMusic();
	if (tag == 1){
		Director::getInstance()->replaceScene(my::Game::createScene());
	}
	else{
		Director::getInstance()->replaceScene(GameMenu::createScene());
	}
}


//startBoss函数用于启动大BOSS
void my::Game::startBoss(){
	//删除屏幕所有敌机
	for (unsigned int i = 0; i < _enemyVector.size(); i++){
		auto enemy = _enemyVector.at(i);
		if (enemy->getTag() == E1TAG){
			//设置和敌机关联的进度条为0
			enemy->getHP()->setPercent(0);
		}
		this->removeChild(enemy, true);
	}
	//清除敌机数组
	_enemyVector.clear();
	//创建大BOSS，并设置位置、名称和生命值
	_boss = FKSprite::createWithSpriteFrameName("e-10.png");
	_boss->setPosition(_screenWidth / 2, _screenHeight + _boss->getContentSize().height);
	_boss->setTag(BOSSTAG);
	_boss->setLifeValue(100);
	//创建大Boss血条
	auto loadingBar = LoadingBar::create("planeHP.png");
	loadingBar->setScale(0.4);
	//设置进度条方向
	loadingBar->setDirection(LoadingBar::Direction::LEFT);
	//设置进度条初始值为100，即满血
	loadingBar->setPercent(100);
	//设置为大Boss的血条
	_boss->setHP(loadingBar);
	this->addChild(_boss->getHP(), 1);
	_boss->setHPInterval(100 / _boss->getLifeValue());
	//将Boss精灵添加到敌机数组和精灵表单
	_enemyVector.pushBack(_boss);
	this->addChild(_boss, 4);
	//大Boss移动到屏幕上方，移动完成后调用moveBoss函数
	auto moveTo = MoveTo::create(2, Vec2(_screenWidth / 2, _screenHeight - _boss->getContentSize().height));
	auto action = Sequence::create(moveTo, CallFunc::create([=]{
		_isMove = true;
		moveBoss();
	}), NULL);
	//大BOSS执行动作
	_boss->runAction(action);

}

//moveBoss函数用于大Boss启动后在屏幕上方做不规则运动
void my::Game::moveBoss(){
	if (_isMove){
		//_isShoot = true表示大Boss可以开始发射炮弹
		isShoot = true;
		//大Boss不停地在屏幕上方做不规则的运动
		Size bossContentSize = _boss->getContentSize();
		//左边移动动作
		auto moveLeft = MoveTo::create(2.5, Vec2(_screenWidth - bossContentSize.width / 2, _screenHeight - bossContentSize.height));
		//右边移动动作
		auto moveRight = MoveTo::create(2.5, Vec2(bossContentSize.width / 2, _screenHeight - bossContentSize.height));
		//延迟动作
		auto delay = DelayTime::create(1);
		//组合动作
		auto sequence = Sequence::create(delay, moveLeft, moveRight, NULL);
		//重复动作
		auto action = RepeatForever::create(sequence);
		//大BOSS执行动作
		_boss->runAction(action);
	}
}

//shootBoss函数用于大BOSS开始发射炮弹
void my::Game::shootBoss(float delta){
	if (isShoot){
		if (_count % 80 == 0){
			for (int i = 6; i <= 180; i += 18){
				//获得大BOSS位置x和y的坐标
				float startX = _boss->getPositionX();
				float startY = _boss->getPositionY();
				//创建大BOSS的炮弹
				Sprite * bullet = Sprite::create("enemy_bullet1.png");
				//设置炮弹坐标
				bullet->setPosition(startX, startY);
				//将炮弹精灵添加到当前层
				this->addChild(bullet);
				/* 以r2为半径画圆，求出对角的x和y，即子弹发射的位置
				数学中的三角函数，cosf(余弦函数)，sinf(正弦函数)
				*/
				float r2 = _screenHeight + bullet->getContentSize().height / 2;
				float endX = startX - r2*cosf(i*M_PI / 180);
				float endY = startY - r2*sinf(i*M_PI / 180);
				//炮弹移动动作
				MoveTo *moveTo = MoveTo::create(3.0f, Vec2(endX, endY));
				//添加到大BOSS炮弹数组
				_bossBulletVecor.pushBack(bullet);
				//炮弹执行动作
				bullet->runAction(moveTo);
			}
		}
	}
}

//用来更新大Boss的血条
void my::Game::updateHUD(float delta){
	//设置血条的位置，血条跟随boss移动
	if (_boss != NULL && _boss->getLifeValue() > 0){
		//boss的精灵坐标
		Vec2 pos = _boss->getPosition();
		//Boss血条显示坐标位置
		_boss->getHP()->setPosition(Vec2(pos.x, pos.y+100));
	}
}