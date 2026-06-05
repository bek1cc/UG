//Vehicle Exported with Texture Studio By: [uL]Pottus, Crayder, Svyatoy, encoder, devhub/////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <a_samp>
#include <streamer>

new carvid;

public OnFilterScriptInit()
{ 
    new tmpobjid;

    carvid = CreateVehicle(530,-40.665,-1131.826,1.078,74.145,0,0,-1,0);

    SetVehicleVirtualWorld(carvid_1, 0);
    LinkVehicleToInterior(carvid_1, 0);
    tmpobjid = CreateDynamicObject(1685,0.0,0.0,-1000.0,0.0,0.0,0.0,0,0,-1,300.0,300.0);
    SetDynamicObjectMaterial(tmpobjid, 0, 1631, "landjump", "plasticdrum1_128", 0);
    AttachDynamicObjectToVehicle(tmpobjid, carvid, 0.000, 0.870, 0.769, 0.000, 0.000, 0.000);
} 

public OnFilterScriptExit()
{ 
    DestroyVehicle(carvid);
} 

public OnVehicleSpawn(vehicleid)
{ 
    if(vehicleid == carvid)
    { 
    } 
} 
