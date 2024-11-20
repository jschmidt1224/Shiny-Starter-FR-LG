#include "shiny.h"
#include <stdio.h>
#include <stdint.h>
#ifdef CLIPBOARD
	#include <Windows.h>
#endif


/* CORE FUNCTIONS */

#ifdef CLIPBOARD
	// copies n to clipboard
	static void copy_to_clipboard(uint32_t n){
		char buffer[128];
		sprintf(buffer, "%u", n);
		HGLOBAL hMem =  GlobalAlloc(GMEM_MOVEABLE, 129);
		memcpy(GlobalLock(hMem), buffer, 129);
		GlobalUnlock(hMem);
		OpenClipboard(0);
		EmptyClipboard();
		SetClipboardData(CF_TEXT, hMem);
		CloseClipboard();
	}
#endif

// returns 1 if *pkmn has all user-defined properties of the desired Pokemon, otherwise 0
static uint8_t is_target(const config_t *conf, const pkmn_t *pkmn, uint16_t tid, uint16_t sid){
	//check shininess
	if(conf->search_shiny && !is_shiny(pkmn->pid, tid, sid))
		return 0;
	
	//check gender
	if(conf->search_gender != IGNORE_GENDER){
		uint8_t is_male = (pkmn->pid & 0xFF) > 30;
		if((is_male && conf->search_gender == FEMALE) || (!is_male && conf->search_gender == MALE))
			return 0;
	}
	
	//check nature
	uint8_t nature = (uint8_t)(pkmn->pid % 25);
	if(!((conf->search_natures >> nature) & 1))
		return 0;
	
	//check ivs
	if(pkmn->ivs.stats[2] < conf->search_min_ivs.stats[2] 
			|| pkmn->ivs.stats[1] < conf->search_min_ivs.stats[1] 
			|| pkmn->ivs.stats[0] < conf->search_min_ivs.stats[0] 
			|| pkmn->ivs.stats[4] < conf->search_min_ivs.stats[4]
			|| pkmn->ivs.stats[3] < conf->search_min_ivs.stats[3] 
			|| pkmn->ivs.stats[5] < conf->search_min_ivs.stats[5])
		return 0;
	
	return 1;
}

uint8_t find_pid_r(const config_t *conf, uint16_t tid, uint16_t sid, uint32_t *pid_r, uint32_t *table_seed){
	uint32_t old_seeds[conf->pid_r_threshold];
	uint8_t old_seeds_ind = 0;
	uint32_t frame;
	uint32_t seed;
	
	//seed lcrng with TID tid
	seed = tid;
	
	//search for first target in [L, U] and update seeds of last conf->pid_r_threshold frames
	for(frame = 0; frame<=conf->u; frame++){
		pkmn_t p = pkmn(seed, NONE);
		
		if(frame >= conf->l && is_target(conf, &p, tid, sid)){
			*pid_r = frame;
			
			// load oldest saved seed into *table_seed
			*table_seed = old_seeds[old_seeds_ind];
			
			return 1;
		}
		
		old_seeds[old_seeds_ind] = seed;
		old_seeds_ind = (old_seeds_ind + 1) % (conf->pid_r_threshold);
		
		seed = lcrng(seed);
	}
	
	return 0;
}

void shiny(config_t *conf){
	uint16_t tid;
	uint16_t _sid;
	uint32_t a_f;
	uint32_t pid_r;
	uint32_t b_f;
	uint8_t pid_r_found;
	
	uint32_t table_seed;
	
	//calculate and output a_f
	a_f = conf->a + TID_F + conf->a_f_add;
	printf("SID target frame: %hu", a_f);
	
	#ifdef CLIPBOARD
		//copy a_f to clipboard
		copy_to_clipboard(a_f);
			
		printf(" [copied to clipboard]");
	#endif
	
	printf("\n");
	
	//receive TID
	printf("TID: ");
	scanf("%hu", &tid);
	
	//print expected SID
	_sid = sid(conf, tid);
	printf("SID: 0x%04X\n\n", _sid);
	
	//find pid_r and calculate b_f
	pid_r_found = find_pid_r(conf, tid, _sid, &pid_r, &table_seed);
	b_f = pid_r - B_F_SUB + conf->b_f_add;
	
	if(!pid_r_found){
		printf("No Pokemon in range [%u, %u] with user-defined properties.", conf->l, conf->u);
	}else{
		print_table(conf, tid, _sid, table_seed, pid_r-conf->pid_r_threshold);
		
		printf("\n\nPID target frame: %u", b_f);
	
		#ifdef CLIPBOARD
			//copy pid target frame to clipboard
			copy_to_clipboard(b_f);
				
			printf(" [copied to clipboard]");
		#endif
	}
	
	printf("\n");
}