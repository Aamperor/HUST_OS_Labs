#ifndef SENSE_H
#define SENSE_H

#define TEMPORARY 0
#define HUMIDITY 1
#define PHOTOVOLTAIC 2

typedef nx_struct SenseMsg
{
    nx_uint16_t nodeid;
    nx_uint16_t kind;
    nx_uint16_t data;
}SenseMsg;

enum 
{
  AM_TEST_SCESE_MSG = 0x89,
};

#endif