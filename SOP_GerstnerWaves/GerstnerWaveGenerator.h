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
#include "SingleGerstnerWave.h"
#include "Names.h"
#include <string>

namespace GerstnerWaveGeneratorN
{
    using namespace std;
    namespace n = NamesN;

    constexpr fpreal wavelength = 10;
    constexpr fpreal amplitude = 10;
    constexpr fpreal speed = 1;
    constexpr char ENABLE[] = "enable#";
    constexpr char WAVELENGTH[] = "wavelength#";
    constexpr char AMPLITUDE[] = "amplitude#";
    constexpr char SPEED[] = "speed#";
    constexpr char DIRECTION[] = "direction#";
    constexpr char STEEPNESS[] = "steepness#";
    constexpr char SEPARATOR[] = "separator#";
    constexpr char NUMOFWAVES[] = "numberOfWaves";
 

    class GerstnerWaveGenerator : public SOP_Node
    {
        public:
            GerstnerWaveGenerator(OP_Network *net, const char* name, OP_Operator *entry): SOP_Node(net, name, entry)
                
            {
                mySopFlags.setManagesDataIDs(true);
                GerstnerWaveN::GerstnerWaveArgs args(true, 8, 2, 7, UT_Vector3F(-1, 0, 1), 0.6, 2 / 8 * 2);
                singleWave = make_unique<GerstnerWaveN::SingleGerstnerWave>(args);
            }

            ~GerstnerWaveGenerator(){}

            static OP_Node	*Constructor(OP_Network  *net, const char *name,OP_Operator *entry);

            static PRM_Template	 TemplateList[];

            int	WAVEENABLEDSTATUS(int waveIdx) const {return evalIntInst(ENABLE, &waveIdx, 0, 0.00f);}
            int NUMWAVES() const { return evalInt(NUMOFWAVES, 0, 0.0f); }

            fpreal EVALFLOATMULTIPARM(fpreal t, UT_StringHolder name, int &waveIdx) const { return evalFloatInst(name,&waveIdx, 0, t); }
            fpreal EVALFLOATVECMULTIPARM(fpreal t, UT_StringHolder name, int& waveIdx, int vecPos) const { return evalFloatInst(name,&waveIdx, vecPos, t); }

            
        protected:
            OP_ERROR cookMySop(OP_Context &context) override;
            bool updateParmsFlags() override;

            
    private:
        unique_ptr<GerstnerWaveN::SingleGerstnerWave> singleWave;

        fpreal getMaxSteepness(fpreal t) const
        {
            fpreal product = 0;

            for (int i = 1; i <= NUMWAVES(); i++)
            {
                if (WAVEENABLEDSTATUS(i))
                {
                    fpreal freq = 2 / EVALFLOATMULTIPARM(t, "wavelength#",i);
                    fpreal ampl = EVALFLOATMULTIPARM(t, "amplitude#", i);
                    product += freq * ampl;
                }
            }
            /*if (isWave0Enabled)
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
            }*/
            return 1 / product;

        }

        void GetPointPosAdditions(UT_Vector3Array& pointPosAdditionArr, fpreal t);
        void UpdatePointPos(UT_Vector3Array& pointPosAdditionArr);

        UT_ValArray<GerstnerWaveN::GerstnerWaveArgs> getGerstnerWaveArgs(fpreal t)
        {
            auto maxSteepness = getMaxSteepness(t);
            UT_ValArray<GerstnerWaveN::GerstnerWaveArgs> args(NUMWAVES());
            for (int i = 1; i <= NUMWAVES(); i++)
            {
                bool enabled = WAVEENABLEDSTATUS(i) != 0;
                args.append(GerstnerWaveN::GerstnerWaveArgs(
                    enabled,
                    EVALFLOATMULTIPARM(t, WAVELENGTH, i),
                    EVALFLOATMULTIPARM(t, AMPLITUDE,i),
                    EVALFLOATMULTIPARM(t, SPEED, i),
                    UT_Vector3F(EVALFLOATVECMULTIPARM(t, DIRECTION, i, 0), 0, EVALFLOATVECMULTIPARM(t, DIRECTION, i,2)),
                    EVALFLOATMULTIPARM(t, STEEPNESS,i),
                    maxSteepness)
                );


            }
            return args;
        }
                                         
    };
}

#endif