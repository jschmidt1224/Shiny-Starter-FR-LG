#include "shiny.h"

#include <stdio.h>
#include <stdint.h>


/* GRAPHICAL OUTPUT FUNCTIONS */

static void print_table_header(){
	#ifdef LANG_DE
		printf("Offset Frame Shiny Geschl. Wesen    KP   ANG  VER  SANG SDEF INI PID");
	#elif  LANG_US
		printf("Offset Frame Shiny Gender  Nature   HP   ATK  DEF  SATK SDEF SPE PID");
	#endif
}

static void print_seperator(){
	printf("___________________________________________________________________________");
}

// prints shininess, gender, nature and ivs of *pkmn for TID tid and SID sid
static void print_pkmn(const pkmn_t* pkmn, uint16_t tid, uint16_t sid){
	//print shininess
	if(is_shiny(pkmn->pid, tid, sid)){
		printf(" ***  ");
	}else{
		printf("      ");
	}
	
	//print gender (for 12.5% chance of beeing female pokemon)
	//pokemon is female if last byte of pid is in [0, 30] otherwise male
	if((pkmn->pid & 0xFF) <= 30){
		#ifdef LANG_DE
			printf("w       ");
		#elif  LANG_US
			printf("f       ");
		#endif
	}else{
		printf("m       ");
	}
	
	//print nature
	printf("%-9s", NATURES[(uint8_t)(pkmn->pid % 25)]);
	
	//print ivs
	printf("%2u   ", pkmn->ivs.stats[0]);
	printf("%2u   ", pkmn->ivs.stats[1]);
	printf("%2u   ", pkmn->ivs.stats[2]);
	printf("%2u   ", pkmn->ivs.stats[3]);
	printf("%2u   ", pkmn->ivs.stats[4]);
	printf("%2u  ",  pkmn->ivs.stats[5]);
	
	//print pid
	printf("0x%08X", pkmn->pid);
}

void print_table(const config_t *conf, uint16_t tid, uint16_t sid, uint32_t seed, uint32_t first_frame){
	const uint32_t target_frame = first_frame + conf->pid_r_threshold;
	const uint32_t last_frame = target_frame + conf->pid_r_threshold;
	int16_t b_f_rel = -1 * conf->pid_r_threshold;
	
	print_table_header();
	printf("\n");
		
	for(uint32_t frame = first_frame; frame <= last_frame; frame++){
		pkmn_t p = pkmn(seed, conf->starter);
		
		if(frame == target_frame){
			print_seperator();
			printf("\n");
		}
		
		printf("  %+4d %-5u ", b_f_rel, frame);
		print_pkmn(&p, tid, sid);
		printf("\n");
		
		if(frame == target_frame){
			print_seperator();
			printf("\n");
		}
		
		seed = lcrng(seed);
		b_f_rel++;
	}
}