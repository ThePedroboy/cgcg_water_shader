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
#include <UT/UT_ParallelUtil.h>
#include "GEO/GEO_Primitive.h" // needed for GA_FOR_ALL_GROU_PRIMITIVES otherwise the compiler is unaware of GA_Primitive <- GEO_Primitive  
#include <iostream>

#include <memory>

#include "GerstnerWave.h"
#include "Names.h"
#include <string>

namespace GerstnerWaveGeneratorN
{
    using namespace std;
    namespace n = NamesN;

    constexpr fpreal wavelength = 10;
    constexpr fpreal amplitude = 10;
    constexpr fpreal speed = 1;
 

    class GerstnerWaveGenerator : public SOP_Node
    {
        public:
            GerstnerWaveGenerator(OP_Network *net, const char* name, OP_Operator *entry): SOP_Node(net, name, entry),
                isWave0Enabled(true),
                isWave1Enabled(true),
                isWave2Enabled(true),
                isWave3Enabled(true)
                
            {
                mySopFlags.setManagesDataIDs(true);
            }

            ~GerstnerWaveGenerator(){}

            static OP_Node	*Constructor(OP_Network  *net, const char *name,OP_Operator *entry);

            static PRM_Template	 TemplateList[];

            int EVALINT(fpreal t, UT_StringHolder name) const { return evalInt(name, 0, t); }
            fpreal EVALFLOAT(fpreal t, UT_StringHolder name) const { return evalFloat(name, 0, t); }
            fpreal EVALFLOATVEC(fpreal t, UT_StringHolder name, int vecPos) const { return evalFloat(name, vecPos, t); }
        
        protected:
            OP_ERROR cookMySop(OP_Context &context) override;
            bool updateParmsFlags() override;

            
    private:

        bool isWave0Enabled = true;
        bool isWave1Enabled = true;
        bool isWave2Enabled = true;
        bool isWave3Enabled = true;

        fpreal getMaxSteepness(fpreal t) const
        {
            fpreal product = 0;
            if (isWave0Enabled)
            {
                fpreal freq = 2 / EVALFLOAT(t, "wave_0_wavelength");
                fpreal ampl = EVALFLOAT(t, "wave_0_amplitude");
                product += freq * ampl;
            }
            if (isWave1Enabled)
            {
                fpreal freq = 2 / EVALFLOAT(t, "wave_1_wavelength");
                fpreal ampl = EVALFLOAT(t, "wave_1_amplitude");
                product += freq * ampl;
            }
            if (isWave2Enabled)
            {
                fpreal freq = 2 / EVALFLOAT(t, "wave_2_wavelength");
                fpreal ampl = EVALFLOAT(t, "wave_2_amplitude");
                product += freq * ampl;
            }
            if (isWave3Enabled)
            {
                fpreal freq = 2 / EVALFLOAT(t, "wave_3_wavelength");
                fpreal ampl = EVALFLOAT(t, "wave_3_amplitude");
                product += freq * ampl;
            }
            return 1 / product;

        }

        void UpdateIsWaveEnabled(fpreal time)
        {
            isWave0Enabled = EVALINT(time, "wave_0_toggle");
            isWave1Enabled = EVALINT(time, "wave_1_toggle");
            isWave2Enabled = EVALINT(time, "wave_2_toggle");
            isWave3Enabled = EVALINT(time, "wave_3_toggle");
        }
        void GetPointPosAdditions(UT_Vector3Array& pointPosAdditionArr, fpreal t);
        void UpdatePointPos(UT_Vector3Array& pointPosAdditionArr);

        UT_ValArray<GerstnerWaveN::GerstnerWaveArgs> getGerstnerWaveArgs(fpreal t)
        {
            auto maxSteepness = getMaxSteepness(t);
            UT_ValArray<GerstnerWaveN::GerstnerWaveArgs> args(4);

            for (size_t i = 0; i < 4; i++)
            {
                bool enabled = false;
                switch (i) {
                case 0:
                    enabled = isWave0Enabled;
                    break;
                case 1:
                    enabled = isWave1Enabled;
                    break;
                case 2:
                    enabled = isWave2Enabled;
                    break;
                case 3:
                    enabled = isWave3Enabled;
                    break;
                default:
                    enabled = false;
                    break;


                }
                args.append(GerstnerWaveN::GerstnerWaveArgs(
                    enabled,
                    EVALFLOAT(t, "wave_"+ std::to_string(i) + "_wavelength"),
                    EVALFLOAT(t, "wave_" + std::to_string(i) + "_amplitude"),
                    EVALFLOAT(t, "wave_" + std::to_string(i) + "_speed"),
                    UT_Vector3F(EVALFLOATVEC(t, "wave_" + std::to_string(i) + "_direction", 0), 0, EVALFLOATVEC(t, "wave_" + std::to_string(i) + "_direction", 2)),
                    EVALFLOAT(t, "wave_" + std::to_string(i) + "_steepness"),
                    maxSteepness)
                );
            

            }
            return args;

        }
                                         
    };
}

#endif