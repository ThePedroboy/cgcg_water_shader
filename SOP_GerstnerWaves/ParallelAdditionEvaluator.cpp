#include "ParallelAdditionEvaluator.h"

using namespace ParallelAdditionEvaluatorN;

ParallelAdditionEvaluator::ParallelAdditionEvaluator
(
	const UT_ValArray<gw::GerstnerWaveArgs>& _args
) :
	gerstnerWaves(_args.size()),
	localGerstnerWaves(_args.size())

{
	int globUni = 0;
	int locUni = 0;
	int globCirc = 0;

	for (const auto& args : _args)
	{
		if (args.flags & GerstnerWaveN::GERSTNER_WAVE_TYPE_LOCAL &&
			args.flags & GerstnerWaveN::GERSTNER_DIRECTION_TYPE_UNIFORM)
		{
			localGerstnerWaves.append(gw::LocalGerstnerWave(args));
			locUni++;
		}
	

	else if (args.flags & GerstnerWaveN::GERSTNER_WAVE_TYPE_GLOBAL &&
		args.flags & GerstnerWaveN::GERSTNER_DIRECTION_TYPE_UNIFORM)
		{
			gerstnerWaves.append(gw::GerstnerWave(args));
			globUni++;
		}

	else if (args.flags & GerstnerWaveN::GERSTNER_WAVE_TYPE_GLOBAL &&
		args.flags & GerstnerWaveN::GERSTNER_DIRECTION_TYPE_CIRCULAR)
		{
			circularGerstnerWaves.append(gw::CircularGerstnerWave(args));
			globCirc++;
		}
	}

	//std::cout << "gU " << globUni << " lU " << locUni << " gC " << globCirc << std::endl;


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
	for (auto& wave : localGerstnerWaves)
	{
		retY += wave.getYPosAddition(pos, t);
	}
	for (auto& wave : circularGerstnerWaves)
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
	for (auto& wave : localGerstnerWaves)
	{
		retX += wave.getXPosAddition(pos, t);
	}
	for (auto& wave : circularGerstnerWaves)
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
	for (auto& wave : localGerstnerWaves)
	{
		retZ += wave.getZPosAddition(pos, t);
	}
	for (auto& wave : circularGerstnerWaves)
	{
		retZ += wave.getZPosAddition(pos, t);
	}
	return retZ;
}
