#include "defines.h"
#include "defines_battle.h"

#include "../include/constants/pokemon.h"	
#include "../include/constants/items.h"
#include "../include/constants/hold_effects.h"
#include "../include/constants/species.h"
#include "../include/field_weather.h"
#include "../include/battle.h"

extern struct Evolution gEvolutionTable[MAX_NUM_POKEMON][EVOS_PER_MON];

u16 GetEvolutionTargetSpecies(struct Pokemon *mon, u8 type, u16 evolutionItem) {
    int i;
    u16 targetSpecies = 0;
    u16 species = GetMonData(mon, MON_DATA_SPECIES, NULL);
    u16 heldItem = GetMonData(mon, MON_DATA_HELD_ITEM, NULL);
    u32 personality = GetMonData(mon, MON_DATA_PERSONALITY, NULL);
    u8 level;
    u16 friendship;
    u8 beauty = GetMonData(mon, MON_DATA_BEAUTY, 0);
    u16 upperPersonality = personality >> 16;
    u8 holdEffect;
	
    if (heldItem == ITEM_ENIGMA_BERRY)
        holdEffect = gSaveBlock1->enigmaBerry.holdEffect;
    else
        holdEffect = ItemId_GetHoldEffect(heldItem);

    if (holdEffect == ITEM_EFFECT_PREVENT_EVOLVE && type != 3)
        return SPECIES_NONE;

    switch (type)
    {
    case 0:
        level = GetMonData(mon, MON_DATA_LEVEL, 0);
        friendship = GetMonData(mon, MON_DATA_FRIENDSHIP, 0);
		
        for (i = 0; i < EVOS_PER_MON; ++i)
        {
            switch (gEvolutionTable[species][i].method)
            {
				case EVO_FRIENDSHIP:
					if (friendship >= 220)
						targetSpecies = gEvolutionTable[species][i].targetSpecies;
					break;
					
				case EVO_FRIENDSHIP_DAY:
				#ifdef TIME_ENABLED
					if (Clock->hour >= TIME_DAY_START && Clock->hour < TIME_NIGHT_START && friendship >= 220)
						targetSpecies = gEvolutionTable[species][i].targetSpecies;
				#endif
					break;
					
				case EVO_FRIENDSHIP_NIGHT:
				#ifdef TIME_ENABLED
					if (Clock->hour >= TIME_NIGHT_START && Clock->hour < TIME_DAY_START && friendship >= 220)
						targetSpecies = gEvolutionTable[species][i].targetSpecies;
				#endif
					break;
					
				case EVO_LEVEL:					
					if (gEvolutionTable[species][i].param <= level)
						targetSpecies = gEvolutionTable[species][i].targetSpecies;
					break;
					
				case EVO_LEVEL_ATK_GT_DEF:
					if (gEvolutionTable[species][i].param <= level)
						if (GetMonData(mon, MON_DATA_ATK, 0) > GetMonData(mon, MON_DATA_DEF, 0))
							targetSpecies = gEvolutionTable[species][i].targetSpecies;
					break;
					
				case EVO_LEVEL_ATK_EQ_DEF:
					if (gEvolutionTable[species][i].param <= level)
						if (GetMonData(mon, MON_DATA_ATK, 0) == GetMonData(mon, MON_DATA_DEF, 0))
							targetSpecies = gEvolutionTable[species][i].targetSpecies;
					break;
					
				case EVO_LEVEL_ATK_LT_DEF:
					if (gEvolutionTable[species][i].param <= level)
						if (GetMonData(mon, MON_DATA_ATK, 0) < GetMonData(mon, MON_DATA_DEF, 0))
							targetSpecies = gEvolutionTable[species][i].targetSpecies;
					break;
					
				case EVO_LEVEL_SILCOON:
					if (gEvolutionTable[species][i].param <= level && (upperPersonality % 10) <= 4)
						targetSpecies = gEvolutionTable[species][i].targetSpecies;
					break;
					
				case EVO_LEVEL_CASCOON:
					if (gEvolutionTable[species][i].param <= level && (upperPersonality % 10) > 4)
						targetSpecies = gEvolutionTable[species][i].targetSpecies;
					break;
					
				case EVO_LEVEL_NINJASK:
					if (gEvolutionTable[species][i].param <= level)
						targetSpecies = gEvolutionTable[species][i].targetSpecies;
					break;
					
				case EVO_BEAUTY:
					if (gEvolutionTable[species][i].param <= beauty)
						targetSpecies = gEvolutionTable[species][i].targetSpecies;
					break;
					
				case EVO_RAINY_OW:
					if (gEvolutionTable[species][i].param <= level
						&& (GetCurrentWeather() == WEATHER_RAIN_LIGHT
						|| GetCurrentWeather() == WEATHER_RAIN_MED
						|| GetCurrentWeather() == WEATHER_RAIN_HEAVY))
						targetSpecies = gEvolutionTable[species][i].targetSpecies;
					break;
					
				case EVO_HOLD_ITEM_DAY:
					if ((Clock->hour >= TIME_DAY_START && Clock->hour < TIME_NIGHT_START)
					&& heldItem == gEvolutionTable[species][i].param)
					{
						targetSpecies = gEvolutionTable[species][i].targetSpecies;
						#ifdef EVO_HOLD_ITEM_REMOVAL
							FlagSet(FLAG_REMOVE_EVO_ITEM);
						#endif
					}
					break;
					
				case EVO_HOLD_ITEM_NIGHT:
					if (heldItem == gEvolutionTable[species][i].param)
					{
						targetSpecies = gEvolutionTable[species][i].targetSpecies;
						#ifdef EVO_HOLD_ITEM_REMOVAL
							FlagSet(FLAG_REMOVE_EVO_ITEM);
						#endif
					}
					break;
					
				case EVO_MAP:
					if (gSaveBlock1->location.mapGroup == gEvolutionTable[species][i].param
					&& gSaveBlock1->location.mapNum == gEvolutionTable[species][i].unknown)
						targetSpecies = gEvolutionTable[species][i].targetSpecies;
					break;
					
				case EVO_MOVE:
					for (int j = 0; j < MAX_MON_MOVES; ++j)
					{
						if (gEvolutionTable[species][i].param == GetMonData(mon, MON_DATA_MOVE1 + j, NULL))
						{
							targetSpecies = gEvolutionTable[species][i].targetSpecies;
							break;
						}
					}
					break;
					
				case EVO_FAIRY_MOVE:
					for (int j = 0; j < MAX_MON_MOVES; ++j)
					{
						if (gBattleMoves[GetMonData(mon, MON_DATA_MOVE1 + j, NULL)].type == TYPE_FAIRY)
						{
							targetSpecies = gEvolutionTable[species][i].targetSpecies;
							break;
						}
					}
					break;
					
				case EVO_OTHER_PARTY_MON:
					break;
					
				case EVO_DARK_TYPE_IN_PARTY:
					break;
				
            }
        }
        break;
		
    case 1:
        for (i = 0; i < EVOS_PER_MON; ++i)
        {
            switch (gEvolutionTable[species][i].method)
            {
            case EVO_TRADE:
                targetSpecies = gEvolutionTable[species][i].targetSpecies;
                break;
            case EVO_TRADE_ITEM:
                if (gEvolutionTable[species][i].param == heldItem)
                {
                    heldItem = 0;
                    SetMonData(mon, MON_DATA_HELD_ITEM, &heldItem);
                    targetSpecies = gEvolutionTable[species][i].targetSpecies;
                }
                break;
            }
        }
        break;
		
    case 2:
    case 3:
        for (i = 0; i < EVOS_PER_MON; ++i)
        {
            if (gEvolutionTable[species][i].method == EVO_ITEM
             && gEvolutionTable[species][i].param == evolutionItem)
            {
                targetSpecies = gEvolutionTable[species][i].targetSpecies;
                break;
            }
        }
        break;
    }

    return targetSpecies;
};


void ItemEvolutionRemoval(pokemon_t* mon) {
	#ifdef EVO_HOLD_ITEM_REMOVAL
		if (FlagGet(FLAG_REMOVE_EVO_ITEM))
		{
			u32 heldItem = 0;
			SetMonData(mon, MON_DATA_HELD_ITEM, &heldItem);
			FlagClear(FLAG_REMOVE_EVO_ITEM);
		}
	#endif
	
};




