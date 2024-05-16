#pragma once
#pragma once
#include <UT/UT_Vector3.h>
#include <iostream>
#include "GerstnerWave.h"
namespace GerstnerWaveN {
	using namespace std;

	struct SingleGerstnerWave: private GerstnerWave {
	public:
		SingleGerstnerWave() :GerstnerWave() {}
		SingleGerstnerWave(const GerstnerWaveArgs& _args) : GerstnerWave(_args) {}
		
		fpreal getYPosAddition(const UT_Vector3F &pos, fpreal t) override
		{
			fpreal yAddition = GerstnerWave::getYPosAddition(pos, t);
			if (!shouldPointBeEvaluated(pos, yAddition))
				return 0;

			return GerstnerWave::getYPosAddition(pos, t);
		}
		fpreal getXPosAddition(const UT_Vector3F& pos, fpreal t) override
		{
			//fpreal yAddition = GerstnerWave::getYPosAddition(pos, t);
			if (!shouldPointBeEvaluated(pos))
				return 0;

			return GerstnerWave::getXPosAddition(pos, t);

		}
		fpreal getZPosAddition(const UT_Vector3F& pos, fpreal t) override
		{
			//fpreal yAddition = GerstnerWave::getYPosAddition(pos, t);
			if (!shouldPointBeEvaluated(pos))
				return 0;

			return GerstnerWave::getZPosAddition(pos, t);

		}

		void updatePosition(fpreal t)
		{
			args.position = args.initPosition - args.direction * t * args.speed;
		}

		UT_Vector2 getPos() override {
			return args.position;
		}

	private:

		bool shouldPointBeEvaluated(const UT_Vector3F& pos, fpreal yAddition = 0) const
		{
			auto distance = getDistanceFromPointToWave(UT_Vector2(pos.x(), pos.z()));
			if (distance > args.waveLength || yAddition < 0 )
			{
				//cout << "Point '" << pos << "' too far from the wave : " << distance << endl;
				return false;
			}
			return true;
		}

		float getDistanceFromPointToWave(const UT_Vector2& point) const {
			// Vector from pivot to the point in question
			UT_Vector2 pointToPivot = point - args.position;
			

			// Perpendicular vector to the line direction
			/*UT_Vector2 perpDirection = UT_Vector2(args.direction.y(), args.direction.x());
			perpDirection.normalize();*/

			// Calculate the dot product of pointToPivot and the normalized perpendicular vector
			float projection = pointToPivot.dot(args.direction);

			// Normalize the projection by the length of the perpendicular direction to get the distance
			float distance = fabs(projection) / args.direction.length();

			return distance;
		}

	

	};
}

