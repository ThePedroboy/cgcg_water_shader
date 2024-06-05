

#include "GerstnerWaveGenerator.h"
#include <chrono>
#include "ParallelAdditionEvaluator.h"
#include <PRM/PRM_Include.h>
#include <PRM/PRM_SpareData.h>


using namespace GerstnerWaveGeneratorN;
using namespace std;


static PRM_Name gerstnerWaveNames[] =
{
    PRM_Name(GLOBALUNIFORMENABLE, "Enable Wave"),
    PRM_Name(STEEPNESS, "Steepness"),
    PRM_Name(AMPLITUDE, "Amplitude"),
    PRM_Name(WAVELENGTH, "wavelength"),
    PRM_Name(SPEED, "Speed"),
    PRM_Name(DIRECTION, "Direction"),
    PRM_Name(SEPARATOR, "Separator"),
    PRM_Name(NUMOFGLOBALUNIFORMWAVES,"Global Wave Patterns"),

    PRM_Name(0)
};

static PRM_Name gerstnerLocalWaveNames[] =
{
    PRM_Name(LOCALUNIFORMENABLE, "Enable local Wave"),
    PRM_Name(LOCALSSTARTPOS, "Start Position"),
    PRM_Name(LOCALSTEEPNESS, "Steepness"),
    PRM_Name(LOCALAMPLITUDE, "Amplitude"),
    PRM_Name(LOCALWAVELENGTH, "wavelength"),
    PRM_Name(LOCALSPEED, "Speed"),
    PRM_Name(LOCALDIRECTION, "Direction"),
    PRM_Name(LOCALSEPARATOR, "Separator"),
    PRM_Name(NUMOFLOCALUNIFORMWAVES,	"Local Wave Patterns"),

    PRM_Name(0)
};

static PRM_Name gerstnerCircularWaveNames[] =
{
    PRM_Name(GLOBALCIRCULARENABLE, "Enable Circular Wave"),
    PRM_Name(CIRCULARSSTARTPOS, "Start Position"),
    PRM_Name(CIRCULARSTEEPNESS, "Steepness"),
    PRM_Name(CIRCULARAMPLITUDE, "Amplitude"),
    PRM_Name(CIRCULARWAVELENGTH, "wavelength"),
    PRM_Name(CIRCULARSPEED, "Speed"),
    PRM_Name(CIRCULARDIRECTION, "Direction"),
    PRM_Name(CIRCULARSEPARATOR, "Separator"),
    PRM_Name(NUMOFGLOBALCIRCULARWAVES, "Circular Wave Patterns"),
    PRM_Name(ATTENUATIONEXPONENT, "Attenuation Exponent"),

    PRM_Name(0)
};


static PRM_Template	gerstnerWaveTemplate[] = {
    PRM_Template(PRM_TOGGLE, 1, &gerstnerWaveNames[0], PRMoneDefaults),
    PRM_Template(PRM_FLT_J, 1, &gerstnerWaveNames[1], PRMzeroDefaults, 0 , &PRMunitRange),
    PRM_Template(PRM_FLT_J, 1, &gerstnerWaveNames[2], PRMoneDefaults, 0 , &PRMrulerRange),
    PRM_Template(PRM_FLT_J, 1, &gerstnerWaveNames[3], PRMoneDefaults, 0 , &PRMrulerRange),
    PRM_Template(PRM_FLT_J, 1, &gerstnerWaveNames[4], PRMzeroDefaults, 0 , &PRMrulerRange),
    PRM_Template(PRM_DIRECTION, 2, &gerstnerWaveNames[5], PRMxaxisDefaults),
    PRM_Template(PRM_SEPARATOR, 1, &gerstnerWaveNames[6]),
    PRM_Template()
};

static PRM_Template	gerstnerSingleWaveTemplate[] = {
    PRM_Template(PRM_TOGGLE, 1, &gerstnerLocalWaveNames[0], PRMoneDefaults),
    PRM_Template(PRM_DIRECTION, 2, &gerstnerLocalWaveNames[1], PRMxaxisDefaults),
    PRM_Template(PRM_FLT_J, 1, &gerstnerLocalWaveNames[2], PRMzeroDefaults, 0 , &PRMunitRange),
    PRM_Template(PRM_FLT_J, 1, &gerstnerLocalWaveNames[3], PRMoneDefaults, 0 , &PRMrulerRange),
    PRM_Template(PRM_FLT_J, 1, &gerstnerLocalWaveNames[4], PRMoneDefaults, 0 , &PRMrulerRange),
    PRM_Template(PRM_FLT_J, 1, &gerstnerLocalWaveNames[5], PRMzeroDefaults, 0 , &PRMrulerRange),
    PRM_Template(PRM_DIRECTION, 2, &gerstnerLocalWaveNames[6], PRMxaxisDefaults),
    PRM_Template(PRM_SEPARATOR, 1, &gerstnerLocalWaveNames[7]),
    PRM_Template()
};

static PRM_Range PRMattenuationRange(PRM_RANGE_UI, 1.f, PRM_RANGE_UI, 2.f);

static PRM_Template	gerstnerCircularWaveTemplate[] = {
    PRM_Template(PRM_TOGGLE, 1, &gerstnerCircularWaveNames[0], PRMoneDefaults),
    PRM_Template(PRM_DIRECTION, 2, &gerstnerCircularWaveNames[1], PRMxaxisDefaults),
    PRM_Template(PRM_FLT_J, 1, &gerstnerCircularWaveNames[2], PRMzeroDefaults, 0 , &PRMunitRange),
    PRM_Template(PRM_FLT_J, 1, &gerstnerCircularWaveNames[3], PRMoneDefaults, 0 , &PRMrulerRange),
    PRM_Template(PRM_FLT_J, 1, &gerstnerCircularWaveNames[4], PRMoneDefaults, 0 , &PRMrulerRange),
    PRM_Template(PRM_FLT_J, 1, &gerstnerCircularWaveNames[5], PRMzeroDefaults, 0 , &PRMrulerRange),
    PRM_Template(PRM_FLT_J, 1, &gerstnerCircularWaveNames[9], PRMzeroDefaults, 0 , &PRMfrequencyRange),
    PRM_Template(PRM_DIRECTION, 2, &gerstnerCircularWaveNames[6], PRMxaxisDefaults),
    PRM_Template(PRM_SEPARATOR, 1, &gerstnerCircularWaveNames[7]),
    PRM_Template()
};


PRM_Template GerstnerWaveGenerator::TemplateList[] = {
    PRM_Template(PRM_MULTITYPE_LIST, gerstnerWaveTemplate, 2, &gerstnerWaveNames[7],
            PRMoneDefaults),
    PRM_Template(PRM_MULTITYPE_LIST, gerstnerSingleWaveTemplate, 2, &gerstnerLocalWaveNames[8], PRMoneDefaults),
    PRM_Template(PRM_MULTITYPE_LIST, gerstnerCircularWaveTemplate, 2, &gerstnerCircularWaveNames[8], PRMoneDefaults),
    PRM_Template()
};

void GerstnerWaveGenerator::updatePointPos(UT_Vector3Array& pointPosAdditionArr)
{
    GA_RWHandleV3 h(gdp->findAttribute(GA_ATTRIB_POINT, "P"));

    for (size_t i = 0; i < pointPosAdditionArr.size(); i++)
    {
        h.set(gdp->pointOffset(i), pointPosAdditionArr[i]);

    }
}

void GerstnerWaveGenerator::getPointPosAdditions(UT_Vector3Array& pointPosAdditionArr, fpreal t)
{

    GA_Offset start, end;
    start = gdp->pointOffset(0);
    end = gdp->pointOffset(gdp->getNumPoints());

    auto args = getGerstnerWaveArgs(t);

    for (auto& arg : args)
    {
        arg.updateLocalPosition(t);
    }

    UTparallelFor(UT_BlockedRange<GA_Offset>(start, end), [&](const UT_BlockedRange<GA_Offset>& range)
        {
            ParallelAdditionEvaluatorN::ParallelAdditionEvaluator pae(args);
            GA_ROHandleV3 h(gdp->findAttribute(GA_ATTRIB_POINT, "P"));
            if (!h.isValid()) {
                return;
            }
            for (GA_Offset ptoff = range.begin(); ptoff != range.end(); ++ptoff)
            {
                UT_Vector3F pos = h.get(ptoff);
                pos += pae.getPointPosAddition(pos, t); 
                pointPosAdditionArr[gdp->pointIndex(ptoff)] = pos;
            }
        });

}


OP_ERROR GerstnerWaveGenerator::cookMySop(OP_Context &context)
{
    OP_AutoLockInputs inputs(this); 
    if (inputs.lock(context) >= UT_ERROR_ABORT)
    {
        return error();
        
    }
    duplicateSource(0, context);

    fpreal t = context.getTime();
    //UpdateIsWaveEnabled(t);
  
    auto numOfPoints = gdp->getNumPoints();
    UT_Vector3Array pointPosAdditionArr(numOfPoints, numOfPoints);

    getPointPosAdditions(pointPosAdditionArr, t);
    updatePointPos(pointPosAdditionArr);
    //updateSingleGerstnerWave(t);

    gdp->getP()->bumpDataId();
    return error();
}

void newSopOperator(OP_OperatorTable* table)
{
    table->addOperator(
        new OP_Operator(
            "GerstnerWaveGenerator",
            "GerstnerWaveGenerator",
            GerstnerWaveGenerator::Constructor,
            GerstnerWaveGenerator::TemplateList,
            1,
            1
        )
    );
}
OP_Node *GerstnerWaveGenerator::Constructor(OP_Network *net,const char *name,OP_Operator *entry)
{
  
    return new GerstnerWaveGenerator(net, name, entry);
}

bool GerstnerWaveGenerator::updateParmsFlags()
{
    bool changed = false;

    int n = NUMGLOBUNIFORMWAVEPATTERNS();
    for (int i = 1; i <= n; i++)
    {
        changed |= enableParmInst(gerstnerWaveNames[1].getToken(), &i, GLOBALUNIFORMWAVEENABLEDSTATUS(i));
        changed |= enableParmInst(gerstnerWaveNames[2].getToken(), &i, GLOBALUNIFORMWAVEENABLEDSTATUS(i));
        changed |= enableParmInst(gerstnerWaveNames[3].getToken(), &i, GLOBALUNIFORMWAVEENABLEDSTATUS(i));
        changed |= enableParmInst(gerstnerWaveNames[4].getToken(), &i, GLOBALUNIFORMWAVEENABLEDSTATUS(i));
        changed |= enableParmInst(gerstnerWaveNames[5].getToken(), &i, GLOBALUNIFORMWAVEENABLEDSTATUS(i));
    }

    n = NUMLOCALUNIFORMWAVEPATTERNS();
    for (int i = 1; i <= n; i++)
    {
        changed |= enableParmInst(gerstnerLocalWaveNames[1].getToken(), &i, LOCALUNIFORMWAVEENABLEDSTATUS(i));
        changed |= enableParmInst(gerstnerLocalWaveNames[2].getToken(), &i, LOCALUNIFORMWAVEENABLEDSTATUS(i));
        changed |= enableParmInst(gerstnerLocalWaveNames[3].getToken(), &i, LOCALUNIFORMWAVEENABLEDSTATUS(i));
        changed |= enableParmInst(gerstnerLocalWaveNames[4].getToken(), &i, LOCALUNIFORMWAVEENABLEDSTATUS(i));
        changed |= enableParmInst(gerstnerLocalWaveNames[5].getToken(), &i, LOCALUNIFORMWAVEENABLEDSTATUS(i));
        changed |= enableParmInst(gerstnerLocalWaveNames[6].getToken(), &i, LOCALUNIFORMWAVEENABLEDSTATUS(i));
    }

    n = NUMGLOBALCIRCULARWAVEPATTERNS();
    for (int i = 1; i <= n; i++)
    {
        changed |= enableParmInst(gerstnerCircularWaveNames[1].getToken(), &i, GLOBALCIRCULARWAVEENABLEDSTATUS(i));
        changed |= enableParmInst(gerstnerCircularWaveNames[2].getToken(), &i, GLOBALCIRCULARWAVEENABLEDSTATUS(i));
        changed |= enableParmInst(gerstnerCircularWaveNames[3].getToken(), &i, GLOBALCIRCULARWAVEENABLEDSTATUS(i));
        changed |= enableParmInst(gerstnerCircularWaveNames[4].getToken(), &i, GLOBALCIRCULARWAVEENABLEDSTATUS(i));
        changed |= enableParmInst(gerstnerCircularWaveNames[5].getToken(), &i, GLOBALCIRCULARWAVEENABLEDSTATUS(i));
        changed |= enableParmInst(gerstnerCircularWaveNames[6].getToken(), &i, GLOBALCIRCULARWAVEENABLEDSTATUS(i));
        changed |= enableParmInst(gerstnerCircularWaveNames[9].getToken(), &i, GLOBALCIRCULARWAVEENABLEDSTATUS(i));
    }
    return changed;
}

void GerstnerWaveGenerator::updateSingleGerstnerWave(fpreal t)
{
    GA_RWHandleV3 h(gdp->findAttribute(GA_ATTRIB_POINT, "P"));
    if (!h.isValid()) {
        return;
    }

    GA_Offset ptoff;
    GA_FOR_ALL_PTOFF(gdp, ptoff)
    {
        UT_Vector3F pos = h.get(ptoff);
        UT_Vector3F newPos(pos);
        newPos[0] += localWave->getXPosAddition(pos, t);
        newPos[1] += localWave->getYPosAddition(pos, t);
        newPos[2] += localWave->getZPosAddition(pos, t);
        h.set(ptoff, newPos);

    }
}







