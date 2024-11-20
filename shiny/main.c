#include "shiny.h"
#include "ini.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define KEY_IS(x) (strcmp(key, (x)) == 0)

static const char * const CONFIG_FILE_NAME = "config.ini";


// INI parser handler for CONFIG_FILE_NAME
static int config_handler(void *user, const char *section, const char *key, const char *val){
	config_t *conf = (config_t *) user;
	
	if(KEY_IS("SID_ADD")){
		__mingw_sscanf(val, "%hhd", &(conf->a_f_add));
		
	}else if(KEY_IS("PID_ADD")){
		__mingw_sscanf(val, "%hhd", &(conf->b_f_add));
		
	}else if(KEY_IS("BETWEEN_TID_SID")){
		__mingw_sscanf(val, "%u", &(conf->a));
		
	}else if(KEY_IS("LOWER")){
		__mingw_sscanf(val, "%u", &(conf->l));
		
	}else if(KEY_IS("UPPER")){
		__mingw_sscanf(val, "%u", &(conf->u));
		
	}else if(KEY_IS("ROWS")){
		__mingw_sscanf(val, "%hhu", &(conf->pid_r_threshold));
		
	}else if(KEY_IS("SHINY_ONLY")){
		if(strcmp(val, "yes") == 0){
			conf->search_shiny = 1;
		}
		
	}else if(KEY_IS("GENDER")){
		if(strcmp(val, "male") == 0){
			conf->search_gender = MALE;
		}else if(strcmp(val, "female") == 0){
			conf->search_gender = FEMALE;
		}else{
			conf->search_gender = IGNORE_GENDER;
		}
		
	}else if(KEY_IS("MIN_IV_DEF")){
		__mingw_sscanf(val, "%hhu", &(conf->search_min_ivs.stats[2]));
		conf->search_min_ivs.stats[2] %= 32; 
		
	}else if(KEY_IS("MIN_IV_ATK")){
		__mingw_sscanf(val, "%hhu", &(conf->search_min_ivs.stats[1]));
		conf->search_min_ivs.stats[1] %= 32;
		
	}else if(KEY_IS("MIN_IV_HP")){
		__mingw_sscanf(val, "%hhu", &(conf->search_min_ivs.stats[2]));
		conf->search_min_ivs.stats[0] %= 32;
		
	}else if(KEY_IS("MIN_IV_SDEF")){
		__mingw_sscanf(val, "%hhu", &(conf->search_min_ivs.stats[4]));
		conf->search_min_ivs.stats[4] %= 32;
		
	}else if(KEY_IS("MIN_IV_SATK")){
		__mingw_sscanf(val, "%hhu", &(conf->search_min_ivs.stats[3]));
		conf->search_min_ivs.stats[3] %= 32;
		
	}else if(KEY_IS("MIN_IV_SPE")){
		__mingw_sscanf(val, "%hhu", &(conf->search_min_ivs.stats[5]));
		conf->search_min_ivs.stats[5] %= 32;
		
	}else if(KEY_IS("NATURES")){
		if(strcmp(val, "") == 0){
			conf->search_natures = 0xFFFFFFFF;
		}else{
			char *value = (char *) malloc(strlen(val)+1);
			memcpy(value, val, strlen(val)+1);
			
			char *nature = strtok(value, " ");
			while(nature != NULL){
				//search for nature in NATURES and set flag in conf->search_natures accordingly
				uint32_t flag = 1;
				for(uint8_t i=0; i<25; i++){
					if(strcmp(nature, NATURES[i]) == 0){
						conf->search_natures |= flag;
						break;
					}
					flag = flag << 1;
				}
				nature = strtok(NULL, " ");
			}
			free(value);
		}
	}else if(KEY_IS("STARTER")){
		if(strcmp(val, "charmander") == 0){
			conf->starter = CHARMANDER;
		}else if(strcmp(val, "squirtle") == 0){
			conf->starter = SQUIRTLE;
		}else if(strcmp(val, "bulbasaur") == 0){
			conf->starter = BULBASAUR;
		}else{
			conf->starter = NONE;
		}
	}
	return 1;
}

int main(){
	config_t conf = {0};
	
	//try to parse CONFIG_FILE_NAME, abort on failure
	if(ini_parse(CONFIG_FILE_NAME, &config_handler, &conf))
		return 1;
	
	shiny(&conf);
	
	return 0;
}