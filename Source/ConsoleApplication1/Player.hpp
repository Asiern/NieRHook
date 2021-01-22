#pragma once
class Player;
class Player
{
private:
	int _Health;
	int _maxHealth;
	float _Xpos;
	float _Ypos;
	float _Zpos;
public:
	Player();
	void setPosition(float x, float y, float z);
	float getXPosition(void);
	float getYPosition(void);
	float getZPosition(void);
};

