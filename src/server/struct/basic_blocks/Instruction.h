/**
 * Instruction.h
 * Purpose: Information related to 'Instruction'
 * @author Mintae Kim
 */

#pragma once

#include <string>
#include <sstream>
#include <iostream>

#define MAX_INST_PARAM 8

#define HLT_PARAM 0
#define CAL_PARAM 0
#define MOV_PARAM 4
#define PID_PARAM 8
#define GRB_PARAM 2
#define RLZ_PARAM 2
#define DCN_PARAM 0

enum class InstructionType
{
	ERR,
	HLT,
	CAL,
	MOV,
	PID,
	GRB,
	RLZ,
	DCN
};

class Instruction
{
public:
	InstructionType instType;
	float param[MAX_INST_PARAM];
	int paramCnt = 0;

	Instruction();
	Instruction(InstructionType instType, float* param);
	Instruction(Instruction const& inst);

	std::string toString();

	// Operator Overloading
	Instruction& operator =(const Instruction& inst);
};