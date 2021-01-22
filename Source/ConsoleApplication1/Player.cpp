#include "Player.hpp"

Player::Player()
{
	this->_Health = 0;
	this->_maxHealth = 0;
	this->_Xpos = 0;
	this->_Ypos = 0;
	this->_Zpos = 0;
}

void Player::setPosition(float x, float y, float z)
{
	this->_Xpos = x;
	this->_Ypos = y;
	this->_Zpos = z;
}

float Player::getXPosition(void)
{
	return this->_Xpos;
}

float Player::getYPosition(void)
{
	return this->_Ypos;
}

float Player::getZPosition(void)
{
	return this->_Zpos;
}

