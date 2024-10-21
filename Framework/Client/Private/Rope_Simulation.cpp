#include "stdafx.h"
#include "Rope_Simulation.h"
#include "Simulation.h"

CRope_Simulation::CRope_Simulation(
	_int _iNum_Masses,
	_float _M
	)
	: CSimulation(_iNum_Masses, _M)
{

}

CRope_Simulation::~CRope_Simulation()
{
}

void CRope_Simulation::Free()
{
}
