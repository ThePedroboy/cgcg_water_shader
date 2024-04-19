

#include "GerstnerWaveGenerator.h"
#include <chrono>
#include "ParallelAdditionEvaluator.h"


using namespace GerstnerWaveGeneratorN;
using namespace std;

// How to paralelize this shit? 
// It shoudl be easy -> the array access should not produce data racing as each position is accessed only once
// But it blows up Houdini anyway...
void GerstnerWaveGenerator::UpdatePointPos(UT_Vector3Array& pointPosAdditionArr)
{
    GA_RWHandleV3 h(gdp->findAttribute(GA_ATTRIB_POINT, "P"));

    for (size_t i = 0; i < pointPosAdditionArr.size(); i++)
    {
        h.set(gdp->pointOffset(i), pointPosAdditionArr[i]);

    }

}

void GerstnerWaveGenerator::GetPointPosAdditions(UT_Vector3Array& pointPosAdditionArr, fpreal t)
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
    UpdateIsWaveEnabled(t);

    auto numOfPoints = gdp->getNumPoints();
    UT_Vector3Array pointPosAdditionArr(numOfPoints, numOfPoints);

    GetPointPosAdditions(pointPosAdditionArr, t);
    UpdatePointPos(pointPosAdditionArr);


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
    changed |= enableParm(2, isWave0Enabled);
    changed |= enableParm(3, isWave0Enabled);
    changed |= enableParm(4, isWave0Enabled);
    changed |= enableParm(5, isWave0Enabled);
    changed |= enableParm(6, isWave0Enabled);

    changed |= enableParm(9, isWave1Enabled);
    changed |= enableParm(10, isWave1Enabled);
    changed |= enableParm(11, isWave1Enabled);
    changed |= enableParm(12, isWave1Enabled);
    changed |= enableParm(13, isWave1Enabled);

 
    changed |= enableParm(16, isWave2Enabled);
    changed |= enableParm(17, isWave2Enabled);
    changed |= enableParm(18, isWave2Enabled);
    changed |= enableParm(19, isWave2Enabled);
    changed |= enableParm(20, isWave2Enabled);

 
    changed |= enableParm(23, isWave3Enabled);
    changed |= enableParm(24, isWave3Enabled);
    changed |= enableParm(25, isWave3Enabled);
    changed |= enableParm(26, isWave3Enabled);
    changed |= enableParm(27, isWave3Enabled);
    return changed;
}

static PRM_Name names[] = {
    PRM_Name("wave_0_steepness", "steepness"), // 0
    PRM_Name("wave_0_amplitude", "Amplitude"), 
    PRM_Name("wave_0_wavelength", "wavelength"),
    PRM_Name("wave_0_speed", "Speed"),
    PRM_Name("wave_0_direction", "Direction"),
    PRM_Name("wave_0", "Wave 0"),

    PRM_Name("wave_1_steepness", "steepness"), //6
    PRM_Name("wave_1_amplitude", "Amplitude"),  
    PRM_Name("wave_1_wavelength", "wavelength"),
    PRM_Name("wave_1_speed", "Speed"),
    PRM_Name("wave_1_direction", "Direction"),
    PRM_Name("wave_1", "Wave 1"),

    PRM_Name("wave_2_steepness", "steepness"),//12
    PRM_Name("wave_2_amplitude", "Amplitude"),
    PRM_Name("wave_2_wavelength", "wavelength"),
    PRM_Name("wave_2_speed", "Speed"),
    PRM_Name("wave_2_direction", "Direction"),
    PRM_Name("wave_2", "Wave 2"),

    PRM_Name("wave_3_steepness", "steepness"),//18
    PRM_Name("wave_3_amplitude", "Amplitude"),
    PRM_Name("wave_3_wavelength", "wavelength"),
    PRM_Name("wave_3_speed", "Speed"),
    PRM_Name("wave_3_direction", "Direction"),
    PRM_Name("wave_3", "Wave 3"),
    
    PRM_Name("wave_0_toggle", "Enable Wave 0"),//24
    PRM_Name("wave_1_toggle", "Enable Wave 1"),
    PRM_Name("wave_2_toggle", "Enable Wave 2"),
    PRM_Name("wave_3_toggle", "Enable Wave 3"),

};

//PRMunitRange
PRM_Template GerstnerWaveGenerator::TemplateList[] = {
    PRM_Template(PRM_TOGGLE, 1,&names[24]),//0
    PRM_Template(PRM_LABEL, 1, &names[5]),
    PRM_Template(PRM_FLT_J, 1, &names[0], PRMzeroDefaults, 0 , &PRMunitRange),
    PRM_Template(PRM_FLT_J, 1, &names[1], PRMzeroDefaults, 0, &PRMrulerRange),
    PRM_Template(PRM_FLT_J, 1, &names[2], PRMzeroDefaults, 0, &PRMrulerRange),
    PRM_Template(PRM_FLT_J, 1, &names[3], PRMzeroDefaults, 0, &PRMrulerRange),
    PRM_Template(PRM_DIRECTION, 3, &names[4], PRMxaxisDefaults),


    PRM_Template(PRM_TOGGLE, 1,&names[25]), //7
    PRM_Template(PRM_LABEL, 1, &names[11]),
    PRM_Template(PRM_FLT_J, 1, &names[6], PRMzeroDefaults, 0 , &PRMunitRange),
    PRM_Template(PRM_FLT_J, 1, &names[7], PRMzeroDefaults, 0, &PRMrulerRange),
    PRM_Template(PRM_FLT_J, 1, &names[8], PRMzeroDefaults, 0, &PRMrulerRange),
    PRM_Template(PRM_FLT_J, 1, &names[9], PRMzeroDefaults, 0, &PRMrulerRange),
    PRM_Template(PRM_DIRECTION, 3, &names[10], PRMxaxisDefaults),

    PRM_Template(PRM_TOGGLE, 1,&names[26]), //14
    PRM_Template(PRM_LABEL, 1, &names[17]),
    PRM_Template(PRM_FLT_J, 1, &names[12], PRMzeroDefaults, 0 , &PRMunitRange),
    PRM_Template(PRM_FLT_J, 1, &names[13], PRMzeroDefaults, 0, &PRMrulerRange),
    PRM_Template(PRM_FLT_J, 1, &names[14], PRMzeroDefaults, 0, &PRMrulerRange),
    PRM_Template(PRM_FLT_J, 1, &names[15], PRMzeroDefaults, 0, &PRMrulerRange),
    PRM_Template(PRM_DIRECTION, 3, &names[16], PRMxaxisDefaults),

    PRM_Template(PRM_TOGGLE, 1,&names[27]), //21
    PRM_Template(PRM_LABEL, 1, &names[23]),
    PRM_Template(PRM_FLT_J, 1, &names[18], PRMzeroDefaults, 0 , &PRMunitRange),
    PRM_Template(PRM_FLT_J, 1, &names[19], PRMzeroDefaults, 0, &PRMrulerRange),
    PRM_Template(PRM_FLT_J, 1, &names[20], PRMzeroDefaults, 0, &PRMrulerRange),
    PRM_Template(PRM_FLT_J, 1, &names[21], PRMzeroDefaults, 0, &PRMrulerRange),
    PRM_Template(PRM_DIRECTION, 3, &names[22], PRMxaxisDefaults),
    PRM_Template()
};