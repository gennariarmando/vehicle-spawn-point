#pragma once
class CVehicle;

class CVehicleGenerator {
public:
    static std::list<CVehicleGenerator*> ListOfVehicles;

public:
    int index;
    CVector pos;
    float angle;
    bool locked;
    int timer;
    int time;
    bool first;

public:
    CVehicleGenerator();
    CVehicleGenerator(int index, const CVector pos, const float angle, const int timer, const bool locked);
    ~CVehicleGenerator();

    void Process();

private:
    CVehicle* Spawn();

public:
    static void AddSpawnPoint(int index, const CVector pos, const float angle, const int timer, const bool locked);
    static void DeleteAllSpawnPoints();
};

//class CVehicleGenerator {
//public:
//    static std::list<CVehicleGenerator*> ListOfVehicle;
//
//public:
//    bool spawned;
//    int time;
//    unsigned int index;
//    CVector pos;
//    float angle;
//
//public:
//    CVehicle* Spawn();
//    void AddVehicleSpawnPoint(const CVector pos, float angle, unsigned int index, float distance);
//};
