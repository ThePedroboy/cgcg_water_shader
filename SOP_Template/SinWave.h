#pragma once
#include <UT/UT_Vector3.h>
namespace SinWaveN {
	class SinWave {
	public:

		SinWave(
			fpreal _waveLength,
			fpreal _amplitude,
			fpreal _speed,
			const UT_Vector3F & _direction):
			waveLength(_waveLength),
			frequency(2/_waveLength),
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
			const UT_Vector3F& _direction)
		{
			waveLength = _waveLength;
			frequency = 2 / _waveLength;
			amplitude = _amplitude;
			speed = _speed;
			phaseConstant = _speed * 2 / _waveLength;
			direction = _direction;
			direction.normalize();

		}

		fpreal getYPos(fpreal x, fpreal y, fpreal z, fpreal t)
		{
			return amplitude * sin(dot(direction, UT_Vector3F(x, y, z) * frequency + t * phaseConstant));
		}

	private:
		fpreal waveLength;
		fpreal frequency;
		fpreal amplitude;
		fpreal phaseConstant;
		fpreal speed;
		UT_Vector3F direction;
	};
}