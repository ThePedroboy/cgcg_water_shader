
#pragma once

#include <UT/UT_VectorTypes.h>
#include <UT/UT_Vector3.h>
#include <GU/GU_Detail.h>
#include "GerstnerWave.h"
#include "LocalGerstnerWave.h"
#include "CircularGerstnerWave.h"



namespace ParallelAdditionEvaluatorN
{
	namespace gw = GerstnerWaveN;

	struct ParallelAdditionEvaluator
	{
	public:
		ParallelAdditionEvaluator(const UT_ValArray<gw::GerstnerWaveArgs>& _args);

		void updateWavesArgs(const UT_ValArray<gw::GerstnerWaveArgs>& _args);

		// Once I figure out how to update point poss in parallel
		/*void modifyPoint(GU_Detail* gdp, GA_Offset ptoff, fpreal t);*/

		UT_Vector3F getPointPosAddition(UT_Vector3F pos, fpreal t);
		

	private:

		fpreal evaluateYPos(UT_Vector3F pos, fpreal t);
		fpreal evaluateXPos(UT_Vector3F pos, fpreal t);
		fpreal evaluateZPos(UT_Vector3F pos, fpreal t);
	
		UT_ValArray<gw::GerstnerWave> gerstnerWaves;
		UT_ValArray<gw::LocalGerstnerWave> localGerstnerWaves;
		UT_ValArray<gw::CircularGerstnerWave> circularGerstnerWaves;


	};
}