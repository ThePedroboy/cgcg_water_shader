#include "ParallelAdditionEvaluator.h"

using namespace ParallelAdditionEvaluatorN;

ParallelAdditionEvaluator::ParallelAdditionEvaluator
(
	const UT_ValArray<gw::GerstnerWaveArgs>& _args
) :
	gerstnerWaves(_args.size())
{
	for (const auto& args : _args)
	{
		gerstnerWaves.append(gw::GerstnerWave(args));
	}
}

void ParallelAdditionEvaluator::updateWavesArgs(const UT_ValArray<gw::GerstnerWaveArgs>& _args)
{
	if (_args.size() != gerstnerWaves.size())
	{
		std::cout << "Array Size Mismatch" << std::endl;
		return;
	}
	for (std::size_t i = 0; i < _args.size(); i++)
	{
		gerstnerWaves[i].updateEquationArguments(_args[i]);
	}
}

UT_Vector3F ParallelAdditionEvaluator::getPointPosAddition(UT_Vector3F pos, fpreal t)
{
	return UT_Vector3F(
		evaluateXPos(pos, t),
		evaluateYPos(pos, t),
		evaluateZPos(pos, t)
	);
}

fpreal ParallelAdditionEvaluator::evaluateYPos(UT_Vector3F pos, fpreal t) 
{

	fpreal retY = 0;
	for (auto& wave : gerstnerWaves)
	{
		retY += wave.getYPosAddition(pos, t);
	}
	return retY;
}

fpreal ParallelAdditionEvaluator::evaluateXPos(UT_Vector3F pos, fpreal t) 
{
	fpreal retX = 0;
	for (auto& wave : gerstnerWaves)
	{
		retX += wave.getXPosAddition(pos, t);
	}
	return retX;
}
fpreal ParallelAdditionEvaluator::evaluateZPos(UT_Vector3F pos, fpreal t) 
{
	fpreal retZ = 0;
	for (auto& wave : gerstnerWaves)
	{
		retZ += wave.getZPosAddition(pos, t);
	}
	return retZ;
}
