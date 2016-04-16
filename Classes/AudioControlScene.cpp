/*
ProjectName: 雷霆战机
Author: 马三小伙儿
Blog: http://www.cnblogs.com/msxh/
Github:https://github.com/XINCGer
Date: 2016/04/01
*/

#include "AudioControlScene.h"
#include "ui/CocosGUI.h"
#include "ReadJson.h"
USING_NS_CC;
using namespace ui;
using namespace cocos2d::extension;

bool AudioControl::init(){
	if (!Layer::init()){
		return false;
	}
	Size visibleSize = Director::getInstance()->getVisibleSize();

	/******************背景音乐标签和滑条设计*******************/
	//创建“背景音乐”文本并添加为当前层的子节点
	auto music_text = Text::create(ReadJson::getString("bgMusic"), "Arial", 32);
	music_text->setFontName("微软雅黑");
	music_text->setPosition(Vec2(visibleSize.width*0.25,visibleSize.height*0.7));
	this->addChild(music_text);

	//创建一个滑动条
	auto music_slider = Slider::create();
	//设置滑动条原理
	music_slider->loadBarTexture("sliderTrack.png");
	//设置滑动条的滚轮纹理
	music_slider->loadSlidBallTextures("sliderThumb.png", "sliderThumb.png", "");
	//设置滑动条的进度条纹理
	music_slider->loadProgressBarTexture("sliderProgress.png");
	//获取之前设置的背景音乐音量
	float musicPercent = UserDefault::getInstance()->getFloatForKey("musicPercent");
	//如果是第一次进入设置场景，设置背景音乐滑动条默认值为100
	if (musicPercent == 0.0f){
		musicPercent = 100.0f;
	}
	//设置背景音乐滑动条的初始值
	music_slider->setPercent(musicPercent);
	music_slider->setPosition(Vec2(visibleSize.width*0.6, visibleSize.height*0.7));
	//添加事件监听器，调整背景音乐音量
	music_slider->addEventListener([=](Ref *pSender, Slider::EventType type){
		//当滑动条的百分比发生变化时
		if (type == Slider::EventType::ON_PERCENTAGE_CHANGED){
			//获得滑动条百分比
			int percent = music_slider->getPercent();
			//设置背景音乐值为滑动条百分比/100,因为Slider的百分比是1~100，而MusicVolume的取值是0.0~1.0
			SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(float(percent) / 100);
			//存储设置的背景音乐值
			UserDefault::getInstance()->setFloatForKey("musicPercent", percent);
		}
	});
	this->addChild(music_slider);

	/******************************音效音量标签和滑动条设计*************************/
	//创建“音效音量”文本并添加为当前层的子节点
	auto sound_text = Text::create(ReadJson::getString("effectMusic"), "Arial", 32);
	sound_text->setFontName("微软雅黑");
	sound_text->setPosition(Vec2(visibleSize.width*0.25, visibleSize.height*0.5));
	this->addChild(sound_text);
	//创建一个滑动条
	auto effect_slider = Slider::create();
	//设置滑动条的纹理
	effect_slider->loadBarTexture("sliderTrack.png");
	effect_slider->loadSlidBallTextures("sliderThumb.png", "sliderThumb.png", "");
	effect_slider->loadProgressBarTexture("sliderProgress.png");
	//获取之前设置的音效音量
	float effectPercent = UserDefault::getInstance()->getFloatForKey("effectPercent");
	//如果是第一次进入设置场景，设置音效滑动条默认初识值为100
	if (effectPercent == 0.0f){
		effectPercent = 100.0f;
	}
	//设置音效滑动条的初始值
	effect_slider->setPercent(effectPercent);
	effect_slider->setPosition(Vec2(visibleSize.width*0.6, visibleSize.height*0.5));
	//添加事件监听器，调整音效音量
	effect_slider->addEventListener([=](Ref *pSender, Slider::EventType type){
		//当滑块的百分比发生变化时
		if (type == Slider::EventType::ON_PERCENTAGE_CHANGED){
			//获得滑条百分比
			int percent = effect_slider->getPercent();
			//设置背景音效值
			SimpleAudioEngine::getInstance()->setEffectsVolume(float(percent) / 100);
			//存储设置的背景音乐值
			UserDefault::getInstance()->setFloatForKey("effectPercent", percent);
		}
	});
	this->addChild(effect_slider);

	//创建“返回”按钮，点击时调用returnToMenu函数
	auto return_button = Button::create("b1.png");
	return_button->setPosition(Vec2(visibleSize.width - return_button->getContentSize().width / 2,
		return_button->getContentSize().height / 2));
	return_button->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type){
		if (type == Widget::TouchEventType::ENDED){
			//切换到MenuScene场景
			auto transtion = TransitionSlideInL::create(2.0, GameMenu::createScene());
			Director::getInstance()->replaceScene(transtion);
		}
	});
	this->addChild(return_button);

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

Scene* AudioControl::createScene(){
	// 创建一个场景对象，该对象将会由自动释放池管理内存的释放
	auto scene = Scene::create();
	// 创建层对象，该对象将会由自动释放池管理内存的释放
	auto layer = AudioControl::create();
	// 将GameSet层作为子节点添加到场景
	scene->addChild(layer);
	// 返回场景对象
	return scene;
}