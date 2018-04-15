//
// Created by wings on 18-4-11.
//
#ifndef OSLABS_MYOSLIB_SEM_H
#define OSLABS_MYOSLIB_SEM_H

#include <sys/types.h>
#include <linux/sem.h>


void P(int semid,int index);
void V(int semid,int index);



#endif //OSLABS_MYOSLIB_SEM_H
