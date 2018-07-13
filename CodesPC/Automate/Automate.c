/*
 * Automate.c
 *
 *  Created on: 13 juil. 2018
 *      Author: liehnfl
 */

#include "Automate.h"

void loadTargetsFromFile(File_Target* targets,int* tot,
		char* target_path,char*file_name){
	targets[*tot]=(File_Target){};
	//path
	strcpy(targets[*tot].file,target_path);
	strcat(targets[*tot].file,"/");
	strcat(targets[*tot].file,file_name);
	//extract time
	//compute prio
	printf("%s\n",targets[*tot].file);
	(*tot)++;
}

void loadTargetsFromDir(File_Target* targets,int* tot ,char* mission_path){
	//open directory
	DIR*mission_dir=opendir(mission_path);
	if(!mission_dir){
		fprintf(stderr,"Can't open dir:%s\n",mission_path);
		exit(0);
	}
	//read files
	struct dirent*target_file;
	while((target_file=readdir(mission_dir))!=NULL){
		if(target_file->d_type==DT_REG && target_file->d_name[0]!='.')
		loadTargetsFromFile(targets,tot,mission_path,target_file->d_name);
	}
	closedir(mission_dir);
}

void updateTargetsTime(File_Target* targets,int*tot){
	//reini targetes
	*tot=0;
	//open directory
	DIR* missions_dir=opendir(REP_SUIVI);
	if(!missions_dir){
		fprintf(stderr,"Can't open dir:%s\n",REP_SUIVI);
		exit(0);
	}
	//read mission directory
	struct dirent*mission_cont;
	while((mission_cont=readdir(missions_dir))!=NULL){
		if(mission_cont->d_type==DT_DIR && mission_cont->d_name[0]!='.'){
			char path[MAX_FILE_NAME_LENGTH];
			strcpy(path,REP_SUIVI);
			strcat(path,mission_cont->d_name);
			loadTargetsFromDir(targets,tot,path);
		}
	}
	closedir(missions_dir);
}

void main(void){
	//init variable
	File_Target possible_targets[MAX_TARGETS];
	int tot=0;

	//while(1){
		updateTargetsTime(possible_targets,&tot);

		//Check new suivi
			//DO IT
	//}

}
