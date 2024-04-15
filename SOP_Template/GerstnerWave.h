#pragma once
#include <UT/UT_Vector3.h>
#include <iostream>
namespace GerstnerWaveN {
	class GerstnerWave {
	public:

		GerstnerWave(
			fpreal _waveLength,
			fpreal _amplitude,
			fpreal _speed,
			const UT_Vector3F& _direction) :
			waveLength(_waveLength),
			frequency(2 / _waveLength),
			amplitude(_amplitude),
			speed(_speed),
			phaseConstant(_speed * 2 / _waveLength),
			direction(_direction)

		{
			direction.normalize();
		}

		void updateEquationArguments(
			fpreal _waveLength,
			fpreal _amplitude,
			fpreal _speed,
			const UT_Vector3F& _direction,
			fpreal _steepness, // 0 -> 1
			fpreal _maxSteepness)
		{
			waveLength = _waveLength;
			frequency = 2 / _waveLength;
			amplitude = _amplitude;
			speed = _speed;
			phaseConstant = _speed * 2 / _waveLength;
			direction = _direction;
			direction.normalize();
			steepness = _maxSteepness * _steepness;
			std::cout << "steepness: " << steepness << std::endl;


		}

		fpreal getYPosAddition(fpreal x, fpreal y, fpreal z, fpreal t)
		{
			
			return amplitude * sin
			(
				frequency *
				dot
				(
					UT_Vector2F(direction[0], direction[2]), UT_Vector2F(x, z)
				) 
				+ t * phaseConstant);
		}
		fpreal getXPosAddition(fpreal x, fpreal y, fpreal z, fpreal t)
		{
			return steepness * amplitude * direction[0] *
				cos(
					dot(frequency * UT_Vector2F(direction[0], direction[2]), UT_Vector2F(x, z))
					+ phaseConstant * t
				);
			
		}
		fpreal getZPosAddition(fpreal x, fpreal y, fpreal z, fpreal t)
		{
			return steepness * amplitude * direction[2] *
				cos(
					dot(frequency* UT_Vector2F(direction[0], direction[2]), UT_Vector2F(x, z))
					+ phaseConstant * t
				);
		
		}

	private:
		fpreal waveLength;
		fpreal frequency;
		fpreal amplitude;
		fpreal phaseConstant;
		fpreal speed;
		UT_Vector3F direction;
		fpreal steepness;
	};
}