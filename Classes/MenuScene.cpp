/*
ProjectName: 雷霆战机
Author: 马三小伙儿
Blog: http://www.cnblogs.com/msxh/
Github:https://github.com/XINCGer
Date: 2016/04/01
*/
#include "MenuScene.h"
#include "ui\CocosGUI.h"
#include "PreLoadScene.h"
#include "ReadJson.h"

using namespace cocos2d;
using namespace ui;

bool GameMenu::init(){
	//调用父类的init方法
	if (!Layer::init()){
		return false;
	}
	//获得设备可见视图大小
	Size visibleSize = Director::getInstance()->getVisibleSize();

	//开始游戏按钮
	auto start_button = Button::create("button.png");
	start_button->setScale(2);
	start_button->setTitleText(ReadJson::getString("start"));
	start_button->setTitleFontName("微软雅黑");
	start_button->setTitleFontSize(16);
	start_button->setPosition(Vec2(visibleSize.width / 2, visibleSize.height*0.7));
	start_button->addTouchEventListener([](Ref *pSender, Widget::TouchEventType type){
		if (type == Widget::TouchEventType::ENDED){
			//切换到PreLoad场景
			auto transition = TransitionSlideInL::create(2.0, PreLoad::createScene());
			Director::getInstance()->replaceScene(transition);
		}
	});

	this->addChild(start_button);

	//游戏设置按钮
	auto set_button = Button::create("button.png");
	set_button->setScale(2);
	set_button->setTitleText(ReadJson::getString("setting"));
	set_button->setTitleFontName("微软雅黑");
	set_button->setTitleFontSize(16);
	set_button->setPosition(Vec2(visibleSize.width / 2, visibleSize.height*0.55));
	set_button->addTouchEventListener([](Ref *pSender, Widget::TouchEventType type){
		if (type == Widget::TouchEventType::ENDED){
			//切换到AudioControl场景
			auto transtion = TransitionSlideInL::create(2.0, AudioControl::createScene());
			Director::getInstance()->replaceScene(transtion);
		}
	});
	this->addChild(set_button);

	//退出游戏按钮
	auto close_button = Button::create("button.png");
	close_button->setScale(2);
	close_button->setTitleText(ReadJson::getString("exitGame"));
	close_button->setTitleFontName("微软雅黑");
	close_button->setTitleFontSize(16);
	close_button->setPosition(Vec2(visibleSize.width / 2, visibleSize.height*0.4));
	close_button->addTouchEventListener([](Ref *pSender, Widget::TouchEventType type){
		if (type == Widget::TouchEventType::ENDED){
			Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
			MessageBox("您将退出游戏！", "提示信息");
			exit(0);
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
			exit(0);
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
			exit(0);
#endif
		}
	});
	this->addChild(close_button);
	//开发者
	auto banner_text = Text::create(ReadJson::getString("info"), "微软雅黑", 32);
	banner_text->setPosition(Vec2(visibleSize.width / 2, visibleSize.height*0.15));
	this->addChild(banner_text);
	//网址
	auto blog_text = Text::create("www.cnblogs.com/msxh", "微软雅黑", 32);
	blog_text->setPosition(Vec2(visibleSize.width / 2, visibleSize.height*0.1));
	this->addChild(blog_text);
	return true;
}

Scene* GameMenu::createScene()
{
	// 创建一个场景对象，该对象将会由自动释放池管理内存的释放
	auto scene = Scene::create();
	// 创建层对象，该对象将会由自动释放池管理内存的释放
	auto layer = GameMenu::create();
	// 将GameMenu层作为子节点添加到场景
	scene->addChild(layer);
	// 返回场景对象
	return scene;
}