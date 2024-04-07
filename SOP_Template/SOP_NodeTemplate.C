

#include "SOP_NodeTemplate.h"



using namespace SOP_Template;

OP_ERROR SOP_NodeTemplate::cookMySop(OP_Context &context)
{
    OP_AutoLockInputs inputs(this); 
    if (inputs.lock(context) >= UT_ERROR_ABORT)
    {
        return error();
        
    }
    duplicateSource(0, context);
    
    fpreal time = context.getTime();
    UpdateIsWaveEnabled(time);
    UpdateSinWave(time);
    GA_Offset ptoff;
    //     pos[1] = math.sin(pos[0] * 0.2 + pos[2] * 0.3 + f)
    std::cout << "Calculating wave pos" << std::endl;
    GA_FOR_ALL_PTOFF(gdp, ptoff)
    {
        
        GA_RWHandleV3 h(gdp, GA_ATTRIB_POINT, "P");
        UT_Vector3F pos = h.get(ptoff);
        auto waveYPos = evaluateYPos(pos[0], pos[1], pos[2], time);
        UT_Vector3F newPos(pos[0], waveYPos, pos[2]);
        h.set(ptoff, newPos);
        if (gdp->pointIndex(ptoff) == 0)
            std::cout <<  newPos << std::endl;
        
    }
    gdp->getP()->bumpDataId();
    return error();
}

void newSopOperator(OP_OperatorTable* table)
{
    table->addOperator(
        new OP_Operator(
            "SOP_Node_Template",
            "SOP_Node_Template",
            SOP_NodeTemplate::Constructor,
            SOP_NodeTemplate::TemplateList,
            1,
            1
        )
    );
}
OP_Node *SOP_NodeTemplate::Constructor(OP_Network *net,const char *name,OP_Operator *entry)
{
  
    return new SOP_NodeTemplate(net, name, entry);
}

bool SOP_NodeTemplate::updateParmsFlags()
{
    bool changed = false;
    changed |= enableParm(2, isWave0Enabled);
    changed |= enableParm(3, isWave0Enabled);
    changed |= enableParm(4, isWave0Enabled);
    changed |= enableParm(5, isWave0Enabled);

    changed |= enableParm(8, isWave1Enabled);
    changed |= enableParm(9, isWave1Enabled);
    changed |= enableParm(10, isWave1Enabled);
    changed |= enableParm(11, isWave1Enabled);

    changed |= enableParm(14, isWave2Enabled);
    changed |= enableParm(15, isWave2Enabled);
    changed |= enableParm(16, isWave2Enabled);
    changed |= enableParm(17, isWave2Enabled);

    changed |= enableParm(20, isWave3Enabled);
    changed |= enableParm(21, isWave3Enabled);
    changed |= enableParm(22, isWave3Enabled);
    changed |= enableParm(23, isWave3Enabled);
    return changed;
}

static PRM_Name names[] = {
    PRM_Name("wave_0_amplitude", "Amplitude"), // 0
    PRM_Name("wave_0_wavelength", "wavelength"),
    PRM_Name("wave_0_speed", "Speed"),
    PRM_Name("wave_0_direction", "Direction"),
    PRM_Name("wave_0", "Wave 0"),

    PRM_Name("wave_1_amplitude", "Amplitude"), //5 
    PRM_Name("wave_1_wavelength", "wavelength"),
    PRM_Name("wave_1_speed", "Speed"),
    PRM_Name("wave_1_direction", "Direction"),
    PRM_Name("wave_1", "Wave 1"),

    PRM_Name("wave_2_amplitude", "Amplitude"),//10
    PRM_Name("wave_2_wavelength", "wavelength"),
    PRM_Name("wave_2_speed", "Speed"),
    PRM_Name("wave_2_direction", "Direction"),
    PRM_Name("wave_2", "Wave 2"),

    PRM_Name("wave_3_amplitude", "Amplitude"),//15
    PRM_Name("wave_3_wavelength", "wavelength"),
    PRM_Name("wave_3_speed", "Speed"),
    PRM_Name("wave_3_direction", "Direction"),
    PRM_Name("wave_3", "Wave 3"),
    
    PRM_Name("wave_0_toggle", "Enable Wave 0"),
    PRM_Name("wave_1_toggle", "Enable Wave 1"),
    PRM_Name("wave_2_toggle", "Enable Wave 2"),
    PRM_Name("wave_3_toggle", "Enable Wave 3"),

};


PRM_Template SOP_NodeTemplate::TemplateList[] = {
    PRM_Template(PRM_TOGGLE, 1,&names[20]),//0
    PRM_Template(PRM_LABEL, 1, &names[4]),
    PRM_Template(PRM_FLT_J, 1, &names[0], PRMzeroDefaults, 0, &PRMfrequency10Range),
    PRM_Template(PRM_FLT_J, 1, &names[1], PRMzeroDefaults, 0, &PRMfrequency10Range),
    PRM_Template(PRM_FLT_J, 1, &names[2], PRMzeroDefaults, 0, &PRMfrequency10Range),
    PRM_Template(PRM_DIRECTION, 3, &names[3], PRMxaxisDefaults),


    PRM_Template(PRM_TOGGLE, 1,&names[21]), //6
    PRM_Template(PRM_LABEL, 1, &names[9]),
    PRM_Template(PRM_FLT_J, 1, &names[5], PRMzeroDefaults, 0, &PRMfrequency10Range),
    PRM_Template(PRM_FLT_J, 1, &names[6], PRMzeroDefaults, 0, &PRMfrequency10Range),
    PRM_Template(PRM_FLT_J, 1, &names[7], PRMzeroDefaults, 0, &PRMfrequency10Range),
    PRM_Template(PRM_DIRECTION, 3, &names[8], PRMxaxisDefaults),

    PRM_Template(PRM_TOGGLE, 1,&names[22]), // 12
    PRM_Template(PRM_LABEL, 1, &names[14]),
    PRM_Template(PRM_FLT_J, 1, &names[10], PRMzeroDefaults, 0, &PRMfrequency10Range),
    PRM_Template(PRM_FLT_J, 1, &names[11], PRMzeroDefaults, 0, &PRMfrequency10Range),
    PRM_Template(PRM_FLT_J, 1, &names[12], PRMzeroDefaults, 0, &PRMfrequency10Range),
    PRM_Template(PRM_DIRECTION, 3, &names[13], PRMxaxisDefaults),

    PRM_Template(PRM_TOGGLE, 1,&names[23]), //18
    PRM_Template(PRM_LABEL, 1, &names[19]),
    PRM_Template(PRM_FLT_J, 1, &names[15], PRMzeroDefaults, 0, &PRMfrequency10Range),
    PRM_Template(PRM_FLT_J, 1, &names[16], PRMzeroDefaults, 0, &PRMfrequency10Range),
    PRM_Template(PRM_FLT_J, 1, &names[17], PRMzeroDefaults, 0, &PRMfrequency10Range),
    PRM_Template(PRM_DIRECTION, 3, &names[18], PRMxaxisDefaults),
    PRM_Template()
};