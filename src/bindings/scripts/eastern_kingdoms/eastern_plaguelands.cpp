/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Eastern_Plaguelands
SD%Complete: 100
SDComment: Quest support: 5211, 5742. Special vendor Augustus the Touched
SDCategory: Eastern Plaguelands
EndScriptData */

/* ContentData
mobs_ghoul_flayer
npc_augustus_the_touched
npc_darrowshire_spirit
npc_tirion_fordring
npc_eris_havenfire
EndContentData */

#include "precompiled.h"

//id8530 - cannibal ghoul
//id8531 - gibbering ghoul
//id8532 - diseased flayer

struct MANGOS_DLL_DECL mobs_ghoul_flayerAI : public ScriptedAI
{
    mobs_ghoul_flayerAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset() { }

    void JustDied(Unit* Killer)
    {
        if (Killer->GetTypeId() == TYPEID_PLAYER)
            m_creature->SummonCreature(11064, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 60000);
    }

};

CreatureAI* GetAI_mobs_ghoul_flayer(Creature* pCreature)
{
    return new mobs_ghoul_flayerAI(pCreature);
}

/*######
## npc_augustus_the_touched
######*/

bool GossipHello_npc_augustus_the_touched(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pCreature->isVendor() && pPlayer->GetQuestRewardStatus(6164))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_augustus_the_touched(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_TRADE)
        pPlayer->SEND_VENDORLIST(pCreature->GetGUID());
    return true;
}

/*######
## npc_darrowshire_spirit
######*/

#define SPELL_SPIRIT_SPAWNIN    17321

struct MANGOS_DLL_DECL npc_darrowshire_spiritAI : public ScriptedAI
{
    npc_darrowshire_spiritAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset()
    {
        DoCast(m_creature,SPELL_SPIRIT_SPAWNIN);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }
};

CreatureAI* GetAI_npc_darrowshire_spirit(Creature* pCreature)
{
    return new npc_darrowshire_spiritAI(pCreature);
}

bool GossipHello_npc_darrowshire_spirit(Player* pPlayer, Creature* pCreature)
{
    pPlayer->SEND_GOSSIP_MENU(3873, pCreature->GetGUID());
    pPlayer->TalkedToCreature(pCreature->GetEntry(), pCreature->GetGUID());
    pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    return true;
}

/*######
## npc_tirion_fordring
######*/

bool GossipHello_npc_tirion_fordring(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestStatus(5742) == QUEST_STATUS_INCOMPLETE && pPlayer->getStandState() == UNIT_STAND_STATE_SIT)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I am ready to hear your tale, Tirion.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_tirion_fordring(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Thank you, Tirion.  What of your identity?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(4493, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "That is terrible.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            pPlayer->SEND_GOSSIP_MENU(4494, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I will, Tirion.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            pPlayer->SEND_GOSSIP_MENU(4495, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->AreaExploredOrEventHappens(5742);
            break;
    }
    return true;
}
/*######
## npc_eris_havenfire
######*/

enum
{
    SPELL_DEATH_DOOR            =   23127,
    SPELL_SHOOT                 =   6660,

    NPC_SCOURGE_FOOTSOLDIER     =   14486,
    NPC_SCOURGE_ARCHER          =   14489,
    NPC_INJURED_PEASANT         =   14484,
    NPC_PLAGUED_PEASANT         =   14485,
    NPC_SPOTLIGHT               =   15631,
    NPC_ERIS_HAVENFIRE          =   14494,
    NPC_THE_CLEANER             =   14503,
    SPELL_EYE_OF_DIVINITY       =   23101,
    SPELL_NORDRASSIL_BLESSING   =   23108,

    SAY_BE_HEALED               =   -1000606,
    SAY_ALONE                   =   -1000607,
    SAY_WAVE1                   =   -1000601,
    SAY_WAVE2                   =   -1000600,
    SAY_WAVE3                   =   -1000604,
    SAY_DEATH                   =   -1000603,
    SAY_PLAYER                  =   -1000602,
    SAY_FAILED                  =   -1000608,
    SAY_COMPLETE                =   -1000605,
    SAY_COMPLETE2               =   -1000609,
    SAY_COMPLETE3               =   -1000610,
    SAY_COMPLETE4               =   -1000611,

    QUEST_BALANCE_LIGHT_SHADOW  =   7622
};

struct SpawnLocations
{    
    float x,y,z;
};

static SpawnLocations NPCs[]=
{
    {3362.941895,-3053.342041,165.264999},          //peasants+skeletons
    {3309.247803,-2954.100342,156.220901},          //destionation

    //cave archers (2):
    {3371.690918,-3069.937500,175.218475},
    {3359.887695,-3075.041260,174.661163},
    //mound archers:
    {3334.310303,-3052.889404,174.161087},
    {3363.796143,-3008.461426,185.866699},
    {3327.488770,-3017.561523,171.630508},
    {3367.691650,-3023.956543,171.292053},
    {3348.612549,-2989.941406,172.868393},
    {3380.985840,-3059.786377,182.815903},
    {3362.204102,-3005.437012,184.587387}
};

struct MANGOS_DLL_DECL npc_eris_havenfireAI : public ScriptedAI
{
    npc_eris_havenfireAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint64 m_uiPlayerGUID;

    uint32 m_uiPlague_Timer;
    uint32 m_uiPeasantWave_Timer;
    uint32 m_uiScourgeWave_Timer;
    uint8 m_uiWaveCount;

    bool m_uiEventStarted;
    bool InitiallySummoned;
    bool CleanerSpawned;

    void Reset()
    {
        m_uiEventStarted = false;
        m_uiPlayerGUID = NULL;
        m_uiScourgeWave_Timer = 30000;
        CleanerSpawned = false;
    }

    void StartEvent()
    {
        m_uiWaveCount = 0;
        m_uiEventStarted = true;
    }

    void JustSummoned(Creature *pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_SCOURGE_FOOTSOLDIER)
            if (Creature *pTarget = GetClosestCreatureWithEntry(pSummoned,NPC_INJURED_PEASANT,50.f))
            {
                pSummoned->AI()->AttackStart(pTarget);
                pTarget->SetInCombatState(false);
            }

        if (pSummoned->GetEntry() == NPC_INJURED_PEASANT || pSummoned->GetEntry() == NPC_PLAGUED_PEASANT)
            pSummoned->GetMotionMaster()->MovePoint(1,NPCs[1].x,NPCs[1].y,NPCs[1].z);
    }

    void MoveInLineOfSight(Unit *pUnit)
    {
        if (pUnit->GetTypeId() == TYPEID_PLAYER)
        {
            if (!CleanerSpawned)
                if (m_uiEventStarted)
                    if (m_uiPlayerGUID)
                        if (pUnit != Unit::GetUnit(*m_creature,m_uiPlayerGUID))
                            if (Unit *pCleaner = m_creature->SummonCreature(NPC_THE_CLEANER,pUnit->GetPositionX(),pUnit->GetPositionY(),pUnit->GetPositionZ(),0,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,60000))
                            {
                                DoScriptText(SAY_ALONE,pCleaner);
                                CleanerSpawned = true;
                            }

            if (((Player*)pUnit)->GetQuestStatus(QUEST_BALANCE_LIGHT_SHADOW) == QUEST_STATUS_FAILED && m_uiEventStarted)
            {
                m_uiEventStarted = false;
                InitiallySummoned = false;
                std::list<Creature*> m_lArcher;
                    GetCreatureListWithEntryInGrid(m_lArcher,m_creature,NPC_SCOURGE_ARCHER,100.0f);
                    if (!m_lArcher.empty())
                        for(std::list<Creature*>::iterator iter = m_lArcher.begin(); iter != m_lArcher.end(); ++iter)
                            (*iter)->ForcedDespawn();
            }

            if (m_uiWaveCount >= 4)
            {
                if (((Player*)pUnit)->GetQuestStatus(QUEST_BALANCE_LIGHT_SHADOW) == QUEST_STATUS_INCOMPLETE)
                {
                    ((Player*)pUnit)->CompleteQuest(QUEST_BALANCE_LIGHT_SHADOW);
                    DoScriptText(SAY_COMPLETE,m_creature);
                    m_uiEventStarted = false;
                    if (Creature *pPeasant = GetClosestCreatureWithEntry(m_creature,NPC_INJURED_PEASANT,50.0f))
                    {
                        DoScriptText(SAY_COMPLETE2,pPeasant);
                        DoScriptText(SAY_COMPLETE3,pPeasant);
                        DoScriptText(SAY_COMPLETE4,pPeasant);
                    }
                    return;
                }
            }
            m_uiPlayerGUID = pUnit->GetGUID();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_uiEventStarted)
            return;

        if (m_uiPlayerGUID)
        {
            if (!InitiallySummoned)
            {
                for (uint8 i = 4; i < 11; ++i)
                    m_creature->SummonCreature(NPC_SCOURGE_ARCHER,NPCs[i].x,NPCs[i].y,NPCs[i].z,3.0f,TEMPSUMMON_TIMED_DESPAWN,300000);
                m_creature->SummonCreature(NPC_SPOTLIGHT,NPCs[1].x,NPCs[1].y,NPCs[1].z,0,TEMPSUMMON_TIMED_DESPAWN,300000);
                InitiallySummoned = true;
            }

            if (m_uiPeasantWave_Timer < uiDiff)
            {
                ++m_uiWaveCount;
                if (m_uiWaveCount >= 4)
                    return;

                for (uint8 i = 0; i < 12; ++i)
                    m_creature->SummonCreature(NPC_INJURED_PEASANT,NPCs[0].x + rand()%5,NPCs[0].y + rand()%5,NPCs[0].z,3.0f,TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN,60000);
                if (Creature *pPlagued = m_creature->SummonCreature(NPC_PLAGUED_PEASANT,NPCs[0].x + rand()%2,NPCs[0].y + rand()%2,NPCs[0].z,3.0f,TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN,60000))
                {
                    switch (m_uiWaveCount)
                    {
                    case 1:DoScriptText(SAY_WAVE1,pPlagued);break;
                    case 2:DoScriptText(SAY_WAVE2,pPlagued);break;
                    case 3:DoScriptText(SAY_WAVE3,pPlagued);break;
                    }
                }
                m_uiPeasantWave_Timer = 60000;
                
            }
            else
                m_uiPeasantWave_Timer -= uiDiff;

            if (m_uiScourgeWave_Timer < uiDiff)
            {
                for (uint8 i = 0; i < rand()%5 + 3; ++i)
                    m_creature->SummonCreature(NPC_SCOURGE_FOOTSOLDIER,NPCs[0].x + rand()%2,NPCs[0].y + rand()%2,NPCs[0].z,3.0f,TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN,60000);
                m_uiScourgeWave_Timer = 30000;
            }
            else
                m_uiScourgeWave_Timer -= uiDiff;

            if (m_uiPlague_Timer < uiDiff)
            {
                if (rand()%5<1)
                {
                    std::list<Creature*> m_lArcher;
                    GetCreatureListWithEntryInGrid(m_lArcher,m_creature,NPC_SCOURGE_ARCHER,100.0f);
                    if (!m_lArcher.empty())
                        for(std::list<Creature*>::iterator iter = m_lArcher.begin(); iter != m_lArcher.end(); ++iter)
                            if (Creature* pPeasant = GetClosestCreatureWithEntry((*iter),NPC_INJURED_PEASANT,100.0f))
                            {
                                ((*iter))->CastSpell(pPeasant,SPELL_SHOOT,false);
                                pPeasant->SetInCombatState(false);
                            }
                }
                if (rand()%30 <1)
                    if (Unit *pPlayer = Unit::GetUnit(*m_creature,m_uiPlayerGUID))
                    {                        
                        pPlayer->CastSpell(pPlayer,SPELL_NORDRASSIL_BLESSING,false);
                        DoScriptText(SAY_BE_HEALED,m_creature);
                    }
                m_uiPlague_Timer = 1000;
            }
            else
                m_uiPlague_Timer -= uiDiff;
        }
    }
};

bool QuestAccept_npc_eris_havenfire(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_BALANCE_LIGHT_SHADOW)
        ((npc_eris_havenfireAI*)pCreature->AI())->StartEvent();
    return true;
}

CreatureAI* GetAI_npc_eris_havenfire(Creature* pCreature)
{
    return new npc_eris_havenfireAI(pCreature);
}

struct MANGOS_DLL_DECL npc_injured_peasantAI : public ScriptedAI
{
    npc_injured_peasantAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiDeath_Door_Timer;
    uint8 m_uiFailed_Count;

    void Reset()
    {
        m_uiDeath_Door_Timer = 5000;
        SetCombatMovement(false);
    }

    void SpellHit(Unit *pCaster, const SpellEntry *pSpell)
    {
        if (pCaster->GetTypeId() == TYPEID_PLAYER)
            if (rand()%15 < 1)
                DoScriptText(SAY_PLAYER,pCaster);
    }

    void JustDied(Unit *pKiller)
    {
        if (rand()%10 < 1)
            DoScriptText(SAY_DEATH,m_creature);

        ++m_uiFailed_Count;
        std::list<Creature*> m_lFoot;
        GetCreatureListWithEntryInGrid(m_lFoot,m_creature,NPC_SCOURGE_FOOTSOLDIER,100.0f);
        if (!m_lFoot.empty())
            for(std::list<Creature*>::iterator iter = m_lFoot.begin(); iter != m_lFoot.end(); ++iter)
                if (Creature* pPeasant = GetClosestCreatureWithEntry((*iter),NPC_INJURED_PEASANT,100.0f))
                    (*iter)->AI()->AttackStart(pPeasant);
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (m_uiFailed_Count >= 15)
            if (pWho->GetTypeId() == TYPEID_PLAYER)
                if (((Player*)pWho)->GetQuestStatus(QUEST_BALANCE_LIGHT_SHADOW) == QUEST_STATUS_INCOMPLETE)
                {
                    ((Player*)pWho)->FailQuest(QUEST_BALANCE_LIGHT_SHADOW);
                    if (Creature *pEris = GetClosestCreatureWithEntry(m_creature,NPC_ERIS_HAVENFIRE,50.0f))
                        DoScriptText(SAY_FAILED,pEris);
                }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiDeath_Door_Timer < uiDiff)
        {
            if (rand()%10 < 1)
            {
                m_creature->SetInCombatState(false);
                DoCast(m_creature,SPELL_DEATH_DOOR);
                m_creature->GetMotionMaster()->MovePoint(1,NPCs[1].x,NPCs[1].y,NPCs[1].z);
            }

            m_uiDeath_Door_Timer = 5000;
        }
        else
            m_uiDeath_Door_Timer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_injured_peasant(Creature* pCreature)
{
    return new npc_injured_peasantAI(pCreature);
}


void AddSC_eastern_plaguelands()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "mobs_ghoul_flayer";
    newscript->GetAI = &GetAI_mobs_ghoul_flayer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_augustus_the_touched";
    newscript->pGossipHello = &GossipHello_npc_augustus_the_touched;
    newscript->pGossipSelect = &GossipSelect_npc_augustus_the_touched;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_darrowshire_spirit";
    newscript->GetAI = &GetAI_npc_darrowshire_spirit;
    newscript->pGossipHello = &GossipHello_npc_darrowshire_spirit;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_tirion_fordring";
    newscript->pGossipHello =  &GossipHello_npc_tirion_fordring;
    newscript->pGossipSelect = &GossipSelect_npc_tirion_fordring;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_eris_havenfire";
    newscript->GetAI = &GetAI_npc_eris_havenfire;
    newscript->pQuestAccept = &QuestAccept_npc_eris_havenfire;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_injured_peasant";
    newscript->GetAI = &GetAI_npc_injured_peasant;
    newscript->RegisterSelf();
}
