#include "shiny.h"
#include <stdint.h>

#define L_16(x) (x & 0x0000FFFFU)	
#define H_16(x)	(x >> 16)	

#define DEF_SDEF(x)	((x & 0b0111110000000000U) >> 10)
#define ATK_SATK(x)	((x & 0b0000001111100000U) >> 5)				
#define   HP_SPE(x) (x & 0b0000000000011111U)				

/* CONSTS */

static const ivs_t charmander = {{39, 52, 43, 60, 50, 65}};
static const ivs_t squirtle = {{44, 48, 65, 50, 64, 43}};
static const ivs_t bulbasaur = {{45, 49, 49, 65, 65, 45}};
//                                 HP AT DF SA SD SP
static const ivs_t natures[25] = {{{0, 0, 0, 0, 0, 0}}, // Hardy
								  {{0, 1, 2, 0, 0, 0}}, // Lonely
								  {{0, 1, 0, 0, 0, 2}}, // rave
								  {{0, 1, 0, 2, 0, 0}}, // Adamant
								  {{0, 1, 0, 0, 2, 0}}, // Naughty
								  {{0, 2, 1, 0, 0, 0}}, // Bold
								  {{0, 0, 0, 0, 0, 0}}, // Docile
								  {{0, 0, 1, 0, 0, 2}}, // Relaxed
								  {{0, 0, 1, 2, 0, 0}}, // Impish
								  {{0, 0, 1, 0, 2, 0}}, // Lax
								  {{0, 2, 0, 0, 0, 1}}, // Timid
								  {{0, 0, 2, 0, 0, 1}}, // Hasty
								  {{0, 0, 0, 0, 0, 0}}, // Serious
								  {{0, 0, 0, 2, 0, 1}}, // Jolly
								  {{0, 0, 0, 0, 2, 1}}, // Naive
								  {{0, 2, 0, 1, 0, 0}}, // Modest
								  {{0, 0, 2, 1, 0, 0}}, // Mild
								  {{0, 0, 0, 1, 0, 2}}, // Quiet
								  {{0, 0, 0, 0, 0, 0}}, // Bashful
								  {{0, 0, 0, 1, 2, 0}}, // Rash
								  {{0, 2, 0, 0, 1, 0}}, // Calm
								  {{0, 0, 2, 0, 1, 0}}, // Gentle
								  {{0, 0, 0, 0, 1, 2}}, // Sassy
								  {{0, 0, 0, 2, 1, 0}}, // Careful
								  {{0, 0, 0, 0, 0, 0}} // Quirky
};

static const float nature_mult[3] = {1.0, 1.1, 0.9};
static pkmn_t new_stats(pkmn_t old, starter_t starter);
static int calc_stat(ivs_t pokemon, pkmn_t ivs, int stat);


/* CALC FUNCTIONS */

uint32_t lcrng(uint32_t seed){
	return (uint32_t)(0x41C64E6DULL*((uint64_t)seed) + 0x00006073ULL);
}

// returns the pid based on the given seed and updates seed
static uint32_t pid(uint32_t *seed){
	uint32_t pid = 0;
	
	//lower 16 bits are determined by the 16 upper bits of the first rng-call
	*seed = lcrng(*seed);
	pid = H_16(*seed);
	
	//upper 16 bits are determined by the 16 upper bits of the second rng-call
	*seed = lcrng(*seed);
	pid |= (*seed & 0xFFFF0000U);
	
	return pid;
}

pkmn_t pkmn(uint32_t seed, starter_t starter){
	uint16_t seed_h_16;
	
	//zero-init
	pkmn_t pkmn = {0};
	pkmn_t pkmn2 = {0};
	
	//game makes an lcrng-call first
	seed = lcrng(seed);
	
	//calculate and set pid
	pkmn.pid = pid(&seed);
	
	//third call of rng for the ivs def, atk and hp
	seed = lcrng(seed);
	seed_h_16 = H_16(seed);
	
	pkmn.ivs.stats[2] |= (uint8_t) DEF_SDEF(seed_h_16); 
	pkmn.ivs.stats[1] |= (uint8_t) ATK_SATK(seed_h_16);
	pkmn.ivs.stats[0] |= (uint8_t)   HP_SPE(seed_h_16);
	
	//fourth call of rng for the ivs sdef, satk, spe 
	seed = lcrng(seed);
	seed_h_16 = H_16(seed);
	
	pkmn.ivs.stats[4] |= (uint8_t) DEF_SDEF(seed_h_16); 
	pkmn.ivs.stats[3] |= (uint8_t) ATK_SATK(seed_h_16);
	pkmn.ivs.stats[5] |= (uint8_t)   HP_SPE(seed_h_16);

	pkmn2 = new_stats(pkmn, starter);
	
	return pkmn2;
}

uint16_t sid(const config_t *conf, uint16_t tid){
	uint32_t seed = tid;
	
	for(uint16_t frame=0; frame<(conf->a + SID_R_ADD); frame++){
		seed = lcrng(seed);
	}
	
	return (uint16_t)H_16(seed);
}

uint8_t is_shiny(uint32_t pid, uint16_t tid, uint16_t sid){
	//a Pokemon is shiny if (tid xor pid xor (lower half of pid) xor (upper half of pid)) < 8
	
	return (tid ^ sid ^ ((uint16_t) L_16(pid)) ^ ((uint16_t) H_16(pid))) < 8;
}

static int calc_stat(ivs_t pokemon, pkmn_t ivs, int stat){
	return (((pokemon.stats[stat] * 2) + ivs.ivs.stats[stat]) * 5) / 100;
}

static pkmn_t new_stats(pkmn_t old, starter_t starter){
	pkmn_t new = old;
	int i = 1;
	switch (starter){
		case BULBASAUR:
			new.ivs.stats[0] = calc_stat(bulbasaur, old, 0) + 15;
			for ( ; i < 6; i++){
				new.ivs.stats[i] = (calc_stat(bulbasaur, old, i) + 5) * nature_mult[natures[(uint8_t)(old.pid % 25)].stats[i]];
			}
			break;
		case CHARMANDER:
			new.ivs.stats[0] = calc_stat(charmander, old, 0) + 15;
			for ( ; i < 6; i++){
				new.ivs.stats[i] = (calc_stat(charmander, old, i) + 5) * nature_mult[natures[(uint8_t)(old.pid % 25)].stats[i]];
			}
			break;
		case SQUIRTLE:
			new.ivs.stats[0] = calc_stat(squirtle, old, 0) + 15;
			for ( ; i < 6; i++){
				new.ivs.stats[i] = (calc_stat(squirtle, old, i) + 5) * nature_mult[natures[(uint8_t)(old.pid % 25)].stats[i]];
			}
			break;
		case NONE:
		default:
			break;
	}
	return new;
}