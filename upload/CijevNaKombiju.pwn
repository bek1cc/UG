//Vehicle Exported with Texture Studio By: [uL]Pottus, Crayder, Svyatoy, encoder, devhub/////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <a_samp>
#include <streamer>

new carvid_0;

public OnFilterScriptInit()
{ 
    new tmpobjid;

    carvid_0 = CreateVehicle(578,-66.043,-1110.865,1.078,338.160,-1,-1,-1,0);
    SetVehicleVirtualWorld(carvid_1, 0);
    LinkVehicleToInterior(carvid_1, 0);



    tmpobjid = CreateDynamicObject(3675,0.0,0.0,-1000.0,0.0,0.0,0.0,0,0,-1,300.0,300.0);
    SetDynamicObjectMaterial(tmpobjid, 0, 14600, "paperchase_bits2", "ab_plasticBin", 0);
    AttachDynamicObjectToVehicle(tmpobjid, carvid_0, -0.496, 0.649, 1.315, -1.600, 78.999, 88.300);

} 

public OnFilterScriptExit()
{ 
    DestroyVehicle(carvid_0);
} 

public OnVehicleSpawn(vehicleid)
{ 
    if(vehicleid == carvid_0)
    {
    }
} 
