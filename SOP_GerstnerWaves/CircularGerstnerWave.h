#pragma once
#include <UT/UT_Vector3.h>
#include <iostream>
#include "GerstnerWave.h"
#include "limits"

namespace GerstnerWaveN {
	using namespace std;
	constexpr fpreal FREQ_ATTENUATION_LIMIT = 50.0f;
	struct CircularGerstnerWave: public GerstnerWave {
	public:
		CircularGerstnerWave() :GerstnerWave() {}
		CircularGerstnerWave(const GerstnerWaveArgs& _args) : GerstnerWave(_args) {}
		
		fpreal getYPosAddition(const UT_Vector3F &pos, fpreal t) override
		{
			if (!args.enabled)
				return 0;

			UT_Vector2F direction = args.initPosition - UT_Vector2F(pos[0], pos[2]);
			if (direction.equalZero())
				return 0;
			direction.normalize();
			return getFreqBasedOnDistanceFromInitPos(pos) * sin
			(
				args.frequency *
				dot
				(
					direction, UT_Vector2F(pos[0], pos[2]) - args.initPosition
				)
				+ t * args.phaseConstant);
		}
		fpreal getXPosAddition(const UT_Vector3F& pos, fpreal t) override
		{
	
			if (!args.enabled)
				return 0;

			UT_Vector2F direction = args.initPosition - UT_Vector2F(pos[0], pos[2]);
			direction.normalize();
			return args.steepness * args.amplitude * direction[0] *
				cos(
					dot(args.frequency * direction, UT_Vector2F(pos[0], pos[2]) - args.initPosition)
					+ t * args.phaseConstant
				);

		}
		fpreal getZPosAddition(const UT_Vector3F& pos, fpreal t) override
		{

			if (!args.enabled)
				return 0;

			UT_Vector2F direction = args.initPosition - UT_Vector2F(pos[0], pos[2]);
			direction.normalize();

			return args.steepness * args.amplitude* direction[1] *
				cos(
					dot(args.frequency * direction, UT_Vector2F(pos[0], pos[2]) - args.initPosition)
					+ t * args.phaseConstant
				);

		}

		UT_Vector2 getPos() override {
			return args.position;
		}

		fpreal getFreqBasedOnDistanceFromInitPos(const UT_Vector3F& pos) const
		{
			if (!args.attenuationExponent.has_value())
				return args.amplitude;

			fpreal dist2 = (UT_Vector2F(pos.x(), pos.z()) - args.initPosition).length2();
			if ( dist2 <= numeric_limits<float>::epsilon())
				return args.amplitude;

			fpreal distance = sqrt(dist2);
			
			return args.amplitude / ( 1 +  easeInExpo(distance));

		}
		inline fpreal easeInExpo(const fpreal &val) const
		{
			return val == 0 ? 0 : powf(args.attenuationExponent.value(), val);
		}
	

	};
}

