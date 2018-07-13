/*
 * Automate.h
 *
 *  Created on: 13 juil. 2018
 *      Author: liehnfl
 */

#ifndef CODESPC_AUTOMATE_AUTOMATE_H_
#define CODESPC_AUTOMATE_AUTOMATE_H_

#include "../PcParameters.h"
#include <dirent.h>

#define REP_SUIVI "../../Fichiers_de_designations/"
#define REP_CORTEX_CONFIG ???? //TODO connect to cortex IHM

#define PREPARATION_TIME 15min

#define MAX_FILE_NAME_LENGTH 1024
#define MAX_TARGETS 1000
typedef struct {
	char file[MAX_FILE_NAME_LENGTH];
	uint64_t beginning;
	uint64_t ending;
	int priority_level;
}File_Target;

void loadTargetsFromFile(File_Target* targets,int* tot,
		char* target_path,char*file_name);
void loadTargetsFromDir(File_Target* targets,int* tot ,char* mission_path);
void updateTargetsTime(File_Target* targets,int*tot);
void main(void);

#endif /* CODESPC_AUTOMATE_AUTOMATE_H_ */
