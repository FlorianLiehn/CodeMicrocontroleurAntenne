/*
 * Automate.c
 *
 *  Created on: 13 juil. 2018
 *      Author: liehnfl
 */

#include "Automate.h"

void updateTargetsTime(File_Target* targets,int*tot,time_t current_time,int*next){
	//reini targets
	*tot=0;*next=-1;
	char missions[MAX_MISSION][MAX_FILE_NAME_LENGTH];
	int nb_mission=readMissions(missions);

	//read mission directory
	for(int i=0;i<nb_mission;i++){
			loadTargetsFromDir(targets,tot,missions[i],i,current_time,next);
	}
}

int readMissions(char missions[MAX_MISSION][MAX_FILE_NAME_LENGTH]){

	int nb_mission=0;
	char buf [MAX_FILE_NAME_LENGTH];
	FILE *ptr_file =fopen(PRIO_FILE, "r");
	while (fgets(buf,MAX_FILE_NAME_LENGTH, ptr_file)!=NULL){
			//Skip commented line
			if(buf[0]=='#')continue;

			strcpy(missions[nb_mission],REP_SUIVI);
			strcat(missions[nb_mission],buf);
			//replace \n by /
			missions[nb_mission][strlen(missions[nb_mission])-1]='/';
			printf("Mission %d:%s\n",nb_mission,missions[nb_mission]);
			nb_mission++;
	}
	return nb_mission;
}

void loadTargetsFromDir(File_Target* targets,int* tot ,char* mission_path,int prio,
						time_t current_time,int*next){
	//open directory
	DIR*mission_dir=opendir(mission_path);
	if(!mission_dir){
		fprintf(stderr,"Can't open dir:%s\n",mission_path);
		exit(0);
	}
	//read files
	struct dirent*target_file;
	while((target_file=readdir(mission_dir))!=NULL){
		if(target_file->d_type==DT_REG && target_file->d_name[0]!='.'){
			loadTargetsFromFile(targets,tot,mission_path,prio,
					target_file->d_name,current_time,next);
		}
	}
	closedir(mission_dir);
}

void loadTargetsFromFile(File_Target* targets,int* tot,char* target_path,int prio,
						char*file_name,time_t current_time,int*next){
	targets[*tot]=(File_Target){.priority_level=prio};
	//path
	strcpy(targets[*tot].file,target_path);
	strcat(targets[*tot].file,file_name);
	//extract time
	extractTimeFromFile(targets[*tot].file,
				&targets[*tot].beginning,
				&targets[*tot].ending);
	//check if near
	double waiting_time=difftime(targets[*tot].beginning,current_time);
	if( 0 < waiting_time && waiting_time < PREPARATION_TIME ){
		printf("NEW:%f\n",difftime(targets[*tot].beginning,current_time));
		//if no next or earlier
		if(*next<0 || difftime(targets[*tot ].beginning,
							   targets[*next].beginning) < 0 ){
			*next=*tot;
		}
	}
	(*tot)++;
}

void extractTimeFromFile(char * path,time_t* begin,time_t* end){
	//init
	*begin=*end=0;
	FILE *ptr_file =fopen(path, "r");

	//read (first = begin date)
	char buf[MAX_FILE_LINE_LENGTH];
	char last_uncommented_line[MAX_FILE_LINE_LENGTH];
	while (fgets(buf,MAX_FILE_LINE_LENGTH, ptr_file)!=NULL){
		//Skip commented line
		if(buf[0]=='#')continue;
		else if (*begin==0){
			extractTimeFromLine(buf,begin);
		}
		strcpy(last_uncommented_line,buf);
	}
	//read last uncommented line (end)
	extractTimeFromLine(last_uncommented_line,end);
}

void extractTimeFromLine(char * line,time_t* unix_time){
	*unix_time=0;
	//scan date
	struct tm date={};
	sscanf(line,"%d-%d-%d %d:%d:%d",
			&date.tm_year,&date.tm_mon,&date.tm_mday,
			&date.tm_hour,&date.tm_min,&date.tm_sec);
	//offset
	date.tm_year-=TM_STRUCT_YEAR_OFFSET;
	date.tm_mon-= TM_STRUCT_MONTH_OFFSET;
	//compute
	*unix_time=mktime(&date);
}

int checkTimeInTargetPeriod(time_t t,File_Target target){
	//check > beginning && < ending
	if(difftime(target.beginning,t)<PREPARATION_TIME &&
	   difftime(t,   target.ending)<PREPARATION_TIME)
		return 1;
	return 0;
}

int checkTargetsCollision(File_Target tar1,File_Target tar2){
	return  checkTimeInTargetPeriod(tar1.beginning,tar2) ||
			checkTimeInTargetPeriod(tar1.ending   ,tar2) ||
			checkTimeInTargetPeriod(tar2.beginning,tar1) ||
			checkTimeInTargetPeriod(tar2.ending   ,tar1);
}

void checkForNextTargeting(File_Target* targets,int tot,int* next){
	if(*next<0)return;
	for(int i=0;i<tot;i++){
		if(i!=*next && targets[i].priority_level<targets[*next].priority_level &&
		 checkTargetsCollision(targets[i],targets[*next]) ){
			//check the new date
			time_t now=mktime(gmtime(&(time_t){time(NULL)}));
			if(difftime(targets[i].beginning,now)<PREPARATION_TIME){
				//re-check with the new targets
				printf("Collision %d VS %d\n",*next,i);
				*next=i;
				return checkForNextTargeting(targets, tot,next);
			}
			//too early
			*next=-1;
			return;
		}
	}
}

void main(void){
	//init variable
	File_Target possibles_targets[MAX_TARGETS];
	int tot,next;

	//while(1){
		//get current date & update targets
		time_t now=mktime(gmtime(&(time_t){time(NULL)}));
		updateTargetsTime(possibles_targets,&tot,now,&next);
		//Check if next targeting is correct
		checkForNextTargeting(possibles_targets,tot,&next);
		if(next>=0){
	    	printf("Next :%s\n",possibles_targets[next].file);
	    	//do the targeting
	    	targetingFromFile(possibles_targets[next].file);
	    	//sleep(=> unactivate until end of targetting)
			now=mktime(gmtime(&(time_t){time(NULL)}));
			sleep( MAX(difftime(possibles_targets[next].ending,now),0) );
		}
		else{
	    	printf("No next targets\n");
	    	//sleep(CHECKING_LOOP_TIME);
		}
	//}
	printf("Exit\n");
}
