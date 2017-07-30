//
//  Weapon.cpp
//  TowerDefence
//
//  Created by Hackintosh on 7/28/17.
//
//

#include "Weapon.h"

Weapon::Weapon(Grid *grid, Point gridPosition)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
    
    this->grid = grid;
    this->initWithFile("tower_weapon.png");
    auto location = grid->getCell(gridPosition)->getCenterLocation() + origin;
    this->setPosition(location);
    this->setContentSize(Size(0.022 * visibleSize.width,
                              0.0765 * visibleSize.height));
    this->gridPosition = gridPosition;
    //this->setGlobalZOrder(25);
    this->angle = 0;
    
    this->base = Sprite::create("tower_base.png");
    this->base->setPosition(this->getPosition());
    this->base->setContentSize(Size(0.0299 * visibleSize.width,
                                    0.0531 * visibleSize.height));
    
    this->activeRadius = grid->getCellSize().width * 3;
    this->time = 0;
    this->delay = 1;
//    CCLOG("Weapon Position x:%f y:%f", this->getPosition().x, this->getPosition().y);
//    CCLOG("Base Position x:%f y:%f", base->getPosition().x, base->getPosition().y);
//    CCLOG("Base Anchor x:%f y:%f", base->getAnchorPoint().x, base->getAnchorPoint().y);
}

Sprite* Weapon::getBase()
{
    return base;
}

void Weapon::rotate()
{
    angle += 1;
    this->setRotation(angle);
}

Point Weapon::findTarget()
{
    Point target = Point(-1,-1);
    
    auto gridSize = grid->getGridSize();
    auto width = int(gridSize.width);
    auto height = int(gridSize.height);
    float minDistance = -1;
    
    for(int i = 0; i < width; i++) {
        for(int j = 0; j < height; j++) {
            auto cell = grid->getCell(Point(i,j));
            if(cell->getTankState()) {
               auto distance = distanceTo(cell->getCenterLocation());
               if(distance <= activeRadius &&
                   (distance < minDistance || minDistance == -1)){
                    minDistance = distance;
                    target = cell->getCenterLocation();
                }
            }
        }
    }
    if(target != Point(-1,-1)) {
        angle = getAngle(target, minDistance);
        this->setRotation(angle);
    }
    
    return target;
}

float Weapon::distanceTo(Point target)
{
    auto selfPosition = this->getPosition() - origin;
    
    return sqrt(pow(target.x - selfPosition.x, 2) +
                pow(target.y - selfPosition.y, 2));
}


void Weapon::update(float delta)
{
    vector<int> deleteBullets;
    
    auto target = findTarget();
    //CCLOG("Weapon row:%f column:%f Target row:%f column:%f",gridPosition.x,gridPosition.y,target.x,target.y);
    if(time != 0) {
        time += delta;
    }
    if(target != Point(-1,-1) && (time > delay || time == 0)) {
        fire(target);
        time = delta;
    }
    else {
        rotate();
    }
    for(int i = 0; i < bullets.size(); i++) {
        if(bullets[i]->isVisible() && bullets[i]->isOnScreen()) {
            bullets[i]->update();
        }
        else {
            deleteBullets.push_back(i);
        }
    }
    
    if(deleteBullets.size() > 0) {
        dropBullets(deleteBullets);
    }
//    if(bullets.size() >= 1) {
//    CCLOG("Weapon x:%f y:%f Bullet 0 x:%f y:%f",this->getPosition().x,this->getPosition().y,
//                                                bullets[0]->getPosition().x, bullets[0]->getPosition().y);
//        CCLOG("Bullet 0 path x:%f y:%f", bullets[0]->getPath().x, bullets[0]->getPath().y);
//    }
}

void Weapon::fire(Point target)
{
    auto path = Vec2();
    auto selfPosition = this->getPosition();
    target = target + origin;
    auto delta_x = target.x - selfPosition.x;
    
    path.x = (target.y - selfPosition.y) / (target.x - selfPosition.x);
    path.y = (target.x * selfPosition.y - selfPosition.x * target.y) / (target.x - selfPosition.x);
    
    auto bullet = new Bullet(selfPosition, path, delta_x);
    bullets.push_back(bullet);
    this->getParent()->addChild(bullet);
}

float Weapon::getAngle(Point target, float distance)
{
    float angle;
    auto selfPosition = this->getPosition();
    target = target + origin;
    auto delta_y = target.y - selfPosition.y;
    float cosine = abs(delta_y) / distance;
    angle = acos(cosine) * 180 / M_PI;
    auto delta_x = target.x - selfPosition.x;
    if(delta_x < 0) {
        angle = -angle;
    }
    if(delta_y < 0) {
        angle = -180 - angle;
    }
    
    return angle;
}

vector<Bullet*> Weapon::getBullets()
{
    return bullets;
}


void Weapon::dropBullets(vector<int> deleteBullets) {
    for(int i = 0; i < deleteBullets.size(); ++i) {
        auto bullet = bullets[deleteBullets[i]];
        bullets.erase(bullets.begin() + deleteBullets[i]);
        bullet->release();
    }
}









































