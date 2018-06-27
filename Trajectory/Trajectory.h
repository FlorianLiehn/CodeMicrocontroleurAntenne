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
	int _nb_point;
	bool _locked;
	int _current_point;
	RTCDateTime _init_date;
	//Table of pointing messages
	uint8_t _table[MAX_TRAJ_LENGTH][ANTENNA_MESSAGE_LENGTH];
}Trajectory;

static inline void TrajInit(Trajectory* traj){
	traj->_nb_point=-2;
	traj->_current_point=-1;
	traj->_locked=false;
	rtcGetTime(&RTCD1,&(traj->_init_date));
	memset(traj->_table,0,MAX_TRAJ_LENGTH);
}

static inline void TrajSetLength(Trajectory* traj,int length){
	traj->_nb_point=length;
	traj->_current_point=0;
}

static inline void TrajAddPoint(Trajectory* traj,uint8_t*point){
	for(int i=0;i<ANTENNA_MESSAGE_LENGTH;i++)
		traj->_table[traj->_current_point][i]=point[i];
	traj->_current_point++;
}

static inline bool CheckTrajCorrectLength(Trajectory* traj){
	return traj->_current_point==traj->_nb_point;
}

static inline void getTrajDate(Trajectory* traj,RTCDateTime* date){
	*date=(traj->_init_date);
}
static inline void setTrajDate(Trajectory* traj,RTCDateTime* date){
	traj->_init_date=*date;
}

#endif /* TRAJECTORY_H_ */
