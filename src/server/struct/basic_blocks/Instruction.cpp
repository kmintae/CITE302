/**
 * Instruction.cpp
 * Purpose: Information related to 'Instruction'
 * @author Mintae Kim
 */

#include "Instruction.h"

Instruction::Instruction()
{
	instType = InstructionType::ERR;
	paramCnt = 0;
}
Instruction::Instruction(InstructionType instType, float* param)
{
	this->instType = instType;
	switch (instType)
	{
	// Server -> Client 
	case InstructionType::HLT:
		paramCnt = 0;
		break;
	case InstructionType::MOV:
		paramCnt = 4; // (cur_pos_x, cur_pos_y, cur_dir_x, cur_dir_y)
		break;
	case InstructionType::DCN:
		paramCnt = 0;
		break;

	// Server <-> Client
	case InstructionType::GRB:
		paramCnt = 2; // (r, z)
		break;
	case InstructionType::RLZ:
		paramCnt = 2; // (r, z)
		break;
	case InstructionType::CAL:
		paramCnt = 0;
		break;
	}
	case InstructionType::PID:
		paramCnt = 8; // curPos, curDir, keypointPos, keypointDir
		break;
	}

	// Parameter Deep Copy
	for (int i = 0; i < paramCnt; i++) {
		(this->param)[i] = param[i];
		paramCnt++;
	}
}
Instruction::Instruction(Instruction const& inst)
{
	paramCnt = inst.paramCnt;
	instType = inst.instType;

	// Parameter Deep Copy
	for (int i = 0; i < paramCnt; i++) {
		param[i] = inst.param[i];
	}
}
Instruction& Instruction::operator =(const Instruction& inst)
{
	paramCnt = inst.paramCnt;
	instType = inst.instType;

	// Parameter Deep Copy
	for (int i = 0; i < paramCnt; i++) {
		param[i] = inst.param[i];
	}
}

std::string Instruction::toString()
{
	std::string instRaw;
	switch (instType)
	{
	// Server -> Client 
	case InstructionType::HLT:
		instRaw = "HLT";
		break;
	case InstructionType::MOV:
		instRaw = "MOV";
		break;
	case InstructionType::DCN:
		instRaw = "DCN";
		break;
	
	// Server <-> Client
	case InstructionType::GRB:
		instRaw = "GRB";
		break;
	case InstructionType::RLZ:
		instRaw = "RLZ";
		break;
	case InstructionType::CAL:
		instRaw = "CAL";
		break;
	case InstructionType::PID:
		instRaw = "PID";
		break;
	}

	for (int i = 0; i < paramCnt; i++) instRaw.append(" ").append(std::to_string(param[i]));
	return instRaw;
}