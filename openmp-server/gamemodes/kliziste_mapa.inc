#if defined _kliziste_mapa_included
        #endinput
#endif
#define _kliziste_mapa_included

#define MAX_KLIZ_MAP_OBJ 9
static KlizMapObjIDs[MAX_KLIZ_MAP_OBJ] = { -1, ... };

stock Kliziste_CreateMapObjects()
{
        new object_world = -1, object_int = -1;
        KlizMapObjIDs[0] = CreateDynamicObject(16139, 1667.473022, -413.794647, 29.183851, 0.000000, 0.000000, 0.000000, object_world, object_int, -1, 300.00, 300.00);
        KlizMapObjIDs[1] = CreateDynamicObject(16139, 1712.683715, -413.794647, 29.183851, 0.000000, 0.000000, 0.000000, object_world, object_int, -1, 300.00, 300.00);
        KlizMapObjIDs[2] = CreateDynamicObject(16139, 1733.594116, -413.794647, 36.553852, 0.000000, 0.000000, 0.000000, object_world, object_int, -1, 300.00, 300.00);
        KlizMapObjIDs[3] = CreateDynamicObject(16139, 1710.193603, -411.564788, 34.563827, 0.000000, 0.000000, 0.000000, object_world, object_int, -1, 300.00, 300.00);
        KlizMapObjIDs[4] = CreateDynamicObject(16139, 1676.393554, -419.654632, 30.213829, 0.000000, 0.000000, 0.000000, object_world, object_int, -1, 300.00, 300.00);
        KlizMapObjIDs[5] = CreateDynamicObject(16139, 1664.564208, -423.414611, 28.283802, 0.000000, 0.000000, 0.000000, object_world, object_int, -1, 300.00, 300.00);
        KlizMapObjIDs[6] = CreateDynamicObject(16139, 1707.982910, -402.294647, 28.313810, 0.000000, 0.000000, -11.600002, object_world, object_int, -1, 300.00, 300.00);
        KlizMapObjIDs[7] = CreateDynamicObject(16139, 1732.643066, -403.354766, 37.763751, 0.000000, 0.000000, -11.600002, object_world, object_int, -1, 300.00, 300.00);
        KlizMapObjIDs[8] = CreateDynamicObject(16139, 1650.749267, -408.644470, 36.913757, 0.000000, 0.000000, 127.199958, object_world, object_int, -1, 300.00, 300.00);
        return 1;
}

stock Kliziste_DestroyMapObjects()
{
        for(new i = 0; i < MAX_KLIZ_MAP_OBJ; i++) {
                if(KlizMapObjIDs[i] != -1) {
                        DestroyDynamicObject(KlizMapObjIDs[i]);
                        KlizMapObjIDs[i] = -1;
                }
        }
        return 1;
}
