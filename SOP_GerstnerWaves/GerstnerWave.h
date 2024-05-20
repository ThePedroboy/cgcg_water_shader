#pragma once
#include <UT/UT_Vector3.h>
#include <iostream>
#include <optional>
namespace GerstnerWaveN {
	constexpr long long GERSTNER_WAVE_TYPE_GLOBAL = 1;
	constexpr long long GERSTNER_WAVE_TYPE_LOCAL = 1 << 1;
	constexpr long long GERSTNER_DIRECTION_TYPE_CIRCULAR = 1 << 2;
	constexpr long long GERSTNER_DIRECTION_TYPE_UNIFORM = 1 << 3;

	using namespace std;
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
			direction(UT_Vector2F(0, 0)),
			steepness(0),
			position(UT_Vector2F(0,0)),
			initPosition(UT_Vector2F(0,0)),
			flags(GERSTNER_WAVE_TYPE_GLOBAL | GERSTNER_DIRECTION_TYPE_UNIFORM)
		{

		}
		GerstnerWaveArgs(
			
			bool _enabled,
			fpreal _waveLength,
			fpreal _amplitude,
			fpreal _speed,
			const UT_Vector2F& _direction,
			fpreal _steepness,
			fpreal _maxSteepness,
			const long long & _flags,
			const UT_Vector2F& _position = UT_Vector2F(0,0)):
			
			enabled(_enabled),
			waveLength(_waveLength),
			frequency(2 / _waveLength),
			amplitude(_amplitude),
			speed(_speed),
			phaseConstant(_speed * 2 / _waveLength),
			direction(_direction),
			steepness(_steepness* _maxSteepness),
			flags(_flags)
			

		{
			direction.normalize();
			position = flags & GERSTNER_WAVE_TYPE_LOCAL ? _position +_direction * _waveLength / 2: _position;
			initPosition = position;
		}
		
		bool enabled;
		fpreal waveLength;
		fpreal frequency;
		fpreal amplitude;
		fpreal phaseConstant;
		fpreal speed;
		UT_Vector2F direction;
		fpreal steepness;
		UT_Vector2F position;
		UT_Vector2F initPosition;
		long long flags;

		void updateLocalPosition(fpreal t)
		{
			if (flags & GERSTNER_WAVE_TYPE_LOCAL)
			{
				position = initPosition - direction * t * speed;
				cout << position << endl;
			}
		}
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


		virtual fpreal getYPosAddition(const UT_Vector3F& pos, fpreal t)
		{
			if (!args.enabled)
				return 0;

			return args.amplitude * sin
			(
				args.frequency *
				dot
				(
					args.direction, UT_Vector2F(pos[0], pos[2])
				)
				+ t * args.phaseConstant);
		}
		virtual fpreal getXPosAddition(const UT_Vector3F& pos, fpreal t)
		{
			if (!args.enabled)
				return 0;

			return args.steepness * args.amplitude * args.direction[0] *
				cos(
					dot(args.frequency * args.direction, UT_Vector2F(pos[0], pos[2]))
					+ t * args.phaseConstant
				);

		}
		virtual fpreal getZPosAddition(const UT_Vector3F& pos, fpreal t)
		{
			if (!args.enabled)
				return 0;

			return args.steepness * args.amplitude * args.direction[1] *
				cos(
					dot(args.frequency * args.direction, UT_Vector2F(pos[0], pos[2]))
					+ t * args.phaseConstant
				);

		}
		virtual UT_Vector2 getPos() {
			return args.position;
		}

		GerstnerWaveArgs args;

	};
}