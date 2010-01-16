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
SDName: Boss_Malygos
SD%Complete: 0%
SDComment: 
SDAuthor: Tassadar
SDCategory: Nexus, Eye of Eternity
EndScriptData */

#include "precompiled.h"

enum
{
    //First, say what it should do:
    //////////////// PHASE 0 ////////////////
    // Malygost cast SPELL_PORTAL_BEAM on some sort of portal until get aggroed
    // by Focusing iris
    //
    //////////////// PHASE 1 ////////////////
    // Arcane Breath - normal spell
    //
    // Vortex        - ouuu this will be tuff :/ there are spawned some triggers,
    //                 maybe pull between them?
    //
    // Power Spark   - from wowhead: "Spawned throughout the fight and slowly
    //                 shift towards Malygos. Once they reach him, they buff him
    //                 with Power Spark, increasing the damage output by 50% for
    //                   10 seconds, stacking multiplicatively. If killed, they
    //                   instead grant players in proximity the same buff,
    //                   Power Spark, which especially is a great buff for melee
    //                   players close to Malygos."
    //
    //////////////// PHASE 2 //////////////// - 50% of health
    // On beggining of this phase, malygos will take off slowly and let
    // players 10s to DPS him, then he will fly around(?) and cast his deep breath
    // NPCs on flying discs will be spawned, they will fly around and do damage to raid,
    // when is NPC killed, disc does NOT disappear - its vehicle, players use it to!
    // Players also have to cover in protective bubbles(which are spawned continuosly),
    // they shrink over time so players have to run to another one.
    //
    // Arcane Pulse   - Deep Breath....
    // Arcane Storm   - normal spell...
    //
    //////////////// PHASE 3 //////////////// - when all NPCs from previos phase are dead
    // Malygos will destroy platfrom, players are sent to dragon vehicles.
    //
    // Static Field   - trigger with aura
    // Surge of Power - triggers for that are here
    //
    //////////////// PHASE 4 //////////////// - When malygos reach 0%
    // Outro, Alexstrasza will arive
    

    // ******************************** SPELLS ******************************** //
    SPELL_BERSERK                  = 64238, // Maybe wrong but enought :)
    //////////////// PHASE 0 ////////////////
    SPELL_PORTAL_BEAM              = 56046,

    //////////////// PHASE 1 ////////////////
    SPELL_ARCANE_BREATH            = 56272,
    SPELL_ARCANE_BREATH_H          = 60072,
    SPELL_VORTEX_DUMMY             = 56105, // This is for addons, actualy does nothing
    SPELL_VORTEX                   = 56266, // Cast on trigger in middle of the room, this interrupt their cast and deal dmg
    SPELL_VORTEX_AOE_VISUAL        = 55873,    // visual effect around platform
    SPELL_POWER_SPARK              = 56152, // If spark reach malygos then buff him, if killed then to players
    SPELL_POWER_SPARK_PLAYERS      = 55849, // This one is propably for players

    //////////////// PHASE 2 ////////////////
    SPELL_ARCANE_PULSE             = 57432, // Malygos takes a deep breath...
    SPELL_ARCANE_STORM             = 61693, // AOE
    SPELL_ARCANE_STORM_H           = 61694,
    SPELL_ARCANE_OVERLOAD          = 56432, // Cast this on arcane overload NPCs 
    SPELL_DESTROY_PLATFORM_PRE     = 58842, // lights all over the platform
    SPELL_DESTROY_PLATFROM_BOOM    = 59084, // Big Blue boom
    //NPCs spells
    SPELL_ARCANE_SHOCK             = 57058,
    SPELL_ARCANE_SHOCK_H           = 60073,
    SPELL_ARCANE_BARRAGE           = 63934,


    //////////////// PHASE 3 ////////////////
    SPELL_STATIC_FIELD             = 57428, // Summon trigger and cast this on them should be enought
    SPELL_SURGE_OF_POWER           = 56548, // Summon triggers and cast to random targets
    SPELL_SURGE_OF_POWER_H         = 57407,
    //Dragons spells
    SPELL_FLAME_SPIKE              = 56091,
    SPELL_ENGULF_IN_FLAMES         = 56092,
    SPELL_REVIVIFY                 = 57090,
    SPELL_LIFE_BURST               = 57143,
    SPELL_FLAME_SHIELD             = 57108,
    SPELL_BLAZING_SPEED            = 57092,

    // ******************************** NPCs & GObjects ******************************** //
    NPC_MALYGOS                    = 28859,
    ITEM_KEY_TO_FOCUSING_IRIS      = 44582,
    ITEM_KEY_TO_FOCUSING_IRIS_H    = 44581,
    //////////////// PHASE 1 ////////////////
    NPC_AOE_TRIGGER                = 22517,
    NPC_VORTEX                     = 30090,
    NPC_POWER_SPARK                = 30084,
    NPC_SPARK_PORTAL               = 30118, // For power sparks

    //////////////// PHASE 2 ////////////////
    NPC_HOVER_DISC                 = 30248, // Maybe wrong, two following NPC flying on them (vehicle)
    NPC_NEXUS_LORD                 = 30245, // two (?) of them are spawned on beginning of phase 2
    NPC_SCION_OF_ETERNITY          = 30249, // same, but unknow count
    GO_PLATFORM                    = 193070,

    //////////////// PHASE 3 ////////////////
    NPC_WYRMREST_SKYTALON          = 30161, // Dragon Vehicle in Third Phase
    NPC_SURGE_OF_POWER             = 30334, // What?
    NPC_STATIC_FIELD               = 30592, // Trigger for that spell. Hope its fly

    //////////////// PHASE 4 ////////////////
    NPC_ALEXSTRASZA                = 32295, // The Life-Binder
    GO_ALEXSTRASZAS_GIFT           = 193905, //Loot chest
    GO_EXIT_PORTAL                 = 193908,

    SAY_INTRO1                     = -1616000,
    SAY_INTRO2                     = -1616001,
    SAY_INTRO3                     = -1616002,
    SAY_INTRO4                     = -1616003,
    SAY_INTRO5                     = -1616004,
    SAY_AGGRO1                     = -1616005,

    PHASE_NOSTART                  = 0,
        SUBPHASE_FLY_UP            = 01,
        SUBPHASE_UP                = 03,
        SUBPHASE_FLY_DOWN1         = 04,
        SUBPHASE_FLY_DOWN2         = 05,
    PHASE_FLOOR                    = 1,
        SUBPHASE_VORTEX            = 11,
    PHASE_ADDS                     = 2,
    PHASE_DRAGONS                  = 3,
    PHASE_OUTRO                    = 4,

};
struct Locations
{
    float x, y, z;
    uint32 id;
};
static Locations MalygosLoc[]=
{
    {754.346, 1300.87, 297.249},  //Center of platform

};
static Locations VortexLoc[]=
{
    {754, 1311, 330},
    {701, 1288, 330},
    {747, 1249, 325},
    {795, 1291, 320},
    {781, 1321, 315},
    {752, 1329, 310},
    {719, 1325, 305},
    {720, 1297, 300},
    {743, 1273, 295},
    {764, 1290, 290},
    {754, 1311, 287},
};
#define MAX_VORTEX              13

#define FLOOR_Z                 266.17
#define AIR_Z                   297.24   
/*######
## boss_malygos
######*/

struct MANGOS_DLL_DECL boss_malygosAI : public ScriptedAI
{
    boss_malygosAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    
    uint8 m_uiPhase; //Fight Phase
    uint8 m_uiSubPhase; //Subphase if needed
    uint8 m_uiSpeechCount;
    uint8 m_uiVortexPhase;

    uint32 m_uiEnrageTimer;
    uint32 m_uiSpeechTimer[5];
    uint32 m_uiTimer;
    uint32 m_uiVortexTimer;
    
    void Reset()
    {
        m_uiPhase = PHASE_NOSTART;
        m_uiSubPhase = 0;
        m_uiSpeechCount = 0;
        m_uiVortexPhase = 0;

        m_uiEnrageTimer = 600000;
        m_uiSpeechTimer[0] = 2000;
        m_uiSpeechTimer[1] = 10000;
        m_uiSpeechTimer[2] = 11000;
        m_uiSpeechTimer[3] = 13000;
        m_uiSpeechTimer[4] = 10000;
        m_uiSpeechTimer[5] = 7000;
        m_uiTimer = 7000;
        m_uiVortexTimer = 10000;
        
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void JustReachedHome()
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void AttackStart(Unit* pWho)
    {
        if(m_uiPhase == PHASE_NOSTART)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);

            m_creature->GetMotionMaster()->MoveChase(pWho);
        }
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO1, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        //DoScriptText(SAY_DEATH, m_creature);

    }

    void KilledUnit(Unit* pVictim)
    {
     //   if (urand(0, 1))
       //     DoScriptText(SAY_KILL, m_creature);
    }
    void DoMovement(float x, float y, float z, uint32 time, bool tofly = false)
    {        
        if(tofly){
            m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 50331648);
            m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
        }else{
            m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 0);
            m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
        }
        m_creature->GetMap()->CreatureRelocation(m_creature, x, y, z, m_creature->GetOrientation());
        m_creature->SendMonsterMove(x, y, z, 0, m_creature->GetMonsterMoveFlags(), time);
    }
    void DoVortex(uint8 phase)
    {
        if(phase == 0)
        {
            m_creature->GetMotionMaster()->Clear(false);
            SetCombatMovement(false);
            float x, y, z;
            m_creature->GetPosition(x, y, z);
            z = z + 40;
            DoMovement(x, y, z, 3000, true);
            if(Creature *pTrigger = GetClosestCreatureWithEntry(m_creature, NPC_AOE_TRIGGER, 60.0f))
                pTrigger->CastSpell(pTrigger, SPELL_VORTEX_AOE_VISUAL, false);
            
        }
        else if(phase == 1)
        {
            if(Creature *pTrigger = GetClosestCreatureWithEntry(m_creature, NPC_AOE_TRIGGER, 60.0f))
                pTrigger->CastSpell(pTrigger, SPELL_VORTEX, false);    

            Map* pMap = m_creature->GetMap();
            if(!pMap)
                return;

            Map::PlayerList const &lPlayers = pMap->GetPlayers();
            for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
            {
                if(!itr->getSource()->isAlive())
                    continue;
                itr->getSource()->NearTeleportTo(VortexLoc[0].x, VortexLoc[0].y, VortexLoc[0].z, 0); 
                itr->getSource()->CastSpell(itr->getSource(), SPELL_VORTEX, true);    
            }        
        }
        else if(phase > 1 && phase < 20){
            Map* pMap = m_creature->GetMap();
            if(!pMap)
                return;
            
            

            if(Creature *pVortex = m_creature->SummonCreature(NPC_VORTEX, VortexLoc[phase-1].x, VortexLoc[phase-1].y, VortexLoc[phase-1].z, 0, TEMPSUMMON_TIMED_DESPAWN, 10000))
            {
                pVortex->SetVisibility(VISIBILITY_OFF);
                Map::PlayerList const &lPlayers = pMap->GetPlayers();
                for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                {
                    if(!itr->getSource()->isAlive())
                        continue;

                    
                    float z = itr->getSource()->GetPositionZ() - VortexLoc[phase-1].z;
                    itr->getSource()->KnockBackFrom(pVortex,-float(pVortex->GetDistance2d(itr->getSource())),-z);
                    if(VortexLoc[phase-1].z == 287)
                        itr->getSource()->NearTeleportTo(VortexLoc[10].x, VortexLoc[10].y, VortexLoc[10].z, 0);
                }
            }
        }else if (phase == 20)
        {    
            m_creature->GetMotionMaster()->Clear(false);
            SetCombatMovement(true);
            float x, y, z;
            m_creature->GetPosition(x, y, z);
            z = z - 40;
            DoMovement(x, y, z, 2000);

            if(m_creature->getVictim())
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
        }
        
    }
    void UpdateAI(const uint32 uiDiff)
    {
        if(m_uiPhase == PHASE_NOSTART)
        {
            if(m_uiSubPhase == SUBPHASE_FLY_UP){
                float x, y, z;
                m_creature->GetPosition(x, y, z);
                z = z + 40;
                DoMovement(x, y, z, 0, true);
                m_uiSubPhase = SUBPHASE_UP;
            }
            else if(m_uiSubPhase == SUBPHASE_UP){
                if(m_uiSpeechTimer[m_uiSpeechCount] <= uiDiff)
                {
                    DoScriptText(SAY_INTRO1-m_uiSpeechCount, m_creature);
                    m_uiSpeechCount++;
                    if(m_uiSpeechCount == 5){
                        m_uiSubPhase = SUBPHASE_FLY_DOWN1;
                    }
                }else m_uiSpeechTimer[m_uiSpeechCount] -= uiDiff;
            }
            else if(m_uiSubPhase == SUBPHASE_FLY_DOWN1){
                if(m_uiTimer <= uiDiff)
                {
                    float x, y, z;
                    m_creature->GetPosition(x, y, z);
                    z = z - 40;
                    DoMovement(x, y, z, 0, false);
                    m_uiSubPhase = SUBPHASE_FLY_DOWN2;
                    m_uiTimer = 1500;
                }else m_uiTimer -= uiDiff;
            }
            else if(m_uiSubPhase == SUBPHASE_FLY_DOWN2){
                if(m_uiTimer <= uiDiff)
                {
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    m_uiSubPhase = 0;
                    m_uiPhase = PHASE_FLOOR;
                }else m_uiTimer -= uiDiff;
            }    
        }
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Enrage timer.....
        if(m_uiEnrageTimer <= uiDiff)
        {
            DoCast(m_creature, SPELL_BERSERK);
            m_uiEnrageTimer = 600000;
        }else m_uiEnrageTimer -= uiDiff;

        if(m_uiPhase == PHASE_FLOOR)
        {
            if(m_uiSubPhase == SUBPHASE_VORTEX)
            {
                if(m_uiTimer <= uiDiff)
                {
                    DoVortex(m_uiVortexPhase);
                    
                    if(m_uiVortexPhase == 1 || m_uiVortexPhase == 11){
                        if(m_uiVortexPhase == 1)
                            DoCast(m_creature, SPELL_VORTEX_DUMMY);
                        m_uiTimer = 300;
                    }else
                        m_uiTimer = 1000;

                    if(m_uiVortexPhase >= MAX_VORTEX)
                    {
                        m_uiSubPhase = 0;
                        DoVortex(20);
                        return;
                    }
                    m_uiVortexPhase++;
                }else m_uiTimer -= uiDiff;
                return;
            }

            if(m_uiVortexTimer <= uiDiff)
            {
                DoVortex(0);
                m_uiVortexPhase = 1;
                m_uiSubPhase = SUBPHASE_VORTEX;
                m_uiVortexTimer = 50000;
                m_uiTimer = 3000;
            }else m_uiVortexTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }

    }
};

/*######
## go_focusing_iris
######*/
bool GOHello_go_focusing_iris(Player* pPlayer, GameObject* pGo)
{
    bool m_bIsRegularMode = pGo->GetMap()->IsRegularDifficulty();

    bool hasItem = false;
    if (m_bIsRegularMode)
    {
        if(pPlayer->HasItemCount(ITEM_KEY_TO_FOCUSING_IRIS, 1) || pPlayer->HasItemCount(ITEM_KEY_TO_FOCUSING_IRIS_H, 1)) 
            hasItem = true;
    }else{
        if(pPlayer->HasItemCount(ITEM_KEY_TO_FOCUSING_IRIS_H, 1))
            hasItem = true;        
    }
    if(!hasItem)        
        return false;

    if(Creature *pMalygos = GetClosestCreatureWithEntry(pGo, NPC_MALYGOS, 150.0f))
    {
        ((boss_malygosAI*)pMalygos->AI())->m_uiSubPhase = SUBPHASE_FLY_UP;
    }
    pGo->Delete();
    return false;
}
CreatureAI* GetAI_boss_malygos(Creature* pCreature)
{
    return new boss_malygosAI(pCreature);
}

void AddSC_boss_malygos()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_malygos";
    newscript->GetAI = &GetAI_boss_malygos;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_focusing_iris";
    newscript->pGOHello = &GOHello_go_focusing_iris;
    newscript->RegisterSelf();
}
/*
INSERT INTO npc_spellclick_spells VALUES (30161, 60968, 0, 0, 0, 1);

INSERT INTO `mangostest`.`vehicle_data` (
`entry` ,
`flags` ,
`Spell1` ,
`Spell2` ,
`Spell3` ,
`Spell4` ,
`Spell5` ,
`Spell6` ,
`Spell7` ,
`Spell8` ,
`Spell9` ,
`Spell10` ,
`req_aura` 
)
VALUES (
'264', '24', '56091', '56092', '57090', '57143', '57108', '57092', '0', '0', '0', '0', '0'
)
INSERT INTO `mangostest`.`vehicle_seat_data` (
`seat` ,
`flags` 
)
VALUES (
'2449', '1'
);

UPDATE gameobject_template SET ScriptName="go_focusing_iris" WHERE entry IN (193960, 193958);
UPDATE creature_template SET ScriptName="boss_malygos" WHERE entry=28859;

SELECT * FROM creature_addon WHERE guid IN (SELECT guid FROM creature WHERE id=28859)
SELECT * FROM creature_template WHERE entry=28859;
UPDATE `creature_template` SET `InhabitType` = '3' WHERE `entry` =28859 LIMIT 1 ;
UPDATE `mangostest`.`creature` SET `spawndist` = '0',
`MovementType` = '0' WHERE `creature`.`guid` =132313 LIMIT 1 ;

INSERT INTO `scriptdev2`.`script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES
('-1616000', 'Lesser beings, intruding here! A shame that your excess courage does not compensate for your stupidity!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '14512', '1', '0', '457', NULL),
('-1616001', 'None but the blue dragonflight are welcome here! Perhaps this is the work of Alexstrasza? Well then, she has sent you to your deaths.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '14513', '1', '0', '457', NULL),
('-1616002', 'What could you hope to accomplish, to storm brazenly into my domain? To employ MAGIC? Against ME? <Laughs>', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '14514', '1', '0', '457', NULL),
('-1616003', 'I am without limits here... the rules of your cherished reality do not apply... In this realm, I am in control...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '14515', '1', '0', '457', NULL),
('-1616004', 'I give you one chance. Pledge fealty to me, and perhaps I won''t slaughter you for your insolence!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '14516', '1', '0', '457', NULL),
('-1616005', 'My patience has reached its limit, I WILL BE RID OF YOU!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '14517', '1', '0', '1', NULL);

UPDATE `mangostest`.`creature_template` SET `AIName` = 'NullAI' WHERE `creature_template`.`entry` =22517 LIMIT 1 ;

UPDATE `mangostest`.`creature_template` SET `AIName` = 'NullAI' WHERE `creature_template`.`entry` =30090 LIMIT 1 ;

UPDATE `mangostest`.`creature_template` SET `AIName` = 'NullAI' WHERE `creature_template`.`entry` =30118 LIMIT 1 ;

UPDATE `mangostest`.`creature_template` SET `AIName` = 'NullAI' WHERE `creature_template`.`entry` =30334 LIMIT 1 ;

UPDATE `mangostest`.`creature_template` SET `AIName` = 'NullAI' WHERE `creature_template`.`entry` =31253 LIMIT 1 ;

UPDATE `mangostest`.`creature_template` SET `AIName` = 'NullAI' WHERE `creature_template`.`entry` =32448 LIMIT 1 ;


UPDATE `mangostest`.`creature_template` SET `modelid_A` = '11686',
`modelid_A2` = '11686',
`modelid_H` = '11686',
`modelid_H2` = '11686',
`minhealth` = '44120',
`maxhealth` = '44120',
`AIName` = '',
`flags_extra` = '2' WHERE `creature_template`.`entry` =22517 LIMIT 1 ;

*/