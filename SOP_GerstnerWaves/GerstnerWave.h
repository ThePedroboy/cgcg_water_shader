#pragma once
#include <UT/UT_Vector3.h>
#include <iostream>
namespace GerstnerWaveN {

	struct GerstnerWaveArgs
	{
	public:
		GerstnerWaveArgs() :
			enabled(false),
			waveLength(0),
			frequency(0),
			amplitude(0),
			speed(0),
			phaseConstant(0),
			direction(UT_Vector3F(0, 0, 0)),
			steepness(0)
		{

		}
		GerstnerWaveArgs(
			bool _enabled,
			fpreal _waveLength,
			fpreal _amplitude,
			fpreal _speed,
			const UT_Vector3F& _direction,
			fpreal _steepness,
			fpreal _maxSteepness) :
			enabled(_enabled),
			waveLength(_waveLength),
			frequency(2 / _waveLength),
			amplitude(_amplitude),
			speed(_speed),
			phaseConstant(_speed * 2 / _waveLength),
			direction(_direction),
			steepness(_steepness* _maxSteepness)

		{
			direction.normalize();
		}
		bool enabled;
		fpreal waveLength;
		fpreal frequency;
		fpreal amplitude;
		fpreal phaseConstant;
		fpreal speed;
		UT_Vector3F direction;
		fpreal steepness;
	};
	struct GerstnerWave {
	public:
		GerstnerWave() :
			args()
		{}

		GerstnerWave(
			const GerstnerWaveArgs& _args) :
			args(_args)

		{}


		void updateEquationArguments(
			const GerstnerWaveArgs& _args)
		{
			args = _args;
		}


		fpreal getYPosAddition(UT_Vector3F pos, fpreal t) const
		{
			if (!args.enabled)
				return 0;

			return args.amplitude * sin
			(
				args.frequency *
				dot
				(
					UT_Vector2F(args.direction[0], args.direction[2]), UT_Vector2F(pos[0], pos[2])
				)
				+ t * args.phaseConstant);
		}
		fpreal getXPosAddition(UT_Vector3F pos, fpreal t) const
		{
			if (!args.enabled)
				return 0;

			return args.steepness * args.amplitude * args.direction[0] *
				cos(
					dot(args.frequency * UT_Vector2F(args.direction[0], args.direction[2]), UT_Vector2F(pos[0], pos[2]))
					+ t * args.phaseConstant
				);

		}
		fpreal getZPosAddition(UT_Vector3F pos, fpreal t) const
		{
			if (!args.enabled)
				return 0;

			return args.steepness * args.amplitude * args.direction[2] *
				cos(
					dot(args.frequency * UT_Vector2F(args.direction[0], args.direction[2]), UT_Vector2F(pos[0], pos[2]))
					+ t * args.phaseConstant
				);

		}

	private:
		GerstnerWaveArgs args;

	};
}