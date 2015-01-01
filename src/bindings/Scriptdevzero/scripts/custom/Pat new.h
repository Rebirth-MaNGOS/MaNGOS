#include "precompiled.h"

const double PI = 3.14159265;

struct PatAdd
{
	uint64 guid;
	double abstand;
	double winkel;
	double speed;
	
	explicit PatAdd(uint64 n, double a, double b, double c) : guid(n), abstand(a), winkel(b), speed(c) {} 
};

struct Pat : public ScriptedAI
{

	Pat(Creature* creature) : ScriptedAI(creature) { 
		Adds.clear();
		Reset(); 
	}

	void NewAdd(uint64 guid, double abstand, double winkel, double speed) 
	{ 
		PatAdd pNewAdd(guid,abstand,winkel,speed);
		Adds.push_back(pNewAdd); 
	}

	std::vector<PatAdd> Adds
	float tmpx, tmpy;
	uint32 updatetimer;
	
	void updateguard(PatAdd P)
	{
		Creature* pCreature = m_creature->GetMap()->GetCreature(P.guid);
		if (!pCreature || !pCreature->isAlive())
			return;
		float x, y, z;
		m_creature->GetPosition(x, y, z);
		patrole(P.abstand, x, y, m_creature->GetOrientation(), P.winkel);
		pCreature->MonsterMove(tmpx, tmpy, m_creature->GetPositionZ(), P.speed);
	}

	void Reset()
	{
		updatetimer=500;
	}

	void patrole(double a, float x, float y, float o, double A)
	{
		double b;
		double winkel;
		double winkelbogen;

		b = o * 180 / PI;
		winkel = A + b;
		winkelbogen = winkel*PI/180;

		tmpx = x + (a * cos(winkelbogen));
		tmpy = y + (a * sin(winkelbogen));
	}

	void UpdateAI(const uint32 diff)
	{
		if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
		{
		    if(updatetimer<diff)
		    {
				if (!Adds.empty())
				{
					for(int i = 0; i < Adds.size(); ++i)
					{
						updateguard(Adds[i].pCreature, Adds[i].guid, Adds[i].abstand, Adds[i].winkel, Adds[i].speed);
					}
				}
				updatetimer = 500;
		    }
			else updatetimer-=diff;
		}
	}
};


/*struct testpat : public Pat
{
	testpat(Creature* creature) : Pat(creature)
	{
		PatAdd add(15000);
		add.speed = 1200;
		add.abstand = 5;
		add.winkel = 90;
		NewAdd(add);

		add.winkel = 270;
		NewAdd(add);
	}

	void Update(const uint32 time)
	{
		//Nicht UpdateAI benutzen, sonst funktioniert es nicht mehr!
	}

	void Reset() 
	{
	}


	static CreatureAI* GetAI(Creature* creature)
	{
		return new testpat(creature);
	}
};

void AddTestPat()
{
	Script* pScript = new Script;
	pScript->GetAI = &testpat::GetAI;
	pScript->Name = "testpat";
	pScript->RegisterSelf(false);
}*