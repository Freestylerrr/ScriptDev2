/* Copyright (C) 2006,2007 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* ScriptData
SDName: Boss_Netherspite
SD%Complete: 65%
SDComment: void zone should be immobile (db issue?), red beam has some bugs,
does he do double damage after each banish phase?, netherbreath timer?
SDCategory: Karazhan
EndScriptData */

#include "precompiled.h"
#include "simple_ai.h"
#include "karazhan.h"
#include "GameObject.h"

//spells
#define SPELL_NETHERBURN       30523
//#define SPELL_NETHERBURN_2   30522
#define SPELL_VOID_ZONE		   37063
#define SPELL_NETHERBREATH     38523
#define SPELL_EMPOWERMENT      38549
#define SPELL_NETHER_INFUSION  38688
#define SPELL_NETHERSPITE_ROAR 38684

//beams
#define SPELL_PERSEVERENCE_NS         30466
#define SPELL_PERSEVERENCE_PLR        30421
#define SPELL_SERENITY_NS             30467
#define SPELL_SERENITY_PLR            30422
#define SPELL_DOMINANCE_NS            30468
#define SPELL_DOMINANCE_PLR           30423
#define SPELL_BEAM_DOM				  30402
#define SPELL_BEAM_SER				  30464
#define SPELL_BEAM_PER				  30465
#define BLUE_PORTAL					  30491
#define GREEN_PORTAL				  30490
#define RED_PORTAL					  30487
#define SPELL_EXHAUSTION_DOM		  38639
#define SPELL_EXHAUSTION_SER		  38638
#define SPELL_EXHAUSTION_PER		  38637

//banish
#define SPELL_BANISH_VISUAL         39833
#define SPELL_ROOT					42716

//emotes
#define EMOTE_PHASE_PORTAL          -1532089
#define EMOTE_PHASE_BANISH          -1532090


float BasicCoords[3][3] =
{
	{-11094.493164, -1591.969238, 279.949188},
	{-11195.353516, -1613.237183, 278.237258},
	{-11137.846680, -1685.607422, 278.239258}
};

float RandomedCoords[3][3] = 
{
	{-11094.493164, -1591.969238, 279.949188},
	{-11195.353516, -1613.237183, 278.237258},
	{-11137.846680, -1685.607422, 278.239258}
};

struct MANGOS_DLL_DECL boss_netherspiteAI : public ScriptedAI
{
    boss_netherspiteAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

	ScriptedInstance* m_pInstance;

	Unit* Zonetarget;
    Unit* DOMcandidate;
    Unit* lastDOMcandidate;
	Unit* SERcandidate;
	Unit* lastSERcandidate;
	Unit* PERcandidate;
	Unit* lastPERcandidate;
    Unit* plr;
    Unit* BeamerD;
    Unit* BeamerS;
    Unit* BeamerP;
    Unit* BeamerDhelp;
    Unit* BeamerShelp;
    Unit* BeamerPhelp;
    Unit* ExhaustCandsDOM[10];
    Unit* ExhaustCandsSER[10];
    Unit* ExhaustCandsPER[10];
    Unit* PERMaxHealth[10];
    uint32 Netherburn_timer, Voidzone_timer, Netherbreath_timer, Empowerment_timer, Enrage_timer, PortalPhase_timer, BanishPhase_timer, Beam_initialtimer, Beam_periodictimer, ExhaustCheck_timer, hpbonus;
    bool PortalPhase, BanishPhase, Enraged, existsDOM, existsSER, existsPER, existsMaxH, PERhealthMaxed;
	int bmcolor[2];
    float BossDist, PlrDist, BossAngle, PlrAngle, degrad10;

    void Reset()
    {
		lastDOMcandidate = m_creature;
		lastSERcandidate = m_creature;
		lastPERcandidate = m_creature;
		DOMcandidate = m_creature;
		SERcandidate = m_creature;
		PERcandidate = m_creature;
        Netherburn_timer = 5000;
        Voidzone_timer = 15000;
        Netherbreath_timer = 5000;
        Empowerment_timer = 500;
        Enrage_timer = 540000;
        Enraged = false;
        PortalPhase = true;
        BanishPhase = false;
        PortalPhase_timer = 60000;
        Beam_initialtimer = 10000;
        Beam_periodictimer = 1000;
        ExhaustCheck_timer = 2000;
		hpbonus = 29000;
        degrad10 = 0.3; //0.174;

        for(int i=0;i<10;i++)
		{
            ExhaustCandsDOM[i] = NULL;
            ExhaustCandsSER[i] = NULL;
            ExhaustCandsPER[i] = NULL;
            PERMaxHealth[i] = NULL;
        }

        //DestroyPortals();  // <- for some reason it crashes server, but we must remove portals when he resets...

		if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_NETHERSPITE, NOT_STARTED);

            if (GameObject* pDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(DATA_GO_MASSIVE_DOOR)))
                pDoor->SetGoState(GO_STATE_ACTIVE);
        }
    }

	void RandomizeCoords()
	{
		int RANGE_MAX2 = 2;
		int RANGE_MIN3 = 0;
		int RANGE_MAX1a = 2;
		int RANGE_MIN2a = 1;
		int RANGE_MAX1b = 1;
		int RANGE_MIN2b = 0;
		int color1 = (((double) rand() / (double) RAND_MAX) * RANGE_MAX2 + RANGE_MIN3);
		int color2a = (((double) rand() / (double) RAND_MAX) * RANGE_MAX1a + RANGE_MIN2a);
		int color2b = (((double) rand() / (double) RAND_MAX) * RANGE_MAX1b + RANGE_MIN2b);

		bmcolor[0] = color1;

		if(bmcolor[0] == 0) {
			RandomedCoords[0][0] = BasicCoords[0][0];
			RandomedCoords[0][1] = BasicCoords[0][1];
			RandomedCoords[0][2] = BasicCoords[0][2];
			bmcolor[1] = color2a;

			if(bmcolor[1] == 1) {
				RandomedCoords[1][0] = BasicCoords[1][0];
				RandomedCoords[1][1] = BasicCoords[1][1];
				RandomedCoords[1][2] = BasicCoords[1][2];
				RandomedCoords[2][0] = BasicCoords[2][0];
				RandomedCoords[2][1] = BasicCoords[2][1];
				RandomedCoords[2][2] = BasicCoords[2][2];
			}
			else {
				RandomedCoords[1][0] = BasicCoords[2][0];
				RandomedCoords[1][1] = BasicCoords[2][1];
				RandomedCoords[1][2] = BasicCoords[2][2];
				RandomedCoords[2][0] = BasicCoords[1][0];
				RandomedCoords[2][1] = BasicCoords[1][1];
				RandomedCoords[2][2] = BasicCoords[1][2];
			}
		}

		if(bmcolor[0] == 2)
		{
			RandomedCoords[0][0] = BasicCoords[2][0];
			RandomedCoords[0][1] = BasicCoords[2][1];
			RandomedCoords[0][2] = BasicCoords[2][2];
			bmcolor[1] = color2b;

			if(bmcolor[1] == 0) {
				RandomedCoords[1][0] = BasicCoords[0][0];
				RandomedCoords[1][1] = BasicCoords[0][1];
				RandomedCoords[1][2] = BasicCoords[0][2];
				RandomedCoords[2][0] = BasicCoords[1][0];
				RandomedCoords[2][1] = BasicCoords[1][1];
				RandomedCoords[2][2] = BasicCoords[1][2];
			}
			else {
				RandomedCoords[1][0] = BasicCoords[1][0];
				RandomedCoords[1][1] = BasicCoords[1][1];
				RandomedCoords[1][2] = BasicCoords[1][2];
				RandomedCoords[2][0] = BasicCoords[0][0];
				RandomedCoords[2][1] = BasicCoords[0][1];
				RandomedCoords[2][2] = BasicCoords[0][2];
			}
		}

		if(bmcolor[0] == 1)
		{
			RandomedCoords[0][0] = BasicCoords[1][0];
			RandomedCoords[0][1] = BasicCoords[1][1];
			RandomedCoords[0][2] = BasicCoords[1][2];
			RandomedCoords[1][0] = BasicCoords[2][0];
			RandomedCoords[1][1] = BasicCoords[2][1];
			RandomedCoords[1][2] = BasicCoords[2][2];
			RandomedCoords[2][0] = BasicCoords[0][0];
			RandomedCoords[2][1] = BasicCoords[0][1];
			RandomedCoords[2][2] = BasicCoords[0][2];
		}
	}

	void BlueNetherBeam()
	{
		DOMcandidate = m_creature;
		ThreatList const& m_threatlist = m_creature->getThreatManager().getThreatList();

		for (ThreatList::const_iterator itr = m_threatlist.begin();itr != m_threatlist.end(); ++itr)
		{
			plr = Unit::GetUnit((*m_creature), (*itr)->getUnitGuid());
			BossDist = m_creature->GetDistance(BeamerD);
			BossAngle = m_creature->GetAngle(BeamerD);
			PlrDist = plr->GetDistance(BeamerD);
			PlrAngle = plr->GetAngle(BeamerD);

			if((BossAngle - degrad10) < PlrAngle && PlrAngle < (BossAngle + degrad10))
			{					
				if(PlrDist < BossDist)
				{
					if(!plr->HasAura(SPELL_EXHAUSTION_DOM,0))
					{
						DOMcandidate = plr;
					}
				}
			}
		}

		if(DOMcandidate == m_creature)
		{
			if(DOMcandidate != lastDOMcandidate)
			{
				BeamerD->DealDamage(BeamerDhelp, BeamerDhelp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
				BeamerDhelp = BeamerD->SummonCreature(1557,RandomedCoords[0][0],RandomedCoords[0][1],RandomedCoords[0][2],0,TEMPSUMMON_CORPSE_DESPAWN,0);
				BeamerDhelp->SetUInt32Value(UNIT_FIELD_DISPLAYID, 11686);
				BeamerDhelp->setFaction(m_creature->getFaction());
				BeamerDhelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				BeamerDhelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				BeamerDhelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
				BeamerDhelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
				BeamerD->CastSpell(BeamerD, SPELL_ROOT, true);
				BeamerDhelp->CastSpell(BeamerDhelp, SPELL_ROOT, true);
				lastDOMcandidate = DOMcandidate;
			}
				BeamerD->CastSpell(DOMcandidate,SPELL_DOMINANCE_NS,true);
				BeamerDhelp->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, DOMcandidate->GetGUID());
				BeamerDhelp->SetUInt32Value(UNIT_CHANNEL_SPELL, SPELL_BEAM_DOM);
		}

		else
		{
			if(DOMcandidate != lastDOMcandidate)
			{
				BeamerD->DealDamage(BeamerDhelp, BeamerDhelp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
				BeamerDhelp = BeamerD->SummonCreature(1557,RandomedCoords[0][0],RandomedCoords[0][1],RandomedCoords[0][2],0,TEMPSUMMON_CORPSE_DESPAWN,0);
				BeamerDhelp->SetUInt32Value(UNIT_FIELD_DISPLAYID, 11686);
				BeamerDhelp->setFaction(m_creature->getFaction());
				BeamerDhelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				BeamerDhelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				BeamerDhelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
				BeamerDhelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
				BeamerD->CastSpell(BeamerD, SPELL_ROOT, true);
				BeamerDhelp->CastSpell(BeamerDhelp, SPELL_ROOT, true);
				ExhaustHandler(DOMcandidate,0);
				lastDOMcandidate = DOMcandidate;
			}
				DOMcandidate->CastSpell(DOMcandidate,SPELL_DOMINANCE_PLR,true);
				BeamerDhelp->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, DOMcandidate->GetGUID());
				BeamerDhelp->SetUInt32Value(UNIT_CHANNEL_SPELL, SPELL_BEAM_DOM);
		}
	}

	void GreenNetherBeam()
	{
		SERcandidate = m_creature;
		ThreatList const& m_threatlist = m_creature->getThreatManager().getThreatList();

		for (ThreatList::const_iterator itr = m_threatlist.begin();itr != m_threatlist.end(); ++itr)
		{
			plr = Unit::GetUnit((*m_creature), (*itr)->getUnitGuid());
			BossDist = m_creature->GetDistance(BeamerS);
			BossAngle = m_creature->GetAngle(BeamerS);
			PlrDist = plr->GetDistance(BeamerS);
			PlrAngle = plr->GetAngle(BeamerS);

			if((BossAngle - degrad10) < PlrAngle && PlrAngle < (BossAngle + degrad10))
			{					
				if(PlrDist < BossDist)
				{
					if(!plr->HasAura(SPELL_EXHAUSTION_SER,0))
					{
						SERcandidate = plr;
					}
				}
			}
		}

		if(SERcandidate == m_creature)
		{
			if(SERcandidate != lastSERcandidate)
			{
				BeamerS->DealDamage(BeamerShelp, BeamerShelp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
				BeamerShelp = BeamerS->SummonCreature(1557,RandomedCoords[1][0],RandomedCoords[1][1],RandomedCoords[1][2],0,TEMPSUMMON_CORPSE_DESPAWN,0);
				BeamerShelp->SetUInt32Value(UNIT_FIELD_DISPLAYID, 11686);
				BeamerShelp->setFaction(m_creature->getFaction());
				BeamerShelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				BeamerShelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				BeamerShelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
				BeamerShelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
				BeamerS->CastSpell(BeamerS, SPELL_ROOT, true);
				BeamerShelp->CastSpell(BeamerShelp, SPELL_ROOT, true);
				lastSERcandidate = SERcandidate;
			}
				BeamerS->CastSpell(SERcandidate,SPELL_SERENITY_NS,true);
				BeamerShelp->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, SERcandidate->GetGUID());
				BeamerShelp->SetUInt32Value(UNIT_CHANNEL_SPELL, SPELL_BEAM_SER);
		}
		
		else
		{
			if(SERcandidate != lastSERcandidate)
			{
				BeamerS->DealDamage(BeamerShelp, BeamerShelp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
				BeamerShelp = BeamerS->SummonCreature(1557,RandomedCoords[1][0],RandomedCoords[1][1],RandomedCoords[1][2],0,TEMPSUMMON_CORPSE_DESPAWN,0);
				BeamerShelp->SetUInt32Value(UNIT_FIELD_DISPLAYID, 11686);
				BeamerShelp->setFaction(m_creature->getFaction());
				BeamerShelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				BeamerShelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				BeamerShelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
				BeamerShelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
				BeamerS->CastSpell(BeamerS, SPELL_ROOT, true);
				BeamerShelp->CastSpell(BeamerShelp, SPELL_ROOT, true);
				ExhaustHandler(SERcandidate,1);
				lastSERcandidate = SERcandidate;
			}
				SERcandidate->CastSpell(SERcandidate,SPELL_SERENITY_PLR,true);
				BeamerShelp->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, SERcandidate->GetGUID());
				BeamerShelp->SetUInt32Value(UNIT_CHANNEL_SPELL, SPELL_BEAM_SER);
		}
	}

	void RedNetherBeam()
	{
		PERcandidate = m_creature;
		ThreatList const& m_threatlist = m_creature->getThreatManager().getThreatList();

		for (ThreatList::const_iterator itr = m_threatlist.begin();itr != m_threatlist.end(); ++itr)
		{
			plr = Unit::GetUnit((*m_creature), (*itr)->getUnitGuid());
			BossDist = m_creature->GetDistance(BeamerP);
			BossAngle = m_creature->GetAngle(BeamerP);
			PlrDist = plr->GetDistance(BeamerP);
			PlrAngle = plr->GetAngle(BeamerP);

			if((BossAngle - degrad10) < PlrAngle && PlrAngle < (BossAngle + degrad10))
			{	
				if(PlrDist < BossDist)
				{
					if(!plr->HasAura(SPELL_EXHAUSTION_PER,0))
					{
						PERcandidate = plr;
					}
				}
			}
		}

		if(PERcandidate == m_creature)
		{
			if(PERcandidate != lastPERcandidate)
			{
				BeamerP->DealDamage(BeamerPhelp, BeamerPhelp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
				BeamerPhelp = BeamerP->SummonCreature(1557,RandomedCoords[2][0],RandomedCoords[2][1],RandomedCoords[2][2],0,TEMPSUMMON_CORPSE_DESPAWN,0);
				BeamerPhelp->SetUInt32Value(UNIT_FIELD_DISPLAYID, 11686);
				BeamerPhelp->setFaction(m_creature->getFaction());
				BeamerPhelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				BeamerPhelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				BeamerPhelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
				BeamerPhelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
				BeamerP->CastSpell(BeamerP, SPELL_ROOT, true);
				BeamerPhelp->CastSpell(BeamerPhelp, SPELL_ROOT, true);
				lastPERcandidate = PERcandidate;
			}
				BeamerP->CastSpell(PERcandidate,SPELL_PERSEVERENCE_NS,true);
				BeamerPhelp->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, PERcandidate->GetGUID());
				BeamerPhelp->SetUInt32Value(UNIT_CHANNEL_SPELL, SPELL_BEAM_PER);
		}

		else
		{
			if(PERcandidate != lastPERcandidate)
			{
				BeamerP->DealDamage(BeamerPhelp, BeamerPhelp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
				BeamerPhelp = BeamerP->SummonCreature(1557,RandomedCoords[2][0],RandomedCoords[2][1],RandomedCoords[2][2],0,TEMPSUMMON_CORPSE_DESPAWN,0);
				BeamerPhelp->SetUInt32Value(UNIT_FIELD_DISPLAYID, 11686);
				BeamerPhelp->setFaction(m_creature->getFaction());
				BeamerPhelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				BeamerPhelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				BeamerPhelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
				BeamerPhelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
				BeamerP->CastSpell(BeamerP, SPELL_ROOT, true);
				BeamerPhelp->CastSpell(BeamerPhelp, SPELL_ROOT, true);
				ExhaustHandler(PERcandidate,2);
				PERhealthHandler(PERcandidate);
				lastPERcandidate = PERcandidate;
			}
				PERcandidate->CastSpell(PERcandidate,SPELL_PERSEVERENCE_PLR,true);
				BeamerPhelp->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, PERcandidate->GetGUID());
				BeamerPhelp->SetUInt32Value(UNIT_CHANNEL_SPELL, SPELL_BEAM_PER);
				//m_creature->AddThreat(PERcandidate, 100000.0f);   // we need to have permanent aggro, not tested.
		}
	}

	void ExhaustHandler(Unit *plr, int colour)
	{
		existsDOM = false;
		existsSER = false;
		existsPER = false;

		if(colour == 0)
		{
			for(int i=0;i<10;i++)
			{
				if(ExhaustCandsDOM[i] == plr)
				{
					existsDOM=true;
					break;
				}
			}

			if(!existsDOM)
			{
				for(int i=0;i<10;i++)
				{
					if(ExhaustCandsDOM[i] == NULL)
					{
						ExhaustCandsDOM[i] = plr;
						break;
					}
				}
			}
		}

		if(colour == 1)
		{
			for(int i=0;i<10;i++)
			{
				if(ExhaustCandsSER[i] == plr)
				{
					existsSER=true;
					break;
				}
			}

			if(!existsSER)
			{
				for(int i=0;i<10;i++)
				{
					if(ExhaustCandsSER[i] == NULL)
					{
						ExhaustCandsSER[i] = plr;
						break;
					}
				}
			}
		}

		if(colour == 2)
		{
			for(int i=0;i<10;i++)
			{
				if(ExhaustCandsPER[i] == plr)
				{
					existsPER=true;
					break;
				}
			}

			if(!existsPER)
			{
				for(int i=0;i<10;i++)
				{
					if(ExhaustCandsPER[i] == NULL)
					{
						ExhaustCandsPER[i] = plr;
						break;
					}
				}
			}
		}
	}

	void PERhealthHandler(Unit *plr)
	{
		PERhealthMaxed = false;

		for(int i=0;i<10;i++)
		{
			if(PERMaxHealth[i] == plr)
			{
				PERhealthMaxed = true;
				break;
			}
		}

		if(!PERhealthMaxed)
		{
			for(int i=0;i<10;i++)
			{
				if(PERMaxHealth[i] == NULL)
				{
					PERMaxHealth[i] = plr;
					PERMaxHealth[i]->SetMaxHealth(PERMaxHealth[i]->GetMaxHealth() + hpbonus); // this doesn't work, we need to add 29000 hp to red blockers.
				}
			}
		}
	}

	void DELExhaustCandDOM()
	{
		for(int i=0;i<10;i++)
		{
			if(ExhaustCandsDOM[i] != NULL)
			{
				if(!ExhaustCandsDOM[i]->HasAura(SPELL_DOMINANCE_PLR,0))
				{
					ExhaustCandsDOM[i]->CastSpell(ExhaustCandsDOM[i],SPELL_EXHAUSTION_DOM,true);
					ExhaustCandsDOM[i] = NULL;
				}
			}
		}
	}

	void DELExhaustCandSER()
	{
		for(int i=0;i<10;i++)
		{
			if(ExhaustCandsSER[i] != NULL)
			{
				if(!ExhaustCandsSER[i]->HasAura(SPELL_SERENITY_PLR,0))
				{
					ExhaustCandsSER[i]->CastSpell(ExhaustCandsSER[i],SPELL_EXHAUSTION_SER,true);
					ExhaustCandsSER[i] = NULL;
				}
			}
		}
	}

	void DELExhaustCandPER()
	{
		for(int i=0;i<10;i++)
		{
			if(ExhaustCandsPER[i] != NULL)
			{
				if(!ExhaustCandsPER[i]->HasAura(SPELL_PERSEVERENCE_PLR,0))
				{
					for(int j=0;j<10;j++)
					{
						if(PERMaxHealth[j] == ExhaustCandsPER[i])
						{
							PERMaxHealth[j]->SetMaxHealth(PERMaxHealth[j]->GetMaxHealth());
							PERMaxHealth[j] = NULL;
							break;
						}
					}
					ExhaustCandsPER[i]->CastSpell(ExhaustCandsPER[i],SPELL_EXHAUSTION_PER,true);
					ExhaustCandsPER[i] = NULL;
				}
			}
		}
	}

	void Aggro(Unit *who)
    {
		if (m_pInstance)
            m_pInstance->SetData(TYPE_NETHERSPITE, IN_PROGRESS);

		if (m_pInstance)
        {
             if (GameObject* pDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(DATA_GO_MASSIVE_DOOR)))
                  pDoor->SetGoState(GO_STATE_READY);
        }
		DoScriptText(EMOTE_PHASE_PORTAL,m_creature);
		DoCast(m_creature,SPELL_EMPOWERMENT);
		SpawnPortals();
		DoMeleeAttackIfReady();
    }

	void DestroyPortals()
	{
		if (BeamerD && BeamerD->isAlive())
			BeamerD->DealDamage(BeamerD, BeamerD->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
		if (BeamerS && BeamerS->isAlive())
			BeamerS->DealDamage(BeamerS, BeamerS->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
		if (BeamerP && BeamerP->isAlive())
			BeamerP->DealDamage(BeamerP, BeamerP->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
		if (BeamerDhelp && BeamerDhelp->isAlive())
			BeamerDhelp->DealDamage(BeamerDhelp, BeamerDhelp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
		if (BeamerShelp && BeamerShelp->isAlive())
			BeamerShelp->DealDamage(BeamerShelp, BeamerShelp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
		if (BeamerPhelp && BeamerPhelp->isAlive())
			BeamerPhelp->DealDamage(BeamerPhelp, BeamerPhelp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
	}

	void SwitchPhase()
	{
		if(PortalPhase)  //enter banish phase
		{
			BanishPhase_timer = 30000;
			DoResetThreat();
			PortalPhase = false;
			BanishPhase = true;
			DestroyPortals();
			RandomizeCoords();
			DoCast(m_creature, SPELL_BANISH_VISUAL, true);
            DoCast(m_creature, SPELL_ROOT, true);
			m_creature->RemoveAurasDueToSpell(SPELL_PERSEVERENCE_NS);
			m_creature->RemoveAurasDueToSpell(SPELL_SERENITY_NS);
			m_creature->RemoveAurasDueToSpell(SPELL_DOMINANCE_NS);
			m_creature->RemoveAurasDueToSpell(SPELL_EMPOWERMENT);
			DoScriptText(EMOTE_PHASE_BANISH,m_creature);
			return;
		}

		if(BanishPhase) //enter portal phase
		{
			PortalPhase_timer = 60000;
			m_creature->RemoveAurasDueToSpell(SPELL_ROOT);
			m_creature->RemoveAurasDueToSpell(SPELL_BANISH_VISUAL);
			PortalPhase = true;
			BanishPhase = false;
			SpawnPortals();
			Beam_initialtimer = 10000;
			DoScriptText(EMOTE_PHASE_PORTAL,m_creature);
			DoCast(m_creature,SPELL_EMPOWERMENT);
			return;
		}
	}

	void SpawnPortals()
	{
		BeamerD = m_creature->SummonCreature(1557,RandomedCoords[0][0],RandomedCoords[0][1],RandomedCoords[0][2],0,TEMPSUMMON_CORPSE_DESPAWN,0);
		BeamerS = m_creature->SummonCreature(1557,RandomedCoords[1][0],RandomedCoords[1][1],RandomedCoords[1][2],0,TEMPSUMMON_CORPSE_DESPAWN,0);
		BeamerP = m_creature->SummonCreature(1557,RandomedCoords[2][0],RandomedCoords[2][1],RandomedCoords[2][2],0,TEMPSUMMON_CORPSE_DESPAWN,0);
		BeamerDhelp = BeamerD->SummonCreature(1557,RandomedCoords[0][0],RandomedCoords[0][1],RandomedCoords[0][2],0,TEMPSUMMON_CORPSE_DESPAWN,0);
		BeamerShelp = BeamerS->SummonCreature(1557,RandomedCoords[1][0],RandomedCoords[1][1],RandomedCoords[1][2],0,TEMPSUMMON_CORPSE_DESPAWN,0);
		BeamerPhelp = BeamerP->SummonCreature(1557,RandomedCoords[2][0],RandomedCoords[2][1],RandomedCoords[2][2],0,TEMPSUMMON_CORPSE_DESPAWN,0);

		BeamerD->SetUInt32Value(UNIT_FIELD_DISPLAYID, 11686);
		BeamerD->setFaction(m_creature->getFaction());
		BeamerDhelp->setFaction(m_creature->getFaction());
		BeamerD->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		BeamerD->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		BeamerD->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
		BeamerD->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
		BeamerDhelp->SetUInt32Value(UNIT_FIELD_DISPLAYID, 11686);
		BeamerDhelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		BeamerDhelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		BeamerDhelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
		BeamerDhelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
		BeamerD->CastSpell(BeamerD,BLUE_PORTAL,true);
		BeamerD->CastSpell(BeamerD, SPELL_ROOT, true);
		BeamerDhelp->CastSpell(BeamerDhelp, SPELL_ROOT, true);

		BeamerS->SetUInt32Value(UNIT_FIELD_DISPLAYID, 11686);
		BeamerS->setFaction(m_creature->getFaction());
		BeamerShelp->setFaction(m_creature->getFaction());
		BeamerS->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		BeamerS->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		BeamerS->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
		BeamerS->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
		BeamerShelp->SetUInt32Value(UNIT_FIELD_DISPLAYID, 11686);
		BeamerShelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		BeamerShelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		BeamerShelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
		BeamerShelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
		BeamerS->CastSpell(BeamerS, SPELL_ROOT, true);
		BeamerShelp->CastSpell(BeamerShelp, SPELL_ROOT, true);
		BeamerS->CastSpell(BeamerS,GREEN_PORTAL,true);

		BeamerP->SetUInt32Value(UNIT_FIELD_DISPLAYID, 11686);
		BeamerP->setFaction(m_creature->getFaction());
		BeamerPhelp->setFaction(m_creature->getFaction());
		BeamerP->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		BeamerP->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		BeamerP->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
		BeamerP->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
		BeamerP->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
		BeamerPhelp->SetUInt32Value(UNIT_FIELD_DISPLAYID, 11686);
		BeamerPhelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		BeamerPhelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		BeamerPhelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
		BeamerPhelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
		BeamerPhelp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
		BeamerP->CastSpell(BeamerP, SPELL_ROOT, true);
		BeamerPhelp->CastSpell(BeamerPhelp, SPELL_ROOT, true);
		BeamerP->CastSpell(BeamerP,RED_PORTAL,true);
	}

	void UpdateAI(const uint32 diff)
	{
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if(m_creature->getVictim() && m_creature->isAlive())
        {
			if(ExhaustCheck_timer < diff)
			{
				DELExhaustCandDOM();
				DELExhaustCandSER();
				DELExhaustCandPER();
				ExhaustCheck_timer = 2000;
			}else ExhaustCheck_timer -= diff;

			if(Enrage_timer < diff)
			{
				if(!Enraged)
				{
                    DoCast(m_creature, SPELL_NETHERSPITE_ROAR);
					DoCast(m_creature, SPELL_NETHER_INFUSION);
				Enraged = true;
				}
			}else Enrage_timer -= diff;

			if(PortalPhase)
			{
				if(PortalPhase_timer < diff)
				{
					SwitchPhase();
				}else PortalPhase_timer -= diff;

				if(Beam_initialtimer < diff)
				{
					if(Beam_periodictimer < diff)
					{
						BlueNetherBeam();
						GreenNetherBeam();
						//RedNetherBeam();    // <--uncomment to activate red beam, its still bugged.
						Beam_periodictimer = 1000;
					}else Beam_periodictimer -= diff;
				}else Beam_initialtimer -= diff;

				if(Netherburn_timer < diff)
				{
					DoCast(m_creature,SPELL_NETHERBURN);
					Netherburn_timer = 5000;
				}else Netherburn_timer -= diff;

				if(Voidzone_timer < diff)
				{
					DoCast(SelectUnit(SELECT_TARGET_RANDOM,0),SPELL_VOID_ZONE,true);
					Voidzone_timer = 15000;
				}else Voidzone_timer -= diff;
			}

			if(BanishPhase)
			{
				if(BanishPhase_timer < diff)
				{
					SwitchPhase();
				}else BanishPhase_timer -= diff;

				if(Netherbreath_timer < diff)
				{
					DoCast(SelectUnit(SELECT_TARGET_RANDOM,0),SPELL_NETHERBREATH);
					Netherbreath_timer = 5000;
				}else Netherbreath_timer -= diff;
			}
			
			if(PERcandidate != m_creature)
			{
				AttackStart(PERcandidate);
			}else DoMeleeAttackIfReady();
		}
	}

	void KilledUnit(Unit* Victim)
	{
		for(int i=0;i<10;i++)
		{
			if(Victim == PERMaxHealth[i])
			{
				PERMaxHealth[i]->SetMaxHealth(PERMaxHealth[i]->GetMaxHealth());
				PERMaxHealth[i] = NULL;
				break;
			}
		}
	}

	void JustDied(Unit* Killer)
	{
		for(int i=0;i<10;i++)
		{
			if(PERMaxHealth[i])
				PERMaxHealth[i]->SetMaxHealth(PERMaxHealth[i]->GetMaxHealth());
				PERMaxHealth[i] = NULL;
		}
		m_pInstance->SetData(TYPE_NETHERSPITE, DONE);

		if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_NETHERSPITE, NOT_STARTED);

            if (GameObject* pDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(DATA_GO_MASSIVE_DOOR)))
                pDoor->SetGoState(GO_STATE_ACTIVE);
        }
		DestroyPortals();
	}
};

CreatureAI* GetAI_boss_netherspite(Creature* pCreature)
{
    return new boss_netherspiteAI(pCreature);
}

void AddSC_boss_netherspite()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_netherspite";
    newscript->GetAI = &GetAI_boss_netherspite;
    newscript->RegisterSelf();
}