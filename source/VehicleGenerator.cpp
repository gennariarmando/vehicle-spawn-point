#include "plugin.h"
#include "CPed.h"
#include "CModelInfo.h"
#include "CVehicle.h"
#include "CStreaming.h"
#include "CWorld.h"
#include "CCarCtrl.h"
#include "CTheZones.h"
#include "eVehicleModel.h"

#ifdef GTASA
#include "CMonsterTruck.h"
#include "CQuadBike.h"
#include "CBmx.h"
#include "CTrailer.h"
#endif

#include "CPlane.h"
#include "CBike.h"
#include "CBoat.h"
#include "CTrain.h"
#include "CTheScripts.h"
#include "VehicleGenerator.h"
#include "CCamera.h"
#include "CTimer.h"

std::list<CVehicleGenerator*> CVehicleGenerator::ListOfVehicles = { 0 };

int GetRandomNumberInRange(int mi, int ma) {
    return mi + (rand() * 0.000030517578 * (double)(ma - mi));
}

CVehicleGenerator::CVehicleGenerator() {
    this->index = 0;
    this->pos = { 0.0f, 0.0f, 0.0f };
    this->angle = 0.0f;
    this->timer = 0;
    this->locked = false;
    this->first = true;
}

CVehicleGenerator::CVehicleGenerator(int index, const CVector pos, const float angle, const int timer, const bool locked) {
    if (index == -1) {
        this->index = GetRandomNumberInRange(MODEL_LANDSTAL, MODEL_VICECHEE);
        switch (this->index) {
        case MODEL_LANDSTAL:
        case MODEL_IDAHO:
        case MODEL_STINGER:
        case MODEL_LINERUN:
        case MODEL_PEREN:
        case MODEL_SENTINEL:
        case MODEL_FIRETRUK:
        case MODEL_TRASH:
        case MODEL_STRETCH:
        case MODEL_MANANA:
        case MODEL_INFERNUS:
        case MODEL_VOODOO:
        case MODEL_PONY:
        case MODEL_MULE:
        case MODEL_CHEETAH:
        case MODEL_ESPERANT:
        case MODEL_TAXI:
        case MODEL_WASHING:
        case MODEL_BOBCAT:
        case MODEL_BFINJECT:
        case MODEL_SECURICA:
        case MODEL_BANSHEE:
        case MODEL_ANGEL:
        case MODEL_CABBIE:
        case MODEL_STALLION:
        case MODEL_ROMERO:
        case MODEL_SENTXS:
        case MODEL_ADMIRAL:
        case MODEL_PCJ600:
        case MODEL_FAGGIO:
        case MODEL_FREEWAY:
        case MODEL_GLENDALE:
        case MODEL_OCEANIC:
        case MODEL_SANCHEZ:
        case MODEL_PATRIOT:
        case MODEL_HERMES:
        case MODEL_SABRE:
        case MODEL_PHEONIX:
        case MODEL_COMET:
        case MODEL_DELUXO:
        case MODEL_BURRITO:
        case MODEL_RANCHER:
        case MODEL_VIRGO:
        case MODEL_GREENWOO:
        case MODEL_SANDKING:
        case MODEL_BLISTAC:
            break;
        default:
            this->index = MODEL_GLENDALE;
            break;
        }
    }
    else
        this->index = index;

    this->pos = pos;
    this->angle = angle;
    this->timer = timer;
    this->time = timer + CTimer::m_snTimeInMilliseconds;
    this->locked = locked;
    this->first = true;
}

CVehicleGenerator::~CVehicleGenerator() {

}

void CVehicleGenerator::AddSpawnPoint(int index, const CVector pos, const float angle, const int timer, const bool locked) {
    CVehicleGenerator* veh = new CVehicleGenerator(index, pos, angle, timer, locked);
    ListOfVehicles.push_back(veh);
    printf("Spawn point added at: %f, %f, %f.\n", pos.x, pos.y, pos.z);
}

void CVehicleGenerator::DeleteAllSpawnPoints() {
    for (auto& iter : CVehicleGenerator::ListOfVehicles) {
        if (iter) {
            delete iter;
        }
    }
}

CVehicle* CVehicleGenerator::Spawn() {
    CStreaming::RequestModel(this->index, 1);
    CStreaming::LoadAllRequestedModels(false);

    if (CStreaming::ms_aInfoForModel[this->index].m_nLoadState == LOADSTATE_LOADED) {
        CVehicle* vehicle = nullptr;
        switch (reinterpret_cast<CVehicleModelInfo*>(CModelInfo::ms_modelInfoPtrs[this->index])->m_nVehicleType) {
#ifdef GTASA
        case VEHICLE_MTRUCK:
            vehicle = new CMonsterTruck(this->index, 2);
            break;
        case VEHICLE_QUAD:
            vehicle = new CQuadBike(this->index, 2);
            break;
        case VEHICLE_BMX:
            vehicle = new CBmx(this->index, 2);
            reinterpret_cast<CBmx*>(vehicle)->m_nDamageFlags |= 0x10;
            break;
        case VEHICLE_TRAILER:
            vehicle = new CTrailer(this->index, 2);
            break;
        case VEHICLE_HELI:
            vehicle = new CHeli(this->index, 2);
            break;
#endif
        case VEHICLE_PLANE:
            vehicle = new CPlane(this->index, 2);
            break;
        case VEHICLE_BIKE:
            vehicle = new CBike(this->index, 2);
            reinterpret_cast<CBike*>(vehicle)->m_nDamageFlags |= 0x10;
            break;
        case VEHICLE_BOAT:
            vehicle = new CBoat(this->index, 2);
            break;
        default:
            vehicle = new CAutomobile(this->index, 2
#ifdef GTASA
                , true
#endif
            );
            break;
        }
        if (vehicle) {
#ifdef GTASA
            vehicle->SetPosn(this->pos);
            vehicle->SetOrientation(0.0f, 0.0f, this->angle);
            vehicle->m_nStatus = 4;

            if (locked) {
                vehicle->m_nDoorLock = CARLOCK_LOCKED;
            }
            else
                vehicle->m_nDoorLock = CARLOCK_UNLOCKED;
#else
            this->pos.z = CWorld::FindGroundZForCoord(this->pos.x, this->pos.y);
            this->pos.z += vehicle->GetDistanceFromCentreOfMassToBaseOfModel() / 2;

            vehicle->SetPosition(this->pos);
            
            vehicle->m_placement.SetOrientation(0.0f, 0.0f, this->angle);
            vehicle->m_nState = 4;
            vehicle->m_nVehicleFlags.bEngineOn = false;
            vehicle->m_nVehicleFlags.bIsLocked = false;
            CCarCtrl::JoinCarWithRoadSystem(vehicle);
            vehicle->m_nZoneLevel = CTheZones::GetLevelFromPosition(&pos);

            if (locked)
                vehicle->m_nLockStatus = CARLOCK_LOCKED;
            else
                vehicle->m_nLockStatus = CARLOCK_UNLOCKED;
#endif
            CWorld::Add(vehicle);
            CTheScripts::ClearSpaceForMissionEntity(this->pos, vehicle);

            return vehicle;
        }
    }
    return nullptr;
}

void CVehicleGenerator::Process() {
    float distanceFromCamera = 0.0f;
    distanceFromCamera =
#ifdef GTASA
    (TheCamera.GetPosition() - pos).Magnitude();
#else
        (TheCamera.pos - this->pos).Magnitude();
#endif
    float distanceFromOtherVehicles = 0.0f;

    if (this->time > CTimer::m_snTimeInMilliseconds) {
        return;
    }

    this->time = this->timer + CTimer::m_snTimeInMilliseconds;

    if ((distanceFromCamera > 150.0f && distanceFromCamera < 250.0f) || this->first) {
        bool pointIsFree = true;
        for (int i = CPools::ms_pVehiclePool->m_nSize; i; i--) {
            CVehicle* veh = CPools::ms_pVehiclePool->GetAt(i - 1);

            if (veh) {
                distanceFromOtherVehicles = (veh->GetPosition() - this->pos).Magnitude();

                if (distanceFromOtherVehicles < 5.0f) {
                    pointIsFree = false;
                    return;
                }
            }
        }

        if (pointIsFree) {
            this->Spawn();
            this->first = false;
            printf("Vehicle spawned.\n");
        }
    }
}
