#ifndef _GameLib_AirHockey_h_
#define _GameLib_AirHockey_h_

#include "Common.h"
#include "World.h"
#include "Object.h"
#include "Contact.h"

namespace GameCtrl {

class Puck : public Circle {
	
public:
	virtual void Paint(WorldDraw& wdraw, Draw& draw);
	
};

class Player : public Circle {
	
protected:
	Pointf prev_target;
	double prev_distance;
	String name;
	Puck* puck;
	int id;
	
	void ResetPreviousMove() {prev_distance = DBL_MAX;}
	void SeekPuck(double speed, double force);
	
public:
	Player(int id);
	
	void Process();
	virtual void Paint(WorldDraw& wdraw, Draw& draw);
	
	void SetName(String s) {name = s;}
	void SetPuck(Puck& puck) {this->puck = &puck;}
	void MoveTo(const Pointf& target, double speed, double force);
	
};

struct Goal : public Polygon {
	int id;
};

struct Map : public Polygon {
	int w, h, gw;
	int last_won, last_score;
	void* table;
	
	Map();
	void PlayerWon(int i);
	void PlayerScore(int i);
	virtual void Paint(WorldDraw& wdraw, Draw& draw);
};

class GameTable : public World, public ContactListener {
	Map area_a;
	Polygon area_b, map_l, map_tl, map_tr, map_r, map_bl, map_br;
	Player pl_a, pl_b;
	Array<Puck> puck;
	Goal goal_a, goal_b;
	int score[2];
	int score_limit;
	bool player_a_starts;
	
public:
	typedef GameTable CLASSNAME;
	GameTable();
	
	void Init();
	void Reset();
	void PlayerScore(int i);
	void ResetPuck();
	void ProcessAI();
	void SetPlayer(Pointf p);
	void SetScoreLimit(int i) {score_limit = i;}
	int GetScoreA() {return score[0];}
	int GetScoreB() {return score[1];}
	int GetScoreLimit() {return score_limit;}
	
	virtual void ContactBegin(Contact contact);
	virtual void ContactEnd(Contact contact);
	
	Callback1<int> WhenScore, WhenFinish;
	
	
	
};

}

#endif
