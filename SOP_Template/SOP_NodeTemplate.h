#ifndef _SOP_NodeTemplate
#define _SOP_NodeTemplate

#include <SOP/SOP_Node.h>
#include <OP/OP_Parameters.h>
#include <UT/UT_DSOVersion.h>
#include <type_traits>

#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <OP/OP_AutoLockInputs.h>
#include <GA/GA_Handle.h>
#include<GEO/GEO_AttributeHandle.h>
#include "GEO/GEO_Primitive.h" // needed for GA_FOR_ALL_GROU_PRIMITIVES otherwise the compiler is unaware of GA_Primitive <- GEO_Primitive  
#include <iostream>

#include <memory>
#include "SinWave.h"
#include "GerstnerWave.h"
#include "Names.h"
#include <string>


namespace SOP_Template
{
    using namespace std;
    namespace n = NamesN;

    constexpr fpreal wavelength = 10;
    constexpr fpreal amplitude = 10;
    constexpr fpreal speed = 1;
 

    class SOP_NodeTemplate : public SOP_Node
    {
        public:
            SOP_NodeTemplate(OP_Network *net, const char* name, OP_Operator *entry): SOP_Node(net, name, entry),
                isWave0Enabled(true),
                isWave1Enabled(true),
                isWave2Enabled(true),
                isWave3Enabled(true)
                
            {
                wave0 = make_unique<SinWaveN::SinWave>(wavelength, amplitude, speed, UT_Vector3F(0,1,0));
                wave1 = make_unique<SinWaveN::SinWave>(0, 0, 0, UT_Vector3F(0,0,0));
                wave2 = make_unique<GerstnerWaveN::GerstnerWave>(0, 0, 0, UT_Vector3F(0,0,0));
                wave3 = make_unique<GerstnerWaveN::GerstnerWave>(0, 0, 0, UT_Vector3F(0,0,0));
                mySopFlags.setManagesDataIDs(true);
            }

            ~SOP_NodeTemplate(){}

            static OP_Node	*Constructor(OP_Network  *net, const char *name,OP_Operator *entry);

            static PRM_Template	 TemplateList[];
        
        protected:
            OP_ERROR cookMySop(OP_Context &context) override;
            bool updateParmsFlags() override;

            
    private:

        bool isWave0Enabled = true;
        bool isWave1Enabled = true;
        bool isWave2Enabled = true;
        bool isWave3Enabled = true;
        unique_ptr<SinWaveN::SinWave> wave0 = nullptr;
        unique_ptr<SinWaveN::SinWave> wave1 = nullptr;
        unique_ptr<GerstnerWaveN::GerstnerWave> wave2 = nullptr;
        unique_ptr<GerstnerWaveN::GerstnerWave> wave3 = nullptr;

        void UpdateIsWaveEnabled(fpreal time)
        {
            isWave0Enabled = EVALINT(time, "wave_0_toggle");
            isWave1Enabled = EVALINT(time, "wave_1_toggle");
            isWave2Enabled = EVALINT(time, "wave_2_toggle");
            isWave3Enabled = EVALINT(time, "wave_3_toggle");
        }
        fpreal evaluateYPos(fpreal x, fpreal y, fpreal z, fpreal t)
        {
            fpreal retY = 0;
            if (isWave0Enabled && wave0 != nullptr)
                retY += wave0->getYPos(x, y, z, t);
            if (isWave1Enabled && wave1 != nullptr)
                retY += wave1->getYPos(x, y, z, t);
            if (isWave2Enabled && wave2 != nullptr)
                retY += wave2->getYPosAddition(x, y, z, t);
            if (isWave3Enabled && wave3 != nullptr)
                retY += wave3->getYPosAddition(x, y, z, t);
            return retY;
        }
        fpreal evaluateXPos(fpreal x, fpreal y, fpreal z, fpreal t)
        {
            fpreal retX = x;
            if (isWave2Enabled && wave2 != nullptr)
                retX += wave2->getXPosAddition(x, y, z, t);
            if (isWave3Enabled && wave3 != nullptr)
                retX += wave3->getXPosAddition(x, y, z, t);
      
            return retX;
        }
        fpreal evaluateZPos(fpreal x, fpreal y, fpreal z, fpreal t)
        {
            fpreal retZ = z;
            if (isWave2Enabled && wave2 != nullptr)
                retZ += wave2->getZPosAddition(x, y, z, t);
            if (isWave3Enabled && wave3 != nullptr)
                retZ += wave3->getZPosAddition(x, y, z, t);
  
            return retZ;
        }

        fpreal getMaxSteepness(fpreal t)
        {
            fpreal product = 0;
            if (isWave0Enabled && wave0 != nullptr)
            {
                fpreal freq = 2 / EVALFLOAT(t, "wave_0_wavelength");
                fpreal ampl = EVALFLOAT(t, "wave_0_amplitude");
                product += freq * ampl;
            }
            if (isWave1Enabled && wave1 != nullptr)
            {
                fpreal freq = 2 / EVALFLOAT(t, "wave_1_wavelength");
                fpreal ampl = EVALFLOAT(t, "wave_1_amplitude");
                product += freq * ampl;
            }
            if (isWave2Enabled && wave2 != nullptr)
            {
                fpreal freq = 2 / EVALFLOAT(t, "wave_2_wavelength");
                fpreal ampl = EVALFLOAT(t, "wave_2_amplitude");
                product += freq * ampl;
            }
            if (isWave3Enabled && wave3 != nullptr)
            {
                fpreal freq = 2 / EVALFLOAT(t, "wave_3_wavelength");
                fpreal ampl = EVALFLOAT(t, "wave_3_amplitude");
                product += freq * ampl;
            }
            return 1 / product;

        }

        void UpdateSinWave(fpreal t)
        {
            auto maxSteepness = getMaxSteepness(t);
            std::cout << maxSteepness << std::endl;

            if (isWave0Enabled && wave0 != nullptr)
                wave0->updateEquationArguments(
                    EVALFLOAT(t, "wave_0_wavelength"), 
                    EVALFLOAT(t, "wave_0_amplitude"), 
                    EVALFLOAT(t, "wave_0_speed"),
                    UT_Vector3F(EVALFLOATVEC(t, "wave_0_direction", 0), 0, EVALFLOATVEC(t, "wave_0_direction", 2)));

            if (isWave1Enabled && wave1 != nullptr)
                wave1->updateEquationArguments(
                    EVALFLOAT(t, "wave_1_wavelength"),
                    EVALFLOAT(t, "wave_1_amplitude"),
                    EVALFLOAT(t, "wave_1_speed"),
                    UT_Vector3F(EVALFLOATVEC(t, "wave_1_direction", 0), 0, EVALFLOATVEC(t, "wave_1_direction", 2)));

            if (isWave2Enabled && wave2 != nullptr)
                wave2->updateEquationArguments(
                    EVALFLOAT(t, "wave_2_wavelength"),
                    EVALFLOAT(t, "wave_2_amplitude"),
                    EVALFLOAT(t, "wave_2_speed"),
                    UT_Vector3F(EVALFLOATVEC(t, "wave_2_direction", 0), 0, EVALFLOATVEC(t, "wave_2_direction", 2)),
                    EVALFLOAT(t, "wave_2_steepness"),
                    maxSteepness);
            
            if (isWave3Enabled && wave3 != nullptr)
                wave3->updateEquationArguments(
                    EVALFLOAT(t, "wave_3_wavelength"),
                    EVALFLOAT(t, "wave_3_amplitude"),
                    EVALFLOAT(t, "wave_3_speed"),
                    UT_Vector3F(EVALFLOATVEC(t, "wave_3_direction", 0), 0, EVALFLOATVEC(t, "wave_3_direction", 2)),
                    EVALFLOAT(t, "wave_3_steepness"),
                    maxSteepness);
        }

        int EVALINT(fpreal t, UT_StringHolder name) const { return evalInt(name, 0, t); }
        fpreal EVALFLOAT(fpreal t, UT_StringHolder name) const { return evalFloat(name, 0, t); }
        fpreal EVALFLOATVEC(fpreal t, UT_StringHolder name, int vecPos) const { return evalFloat(name, vecPos, t); }


                                                  
    };
}

#endif