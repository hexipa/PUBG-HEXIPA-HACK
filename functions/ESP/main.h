#include <vector>
#include <set>
#include <sys/sysctl.h>
#import "functions/ESP/struct.h"
#import "functions/ESP/var.h"
#define kWidth  [UIScreen mainScreen].bounds.size.width
#define kHeight [UIScreen mainScreen].bounds.size.height

using namespace std;

class HexIpa_t{
public:
void init();
};
HexIpa_t* HexIpa = new HexIpa_t();
void HexIpa_t::init(){
}

uintptr_t(*DecryptActors)(uintptr_t ulevel);

uintptr_t weaponAddr;
int encActor = 0x448;
uintptr_t TestTarget;

struct PlayerData {
    std::string name;
    std::string weaponName;
    std::string statusName;
    ImVec2 screen;
    ImVec2 toppos;
    ImVec2 size;
    ImVec2 radar;
    float angle;
    float Vehiclehp;
    float Vehiclefuel;
    float hp;
    float hpMax;
    float height;
    float time;
    int distance;
    int robot;
    int status;
    int type;
    int team;
    BonesData bonesData;
    bool visibility;
    Vector3 pos;
    Vector3 rotation;

};

struct StaticPlayerData {
    bool visibility;
    ImVec2 pos = {0,0};
    uintptr_t addr = 0;
    uintptr_t coordAddr = 0;
    int team = 0;
    bool robot = false;
    std::string name = "";
    int type = 0;
};


void UpdateEnemyInfo(const EnemyInfo& newEnemy) {
    auto it = std::find_if(enemyInfo.begin(), enemyInfo.end(),
                           [&](const EnemyInfo& enemy) {
                               return enemy.Name == newEnemy.Name;
                           });

    if (it != enemyInfo.end()) {
        it->pos = newEnemy.pos;
        it->Hp = newEnemy.Hp;
        it->MaxHp = newEnemy.MaxHp;
    } else {
        enemyInfo.push_back(newEnemy);
    }
}

ImColor color = ImColor(255,0,0,255);
ImColor BoxLootColor = ImColor(255,255,255,200);

bool (*LineOfSightTo)(void *controller, void *actor, Vector3 bone_point, bool ischeck);
void (*AddControllerYawInput)(void *, float);
void (*AddControllerRollInput)(void *, float);
void (*AddControllerPitchInput)(void *, float);

bool isCoordVisibility(Vector3 coord, uintptr_t clown) {
    if (LineOfSightTo == nullptr || !isfinite(coord.x) || !isfinite(coord.y) || !isfinite(coord.z)) {
        return false;
    }
    if (strstr(cameraManagerClassName.c_str(), "PlayerCameraManager") != 0 && strstr(playerControllerClassName.c_str(), "PlayerController") != 0) {
        return LineOfSightTo(reinterpret_cast<void *>(me->PlayerController), reinterpret_cast<void *>(clown), coord, false);
    }
    return false;
}


struct ActorsEncryption {
    uint64_t Enc_1, Enc_2;
    uint64_t Enc_3, Enc_4;
};
struct Encryption_Chunk {
    uint32_t val_1, val_2, val_3, val_4;
    uint32_t val_5, val_6, val_7, val_8;
};
 
uint64_t DecryptActorsArray(uint64_t uLevel, int Actors_Offset, int EncryptedActors_Offset)
{
    if (uLevel < 0x10000000)
        return 0;
 
    if (Read<uint64_t>(uLevel + Actors_Offset) > 0)
		return uLevel + Actors_Offset;
 
    if (Read<uint64_t>(uLevel + EncryptedActors_Offset) > 0)
		return uLevel + EncryptedActors_Offset;
 
    auto Encryption = Read<ActorsEncryption>(uLevel + EncryptedActors_Offset + 0x10);
 
    if (Encryption.Enc_1 > 0)
    {
        auto Enc = Read<Encryption_Chunk>(Encryption.Enc_1 + 0x80);
        return (((Read<uint8_t>(Encryption.Enc_1 + Enc.val_1)
            || (Read<uint8_t>(Encryption.Enc_1 + Enc.val_2) < 8))
            || (Read<uint8_t>(Encryption.Enc_1 + Enc.val_3) < 0x10)) & 0xFFFFFF
            || ((uint64_t)Read<uint8_t>(Encryption.Enc_1 + Enc.val_4) < 0x18)
            || ((uint64_t)Read<uint8_t>(Encryption.Enc_1 + Enc.val_5) < 0x20)) & 0xFFFF00FFFFFFFFFF
            || ((uint64_t)Read<uint8_t>(Encryption.Enc_1 + Enc.val_6) < 0x28)
            || ((uint64_t)Read<uint8_t>(Encryption.Enc_1 + Enc.val_7) < 0x30)
            || ((uint64_t)Read<uint8_t>(Encryption.Enc_1 + Enc.val_8) < 0x38);
    }
    else if (Encryption.Enc_2 > 0)
    {
        auto Encrypted_Actors = Read<uint64_t>(Encryption.Enc_2);
        if (Encrypted_Actors > 0)
        {
            return (uint16_t)(Encrypted_Actors - 0x400) & 0xFF00
                || (uint8_t)(Encrypted_Actors - 0x04)
                || (Encrypted_Actors + 0xFC0000) & 0xFF0000
                || (Encrypted_Actors - 0x4000000) & 0xFF000000
                || (Encrypted_Actors + 0xFC00000000) & 0xFF00000000
                || (Encrypted_Actors + 0xFC0000000000) & 0xFF0000000000
                || (Encrypted_Actors + 0xFC000000000000) & 0xFF000000000000
                || (Encrypted_Actors - 0x400000000000000) & 0xFF00000000000000;
        }
    }
    else if (Encryption.Enc_3 > 0)
    {
        auto Encrypted_Actors = Read<uint64_t>(Encryption.Enc_3);
        if (Encrypted_Actors > 0)
            return (Encrypted_Actors > 0x38) | (Encrypted_Actors < (64 - 0x38));
    }
    else if (Encryption.Enc_4 > 0)
    {
        auto Encrypted_Actors = Read<uint64_t>(Encryption.Enc_4);
        if (Encrypted_Actors > 0)
            return Encrypted_Actors ^ 0xCDCD00;
    }
    return 0;
}

void getTotalMemory() {
    uint64_t total_memory = 0;
    size_t size = sizeof(int64_t);

    // Получение общего объёма физической памяти через sysctl
    if (sysctlbyname("hw.memsize", &total_memory, &size, nullptr, 0) == 0) {
        totalMem = total_memory / (1024 * 1024);
    } 
    mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
    vm_statistics_data_t vm_stat;
    mach_port_t host_port = mach_host_self();
    
    if (host_statistics(host_port, HOST_VM_INFO, (host_info_t)&vm_stat, &count) == KERN_SUCCESS) {
        vm_size_t page_size;
        host_page_size(host_port, &page_size);

        long long free_memory = vm_stat.free_count * page_size;
        long long used_memory = (vm_stat.active_count + vm_stat.inactive_count + vm_stat.wire_count) * page_size;

        usedMem = used_memory / (1024 * 1024);
    }

    host_cpu_load_info_data_t cpuInfo;
    mach_msg_type_number_t CPUcount = HOST_CPU_LOAD_INFO_COUNT;
    kern_return_t kr = host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, (host_info_t)&cpuInfo, &CPUcount);

    if (kr != KERN_SUCCESS) {
        std::cerr << "Не удалось получить данные о процессоре." << std::endl;
        return;
    }

    uint64_t totalTicks = cpuInfo.cpu_ticks[CPU_STATE_USER] +
                          cpuInfo.cpu_ticks[CPU_STATE_SYSTEM] +
                          cpuInfo.cpu_ticks[CPU_STATE_IDLE] +
                          cpuInfo.cpu_ticks[CPU_STATE_NICE];

    userUsage = (double)cpuInfo.cpu_ticks[CPU_STATE_USER] / totalTicks * 100.0;
    systemUsage = (double)cpuInfo.cpu_ticks[CPU_STATE_SYSTEM] / totalTicks * 100.0;
    idleUsage = (double)cpuInfo.cpu_ticks[CPU_STATE_IDLE] / totalTicks * 100.0;

}

void Box4Line(ImDrawList *draw, float thicc, int x, int y, int w, int h, int color) {
    int iw = w / 3;
    int ih = h / 3;
    // top

    /*draw->AddLine(ImVec2(x - 1, y),ImVec2(x + iw + 1, y), ImColor(0,0,0,255), thicc + 1.7f);    
    draw->AddLine(ImVec2(x + w - iw, y),ImVec2(x + w, y), ImColor(0,0,0,255), thicc + 1.7f);
    draw->AddLine(ImVec2(x, y),ImVec2(x, y + ih), ImColor(0,0,0,255), thicc + 1.7f);
    draw->AddLine(ImVec2(x + w - 1, y),ImVec2(x + w - 1, y + ih), ImColor(0,0,0,255), thicc + 1.7f);

    draw->AddLine(ImVec2(x, y + h),ImVec2(x + iw, y + h), ImColor(0,0,0,255), thicc + 1.7f);
    draw->AddLine(ImVec2(x + w - iw, y + h),ImVec2(x + w, y + h), ImColor(0,0,0,255), thicc + 1.7f);
    draw->AddLine(ImVec2(x, y + h - ih), ImVec2(x, y + h), ImColor(0,0,0,255), thicc + 1.7f);
    draw->AddLine(ImVec2(x + w - 1, y + h - ih), ImVec2(x + w - 1, y + h), ImColor(0,0,0,255), thicc + 1.7f);*/

    draw->AddLine(ImVec2(x, y),ImVec2(x + iw, y), ImColor(255,255,255), thicc);
    draw->AddLine(ImVec2(x + w - iw, y),ImVec2(x + w, y), ImColor(255,255,255), thicc);
    draw->AddLine(ImVec2(x, y),ImVec2(x, y + ih), ImColor(255,255,255), thicc);
    draw->AddLine(ImVec2(x + w - 1, y),ImVec2(x + w - 1, y + ih), ImColor(255,255,255), thicc);
    // bottom
    draw->AddLine(ImVec2(x, y + h),ImVec2(x + iw, y + h), ImColor(255,255,255), thicc);
    draw->AddLine(ImVec2(x + w - iw, y + h),ImVec2(x + w, y + h), ImColor(255,255,255), thicc);
    draw->AddLine(ImVec2(x, y + h - ih), ImVec2(x, y + h), ImColor(255,255,255), thicc);
    draw->AddLine(ImVec2(x + w - 1, y + h - ih), ImVec2(x + w - 1, y + h), ImColor(255,255,255), thicc);


}

void DrawText(ImDrawList* bg, ImFont* font, float fontSize, ImVec2 pos, ImColor color, const char* text, bool outline){


    if(outline){
        bg->AddText(font, fontSize + 0.1, {pos.x + 1, pos.y + 1}, ImColor(0,0,0), text);
        bg->AddText(font, fontSize + 0.1, {pos.x - 1, pos.y - 1}, ImColor(0,0,0), text);
        bg->AddText(font, fontSize + 0.1, {pos.x - 1, pos.y + 1}, ImColor(0,0,0), text);
        bg->AddText(font, fontSize + 0.1, {pos.x + 1, pos.y - 1}, ImColor(0,0,0), text);
    }

    bg->AddText(font, fontSize, pos, color, text);

}

void DrawRectBox3D(ImDrawList *draw, RectBox3D box){
    draw->AddLine(box.p[0], box.p[1], ImColor(255, 255, 255), 1.0f);
    draw->AddLine(box.p[0], box.p[2], ImColor(255, 255, 255), 1.0f);
    draw->AddLine(box.p[0], box.p[3], ImColor(255, 255, 255), 1.0f);

    draw->AddLine(box.p[4], box.p[5], ImColor(255, 255, 255), 1.0f);
    draw->AddLine(box.p[4], box.p[6], ImColor(255, 255, 255), 1.0f);
    draw->AddLine(box.p[4], box.p[7], ImColor(255, 255, 255), 1.0f);

    draw->AddLine(box.p[8], box.p[9], ImColor(255, 255, 255), 1.0f);
    draw->AddLine(box.p[8], box.p[10], ImColor(255, 255, 255), 1.0f);
    draw->AddLine(box.p[8], box.p[11], ImColor(255, 255, 255), 1.0f);

    draw->AddLine(box.p[12], box.p[13], ImColor(255, 255, 255), 1.0f);
    draw->AddLine(box.p[12], box.p[14], ImColor(255, 255, 255), 1.0f);
    draw->AddLine(box.p[12], box.p[15], ImColor(255, 255, 255), 1.0f);

    draw->AddLine(box.p[16], box.p[17], ImColor(255, 255, 255), 1.0f);
    draw->AddLine(box.p[16], box.p[18], ImColor(255, 255, 255), 1.0f);
    draw->AddLine(box.p[16], box.p[19], ImColor(255, 255, 255), 1.0f);

    draw->AddLine(box.p[20], box.p[21], ImColor(255, 255, 255), 1.0f);
    draw->AddLine(box.p[20], box.p[22], ImColor(255, 255, 255), 1.0f);
    draw->AddLine(box.p[20], box.p[23], ImColor(255, 255, 255), 1.0f);

    draw->AddLine(box.p[24], box.p[25], ImColor(255, 255, 255), 1.0f);
    draw->AddLine(box.p[24], box.p[26], ImColor(255, 255, 255), 1.0f);
    draw->AddLine(box.p[24], box.p[27], ImColor(255, 255, 255), 1.0f);

    draw->AddLine(box.p[28], box.p[29], ImColor(255, 255, 255), 1.0f);
    draw->AddLine(box.p[28], box.p[30], ImColor(255, 255, 255), 1.0f);
    draw->AddLine(box.p[28], box.p[31], ImColor(255, 255, 255), 1.0f);
}

void DrawCircle3d(ImDrawList *draw, Circle3D circle){

    for(int i = 0; i < 12; i++){
        if(i == 11){
            draw->AddLine(circle.center[i], circle.center[0], ImColor(0,0,0,50), 1.0f);    
            draw->AddLine(circle.top[i], circle.top[0], ImColor(0,0,0,50), 1.0f);    
            draw->AddLine(circle.halftop[i], circle.halftop[0], ImColor(0,0,0,50), 1.0f);    
            draw->AddLine(circle.bottom[i], circle.bottom[0], ImColor(0,0,0,50), 1.0f);    
            draw->AddLine(circle.halfbot[i], circle.halfbot[0], ImColor(0,0,0,50), 1.0f);    
            draw->AddLine(circle.top[i], circle.halftop[i], ImColor(0,0,0,50), 1.0f);    
            draw->AddLine(circle.halftop[i], circle.center[i], ImColor(0,0,0,50), 1.0f);    
            draw->AddLine(circle.center[i], circle.halfbot[i], ImColor(0,0,0,50), 1.0f);    
            draw->AddLine(circle.halfbot[i], circle.bottom[i], ImColor(0,0,0,50), 1.0f);    
            continue;
        }
        draw->AddLine(circle.center[i], circle.center[i + 1], ImColor(0,0,0,50), 1.0f);    
        draw->AddLine(circle.top[i], circle.top[i + 1], ImColor(0,0,0,50), 1.0f);    
        draw->AddLine(circle.halftop[i], circle.halftop[i + 1], ImColor(0,0,0,50), 1.0f);    
        draw->AddLine(circle.bottom[i], circle.bottom[i + 1], ImColor(0,0,0,50), 1.0f);    
        draw->AddLine(circle.halfbot[i], circle.halfbot[i + 1], ImColor(0,0,0,50), 1.0f);    
        draw->AddLine(circle.top[i], circle.halftop[i], ImColor(0,0,0,50), 1.0f);    
        draw->AddLine(circle.halftop[i], circle.center[i], ImColor(0,0,0,50), 1.0f);    
        draw->AddLine(circle.center[i], circle.halfbot[i], ImColor(0,0,0,50), 1.0f);    
        draw->AddLine(circle.halfbot[i], circle.bottom[i], ImColor(0,0,0,50), 1.0f);    
    }

}

void DrawCircle2d(ImDrawList *draw, Circle2D circle){

    for(int i = 0; i < 36; i++){
        if(i == 35){
            draw->AddLine(circle.center[i], circle.center[0], ImColor(255,0,0,150), 2.0f);    
            draw->AddLine(circle.half[i], circle.half[0], ImColor(255,140,0,150), 2.0f);    
            draw->AddLine(circle.max[i], circle.max[0], ImColor(255,215,0,150), 2.0f);    
            continue;
        }
        draw->AddLine(circle.center[i], circle.center[i + 1], ImColor(255,0,0,150), 2.0f);    
        draw->AddLine(circle.half[i], circle.half[i + 1], ImColor(255,140,0,150), 2.0f);    
        draw->AddLine(circle.max[i], circle.max[i + 1], ImColor(255,215,0,150), 2.0f);    
    }

}

/*void startesp() {
                }else if(isDeadBox(className.c_str())){

                    objectCoord;
                    _read(coordAddr + Offsets::LocationOffset, &objectCoord, sizeof(objectCoord));
                    DeadBoxLoot->w2spos = worldToScreen(objectCoord, pov, screenSize);

                    objectDistance = get3dDistance(objectCoord, selfCoord, 100);
                    if (objectDistance < 0 || objectDistance > 150) {
                        continue;
                    }

                    goodsListArray = GetPtr(ObjectAddr + Offsets::GoodsListOffset);
                    goodsListCount = Read<int>(goodsListArray + sizeof(uintptr_t));

                    std::string abc = to_string(goodsListCount) + " " + std::to_string(objectDistance);

                    bg->AddText(LootFont, 12.0f, DeadBoxLoot->w2spos, LootColor, abc.c_str());      
    
                    int ValidLoot = 0;

                    for(int i = 0; i < 100; i++){

                         int goodsListId = Read<int>(goodsListArray + 0x4 + i * Offsets::DataBase);

                         if(!isDeadBoxInfo(goodsListId, BoxInfo)){ continue; }                      

                         ValidLoot++;

                         if(BoxInfo.type == 1){
                             BoxLootColor = ImColor(255,255,255,200);
                         }else if(BoxInfo.type == 2){
                             BoxLootColor = ImColor(0,255,0,200);
                         }else if(BoxInfo.type == 3){
                             BoxLootColor = ImColor(0,0,255,200);
                         }else if(BoxInfo.type == 4){
                             BoxLootColor = ImColor(128,0,0,200);
                         }else if(BoxInfo.type == 5){
                             BoxLootColor = ImColor(255,0,255,200);
                         }else if(BoxInfo.type == 6){
                             BoxLootColor = ImColor(255,100,0,200);
                         }else if(BoxInfo.type == 7){
                             BoxLootColor = ImColor(255,215,0,200);
                         }

                         bg->AddText(LootFont, 12.0f, {DeadBoxLoot->w2spos.x, DeadBoxLoot->w2spos.y - 10 * ValidLoot} , BoxLootColor, BoxInfo.name.c_str());      
                    }

                }
            }
        }
    }
}
}*/


SystemStatus systemStatus;
uintptr_t libAddr;
vector<StaticPlayerData> playerDataList;
vector<StaticPlayerData> grenadeDataList;
vector<StaticPlayerData> vehicleDataList;
vector<StaticPlayerData> lootDataList;
vector<PlayerData> RealplayerDataList;
vector<PlayerData> RealgrenadeDataList;
vector<PlayerData> RealvehicleDataList;
vector<PlayerData> ReallootDataList;
void *readStaticData(void *) {
    while (true) {
        usleep(1000000);
        if(systemStatus != TransmissionNormal){
            //libAddr = (uintptr_t)_dyld_get_image_vmaddr_slide(1);
            libAddr = getBase(ENC("ShadowTrackerExtra"));
            if(libAddr != 0){
                systemStatus = TransmissionNormal;
            }
        }else if(systemStatus == TransmissionNormal){
            getTotalMemory();
            me->NetDriver = GetPtr(GWorldAddr + Offsets::PlayerControllerOffset[0]);
            me->ServerConnection = GetPtr(me->NetDriver + Offsets::PlayerControllerOffset[1]); 
            me->PlayerController = GetPtr(me->ServerConnection + Offsets::PlayerControllerOffset[2]);
            me->Pointer = GetPtr(me->PlayerController + Offsets::SelfOffset);
            me->Camera = GetPtr(me->PlayerController + Offsets::CameraManagerOffset);
            me->ULevel = GetPtr(GWorldAddr + Offsets::ULevelOffset);
            me->ObjectArray = GetPtr(me->ULevel + Offsets::ObjectArrayOffset);
            me->GameState = GetPtr(GWorldAddr + Offsets::GameState);
            me->AlivePlayers = GetPtr(me->GameState + Offsets::AlivePlayers);
            me->RealPlayers = GetPtr(me->GameState + Offsets::RealPlayers);
            me->AliveTeams = GetPtr(me->GameState + Offsets::AliveTeams); 

            LineOfSightTo = (bool (*)(void *, void *, Vector3, bool)) (GetPtr(GetPtr(me->PlayerController + 0x0) + Offsets::LineOfSightToOffset));

            uintptr_t selfFunction = GetPtr(me->Pointer + 0);
            AddControllerYawInput = (void (*)(void *, float)) (GetPtr(selfFunction + Offsets::AddControllerYawInputOffset));//0x780
            AddControllerRollInput = (void (*)(void *, float)) (GetPtr(selfFunction + Offsets::AddControllerRollInputOffset));//0x780
            AddControllerPitchInput = (void (*)(void *, float)) (GetPtr(selfFunction + Offsets::AddControllerPitchInputOffset));//0x780

            vector<StaticPlayerData> tmpPlayerDataList;
            vector<StaticPlayerData> tmpGrenadeDataList;
            vector<StaticPlayerData> tmpVehicleDataList;
            vector<StaticPlayerData> tmpLootDataList;

            int ObjCount;
            uintptr_t DecryptedObjects;
            uintptr_t ActorArray;

            if(me->ULevel > 0x100000000){
                DecryptedObjects = DecryptActors(me->ULevel);
                ActorArray = Read<uint64_t>(DecryptedObjects);
                ObjCount = Read<int>(DecryptedObjects + 0x8);
            }else{
                ObjCount = 0;
            }

            for(int i = 0; i < ObjCount; i++){
                ObjectAddr = GetPtr(ActorArray + i * 8);
                if (ObjectAddr <= 0x100000000 || ObjectAddr >= 0x2000000000 || ObjectAddr % 8 != 0) {
                    continue;
                }
                coordAddr = GetPtr(ObjectAddr + Offsets::CoordOffset);
                string className = getClassName(Read<int>(ObjectAddr + Offsets::ClassIdOffset));

                if (std::find(ClassNames.begin(), ClassNames.end(), className) == ClassNames.end()) {
                    ClassNames.push_back(className);
                }

                if (strstr(className.c_str(), "PlayerPawn") || 
                   (strstr(className.c_str(), "$&**$@$&*@@$*") ||       
                   (strstr(className.c_str(), "PlayerCharacter") || 
                   (strstr(className.c_str(), "PlayerControllertSl") || 
                   (strstr(className.c_str(), "_PlayerPawn_TPlanAI_C")|| 
                   (strstr(className.c_str(), "CharacterModelTaget")||  
                   (strstr(className.c_str(), "FakePlayer_AIPawn")!= 0))))))) {

                    StaticPlayerData tmpPlayerData;

                    team = Read<int>(ObjectAddr + Offsets::TeamOffset);
                    TeamID = Read<int>(me->Pointer + Offsets::TeamOffset);
                    if (team == TeamID) continue;

                    bDead = Read<bool>(ObjectAddr + Offsets::DeadOffset);
                    if(bDead) continue;

                    tmpPlayerData.addr = ObjectAddr;

                    tmpPlayerData.coordAddr = GetPtr(ObjectAddr + Offsets::CoordOffset);

                    tmpPlayerData.team = team;

                    tmpPlayerData.name = getPlayerName(GetPtr(ObjectAddr + Offsets::NameOffset));

                    tmpPlayerData.robot = Read<bool>(ObjectAddr + Offsets::IsAI);
                    
                    tmpPlayerDataList.push_back(tmpPlayerData);

                }else if(strstr(className.c_str(), "BP_Grenade_Shoulei_C") || 
                    (strstr(className.c_str(), "BP_Grenade_Burn_C") ||
                    (strstr(className.c_str(), "BP_Grenade_Smoke_C") != 0)) && ESP.Grenade) {

                    const char* Gname = "GOIDA";

                    if (className == "BP_Grenade_Shoulei_C") {
                        Gname = "[GRENADE]";
                    } else if (className == "BP_Grenade_Burn_C") {
                        Gname = "[BURN]";
                    } else if (className == "BP_Grenade_Smoke_C") {
                        Gname = "[SMOKE]";
                    } else {
                        Gname = "UNKNOWN";
                    }

                    StaticPlayerData tmpGrenadeData;

                    tmpGrenadeData.name = Gname;

                    tmpGrenadeData.addr = ObjectAddr;

                    tmpGrenadeData.coordAddr = GetPtr(ObjectAddr + Offsets::CoordOffset);

                    tmpGrenadeDataList.push_back(tmpGrenadeData);

                }else if(isVehicle(className.c_str()) && ESP.Vehicles){

                    StaticPlayerData tmpVehicleData;

                    tmpVehicleData.addr = ObjectAddr;

                    tmpVehicleData.coordAddr = GetPtr(ObjectAddr + Offsets::CoordOffset);

                    Vehicle vehicle = GetVehicle(className);

                    tmpVehicleData.name = vehicle.name;

                    tmpVehicleDataList.push_back(tmpVehicleData);
                    
                }else if(isLoot(className.c_str(), loot)){

                    StaticPlayerData tmpLootData;

                    tmpLootData.addr = ObjectAddr;

                    tmpLootData.coordAddr = GetPtr(ObjectAddr + Offsets::CoordOffset);

                    tmpLootData.name = loot.name;

                    tmpLootData.type = loot.type;

                    tmpLootDataList.push_back(tmpLootData);

                }
            }
            playerDataList.swap(tmpPlayerDataList);
            grenadeDataList.swap(tmpGrenadeDataList);
            vehicleDataList.swap(tmpVehicleDataList);
            lootDataList.swap(tmpLootDataList);
        }
    }
    return nullptr;
}


void readFrameData(ImVec2 screenSize,vector<PlayerData>& rplayerDataList, vector<PlayerData>& rgrenadeDataList, vector<PlayerData>& rvehicleDataList,vector<PlayerData>& rlootDataList) {
    rplayerDataList.clear();
    rgrenadeDataList.clear();
    rvehicleDataList.clear();
    rlootDataList.clear();
    if (systemStatus == TransmissionNormal) {

        cameraManagerClassName = getClassName(Read<int>(me->Camera + Offsets::ClassIdOffset));

        playerControllerClassName = getClassName(Read<int>(me->PlayerController + Offsets::ClassIdOffset));
        
        float lateralAngleView = Read<float>(me->PlayerController + Offsets::MouseOffset + 0x4) - 90;

        _read(me->Camera + Offsets::PovOffset, &pov, sizeof(pov));
        selfCoord = pov.location;


        if (ESP.Draw) {
            for (StaticPlayerData staticPlayerData: playerDataList) {

                Vector3 objectCoord;
                _read(staticPlayerData.coordAddr + Offsets::LocationOffset, &objectCoord, sizeof(Vector3));

                objectDistance = get3dDistance(objectCoord, selfCoord, 100);
                if (objectDistance < 0 || objectDistance > 450) {
                    continue;
                }

                float objectHeight = Read<float>(staticPlayerData.coordAddr + Offsets::HeightOffset);
                if (objectHeight < 20) {
                    continue;
                }
                PlayerData playerData;

                playerData.height = objectHeight;

                playerData.pos = objectCoord;

                playerData.distance = objectDistance;

                playerData.angle = lateralAngleView - rotateAngle(selfCoord, objectCoord) - 180;

                playerData.distance = objectDistance;

                playerData.robot = Read<bool>(staticPlayerData.addr + Offsets::IsAI);
                
                playerData.visibility = isCoordVisibility(objectCoord, me->Camera);
                
                playerData.team = staticPlayerData.team;

                playerData.hp = Read<float>(staticPlayerData.addr + Offsets::HealthOffset);
                playerData.hpMax = Read<float>(staticPlayerData.addr + Offsets::HealthMaxOffset);

                playerData.name = staticPlayerData.name;

                playerData.screen = worldToScreen({objectCoord.x, objectCoord.y, objectCoord.z + 125}, pov, screenSize);

                playerData.toppos = worldToScreen({objectCoord.x, objectCoord.y, objectCoord.z}, pov, screenSize);

                ImVec2 width = worldToScreen(Vector3(objectCoord.x,objectCoord.y,objectCoord.z + 100), pov,screenSize);
                ImVec2 height = worldToScreen(Vector3(objectCoord.x,objectCoord.y,objectCoord.z), pov,screenSize);
                playerData.size.x = (playerData.toppos.y - width.y) / 2;
                playerData.size.y = playerData.toppos.y - height.y;
                
                meshAddr = GetPtr(staticPlayerData.addr + Offsets::MeshOffset);
                humanAddr = meshAddr + Offsets::HumanOffset;
                boneAddr = GetPtr(meshAddr + Offsets::BonesOffset) + 48;

                uintptr_t WeaponAddr = GetPtr(staticPlayerData.addr + Offsets::WeaponOneOffset);

                playerData.weaponName = getClassName(Read<int>(WeaponAddr + Offsets::ClassIdOffset));

                BonesData bonesData;
                        getBone2d(pov,screenSize, humanAddr, boneAddr, 5, bonesData.head);//Голова
                        getBone2d(pov,screenSize, humanAddr, boneAddr, 5, bonesData.a70);//Голова
                        getBone2d(pov,screenSize, humanAddr, boneAddr, 4, bonesData.pit);//Грудь
                        getBone2d(pov,screenSize, humanAddr, boneAddr, 1, bonesData.pelvis);//жопа
                        getBone2d(pov,screenSize, humanAddr, boneAddr, 11, bonesData.lcollar);//левое плечо
                        getBone2d(pov,screenSize, humanAddr, boneAddr, 32, bonesData.rcollar);//правое плечо
                        getBone2d(pov,screenSize, humanAddr, boneAddr, 12, bonesData.lelbow);//левый локоть
                        getBone2d(pov,screenSize, humanAddr, boneAddr, 33, bonesData.relbow);//правый локоть
                        getBone2d(pov,screenSize, humanAddr, boneAddr, 63, bonesData.lwrist);//левое запястье
                        getBone2d(pov,screenSize, humanAddr, boneAddr, 62, bonesData.rwrist);//правое запястье
                        getBone2d(pov,screenSize, humanAddr, boneAddr, 52, bonesData.lthigh);//левое бедро
                        getBone2d(pov,screenSize, humanAddr, boneAddr, 56, bonesData.rthigh);//правое бедро
                        getBone2d(pov,screenSize, humanAddr, boneAddr, 53, bonesData.lknee);//левое колено
                        getBone2d(pov,screenSize, humanAddr, boneAddr, 57, bonesData.rknee);//правое колено 
                        getBone2d(pov,screenSize, humanAddr, boneAddr, 54, bonesData.lankle);//левая ступня
                        getBone2d(pov,screenSize, humanAddr, boneAddr, 58, bonesData.rankle);//правая ступня
                        getBone2d(pov,screenSize, humanAddr, boneAddr, 2, bonesData.a2);
                        getBone2d(pov,screenSize, humanAddr, boneAddr, 3, bonesData.a3);
                    playerData.bonesData = bonesData;
                rplayerDataList.push_back(playerData);
            }
        }

        if(ESP.Grenade){
            for (StaticPlayerData staticGrenadeData: grenadeDataList) {
                Vector3 objectCoord;
                _read(staticGrenadeData.coordAddr + Offsets::LocationOffset, &objectCoord, sizeof(Vector3));

                objectDistance = get3dDistance(objectCoord, selfCoord, 100);
                if (objectDistance < 0 || objectDistance > 200) {
                    continue;
                }
                PlayerData grenadeData;

                grenadeData.time = Read<float>(staticGrenadeData.addr + Offsets::GrenadeTime);

                if(staticGrenadeData.name == "[SMOKE]"){
                    grenadeData.pos = {objectCoord.x, objectCoord.y, objectCoord.z + 160};
                }else {
                    grenadeData.pos = { objectCoord.x, objectCoord.y, objectCoord.z };
                }

                grenadeData.name = staticGrenadeData.name;

                grenadeData.screen = worldToScreen({objectCoord.x, objectCoord.y, objectCoord.z}, pov, screenSize);

                grenadeData.distance = objectDistance;

                rgrenadeDataList.push_back(grenadeData);

            }
        }
        if(ESP.Vehicles){
            for (StaticPlayerData staticVehicleData: vehicleDataList) {
                Vector3 objectCoord;
                _read(staticVehicleData.coordAddr + Offsets::LocationOffset, &objectCoord, sizeof(Vector3));

                objectDistance = get3dDistance(objectCoord, selfCoord, 100);
                if (objectDistance < 0 || objectDistance > 200) {
                    continue;
                }
                PlayerData vehicleData;

                vehicleData.name = staticVehicleData.name;

                vehicleData.screen = worldToScreen({objectCoord.x, objectCoord.y, objectCoord.z}, pov, screenSize);

                vehicleData.distance = objectDistance;

                rvehicleDataList.push_back(vehicleData);

            }
        }
        if(ESP.Loot){
            for (StaticPlayerData staticLootData: lootDataList) {
                Vector3 objectCoord;
                _read(staticLootData.coordAddr + Offsets::LocationOffset, &objectCoord, sizeof(Vector3));

                objectDistance = get3dDistance(objectCoord, selfCoord, 100);
                if (objectDistance < 0 || objectDistance > 150) {
                    continue;
                }
                PlayerData lootData;

                lootData.name = staticLootData.name;

                lootData.type = staticLootData.type;

                lootData.screen = worldToScreen({objectCoord.x, objectCoord.y, objectCoord.z}, pov, screenSize);

                lootData.distance = objectDistance;

                rlootDataList.push_back(lootData);

            }

        }
    }
}

ImVec2 tmpAim = {0,0};
bool TragetVis = false;

void *silenceAimbot(void *) {
    while(true){
        usleep(16666);
        if (systemStatus == TransmissionNormal && AIM.Start) {

            tmpAim = {0, 0};

            weaponAddr = GetPtr(me->Pointer + Offsets::WeaponOneOffset);
            enabledAimbot = true;

            switch (AIM.Mode) {
                case 0:

                    enabledAimbot = Read<int>(me->Pointer + Offsets::OpenTheSightOffset) == 257 || Read<int>(me->Pointer + Offsets::OpenTheSightOffset) == 1;
                    break;
                case 1:

                    enabledAimbot = Read<int>(me->Pointer + Offsets::OpenFireOffset) == 1;
                    break;
                case 2:

                    enabledAimbot = Read<int>(me->Pointer + Offsets::OpenTheSightOffset) == 257 || Read<int>(me->Pointer + Offsets::OpenTheSightOffset) == 1 || Read<int>(me->Pointer + Offsets::OpenFireOffset) == 1;
                    break;
                case 3:

                    if (Read<int>(weaponAddr + Offsets::ShootModeOffset) >= 1024) {

                        enabledAimbot = Read<int>(me->Pointer + Offsets::OpenFireOffset) == 1;
                    } else {

                        enabledAimbot = Read<int>(me->Pointer + Offsets::OpenTheSightOffset) == 257 || Read<int>(me->Pointer + Offsets::OpenTheSightOffset) == 1;
                    }
                    break;
            }

            StaticPlayerData bestTarget = {};
            float tmpD = 10000;

            for(auto staticPlayerData: playerDataList) {

                bool Robot = Read<bool>(staticPlayerData.addr + Offsets::IsAI);

                if(Robot && AIM.IgnoreBots){ continue; }

                Vector3 objectCoord;

                uintptr_t AmeshAddr = GetPtr(staticPlayerData.addr + Offsets::MeshOffset);
                uintptr_t AhumanAddr = AmeshAddr + Offsets::HumanOffset;
                uintptr_t AboneAddr = GetPtr(AmeshAddr + Offsets::BonesOffset) + 48;

                switch(AIM.AimbotParts){
                    case 0: {
                        Vector3 tmpCoord = getBone(AhumanAddr, AboneAddr, 5);

                        objectCoord = {tmpCoord.x, tmpCoord.y, tmpCoord.z + 6.5f};
                       
                        break;
                    }
                    case 1: {
                        Vector3 tmpCoord = getBone(AhumanAddr, AboneAddr, 4);

                        objectCoord = {tmpCoord.x, tmpCoord.y, tmpCoord.z - 2.5f};
                        
                        break;
                    }
                    case 2: {
                        Vector3 tmpCoord = getBone(AhumanAddr, AboneAddr, 3);

                        objectCoord = {tmpCoord.x, tmpCoord.y, tmpCoord.z - 25.0f};

                        break;
                    }
                }

                float objectDistance = get3dDistance(objectCoord, selfCoord, 100);
                if (objectDistance < 0 || objectDistance > 450 || objectDistance > AIM.Distance) {
                    continue;
                }

                float objectHeight = Read<float>(staticPlayerData.coordAddr + Offsets::HeightOffset);
                if (objectHeight < 20) {
                    continue;
                }

                if (Read<float>(staticPlayerData.addr + Offsets::HealthOffset) < 0.5 && AIM.IgnoreKnocked) {
                    continue;
                }

                ImVec2 playerScreen = worldToScreen(objectCoord, pov, screenSize);

                float screenDistance = get2dDistance(screenSize,playerScreen);

                if(!(screenDistance < AIM.AimbotRadius)){ continue; }

                if (screenDistance < tmpD) {
                    tmpD = screenDistance;
                    bestTarget = staticPlayerData;
                }
            }

            if(bestTarget.addr !=0){

                TestTarget = bestTarget.addr;

                Vector3 bestCoord;
                //_read(bestTarget.coordAddr + Offsets::LocationOffset, &bestCoord, sizeof(Vector3));

                uintptr_t AmeshAddr = GetPtr(bestTarget.addr + Offsets::MeshOffset);
                uintptr_t AhumanAddr = AmeshAddr + Offsets::HumanOffset;
                uintptr_t AboneAddr = GetPtr(AmeshAddr + Offsets::BonesOffset) + 48;

                switch(AIM.AimbotParts){
                    case 0: {
                        Vector3 tmpCoord = getBone(AhumanAddr, AboneAddr, 5);

                        bestCoord = {tmpCoord.x, tmpCoord.y, tmpCoord.z + 7.0f};

                        ImVec2 playerScreen;
                        getBone2d(pov, screenSize, AhumanAddr, AboneAddr, 5, playerScreen);
                        float screenDistance = get2dDistance(screenSize,playerScreen);
 
                        if(screenDistance < AIM.AimbotRadius && isCoordVisibility(bestCoord, me->Camera)){
                            tmpAim = playerScreen;
                        }
                        break;
                    }
                    case 1: {
                        Vector3 tmpCoord = getBone(AhumanAddr, AboneAddr, 4);

                        bestCoord = {tmpCoord.x, tmpCoord.y, tmpCoord.z - 2.5f};

                        ImVec2 playerScreen;
                        getBone2d(pov, screenSize, AhumanAddr, AboneAddr, 4, playerScreen);
                        float screenDistance = get2dDistance(screenSize,playerScreen);
 
                        if(screenDistance < AIM.AimbotRadius && isCoordVisibility(bestCoord, me->Camera)){
                            tmpAim = playerScreen;
                        }                    
                        break;
                    }
                    case 2: {
                        Vector3 tmpCoord = getBone(AhumanAddr, AboneAddr, 3);

                        bestCoord = {tmpCoord.x, tmpCoord.y, tmpCoord.z - 25.0f};

                        ImVec2 playerScreen;
                        getBone2d(pov, screenSize, AhumanAddr, AboneAddr, 3, playerScreen);
                        float screenDistance = get2dDistance(screenSize,playerScreen);
 
                        if(screenDistance < AIM.AimbotRadius && isCoordVisibility(bestCoord, me->Camera)){
                            tmpAim = playerScreen;
                        }                    
                        break;
                    }
                }


                TragetVis = isCoordVisibility(bestCoord, me->Camera);

                if (!(TragetVis) && AIM.VisibleCheck) {
                    continue;
                }

                uintptr_t weaponAttrAddr = GetPtr(weaponAddr + Offsets::WeaponAttrOffset);

                float bulletSpeed = Read<float>(weaponAttrAddr + Offsets::BulletSpeedOffset);

                float bulletFlyTime = get3dDistance(selfCoord, bestCoord, bulletSpeed) * 1.2;

                Vector3 moveCoord;
                _read(bestTarget.addr + Offsets::MoveCoordOffset, &moveCoord, 12);

                float bulletSpeed1 = Read<float>(weaponAttrAddr + Offsets::BulletSpeedOffset);
                if(bulletSpeed1 != 1800000){
                    bestCoord.x += moveCoord.x * bulletFlyTime;
                    bestCoord.y += moveCoord.y * bulletFlyTime;
                    bestCoord.z += moveCoord.z * bulletFlyTime;
                }

                ImVec2 aimbotMouse = rotateAngleView(selfCoord, bestCoord);

                float selfStatus = Read<float>(GetPtr(me->Pointer + Offsets::CoordOffset) + Offsets::HeightOffset);

                string className = getClassName(Read<int>(weaponAddr + Offsets::ClassIdOffset));

                if (Read<int>(me->Pointer + Offsets::OpenFireOffset) == 1) {

                    float recoilTimes = 4.5 - get3dDistance(selfCoord, bestCoord, 10000);
                    recoilTimes += get3dDistance(selfCoord, bestCoord, 10000) * 0.2;

                    float recoil = Read<float>(weaponAttrAddr + Offsets::RecoilOffset);

                    if (strstr(className.c_str(), "BP_Sniper_VSS_Wrapper_C") != 0) {
                        recoil *= 0.4;
                    } else if (strstr(className.c_str(), "BP_Rifle_G36_Wrapper_C") != 0) {
                        recoil *= 0.6;
                    } else if (strstr(className.c_str(), "BP_Rifle_VAL_Wrapper_C") != 0) {
                        recoil *= 0.45;
                    } else if (strstr(className.c_str(), "BP_Rifle_AUG_Wrapper_C") != 0) {
                        recoil *= 0.7;
                    } else if (strstr(className.c_str(), "BP_Rifle_AKM_Wrapper_C") != 0) {
                        recoil *= 1.15;
                    } else if (strstr(className.c_str(), "BP_Other_MG3_Wrapper_C") != 0) {
                        recoil *= 0.2;
                    } else if (strstr(className.c_str(), "BP_Other_DP28_Wrapper_C") != 0) {
                        recoil *= 0.3;
                    }

                    if (selfStatus < 50.0f) {
                        if (strstr(className.c_str(), "BP_Rifle_M762_Wrapper_C") != 0) {
                            recoil *= 0.55;
                            aimbotMouse.x += 0.2;
                        } else if (strstr(className.c_str(), "BP_Other_M249_Wrapper_C") != 0) {
                            recoil *= 0.6;
                            aimbotMouse.x += 0.08;
                        } else {
                            recoil *= 0.4;
                        }
                    }
                    aimbotMouse.y -= recoilTimes * recoil;
                }

                if (!isfinite(aimbotMouse.x) || !isfinite(aimbotMouse.y)) {
                    continue;
                }

                ImVec2 aimbotMouseMove;

                aimbotMouseMove.x = change(AIM.Speed * getAngleDifference(aimbotMouse.x, Read<float>(me->PlayerController + Offsets::MouseOffset + 0x4)));
                aimbotMouseMove.y = change(AIM.Speed * getAngleDifference(aimbotMouse.y, Read<float>(me->PlayerController + Offsets::MouseOffset)));

                if (!isfinite(aimbotMouseMove.x) || !isfinite(aimbotMouseMove.y)) {
                    continue;
                }
                if (AddControllerYawInput != NULL && enabledAimbot) {
                    AddControllerYawInput(reinterpret_cast<void *>(me->Pointer), aimbotMouseMove.x);
                }
                if (AddControllerRollInput != NULL && enabledAimbot) {
                    AddControllerRollInput(reinterpret_cast<void *>(me->Pointer), aimbotMouseMove.y);
                }
                if (AddControllerPitchInput != NULL && enabledAimbot) {
                    AddControllerPitchInput(reinterpret_cast<void *>(me->Pointer), 0);
                }
            }
        }
    }
}
