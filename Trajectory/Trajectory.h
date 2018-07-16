/*
 * Trajectory.h
 *
 *  Created on: 20 juin 2018
 *      Author: liehnfl
 */

#ifndef TRAJECTORY_H_
#define TRAJECTORY_H_

#include "Messages/messages.h"

typedef struct{
	bool _locked;

	RTCDateTime _init_date;

	int _nb_point;
	int _current_point;
	//Table of pointing messages
	uint8_t _table[MAX_TRAJ_LENGTH][ANTENNA_MESSAGE_LENGTH];
}Trajectory;

static inline void trajInit(Trajectory* traj){
	traj->_nb_point=-2;
	traj->_current_point=-1;
	traj->_locked=false;
	rtcGetTime(&RTCD1,&(traj->_init_date));
	memset(traj->_table,0,MAX_TRAJ_LENGTH);
}

static inline void trajSetLength(Trajectory* traj,int length){
	traj->_nb_point=length;
	traj->_current_point=0;
}

static inline void trajAddPoint(Trajectory* traj,uint8_t*point){
	for(int i=0;i<ANTENNA_MESSAGE_LENGTH;i++)
		traj->_table[traj->_current_point][i]=point[i];
	traj->_current_point++;
}

static inline bool trajCheckCorrectLength(Trajectory* traj){
	return traj->_current_point==traj->_nb_point;
}

static inline void getTrajDate(Trajectory* traj,RTCDateTime* date){
	*date=(traj->_init_date);
}

static inline void setTrajDate(Trajectory* traj,RTCDateTime* date){
	traj->_init_date=*date;
}

static inline void trajGetNextTarget(Trajectory* traj,char*target){

	for(int i=0;i<ANTENNA_MESSAGE_LENGTH;i++)
		*(target+i)=traj->_table[traj->_current_point][i];
	traj->_current_point++;
}

static inline void trajPrepareTargeting(Trajectory* traj,char*first_target){
	traj->_current_point=0;
	trajGetNextTarget(traj,first_target);
}

#endif /* TRAJECTORY_H_ */
