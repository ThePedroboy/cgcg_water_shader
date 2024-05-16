

#include "GerstnerWaveGenerator.h"
#include <chrono>
#include "ParallelAdditionEvaluator.h"
#include <PRM/PRM_Include.h>
#include <PRM/PRM_SpareData.h>


using namespace GerstnerWaveGeneratorN;
using namespace std;


static PRM_Name gerstnerWaveNames[] =
{
    PRM_Name(NUMOFWAVES,	"Number of Waves"),
    PRM_Name(ENABLE, "Enable Wave #"),
    PRM_Name(STEEPNESS, "Steepness"),
    PRM_Name(AMPLITUDE, "Amplitude"),
    PRM_Name(WAVELENGTH, "wavelength"),
    PRM_Name(SPEED, "Speed"),
    PRM_Name(DIRECTION, "Direction"),
    PRM_Name(SEPARATOR, "Separator"),

    PRM_Name(0)
};


static PRM_Template	gerstnerWaveTemplate[] = {
    PRM_Template(PRM_TOGGLE, 1, &gerstnerWaveNames[1], PRMoneDefaults),
    PRM_Template(PRM_FLT_J, 1, &gerstnerWaveNames[2], PRMzeroDefaults, 0 , &PRMunitRange),
    PRM_Template(PRM_FLT_J, 1, &gerstnerWaveNames[3], PRMoneDefaults, 0 , &PRMrulerRange),
    PRM_Template(PRM_FLT_J, 1, &gerstnerWaveNames[4], PRMoneDefaults, 0 , &PRMrulerRange),
    PRM_Template(PRM_FLT_J, 1, &gerstnerWaveNames[5], PRMzeroDefaults, 0 , &PRMrulerRange),
    PRM_Template(PRM_DIRECTION, 2, &gerstnerWaveNames[6], PRMxaxisDefaults),
    PRM_Template(PRM_SEPARATOR, 1, &gerstnerWaveNames[7]),
    PRM_Template()
};


PRM_Template GerstnerWaveGenerator::TemplateList[] = {
    PRM_Template(PRM_MULTITYPE_LIST, gerstnerWaveTemplate, 2, &gerstnerWaveNames[0],
            PRMoneDefaults),
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
    updateSingleGerstnerWave(t);
    singleWave->updatePosition(t);
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

    int n = NUMWAVES();
    for (int i = 1; i <= n; i++)
    {
        changed |= enableParmInst(gerstnerWaveNames[2].getToken(), &i, WAVEENABLEDSTATUS(i));
        changed |= enableParmInst(gerstnerWaveNames[3].getToken(), &i, WAVEENABLEDSTATUS(i));
        changed |= enableParmInst(gerstnerWaveNames[4].getToken(), &i, WAVEENABLEDSTATUS(i));
        changed |= enableParmInst(gerstnerWaveNames[5].getToken(), &i, WAVEENABLEDSTATUS(i));
        changed |= enableParmInst(gerstnerWaveNames[6].getToken(), &i, WAVEENABLEDSTATUS(i));
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
        newPos[0] += singleWave->getXPosAddition(pos, t);
        newPos[1] += singleWave->getYPosAddition(pos, t);
        newPos[2] += singleWave->getZPosAddition(pos, t);
        h.set(ptoff, newPos);

    }
}







