/*
 * Automate.h
 *
 *  Created on: 13 juil. 2018
 *      Author: liehnfl
 */

#ifndef CODESPC_AUTOMATE_AUTOMATE_H_
#define CODESPC_AUTOMATE_AUTOMATE_H_

#include "../PcParameters.h"
#include <time.h>
#include <dirent.h>
#include <inttypes.h>

//#define ENABLE_COM

#define REP_SUIVI "./Fichiers_de_designations/"
#define REP_CORTEX_CONFIG "./Fichier_de_configuration/"
#define PRIO_FILE REP_SUIVI"Prio.txt"

#define PREPARATION_TIME 15*60	//15 min
#define CHECKING_LOOP_TIME 10	//10 s

#define MAX_FILE_NAME_LENGTH 1024
#define MAX_MISSION 50
#define MAX_TARGETS 1000
#define TM_STRUCT_YEAR_OFFSET 1900
#define TM_STRUCT_MONTH_OFFSET 1
typedef struct {
	char mission_name[MAX_FILE_NAME_LENGTH];
	char file[MAX_FILE_NAME_LENGTH];
	time_t beginning;
	time_t ending;
	int priority_level;
}File_Target;

//update the targets
void updateTargetsTime(File_Target* targets,int*tot,time_t current_time,int*next);
int readMissions(char missions[MAX_MISSION][MAX_FILE_NAME_LENGTH]);
void loadTargetsFromDir(File_Target* targets,int* tot ,char* mission_path,int prio,
						time_t current_time,int*next);
void loadTargetsFromFile(File_Target* targets,int* tot,char* target_path,int prio,
						char*file_name,time_t current_time,int*next);
void extractTimeFromFile(char * path,time_t* begin,time_t* end);
void extractTimeFromLine(char * line,time_t* unix_time);

//Check target & targeting
int checkTimeInTargetPeriod(time_t t,File_Target target);
int checkTargetsCollision(File_Target tar1,File_Target tar2);
void checkForNextTargeting(File_Target* targets,int tot,int* next);

#endif /* CODESPC_AUTOMATE_AUTOMATE_H_ */
