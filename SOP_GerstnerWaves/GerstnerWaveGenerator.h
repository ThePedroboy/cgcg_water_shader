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
#include "LocalGerstnerWave.h"
#include "Names.h"
#include <string>

namespace GerstnerWaveGeneratorN
{
    using namespace std;
    namespace n = NamesN;

    constexpr fpreal wavelength = 10;
    constexpr fpreal amplitude = 10;
    constexpr fpreal speed = 1;
    constexpr char GLOBALUNIFORMENABLE[] = "enable#";
    constexpr char WAVELENGTH[] = "wavelength#";
    constexpr char AMPLITUDE[] = "amplitude#";
    constexpr char SPEED[] = "speed#";
    constexpr char DIRECTION[] = "direction#";
    constexpr char STEEPNESS[] = "steepness#";
    constexpr char SEPARATOR[] = "separator#";
    constexpr char NUMOFGLOBALUNIFORMWAVES[] = "numberOfWaves";


    constexpr char LOCALUNIFORMENABLE[] = "localEnable#";
    constexpr char LOCALWAVELENGTH[] = "localWavelength#";
    constexpr char LOCALAMPLITUDE[] = "localAmplitude#";
    constexpr char LOCALSPEED[] = "localSpeed#";
    constexpr char LOCALDIRECTION[] = "localDirection#";
    constexpr char LOCALSTEEPNESS[] = "localSteepness#";
    constexpr char LOCALSSTARTPOS[] = "localStartPos#";
    constexpr char LOCALSEPARATOR[] = "localSeparator#";
    constexpr char NUMOFLOCALUNIFORMWAVES[] = "numberOfSingleWaves";

    constexpr char GLOBALCIRCULARENABLE[] = "circularEnable#";
    constexpr char CIRCULARWAVELENGTH[] = "circularWavelength#";
    constexpr char CIRCULARAMPLITUDE[] = "circularAmplitude#";
    constexpr char CIRCULARSPEED[] = "circularSpeed#";
    constexpr char CIRCULARDIRECTION[] = "circularDirection#";
    constexpr char CIRCULARSTEEPNESS[] = "circularSteepness#";
    constexpr char CIRCULARSSTARTPOS[] = "circularStartPos#";
    constexpr char CIRCULARSEPARATOR[] = "circularSeparator#";
    constexpr char NUMOFGLOBALCIRCULARWAVES[] = "numberOfCircularWaves";
    constexpr char ATTENUATIONEXPONENT[] = "attenuationExponent";
 

    class GerstnerWaveGenerator : public SOP_Node
    {
        public:
            GerstnerWaveGenerator(OP_Network *net, const char* name, OP_Operator *entry): SOP_Node(net, name, entry)
                
            {
                mySopFlags.setManagesDataIDs(true);
                /*GerstnerWaveN::GerstnerWaveArgs args(true, true, 8, 2, 7, UT_Vector2F(0,-1), 1, 1);
                localWave = make_unique<GerstnerWaveN::SingleGerstnerWave>(args);*/
            }

            ~GerstnerWaveGenerator(){}

            static OP_Node	*Constructor(OP_Network  *net, const char *name,OP_Operator *entry);

            static PRM_Template	 TemplateList[];

            int	GLOBALUNIFORMWAVEENABLEDSTATUS(int waveIdx) const {return evalIntInst(GLOBALUNIFORMENABLE, &waveIdx, 0, 0.00f);}
            int	LOCALUNIFORMWAVEENABLEDSTATUS(int waveIdx) const {return evalIntInst(LOCALUNIFORMENABLE, &waveIdx, 0, 0.00f);}
            int	GLOBALCIRCULARWAVEENABLEDSTATUS(int waveIdx) const {return evalIntInst(GLOBALCIRCULARENABLE, &waveIdx, 0, 0.00f);}
 
            int NUMGLOBUNIFORMWAVEPATTERNS() const { return evalInt(NUMOFGLOBALUNIFORMWAVES, 0, 0.0f); }
            int NUMLOCALUNIFORMWAVEPATTERNS() const { return evalInt(NUMOFLOCALUNIFORMWAVES, 0, 0.0f); }
            int NUMGLOBALCIRCULARWAVEPATTERNS() const { return evalInt(NUMOFGLOBALCIRCULARWAVES, 0, 0.0f); }

            fpreal EVALFLOATMULTIPARM(fpreal t, UT_StringHolder name, int &waveIdx) const { return evalFloatInst(name,&waveIdx, 0, t); }
            fpreal EVALFLOATVECMULTIPARM(fpreal t, UT_StringHolder name, int& waveIdx, int vecPos) const { return evalFloatInst(name,&waveIdx, vecPos, t); }

            
        protected:
            OP_ERROR cookMySop(OP_Context &context) override;
            bool updateParmsFlags() override;

            
    private:
        unique_ptr<GerstnerWaveN::LocalGerstnerWave> localWave;

        fpreal getMaxSteepness(fpreal t) const
        {
            fpreal product = 0;

            for (int i = 1; i <= NUMGLOBUNIFORMWAVEPATTERNS(); i++)
            {
                if (GLOBALUNIFORMWAVEENABLEDSTATUS(i))
                {
                    fpreal freq = 2 / EVALFLOATMULTIPARM(t, WAVELENGTH,i);
                    fpreal ampl = EVALFLOATMULTIPARM(t, AMPLITUDE, i);
                    product += freq * ampl;
                }
            }

            for (int i = 1; i <= NUMLOCALUNIFORMWAVEPATTERNS(); i++)
            {
                if (LOCALUNIFORMWAVEENABLEDSTATUS(i))
                {
                    fpreal freq = 2 / EVALFLOATMULTIPARM(t, LOCALWAVELENGTH, i);
                    fpreal ampl = EVALFLOATMULTIPARM(t, LOCALAMPLITUDE, i);
                    product += freq * ampl;
                }
            }

            for (int i = 1; i <= NUMGLOBALCIRCULARWAVEPATTERNS(); i++)
            {
                if (GLOBALCIRCULARWAVEENABLEDSTATUS(i))
                {
                    fpreal freq = 2 / EVALFLOATMULTIPARM(t, CIRCULARWAVELENGTH, i);
                    fpreal ampl = EVALFLOATMULTIPARM(t, CIRCULARAMPLITUDE, i);
                    product += freq * ampl;
                }
            }
            return 1 / product;

        }

        void getPointPosAdditions(UT_Vector3Array& pointPosAdditionArr, fpreal t);
        void updatePointPos(UT_Vector3Array& pointPosAdditionArr);
        void updateSingleGerstnerWave(fpreal t);

        UT_ValArray<GerstnerWaveN::GerstnerWaveArgs> getGerstnerWaveArgs(fpreal t)
        {
            auto maxSteepness = getMaxSteepness(t);
       
            UT_ValArray<GerstnerWaveN::GerstnerWaveArgs> args(NUMGLOBUNIFORMWAVEPATTERNS());
            for (int i = 1; i <= NUMGLOBUNIFORMWAVEPATTERNS(); i++)
            {
                bool enabled = GLOBALUNIFORMWAVEENABLEDSTATUS(i) != 0;
                args.append(GerstnerWaveN::GerstnerWaveArgs(
                    enabled,
                    EVALFLOATMULTIPARM(t, WAVELENGTH, i),
                    EVALFLOATMULTIPARM(t, AMPLITUDE,i),
                    EVALFLOATMULTIPARM(t, SPEED, i),
                    UT_Vector2F(EVALFLOATVECMULTIPARM(t, DIRECTION, i, 0), EVALFLOATVECMULTIPARM(t, DIRECTION, i,1)),
                    EVALFLOATMULTIPARM(t, STEEPNESS,i),
                    maxSteepness,
                    GerstnerWaveN::GERSTNER_WAVE_TYPE_GLOBAL | GerstnerWaveN::GERSTNER_DIRECTION_TYPE_UNIFORM)
                );


            }
            for (int i = 1; i <= NUMLOCALUNIFORMWAVEPATTERNS(); i++)
            {
                bool enabled = LOCALUNIFORMWAVEENABLEDSTATUS(i) != 0;
                args.append(GerstnerWaveN::GerstnerWaveArgs(
                    enabled,
                    EVALFLOATMULTIPARM(t, LOCALWAVELENGTH, i),
                    EVALFLOATMULTIPARM(t, LOCALAMPLITUDE, i),
                    EVALFLOATMULTIPARM(t, LOCALSPEED, i),
                    UT_Vector2F(EVALFLOATVECMULTIPARM(t, LOCALDIRECTION, i, 0), EVALFLOATVECMULTIPARM(t, LOCALDIRECTION, i, 1)),
                    EVALFLOATMULTIPARM(t, LOCALSTEEPNESS, i),
                    maxSteepness,
                    GerstnerWaveN::GERSTNER_WAVE_TYPE_LOCAL | GerstnerWaveN::GERSTNER_DIRECTION_TYPE_UNIFORM,
                    UT_Vector2F(EVALFLOATVECMULTIPARM(t, LOCALSSTARTPOS, i, 0), EVALFLOATVECMULTIPARM(t, LOCALSSTARTPOS, i, 1))
                    )
                );


            }

            for (int i = 1; i <= NUMGLOBALCIRCULARWAVEPATTERNS(); i++)
            {
                bool enabled = GLOBALCIRCULARWAVEENABLEDSTATUS(i) != 0;
                args.append(GerstnerWaveN::GerstnerWaveArgs(
                    enabled,
                    EVALFLOATMULTIPARM(t, CIRCULARWAVELENGTH, i),
                    EVALFLOATMULTIPARM(t, CIRCULARAMPLITUDE, i),
                    EVALFLOATMULTIPARM(t, CIRCULARSPEED, i),
                    UT_Vector2F(EVALFLOATVECMULTIPARM(t, CIRCULARDIRECTION, i, 0), EVALFLOATVECMULTIPARM(t, CIRCULARDIRECTION, i, 1)),
                    EVALFLOATMULTIPARM(t, CIRCULARSTEEPNESS, i),
                    maxSteepness,
                    GerstnerWaveN::GERSTNER_WAVE_TYPE_GLOBAL | GerstnerWaveN::GERSTNER_DIRECTION_TYPE_CIRCULAR,
                    UT_Vector2F(EVALFLOATVECMULTIPARM(t, CIRCULARSSTARTPOS, i, 0), EVALFLOATVECMULTIPARM(t, CIRCULARSSTARTPOS, i, 1)),
                    EVALFLOATMULTIPARM(t, ATTENUATIONEXPONENT, i)
                )
                );


            }
            return args;
        }
                                         
    };
}

#endif