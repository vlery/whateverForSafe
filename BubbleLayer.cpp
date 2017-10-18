#include "BubbleLayer.h"
#include "Bulk.h"
USING_NS_CC;
bool BubbleLayer::init() {
	if (!Layer::init()) {
		return false;
	}
	this->scheduleUpdate();
	return true;
	
}

void BubbleLayer::update(float delta) {
	if (shootList.size() > 0) {
		auto begin = shootList.begin();
		auto end = shootList.end();
		std::for_each(begin, end,[=](BubbleNode* node) {
			if (node->getState() == BubbleState::SHOOT) {
				
			
					if (node->getPosition().x <= bubbleSize / 2) {
						node->velocity.x *= -1;
					}else if (node->getPosition().x >= width-bubbleSize / 2) {
						node->velocity.x *= -1;
					}
					node->moveForward();
					this->testConnection(node);
			}
		});
		this->cleanShootList();
	}
}
void BubbleLayer::setWidth(float w) {
	this->setContentSize(Size(w, 2));
	this->width = w;
}

void BubbleLayer::initBubbles(int bubbleSize,int layer) {
	this->bubbleSize = bubbleSize;
	hexmap = new HexMap(bubbleSize, width, layer);
	while (layer) {
		moveDown();
		layer--;
	}

}
void BubbleLayer::addBubbles() {


	std::list<BubbleNode*> list = hexmap->getBubblesList();
	std::for_each(list.begin(), list.end(), [=](BubbleNode* node) {
		if (node->getBubble()->getParent() == nullptr) {
			addChild(node->getBubble());
		}
	}
	);


	list = hexmap->getAttachList();
	std::for_each(list.begin(), list.end(), [=](BubbleNode* node) {
		if (node->getBubble()->getParent() == nullptr) {
			addChild(node->getBubble());
		}
	}
	);

	list = hexmap->getTopList();
	std::for_each(list.begin(), list.end(), [=](BubbleNode* node) {
		if (node->getBubble()->getParent()==nullptr) {
			addChild(node->getBubble());
		}
	}
	);
}


void BubbleLayer::addLayer() {
	
	hexmap->generateRow([this]() {
		addBubbles();
		moveDown();
	});
	
}

void BubbleLayer::moveDown() {
	runAction(
		MoveBy::create(ANM_TIME, Vec2(0, -sqrt(3) / 2* bubbleSize))
		);
}

void BubbleLayer::moveSelect(ConnectType type) {
	if (select == nullptr) {
		return;
	}
	BubbleNode* next = select->getNeighbour(type);
	
	if (next != nullptr&&next->isBubble()) {
	

		if (next->getBulk() == select->getBulk()) {
			select->select();
			select = next;
			select->getBubble()->setOpacity(10);
		}
		else {
			
			select->getBulk()->unselect();
			select = next;
			select->getBulk()->select();
			select->getBubble()->setOpacity(10);
		}
	}

}


void BubbleLayer::setOriginSelect() {
	if (select != nullptr) {
		select->getBulk()->unselect();
	}
	select = (*hexmap->getBubblesList().begin());
	//select= (*hexmap->getAttachList().begin());
	select->unselect();
	select->getBulk()->select();
	
}

void BubbleLayer::processShootBubble(BubbleNode* bubble) {
	addChild(bubble->getBubble());
	shootList.push_back(bubble);
}

void BubbleLayer::testConnection(BubbleNode* node) {
	float boundry_L = SQRT_2*bubbleSize;
	float boundry_M= bubbleSize *2/3 ;
	auto list = hexmap->getAttachList();
	auto itr = list.begin();
	while (itr!= list.end()) {
		
		Point attachPos = (*itr)->getPosition();
		Point nodePos = node->getPosition();
		if (abs(attachPos.x - nodePos.x) + abs(attachPos.y - nodePos.y) >boundry_L) {
			++itr;
			continue;
		}
		float distance = attachPos.distance(nodePos);
		Vec2 connectDir = attachPos - nodePos;
		connectDir.normalize();
		Vec2 moveDir = node->velocity;
		moveDir.normalize();
		float angle = connectDir.dot(moveDir);
		if (distance< boundry_M) {
			if (angle > 0.3f) {
				node->nextState();
				attachBubble(node, (*itr));
				CCLOG("AttachWay: small distance");
				break;
			}
		}else {
			 if (angle > 0.88f) {
				 node->nextState();
				 attachBubble(node, (*itr));
				 CCLOG("AttachWay: Large distance");
				 break;
			}
			node->velocity.normalize();
			node->velocity *= SHOOT_SPEED;
		}
		itr++;
	}

}
void BubbleLayer::attachBubble(BubbleNode* node, BubbleNode* attachNode) {
	node->attachTo(attachNode->getPosition());
	attachNode->select();
}

void BubbleLayer::cleanShootList() {

}