

#include "precompiled.h"
#include "events.h"
//#include "SpellMgr.h"
//#include "def_scarlet_monastery.h"

enum HorsemanTexts
{
    SAY_ENTRANCE           = -1189022,
    SAY_REJOINED           = -1189023,
    SAY_BODY_DEFEAT        = -1189024,
    SAY_LOST_HEAD          = -1189025,
    SAY_CONFLAGRATION      = -1189026,
    SAY_SPROUTING_PUMPKINS = -1189027,
    SAY_SLAY               = -1189028,
    SAY_DEATH              = -1189029,

    EMOTE_LAUGH            = -1189030,

    SAY_PLAYER1            = -1189031,
    SAY_PLAYER2            = -1189032,
    SAY_PLAYER3            = -1189033,
    SAY_PLAYER4            = -1189034
};

static const uint32 RandomLaugh[3] = {11965, 11975, 11976};

enum Enryes
{
    HH_MOUNTED         =     23682,
    HH_UNHORSED        =     23800,
    HEAD               =     23775,
    PULSING_PUMPKIN    =     23694,
    PUMPKIN_FIEND      =     23545,
    HELPER             =     23686,
    WISP_INVIS         =     24034,
};

enum HorsemanActiveSpells
{
    SPELL_CLEAVE           =     42587,
    SPELL_CONFLAGRATION    =     42380,       //Phase 2, can't find real spell(Dim Fire?)
    SPELL_CONFL_SPEED      =     22587,       //8% increase speed, value 22587 from SPELL_CONFLAGRATION mains that spell?
    SPELL_SUMMON_PUMPKIN   =     42394,
};

enum HorsemanConfusedSpells
{
    SPELL_WHIRLWIND    =                43116,
    SPELL_IMMUNE       =                42556,
    //    00 Killed by agressor that resive experience or honor
    //Effect: (006) SPELL_EFFECT_APPLY_AURA
    //        Base point = 0
    //        Target A (TARGET_SELF), Target B (No target)
    //        Aura (039) SPELL_AURA_SCHOOL_IMMUNITY, value = 1, misc = 127, miscB = 0, periodic = 0
    //Effect: (006) SPELL_EFFECT_APPLY_AURA
    //        Base point = 0
    //        Target A (TARGET_SELF), Target B (No target)
    //        Aura (042) SPELL_AURA_PROC_TRIGGER_SPELL, value = 1, misc = 0, miscB = 0, periodic = 0
    //        Trigger spell (42587) Horseman's Cleave. Chance = 101
    SPELL_BODY_REGEN   =                42403,
    //Effect: (006) SPELL_EFFECT_APPLY_AURA
    //        Base point = 4
    //        Target A (TARGET_SELF), Target B (No target)
    //        Aura (020) SPELL_AURA_OBS_MOD_HEALTH, value = 4, misc = 0, miscB = 0, periodic = 1000
    //Effect: (006) SPELL_EFFECT_APPLY_AURA
    //        Base point = 0
    //        Target A (TARGET_SELF), Target B (No target)
    //        Aura (056) SPELL_AURA_TRANSFORM, value = 1, misc = 23800, miscB = 0, periodic = 0
    SPELL_CONFUSE      =                43105,
    //    00 Killed by agressor that resive experience or honor
    //Effect: (006) SPELL_EFFECT_APPLY_AURA
    //        Base point = 0
    //        Target A (TARGET_SELF), Target B (No target)
    //        Aura (005) SPELL_AURA_MOD_CONFUSE, value = 1, misc = 0, miscB = 0, periodic = 0
    SPELL_BODY_STAGE_1 =                42547,  // strange duration here - 3600 secs
    SPELL_BODY_STAGE_2 =                42548,
    SPELL_BODY_STAGE_3 =                42549,
    SPELL_COMMAND_RETURN_HEAD =         42405,  // TARGET_SCRIPT head 23775, no visual,  when body's hp reaches up to 100%, and phase not increases
    SPELL_COMMAND_HEAD_RETURNS       =  42410,  // TARGET_SCRIPT body 23682, no visual, all ok, next phase begins, cancel regen body checks
    SPELL_COMMAND_TOSS_HEAD          =  43101,  // TARGET_SCRIPT head 23775, 10687 visual, when body "dies"
};  /*
        Delete from `spell_script_target` where entry in (42405, 42410, 43101);
        Insert into `spell_script_target` values
        (42405, 1, 23775),
        (42410, 1, 23682),
        (43101, 1, 23775);
    */

//Effects
enum SpellVisuals
{
    SPELL_RHYME_BIG        =     42909,
    SPELL_RHYME_SMALL      =     42910,
    SPELL_BODY_FLAME       =     42074,      // apply aura, after horseman dies
    SPELL_HEAD_FLAME       =     42971,
    SPELL_ENRAGE_VISUAL    =     42438,      // he uses this spell?
    SPELL_WISP_BLUE        =     42821,
    SPELL_WISP_FLIGHT_PORT =     42818,
    SPELL_WISP_INVIS       =     42823,
    SPELL_SMOKE            =     42355,
    SPELL_DEATH            =     42566,       //not correct spell
};

struct Locations
{
    float x, y, z;
};

#define NODES_COUNT             21
#define FLIGHT_TRAVEL_TIME      16000

static const Locations FlightPoint[NODES_COUNT]=
{
    {1754.00,1346.00,17.50},
    {1765.00,1347.00,19.00},
    {1784.00,1346.80,25.40},
    {1803.30,1347.60,33.00},
    {1824.00,1350.00,42.60},
    {1838.80,1353.20,49.80},
    {1852.00,1357.60,55.70},
    {1861.30,1364.00,59.40},
    {1866.30,1374.80,61.70},
    {1864.00,1387.30,63.20},
    {1854.80,1399.40,64.10},
    {1844.00,1406.90,64.10},
    {1824.30,1411.40,63.30},
    {1801.00,1412.30,60.40},
    {1782.00,1410.10,55.50},
    {1770.50,1405.20,50.30},
    {1765.20,1400.70,46.60},
    {1761.40,1393.40,41.70},
    {1759.10,1386.70,36.60},
    {1757.80,1378.20,29.00},
    {1758.00,1367.00,19.51}
};

static const Locations Spawn[]=
{
    {1776.27,1348.74,19.20},        //spawn point for pumpkin shrine mob
    {1765.28,1347.46,17.55}         //spawn point for smoke
};

enum PumpkinSpells
{
    SPELL_PUMPKIN_AURA       =   42280, //  Pumpkin Life Cycle
    //    Range 30.00 - 30.00 yards
    //    Duration = -1, 0, -1
    //Effect: (006) SPELL_EFFECT_APPLY_AURA
    //        Base point = 0
    //        Target A (TARGET_SELF), Target B (No target)
    //        Aura (226) SPELL_AURA_DUMMY_2, value = 0, misc = 0, miscB = 0, periodic = 1000
    //Effect: (006) SPELL_EFFECT_APPLY_AURA
    //        Base point = 0
    //        Target A (TARGET_SELF), Target B (No target)
    //        Aura (026) SPELL_AURA_MOD_ROOT, value = 1, misc = 0, miscB = 0, periodic = 0
    SPELL_PUMPKIN_AURA_GREEN =   42294,
    //    Category = 0, SpellIconID = 1, activeIconID = 0, SpellVisual_0 = 9505, SpellVisual_1 = 0
    //    Family SPELLFAMILY_GENERIC, flag 0x00000000 00000000 00000000
    //    Duration = -1, 0, -1
    //Effect: (006) SPELL_EFFECT_APPLY_AURA
    //        Base point = 0
    //        Target A (TARGET_SELF), Target B (No target)
    //        Aura (004) SPELL_AURA_DUMMY, value = 1, misc = 0, miscB = 0, periodic = 0
    SPELL_SQUASH_SOUL        =   42514,
    SPELL_SPROUTING          =   42281,
    //    School = SPELL_SCHOOL_ARCANE, DamageClass = SPELL_DAMAGE_CLASS_NONE, PreventionType = NONE
    //    CastingTime = 15.00
    //Effect: (077) SPELL_EFFECT_SCRIPT_EFFECT
    //        Base point = 0
    //        Target A (TARGET_SELF), Target B (No target)
    //Effect: (136) SPELL_EFFECT_HEAL_PCT
    //        Base point = 100
    //        Target A (TARGET_SELF), Target B (No target)
    SPELL_SPROUT_BODY        =   42285,
    //    Duration = -1, 0, -1
    //Effect: (006) SPELL_EFFECT_APPLY_AURA
    //        Base point = 0
    //        Target A (TARGET_SELF), Target B (No target)
    //        Aura (056) SPELL_AURA_TRANSFORM, value = 1, misc = 23545, miscB = 0, periodic = 0
    //Effect: (002) SPELL_EFFECT_SCHOOL_DAMAGE
    //        Base point = 500
    //        Target A (TARGET_ALL_AROUND_CASTER), Target B (TARGET_ALL_ENEMY_IN_AREA)
    //        Radius (8) 5.00
};


// hacky way to work with some non stackable auras (but they must be stackable)
class MANGOS_DLL_DECL BugAura : public Aura
{
public:
    BugAura(SpellEntry *spell, uint32 eff, int32 *bp, Unit *target, Unit *caster) : Aura(spell, eff, bp, target, caster, NULL)
    {}
};

static void ApplyAuras(Unit* target, uint32 spellId)
{
    if (SpellEntry *spell = (SpellEntry *)GetSpellStore()->LookupEntry(spellId))
        for(uint8 i = 0; i < 3; ++i)
        {
            if(!spell->Effect[i] || spell->Effect[i] != SPELL_EFFECT_APPLY_AURA)
                continue;

            BugAura* Aur = new BugAura(spell, i, &spell->EffectBasePoints[i],target,target);
            Aur->_AddAura();
            target->GetAuras().insert(Unit::AuraMap::value_type(Unit::spellEffectPair(Aur->GetId(), Aur->GetEffIndex()), Aur));
            Unit::AuraList & aurlist = ((Unit::AuraList&)target->GetAurasByType(Aur->GetModifier()->m_auraname));
            aurlist.push_back(Aur);
            Aur->ApplyModifier(true,true);
        }
};

static void SendMoveByPath(Unit *obj, uint32 traveltime)
{
    WorldPacket data( SMSG_MONSTER_MOVE, (obj->GetPackGUID().size()+1+4+4+4+4+1+4+4+4+NODES_COUNT*4*3) );
    data.append(obj->GetPackGUID());
    data << uint8(0);
    data << FlightPoint[0].x;
    data << FlightPoint[0].y;
    data << FlightPoint[0].z;
    data << uint32(getMSTime());
    data << uint8(0);
    data << uint32(0x3000);
    data << uint32(traveltime);
    data << uint32(NODES_COUNT);
    data.append((char*)FlightPoint, NODES_COUNT * 4 * 3);
    obj->SendMessageToSet(&data, true);
};

#define SELF_CAST(id, force) m_creature->CastSpell(m_creature, id, force)

struct MANGOS_DLL_DECL mob_pulsing_pumpkinAI : public ScriptedAI   // also known as pumpkin fiend 
{
    mob_pulsing_pumpkinAI(Creature *c) : ScriptedAI(c) {InitRootedState();}

    void Reset() {}

    void InitRootedState()
    {
        SetCombatMovement(false);

        //DoCast is not very good function here 
        //little problem: first two auras here are not stackable
        //so i commmented less important aura
        //SELF_CAST(SPELL_PUMPKIN_AURA_GREEN,true);
        //SELF_CAST(SPELL_PUMPKIN_AURA,true);
        ApplyAuras(m_creature, SPELL_PUMPKIN_AURA_GREEN);
        ApplyAuras(m_creature, SPELL_PUMPKIN_AURA);
        SELF_CAST(SPELL_SPROUTING,false);
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        switch(spell->Id)
        {
        case SPELL_SPROUTING:
            //m_creature->RemoveAllAuras(); not sure that need remove all auras
            //SPELL_AURA_TRANSFORM works incorrect - 
            //it must really change entry, stats of creatue, not displayid only
            SELF_CAST(SPELL_SPROUT_BODY,true);
            SetCombatMovement(true);
            break;
        case 42428:
            m_creature->setDeathState(JUST_DIED);
            break;
        }
    }

    void MoveInLineOfSight(Unit *who)
    {
        if(IsCombatMovement())
            ScriptedAI::MoveInLineOfSight(who);
        else   
        {
            if (m_creature->IsWithinDist(who,0.1f) && m_creature->IsHostileTo(who))
                m_creature->CastSpell(who,SPELL_SQUASH_SOUL,true);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(IsCombatMovement())
            ScriptedAI::UpdateAI(diff);
    }

    void AttackStart(Unit* who)
    {
        if(IsCombatMovement())
            ScriptedAI::AttackStart(who);
    }

    void EnterEvadeMode()
    {
        if (IsCombatMovement()){
            ScriptedAI::EnterEvadeMode();
            //restore some auras - that's hack, need remove only negative auras on enter evade call
            SELF_CAST(SPELL_SPROUT_BODY,true);  
        }
    }
};

CreatureAI* GetAI_mob_pulsing_pumpkin(Creature* pCreature)
{
    return new mob_pulsing_pumpkinAI(pCreature);
}

#define MSK(m)  (1 << (m))

enum Phases
{
    PH_INTRO        = 0,//x01
                        //x02
    PH_ONE          = 2,//x04
    PH_TWO          = 3,//x08
    PH_THREE        = 4,

    STATE_MOUNTED   = 1 << 16, //x01
    STATE_HEADLESS  = 1 << 17, //x02
    STATE_COMBAT    = 1 << 18, //x04
    HEAD_SPAWNED    = 1 << 19,
};

enum HorsemanEvents
{
    EV_LAUGHT,
    EV_INTRO_TIMER,
    EV_CLEAVE,
    EV_CONFLAGRATE,
    EV_SPROUT_PUMPKINS,
    EV_WHIRLWIND,
    EV_SPELL_42403_TICK,
    EV_HEAD_FLEE,
    MAX_EVENTS,
};

static const EvInfo EventInfo[MAX_EVENTS] =
{
    {/*EV_LAUGHT,*/            STATE_MOUNTED|STATE_COMBAT|CONDITION_OR},
    {/*EV_INTRO_TIMER,*/       MSK(PH_INTRO)|CONDITION_OR},
    {/*EV_CLEAVE,  */          STATE_COMBAT|CONDITION_OR},
    {/*EV_CONFLAGRATE,*/       MSK(PH_TWO)|STATE_COMBAT},
    {/*EV_SPROUT_PUMPKINS,*/   MSK(PH_THREE)|STATE_COMBAT},

    {/*EV_WHIRL_OR_CONFUSED,*/ STATE_HEADLESS|CONDITION_OR},
    {/*EV_SPELL_42403_TICK,*/  STATE_HEADLESS|CONDITION_OR},
    {/*EV_HEAD_FLEE,*/         STATE_COMBAT|CONDITION_OR},
};


enum HorsemanHeadSpells
{
    SPELL_FLYING_HEAD     =      42399,       //visual flying head
    //    Range 50000.00 - 50000.00 yards
    //    Speed 9.00
    //Effect: (077) SPELL_EFFECT_SCRIPT_EFFECT
    //        Base point = 0
    //        Target A (TARGET_DUELVSPLAYER), Target B (No target)
    //Effect: (061) SPELL_EFFECT_SEND_EVENT
    //        Base point = 0
    //        Target A (No target), Target B (No target)
    //        EffectMiscValue = 15394
    SPELL_HEAD            =      42413,       //visual buff, "head"
    //    Duration = -1, 0, -1
    //Effect: (006) SPELL_EFFECT_APPLY_AURA
    //        Base point = 0
    //        Target A (TARGET_SELF), Target B (No target)
    //        Aura (004) SPELL_AURA_DUMMY, value = 1, misc = 0, miscB = 0, periodic = 0
    SPELL_HEAD_SPEAKS      =     43129,
    //    Duration = 2000, 0, 2000
    //Effect: (006) SPELL_EFFECT_APPLY_AURA
    //        Base point = 0
    //        Target A (TARGET_SELF), Target B (No target)
    //        Aura (004) SPELL_AURA_DUMMY, value = 1, misc = 0, miscB = 0, periodic = 0
    SPELL_HEAD_LANDS       =     42400,
    //    Range 50000.00 - 50000.00 yards
    //Effect: (077) SPELL_EFFECT_SCRIPT_EFFECT
    //        Base point = 0
    //        Target A (TARGET_DUELVSPLAYER), Target B (No target)
    SPELL_HEAD_INVISIBLE   =     44312,
};

enum DeathSpells
{
    SPELL_BODY_DEATH      =      42429,
    //Effect: (001) SPELL_EFFECT_INSTAKILL
    //        Base point = 0
    //        Target A (TARGET_SELF), Target B (No target)
    SPELL_HEAD_IS_DEAD    =      42428,       //at killing head, Phase 3, looks like explosion
    //    Range 50000.00 - 50000.00 yards
    //Effect: (003) SPELL_EFFECT_DUMMY
    //        Base point = 0
    //        Target A (TARGET_ALL_AROUND_CASTER), Target B (TARGET_7)
    //        Radius (22) 200.00
    //Effect: (061) SPELL_EFFECT_SEND_EVENT
    //        Base point = 0
    //        Target A (No target), Target B (No target)
    //        EffectMiscValue = 15407
    //Effect: (064) SPELL_EFFECT_TRIGGER_SPELL
    //        Base point = 0
    //        Target A (TARGET_SELF), Target B (No target)
    //        Trigger spell (42566) Headless Horseman Climax - Head Is Dead. Chance = 101
    //--------------------------
    //ID - 42566 Headless Horseman Climax - Head Is Dead () // missile, that fly to horseman's body?
    //Speed 10.00
    //Effect: (003) SPELL_EFFECT_DUMMY
    //        Base point = 0
    //        Target A (TARGET_EFFECT_SELECT), Target B (No target)
    SPELL_BODY_LEAVES_COMBAT  = 43805,
    //    Range 50000.00 - 50000.00 yards
    //Effect: (003) SPELL_EFFECT_DUMMY
    //        Base point = 0
    //        Target A (TARGET_ALL_AROUND_CASTER), Target B (TARGET_7)
    //        Radius (22) 200.00
    //Effect: (064) SPELL_EFFECT_TRIGGER_SPELL
    //        Base point = 0
    //        Target A (TARGET_SELF), Target B (No target)
    //        Trigger spell (42566) Headless Horseman Climax - Head Is Dead. Chance = 101
};

struct MANGOS_DLL_DECL mob_horseman_headAI : public ScriptedAI, public EventsPool
{
    mob_horseman_headAI(Creature *c) : ScriptedAI(c), EventsPool(EventInfo) { Reset();}

    void SaySound(int32 textEntry, Unit *target = 0)
    {
        DoScriptText(textEntry, m_creature, target);
        ApplyAuras(m_creature,SPELL_HEAD_SPEAKS);
    }

    void Reset()
    {
        ClearTimers();
        SetState(0xFFFFFFFF, false);
        SetPhase(PH_ONE-1);
    }

    void MoveInLineOfSight(Unit*) {}
    void AttackStart(Unit*){}
    void EnterEvadeMode(){}

    void DamageTaken(Unit* done_by,uint32 &damage)
    {
        if(!HasState(STATE_COMBAT))
        { // set incoming dmg to null while invisible
            damage = 0;
            return;
        }

        uint32 health_perc = (m_creature->GetHealth() - damage)*100/m_creature->GetMaxHealth();

        switch(GetPhaseMask())
        {
        case MSK(PH_ONE):
            if (health_perc < 67)
            {
                SELF_CAST(SPELL_COMMAND_HEAD_RETURNS, true);
                SetState(STATE_COMBAT, false);
            }break;
        case MSK(PH_TWO):
            if (health_perc < 34)
            {
                SELF_CAST(SPELL_COMMAND_HEAD_RETURNS, true);
                SetState(STATE_COMBAT, false);
            }break;
        case MSK(PH_THREE):
            if (damage >= m_creature->GetHealth())
            {
                damage = m_creature->GetHealth() - m_creature->GetMaxHealth() * 0.01;
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                SELF_CAST(SPELL_HEAD_IS_DEAD,true);
            }break;
        }
    }

    void SpellHit(Unit *caster, const SpellEntry* spell)
    {
        switch(spell->Id)
        {
        case SPELL_FLYING_HEAD:
            error_log("HeadAI::SpellHit by SPELL_FLYING_HEAD");
            MovePhase(true);
            SetState(STATE_COMBAT, true);
            SaySound(SAY_LOST_HEAD);
            break;
        case SPELL_COMMAND_RETURN_HEAD:
            MovePhase(false);
            m_creature->CastSpell(caster,SPELL_FLYING_HEAD, true);
            SetState(STATE_COMBAT, false);
            break;
        }
    }

    void SpellHitTarget(Unit* target, const SpellEntry* spell)
    {
        if(spell->Id == SPELL_COMMAND_HEAD_RETURNS)
            m_creature->CastSpell(target,SPELL_FLYING_HEAD, true);
    }

    void OnStateChanged(uint32 state)
    {
        switch(state)
        {
        case STATE_COMBAT:
            if (HasState(STATE_COMBAT))
            {
                error_log("HeadAI applying STATE_COMBAT");
                m_creature->RemoveAurasDueToSpell(SPELL_HEAD_INVISIBLE);
                SELF_CAST(SPELL_HEAD_LANDS,true);
                ApplyAuras(m_creature,SPELL_HEAD);

                SheduleProcess(EV_LAUGHT,11000,5000,12000);
                SheduleProcess(EV_HEAD_FLEE,1000);
            }
            else
            {
                error_log("HeadAI removing STATE_COMBAT");
                m_creature->RemoveAllAuras();
                ApplyAuras(m_creature,SPELL_HEAD_INVISIBLE);
                m_creature->SetHealth(m_creature->GetMaxHealth());
                //m_creature->CombatStop();
            }
            break;
        }
    }

    void ProcessEvent(uint32 id)
    {
        switch(id)
        {
        case EV_LAUGHT:
            SaySound(EMOTE_LAUGH);
            break;
        case EV_HEAD_FLEE:
            if (Unit* u = m_creature->getAttackerForHelper())
            {
                m_creature->GetMotionMaster()->Clear(false);
                m_creature->GetMotionMaster()->MoveFleeing(u);
            }break;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        EventsPool::Update(diff);
    }
};

CreatureAI* GetAI_mob_horseman_head(Creature* pCreature)
{
    return new mob_horseman_headAI(pCreature);
}

struct MANGOS_DLL_DECL boss_headless_horsemanAI : public ScriptedAI, public EventsPool
{
    boss_headless_horsemanAI(Creature *c) : ScriptedAI(c), EventsPool(EventInfo)
    {
        /*if (SpellEntry *confl = GET_SPELL(SPELL_CONFLAGRATION))
        {
            confl->EffectApplyAuraName[0] = SPELL_AURA_PERIODIC_DAMAGE_PERCENT;
            confl->EffectBasePoints[0] = 10;
            confl->EffectBaseDice[0] = 10;
            confl->DmgMultiplier[0] = 1;
        }*/
        Reset();
    }

    uint64 headguid;

    void Reset()
    {
        headguid = 0;
        ClearTimers();
        //for testing
        SetState(0xFFFFFFFF, false);//clean states

        SetState(STATE_MOUNTED, true);
        SetPhase(PH_INTRO);
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (HasState(STATE_COMBAT))
            ScriptedAI::MoveInLineOfSight(who);
    }

    void AttackStart(Unit* who)
    {
        if (HasState(STATE_COMBAT))
            ScriptedAI::AttackStart(who);
    }

    void KilledUnit(Unit*) { SaySound(SAY_PLAYER1 - rand()%4);}
    void JustDied(Unit*) { SaySound(SAY_DEATH);}

    void SaySound(int32 textEntry, Unit *target = 0)
    {
        DoScriptText(textEntry, m_creature, target);
        ApplyAuras(m_creature,SPELL_HEAD_SPEAKS);
        if(EventTimer *t = GetEventTimer(EV_LAUGHT))
            t->ModifyTime(4000);
    }

    void SpellHit(Unit *caster, const SpellEntry* spell)
    {
        switch(spell->Id)
        {
            case SPELL_COMMAND_HEAD_RETURNS:
                CancelEvent(EV_SPELL_42403_TICK);
                break;
            case SPELL_FLYING_HEAD:
                caster->GetMotionMaster()->Clear(false);
                caster->GetMotionMaster()->MoveFollow(m_creature,10.0f,float(rand()%6));

                MovePhase(true);

                SetState(STATE_HEADLESS,false);
                SetState(STATE_MOUNTED|STATE_COMBAT,true);

                SaySound(SAY_REJOINED);
                break;
            case SPELL_HEAD_IS_DEAD:
                SELF_CAST(SPELL_HEAD_IS_DEAD, true);
                SELF_CAST(SPELL_BODY_DEATH, true);
                break;
        }
    }

    void SpellHitTarget(Unit* target, const SpellEntry* spell)
    {
        if(spell->Id == SPELL_COMMAND_TOSS_HEAD)
            m_creature->CastSpell(target,SPELL_FLYING_HEAD,true);
    }

    void DamageTaken(Unit *done_by, uint32 &damage)
    {
        if (damage >= m_creature->GetHealth())
        {
            damage = 0;//m_creature->GetHealth() - m_creature->GetMaxHealth()/100;
            m_creature->SetHealth(1);

            SetState(HEAD_SPAWNED, true);
            SELF_CAST(SPELL_COMMAND_TOSS_HEAD, true);

            SetState(STATE_MOUNTED|STATE_COMBAT, false);
            SetState(STATE_HEADLESS,true);
        }
    }

    void OnStateChanged(uint32 state)
    {
        bool apply = HasState(state);
        switch(state)
        {
        case STATE_MOUNTED:
            if(apply)
            {
                ApplyAuras(m_creature, SPELL_HEAD);
                SheduleProcess(EV_LAUGHT,11000,5000,12000);
            }
            else
                m_creature->RemoveAurasDueToSpell(SPELL_HEAD);
            break;
        case STATE_HEADLESS:
            if(apply)
            {
                //must be part of confused mov generator
                m_creature->SetTargetGUID(0);
                SaySound(SAY_BODY_DEFEAT);

                //it's core problem - damage can remove auras below 
                ApplyAuras(m_creature, SPELL_IMMUNE);
                ApplyAuras(m_creature, SPELL_CONFUSE);
                ApplyAuras(m_creature, SPELL_BODY_REGEN);

                SheduleProcess(EV_SPELL_42403_TICK,1000);
                SheduleProcess(EV_WHIRLWIND,3000,3000,4000);
            }
            else
            {
                m_creature->RemoveAurasDueToSpell(SPELL_IMMUNE);
                m_creature->RemoveAurasDueToSpell(SPELL_BODY_REGEN);
                m_creature->RemoveAurasDueToSpell(SPELL_CONFUSE);
                m_creature->RemoveAurasDueToSpell(SPELL_WHIRLWIND);
            }
            break;
        case HEAD_SPAWNED:
            if (apply)
            {
                float x, y, z;
                m_creature->GetClosePoint(x,y,z,m_creature->GetObjectSize(),float(rand()%6),float(rand()%6));
                headguid = m_creature->SummonCreature(HEAD,x,y,z,0.0f, TEMPSUMMON_DEAD_DESPAWN, 0)->GetGUID();
            }
            else if(Unit* u = Unit::GetUnit((*m_creature), headguid))
            {
                u->AddObjectToRemoveList();
                headguid = 0;
            }
            break;
        case MSK(PH_INTRO):
            if(apply)
            {
                m_creature->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                SendMoveByPath(m_creature, FLIGHT_TRAVEL_TIME);
                SheduleProcess(EV_INTRO_TIMER, FLIGHT_TRAVEL_TIME,FLIGHT_TRAVEL_TIME);
            }
            else
            {
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                const Locations& loc = FlightPoint[NODES_COUNT-1];
                m_creature->Relocate(loc.x,loc.y,loc.z);
                SaySound(SAY_ENTRANCE);
            }break;
        case MSK(PH_ONE):
            if(apply)
            {
                ApplyAuras(m_creature,SPELL_BODY_STAGE_1);
                SheduleProcess(EV_CLEAVE,2000,0,4000);
            }else
                m_creature->RemoveAurasDueToSpell(SPELL_BODY_STAGE_1);
            break;
        case MSK(PH_TWO):
            if(apply)
            {
                ApplyAuras(m_creature,SPELL_BODY_STAGE_2);
                SheduleProcess(EV_CONFLAGRATE,10000,15000,7000);
            }else
                m_creature->RemoveAurasDueToSpell(SPELL_BODY_STAGE_2);
            break;
        case MSK(PH_THREE):
            if(apply)
            {
                ApplyAuras(m_creature,SPELL_BODY_STAGE_3);
                SheduleProcess(EV_SPROUT_PUMPKINS,25000,15000,11000);
            }
            else
                m_creature->RemoveAurasDueToSpell(SPELL_BODY_STAGE_3);
            break;
        }
    }

    void ProcessEvent(uint32 ev_id)
    {
        switch (ev_id)
        {
        case EV_CLEAVE:
            if(m_creature->getVictim())
                m_creature->CastSpell(m_creature->getVictim(),SPELL_CLEAVE, false);
            break;
        case EV_LAUGHT:
            m_creature->MonsterTextEmote(EMOTE_LAUGH,NULL);
            DoPlaySoundToSet(m_creature, RandomLaugh[rand()%3]);
            break;
        case EV_CONFLAGRATE:
            if (Unit *u = SelectUnit(SELECT_TARGET_RANDOM,0))
            {
                SaySound(SAY_CONFLAGRATION, u);
                m_creature->CastSpell(u,SPELL_CONFLAGRATION,false);
                //u->CastSpell(u,SPELL_CONFL_SPEED,true);  SPELL_CONFL_SPEED must be casterd after SPELL_CONFLAGRATION(core)
            }break;
        case EV_SPROUT_PUMPKINS:
            SELF_CAST(SPELL_SUMMON_PUMPKIN,false);
            SaySound(SAY_SPROUTING_PUMPKINS);
            break;
        case EV_WHIRLWIND:
            // other auras removes: (confused, immune and regenerate aura) :(.. fucking mangos
            if (rand()%2)
                ApplyAuras(m_creature,SPELL_WHIRLWIND);
            else
                m_creature->RemoveAurasDueToSpell(SPELL_WHIRLWIND);
            break;
        case EV_SPELL_42403_TICK:
            if (m_creature->GetHealth() == m_creature->GetMaxHealth())
            {
                MovePhase(false);

                SELF_CAST(SPELL_COMMAND_RETURN_HEAD, true);
                CancelEvent(EV_SPELL_42403_TICK);
            }break;
        case EV_INTRO_TIMER:
            SetPhase(PH_ONE);
            SetState(STATE_COMBAT, true);
            CancelEvent(EV_INTRO_TIMER);
            break;
        }

    }

    void UpdateAI(const uint32 diff)
    {
        if (HasState(STATE_COMBAT))
            ScriptedAI::UpdateAI(diff);

        EventsPool::Update(diff);
    }
};

CreatureAI* GetAI_boss_headless_horseman(Creature* pCreature)
{
    return new boss_headless_horsemanAI(pCreature);
}

void AddSC_Headless_Horseman_Event()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "mob_pulsing_pumpkin";
    newscript->GetAI = &GetAI_mob_pulsing_pumpkin;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_headless_horseman";
    newscript->GetAI = &GetAI_boss_headless_horseman;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_horseman_head";
    newscript->GetAI = &GetAI_mob_horseman_head;
    newscript->RegisterSelf();
}
