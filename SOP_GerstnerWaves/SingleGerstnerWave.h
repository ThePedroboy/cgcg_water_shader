#pragma once
#pragma once
#include <UT/UT_Vector3.h>
#include <iostream>
#include "GerstnerWave.h"
namespace GerstnerWaveN {
	using namespace std;

	struct SingleGerstnerWave {
	public:
		SingleGerstnerWave() :
			args()
		{}

		SingleGerstnerWave(
			const GerstnerWaveArgs& _args) :
			args(_args)

		{}


		void updateEquationArguments(
			const GerstnerWaveArgs& _args)
		{
			args = _args;
		}


		fpreal getYPosAddition(const UT_Vector3F &pos, fpreal t) const
		{

			if (!shouldPointBeEvaluated(pos))
				return 0;

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
			if (!shouldPointBeEvaluated(pos))
				return 0;

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
			if (!shouldPointBeEvaluated(pos))
				return 0;

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

		bool shouldPointBeEvaluated(const UT_Vector3F& pos) const
		{
			auto distance = getDistanceFromPointToWave(UT_Vector2(pos.x(), pos.z()));
			if (distance > args.waveLength)
			{
				cout << "Point '" << pos << "' too far from the wave : " << distance << endl;
				return false;
			}
			return true;
		}

		float getDistanceFromPointToWave(const UT_Vector2& point) const {
			// Vector from pivot to the point in question
			UT_Vector2 pointToPivot = point - args.position;

			// Perpendicular vector to the line direction
			UT_Vector2 perpDirection = UT_Vector2(args.direction.y(), -args.direction.x());

			// Calculate the dot product of pointToPivot and the normalized perpendicular vector
			float projection = pointToPivot.dot(perpDirection);

			// Normalize the projection by the length of the perpendicular direction to get the distance
			float distance = fabs(projection) / perpDirection.length();

			return distance;
		}

	

	};
}

