/*
 * Trajectory.h
 *
 *  Created on: 20 juin 2018
 *      Author: liehnfl
 */

#ifndef TRAJECTORY_H_
#define TRAJECTORY_H_

#include "Messages/messages.h"

#define MAX_TRAJ_LENGTH 20*60 //max= 20min

typedef struct{
	int nb_point;
	bool locked;
	int current_point;
	//Table of pointing messages
	uint8_t table[MAX_TRAJ_LENGTH][ANTENNA_MESSAGE_LENGTH];
}Trajectory;

inline void InitTraj(Trajectory* traj){
	traj->nb_point=-2;
	traj->current_point=-1;
	traj->locked=false;
	memset(traj->table,0,MAX_TRAJ_LENGTH);
}

#endif /* TRAJECTORY_H_ */
