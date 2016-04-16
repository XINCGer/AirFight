#ifndef _ReadJson_H_
#define _ReadJson_H_

#include "cocos2d.h"
#include "json\reader.h"
#include "json\value.h"
#include "json\writer.h"
USING_NS_CC;
using namespace std;

class ReadJson :public Node{
public:
	static std::string getString(std::string name);
};
#endif