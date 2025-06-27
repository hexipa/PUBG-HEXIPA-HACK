#import <string>
#import <dlfcn.h>
#import <stdio.h>
#import <mach/mach.h>
#import <mach-o/dyld.h>
#import <mach/vm_map.h>
#import <mach/vm_region.h>
#import <cmath> 
#import "functions/ESP/mem.h"

uintptr_t GWorldAddr = 0;
uintptr_t GNameAddr = 0;

enum SystemStatus {

    WaitingResponse,

    SystemError,

    NeedUpdate,

    LoginFailure,

    CheckFailure,

    TransmissionNormal,
};

namespace Offsets {
    int PlayerControllerOffset[3] = {0x38, 0x78, 0x30};//struct UWorld: NetDriver, ServerConnection, PlayerController
    int SelfOffset = 0x2698;//struct ASTExtraPlayerController: STExtraBaseCharacter 
    int CameraManagerOffset = 0x4d0;//struct APlayerController: PlayerCameraManager
    int ULevelOffset = 0x30;//struct UWorld: PersistentLevel
    int ObjectArrayOffset = 0xA0;
    int ObjectCountOffset = 0xA8;
    int ClassNameOffset = 0xC;
    int ClassIdOffset = 0x18;
    int CoordOffset = 0x1b0;
    int LocationOffset = 0x168;//184
    int PovOffset = 0x4b0 + 0x10;
    int TeamOffset = 0x928;//AUAEPlayerController: TeamID 
    int DeadOffset = 0xddc;
    int NameOffset = 0x8f0;
    int MeshOffset = 0x498;//SkeletalMeshComponent* Mesh;
    //int HumanOffset = 0x1a8;//Character* CharacterOwner;
    int HumanOffset = 0x1b0;
    int BonesOffset = 0x878;//StaticMesh* StaticMesh;
	 int HealthOffset = 0xdb0; 
	 int HealthMaxOffset = 0xdb4;
	 int GameState = 0x1d8;//UWorld : struct AGameStateBase* GameState;
    int AlivePlayers = 0xa9c;
    int RealPlayers = 0x748;
    int AliveTeams = 0xab8;
	 int IsAI = 0x9e9; 
    int LineOfSightToOffset = 0x788;
    int GoodsListOffset = 0x888;
    int DataBase = 0x38;
    int WeaponOneOffset = 0x28d8;//struct FAnimStatusKeyList LastUpdateStatusKeyList;
    int OpenTheSightOffset = 0x1069; //bool bIsGunADS
    int OpenFireOffset = 0x1688; //bool bIsWeaponFiring
    int HeightOffset = 0x17C;
    int AddControllerYawInputOffset = 0x868; 
    int AddControllerRollInputOffset = 0x860; 
    int AddControllerPitchInputOffset = 0x870;
    int ShootModeOffset = 0xf1c;//enum class ESTEWeaponShootType SpecialFixShootType; f04
    int WeaponAttrOffset = 0x1048;//struct UShootWeaponEntity* ShootWeaponEntityComp;
    int BulletSpeedOffset = 0x508;// float BulletFireSpeed;
    int RecoilOffset = 0xc58;//float RecoilKickADS
    int MoveCoordOffset = 0xB0;
    int MouseOffset = 0x468;//struct FRotator ControlRotation
    int GrenadeTime = 0x5cc;	//	float ExplosionTime; // Offset: 0x5cc // Size: 0x04
}

struct EnemyInfo {
    Vector3 pos;
    int Hp;
    int MaxHp;
    std::string Name;
};

struct enemy_t{
    void *ptr;
    void *trans;
    Vector3 pos;
    ImVec2 w2spos;    
    ImVec2 w2stop;    
    float Hp;
    float MaxHp;
    char* Name;
    
};
enemy_t *enemy;

struct Veh_t{
    ImVec2 w2spos;    
    ImVec2 w2stop;    
};
Veh_t *Veh;

struct MapLoot_t{
    ImVec2 w2spos;    
    ImVec2 w2stop;    
};
MapLoot_t *MapLoot;

struct DeadBoxLoot_t{
    ImVec2 w2spos;    
    ImVec2 w2stop;    
};
DeadBoxLoot_t *DeadBoxLoot;

struct me_t{
    uintptr_t NetDriver;
    uintptr_t ServerConnection;
    uintptr_t PlayerController;
    uintptr_t Pointer;
    uintptr_t Camera;
    uintptr_t ULevel;
    uintptr_t ObjectArray;
    int ObjectCount;
    uintptr_t GameState;
    int AlivePlayers;
    int RealPlayers;
    int AliveTeams;
};
me_t *me;

struct AIM {
    float Speed = 0.25;
    float AimbotRadius = 120;
    int Distance = 400;
    int Mode = 1;
    int AimbotParts = 0;
    bool IgnoreKnocked = false;
    bool IgnoreBots = false;
    bool VisibleCheck = false;
    bool Start = true;

}AIM;

struct ESP {
    int Style = 0;
    bool Stream = true;
    bool Draw = true;
    bool Line = true;
    bool Hp = true;
    bool Distance = true;
    bool Bones = true;
    bool Name = true;
    bool Info = true;
    bool Box = false;
    bool Arrows = true;
    bool MatchInfo = true;
    bool Vehicles = true;
    bool Grenade = true;
    bool Loot = false;
    bool LootGun = false;
    bool LootGunSettings = false;
    bool m416 = true;
    bool m762 = true;
    bool aug = true;
    bool akm = true;
    bool groza = true;
    bool scar = true;
    bool ace32 = true;
    bool Bullets = true;
    bool mm556 = true;
    bool mm762 = true;
    bool mm9 = false;
    bool bmg50 = true;
    bool acp45 = false;
    bool guage12 = false;
	 bool Armor = true;
    bool Armor1 = false;    
    bool Armor2 = true;    
    bool Armor3 = true; 
    bool Helmet = true;
    bool Helmet1 = false;    
    bool Helmet2 = true;    
    bool Helmet3 = true; 
	 bool Backpack = true;
    bool Backpack1 = false;    
    bool Backpack2 = true;    
    bool Backpack3 = true;   
}ESP;

struct Vehicle{
    std::string name;
};

Vehicle GetVehicle(std::string name){
    Vehicle tempVehicle; 
    if(strstr(name.c_str(), "BRDM")){
        tempVehicle.name = "BRDM";
        return tempVehicle;
    }else if(strstr(name.c_str(), "Dacia")){
        tempVehicle.name = "Dacia";
        return tempVehicle;
    }else if(strstr(name.c_str(), "MiniBus")){
        tempVehicle.name = "Minibus";
        return tempVehicle;
    }else if(strstr(name.c_str(), "Mirado")){
        tempVehicle.name = "Mirado";
        return tempVehicle;
    }else if(strstr(name.c_str(), "Motorcycle")){
        tempVehicle.name = "Motorcycle";
        return tempVehicle;
    }else if(strstr(name.c_str(), "MotorcycleCart")){
        tempVehicle.name = "Motorcycle cart";
        return tempVehicle;
    }else if(strstr(name.c_str(), "Rony")){
        tempVehicle.name = "Rony";
        return tempVehicle;
    }else if(strstr(name.c_str(), "Scooter")){
        tempVehicle.name = "Scooter";
        return tempVehicle;
    }else if(strstr(name.c_str(), "Snowbike")){
        tempVehicle.name = "Snowbike";
        return tempVehicle;
    }else if(strstr(name.c_str(), "Snowmobile")){
        tempVehicle.name = "Snowmobile";
        return tempVehicle;
    }else if(strstr(name.c_str(), "VH_Tuk")){
        tempVehicle.name = "Tuk Tuk";
        return tempVehicle;
    }else if(strstr(name.c_str(), "UAZ")){
        tempVehicle.name = "UAZ";
        return tempVehicle;
    }else if(strstr(name.c_str(), "Buggy")){
        tempVehicle.name = "Buggy";
        return tempVehicle;
    }else if(strstr(name.c_str(), "UTV")){
        tempVehicle.name = "UTV";
        return tempVehicle;
    }else if(strstr(name.c_str(), "Bigfoot")){
        tempVehicle.name = "Bigfoot";
        return tempVehicle;
    }else if(strstr(name.c_str(), "CoupeRB")){
        tempVehicle.name = "CoupeRB";
        return tempVehicle;
    }else if(strstr(name.c_str(), "ATV")){
        tempVehicle.name = "ATV";
        return tempVehicle;
    }else if(strstr(name.c_str(), "Motorglider")){
        tempVehicle.name = "Motorglider";
        return tempVehicle;
    }else{
        tempVehicle.name = "Unknown";
        return tempVehicle;
    }
}

/*char *getClassName(int classId) {
    char *buf = (char *) malloc(64);
    if (classId > 0 && classId < 2000000) {
        int page = classId / 16384;
        int index = classId % 16384;
        uintptr_t pageAddr = *(uintptr_t*)((uintptr_t)GNameAddr + page * sizeof(uintptr_t));

        uintptr_t PreNameAddr = *(uintptr_t*)((uintptr_t)pageAddr + index * sizeof(uintptr_t));
        uintptr_t NameAddr = *(uintptr_t*)((uintptr_t)PreNameAddr + Offsets::ClassNameOffset); 
        readMemory(NameAddr, 64, buf);
    }
    return buf;
}*/

/*char *getClassName(int classId) {
    char *buf = (char *) malloc(64);
    if (classId > 0 && classId < 2000000) {
        int page = classId / 16384;
        int index = classId % 16384;
        uintptr_t pageAddr = GetPtr(GNameAddr + page * sizeof(uintptr_t));
        uintptr_t nameAddr = GetPtr(pageAddr + index * sizeof(uintptr_t)) + Offsets::ClassNameOffset;
        _read(nameAddr, buf, 64);
    }
    return buf;
}*/

char *getClassName(int classId) {
    static char buf[64]; // Потоконебезопасно
    if (classId > 0 && classId < 2000000) {
        int page = classId / 16384;
        int index = classId % 16384;
        uintptr_t pageAddr = GetPtr(GNameAddr + page * sizeof(uintptr_t));
        uintptr_t nameAddr = GetPtr(pageAddr + index * sizeof(uintptr_t)) + Offsets::ClassNameOffset;
        _read(nameAddr, buf, 64);
    } else {
        buf[0] = '\0';
    }
    return buf;
}

/*char *getPlayerName(uintptr_t addr) {
    char *buf = (char *) malloc(448);
    unsigned short buf16[16] = {0};
    _read(addr, buf16, 28);
    unsigned short *tempbuf16 = buf16;
    char *tempbuf8 = buf;
    char *buf8 = tempbuf8 + 32;
    while (tempbuf16 < tempbuf16 + 28) {
        if (*tempbuf16 <= 0x007F && tempbuf8 + 1 < buf8) {
            *tempbuf8++ = (char) *tempbuf16;
        } else if (*tempbuf16 >= 0x0080 && *tempbuf16 <= 0x07FF && tempbuf8 + 2 < buf8) {
            *tempbuf8++ = (*tempbuf16 >> 6) | 0xC0;
            *tempbuf8++ = (*tempbuf16 & 0x3F) | 0x80;
        } else if (*tempbuf16 >= 0x0800 && *tempbuf16 <= 0xFFFF && tempbuf8 + 3 < buf8) {
            *tempbuf8++ = (*tempbuf16 >> 12) | 0xE0;
            *tempbuf8++ = ((*tempbuf16 >> 6) & 0x3F) | 0x80;
            *tempbuf8++ = (*tempbuf16 & 0x3F) | 0x80;
        } else {
            break;
        }
        tempbuf16++;
    }
    return buf;
}*/

char *getPlayerName(uintptr_t addr) {
    static char buf[448]; // Фиксированный статический буфер
    unsigned short buf16[16] = {0};
    
    // Чтение данных из адреса
    _read(addr, buf16, sizeof(buf16));
    
    unsigned short *tempbuf16 = buf16;
    char *tempbuf8 = buf;
    char *buf8 = tempbuf8 + 32; // Граница для записи

    while (tempbuf16 < buf16 + 16) { // Ограничение по размеру buf16
        if (*tempbuf16 <= 0x007F && tempbuf8 + 1 < buf8) {
            *tempbuf8++ = (char)*tempbuf16;
        } else if (*tempbuf16 >= 0x0080 && *tempbuf16 <= 0x07FF && tempbuf8 + 2 < buf8) {
            *tempbuf8++ = (*tempbuf16 >> 6) | 0xC0;
            *tempbuf8++ = (*tempbuf16 & 0x3F) | 0x80;
        } else if (*tempbuf16 >= 0x0800 && *tempbuf16 <= 0xFFFF && tempbuf8 + 3 < buf8) {
            *tempbuf8++ = (*tempbuf16 >> 12) | 0xE0;
            *tempbuf8++ = ((*tempbuf16 >> 6) & 0x3F) | 0x80;
            *tempbuf8++ = (*tempbuf16 & 0x3F) | 0x80;
        } else {
            break;
        }
        tempbuf16++;
    }

    // Завершаем строку null-терминатором
    *tempbuf8 = '\0';

    return buf;
}

struct Ue4Transform {
    ImVec4 rotation;
    Vector3 translation;
    Vector3 scale3d;
};

struct Ue4Matrix {
    float matrix[4][4];
    
    float *operator[](int index) {
        return matrix[index];
    }
    
};

struct Ue4Rotator {
    float pitch;
    float yaw;
    float roll;
};

struct MinimalViewInfo {
    Vector3 location;
    Vector3 locationLocalSpace;
    Ue4Rotator rotation;
    float fov;
};


static inline ImVec2  operator*(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x * rhs, lhs.y * rhs); }
static inline ImVec2  operator/(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x / rhs, lhs.y / rhs); }
static inline ImVec2  operator+(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x + rhs, lhs.y + rhs); }
static inline ImVec2  operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2  operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
static inline ImVec2  operator-(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x - rhs, lhs.y - rhs); }
static inline ImVec2  operator*(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }
static inline ImVec2  operator/(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x / rhs.x, lhs.y / rhs.y); }
static inline ImVec2& operator*=(ImVec2& lhs, const float rhs) { lhs.x *= rhs; lhs.y *= rhs; return lhs; }
static inline ImVec2& operator/=(ImVec2& lhs, const float rhs) { lhs.x /= rhs; lhs.y /= rhs; return lhs; }
static inline ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs) { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
static inline ImVec2& operator-=(ImVec2& lhs, const ImVec2& rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
static inline ImVec2& operator*=(ImVec2& lhs, const ImVec2& rhs) { lhs.x *= rhs.x; lhs.y *= rhs.y; return lhs; }
static inline ImVec2& operator/=(ImVec2& lhs, const ImVec2& rhs) { lhs.x /= rhs.x; lhs.y /= rhs.y; return lhs; }
static inline ImVec4  operator+(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }
static inline ImVec4  operator-(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); }
static inline ImVec4  operator*(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); }

inline ImVec2 flooring(ImVec2 vec) {
    return { static_cast<float>(vec.x), (float)int(vec.y) };
}

inline ImVec2 flooring(float x, float y) {
    return { (float)int(x), (float)int(y) };
}

inline ImVec2 flooring(int x, int y) {
    return { (float)x, (float)y };
}

Vector3 matrixToVector(Ue4Matrix matrix) {
    return Vector3(matrix[3][0], matrix[3][1], matrix[3][2]);
}

Ue4Matrix matrixMulti(Ue4Matrix m1, Ue4Matrix m2) {
    Ue4Matrix matrix = Ue4Matrix();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                matrix[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }
    return matrix;
}

Ue4Matrix transformToMatrix(Ue4Transform transform) {
    Ue4Matrix matrix;
    
    matrix[3][0] = transform.translation.x;
    matrix[3][1] = transform.translation.y;
    matrix[3][2] = transform.translation.z;
    
    float x2 = transform.rotation.x + transform.rotation.x;
    float y2 = transform.rotation.y + transform.rotation.y;
    float z2 = transform.rotation.z + transform.rotation.z;
    
    float xx2 = transform.rotation.x * x2;
    float yy2 = transform.rotation.y * y2;
    float zz2 = transform.rotation.z * z2;
    
    matrix[0][0] = (1.0f - (yy2 + zz2)) * transform.scale3d.x;
    matrix[1][1] = (1.0f - (xx2 + zz2)) * transform.scale3d.y;
    matrix[2][2] = (1.0f - (xx2 + yy2)) * transform.scale3d.z;
    
    float yz2 = transform.rotation.y * z2;
    float wx2 = transform.rotation.w * x2;
    matrix[2][1] = (yz2 - wx2) * transform.scale3d.z;
    matrix[1][2] = (yz2 + wx2) * transform.scale3d.y;
    
    float xy2 = transform.rotation.x * y2;
    float wz2 = transform.rotation.w * z2;
    matrix[1][0] = (xy2 - wz2) * transform.scale3d.y;
    matrix[0][1] = (xy2 + wz2) * transform.scale3d.x;
    
    float xz2 = transform.rotation.x * z2;
    float wy2 = transform.rotation.w * y2;
    matrix[2][0] = (xz2 + wy2) * transform.scale3d.z;
    matrix[0][2] = (xz2 - wy2) * transform.scale3d.x;
    
    matrix[0][3] = 0;
    matrix[1][3] = 0;
    matrix[2][3] = 0;
    matrix[3][3] = 1;
    
    return matrix;
}

Ue4Matrix rotatorToMatrix(Ue4Rotator rotation) {
    float radPitch = rotation.pitch * ((float) M_PI / 180.0f);
    float radYaw = rotation.yaw * ((float) M_PI / 180.0f);
    float radRoll = rotation.roll * ((float) M_PI / 180.0f);
    
    float SP = sinf(radPitch);
    float CP = cosf(radPitch);
    float SY = sinf(radYaw);
    float CY = cosf(radYaw);
    float SR = sinf(radRoll);
    float CR = cosf(radRoll);
    
    Ue4Matrix matrix;
    
    matrix[0][0] = (CP * CY);
    matrix[0][1] = (CP * SY);
    matrix[0][2] = (SP);
    matrix[0][3] = 0;
    
    matrix[1][0] = (SR * SP * CY - CR * SY);
    matrix[1][1] = (SR * SP * SY + CR * CY);
    matrix[1][2] = (-SR * CP);
    matrix[1][3] = 0;
    
    matrix[2][0] = (-(CR * SP * CY + SR * SY));
    matrix[2][1] = (CY * SR - CR * SP * SY);
    matrix[2][2] = (CR * CP);
    matrix[2][3] = 0;
    
    matrix[3][0] = 0;
    matrix[3][1] = 0;
    matrix[3][2] = 0;
    matrix[3][3] = 1;
    
    return matrix;
}

ImVec2 worldToScreen(Vector3 worldLocation, MinimalViewInfo camViewInfo, ImVec2 screenCenter) {
    Ue4Matrix tempMatrix = rotatorToMatrix(camViewInfo.rotation);
    
    Vector3 vAxisX(tempMatrix[0][0], tempMatrix[0][1], tempMatrix[0][2]);
    Vector3 vAxisY(tempMatrix[1][0], tempMatrix[1][1], tempMatrix[1][2]);
    Vector3 vAxisZ(tempMatrix[2][0], tempMatrix[2][1], tempMatrix[2][2]);
    
    Vector3 vDelta = worldLocation - camViewInfo.location;
    
    Vector3 vTransformed(Vector3::Dot(vDelta, vAxisY), Vector3::Dot(vDelta, vAxisZ), Vector3::Dot(vDelta, vAxisX));
    
    if (vTransformed.z < 1.0f) {
        vTransformed.z = 1.0f;
    }
    ImVec2 screenCoord;
    screenCoord.x = (screenCenter.x + vTransformed.x * (screenCenter.x / tanf(camViewInfo.fov * ((float) M_PI / 360.0f))) / vTransformed.z);
    screenCoord.y = (screenCenter.y - vTransformed.y * (screenCenter.x / tanf(camViewInfo.fov * ((float) M_PI / 360.0f))) / vTransformed.z);
    return screenCoord;
}

float getAngleDifference(float angle1, float angle2) {
    float diff = fmod(angle2 - angle1 + 180, 360) - 180;
    return diff < -180 ? diff + 360 : diff;
}

float change(float num) {
    if (num < 0) {
        return abs(num);
    } else if (num > 0) {
        return num - num * 2;
    }
    return num;
}

float get2dDistance(ImVec2 self, ImVec2 object) {
    float osx = self.x - object.x;
    float osy = self.y - object.y;
    
    return sqrt(osx * osx + osy * osy);
}

float get3dDistance(Vector3 self, Vector3 object, float divice) {
    Vector3 xyz;
    xyz.x = self.x - object.x;
    xyz.y = self.y - object.y;
    xyz.z = self.z - object.z;
    return sqrt(pow(xyz.x, 2) + pow(xyz.y, 2) + pow(xyz.z, 2)) / divice;
}

ImVec2 rotateCoord(float angle, ImVec2 coord) {
    float s = sin(angle * M_PI / 180);
    float c = cos(angle * M_PI / 180);
    
    return {coord.x * c + coord.y * s, -coord.x * s + coord.y * c};
}

float rotateAngle(Vector3 selfCoord, Vector3 targetCoord) {
    float osx = targetCoord.x - selfCoord.x;
    float osy = targetCoord.y - selfCoord.y;
    return (float) (atan2(osy, osx) * 180 / M_PI);
}

ImVec2 rotateAngleView(Vector3 selfCoord, Vector3 targetCoord) {
    
    float osx = targetCoord.x - selfCoord.x;
    float osy = targetCoord.y - selfCoord.y;
    float osz = targetCoord.z - selfCoord.z;
    
    return {(float) (atan2(osy, osx) * 180 / M_PI), (float) (atan2(osz, sqrt(osx * osx + osy * osy)) * 180 / M_PI)};
}

bool isRecycled(const char *name) {
    return strstr(name, "ecycled") != 0;
}

Vector3 getBone(uintptr_t human, uintptr_t bones, int part) {
    Ue4Transform actorftf;
    _read(human, &actorftf.rotation, sizeof(ImVec4));
    _read(human + 0x10, &actorftf.translation, sizeof(Vector3));
    _read(human + 0x20, &actorftf.scale3d, sizeof(Vector3));
    
    Ue4Matrix actormatrix = transformToMatrix(actorftf);
    
    Ue4Transform boneftf;
    
    _read(bones + part * 48, &boneftf.rotation, sizeof(ImVec4));
    _read(bones + part * 48 + 0x10, &boneftf.translation, sizeof(Vector3));
    _read(bones + part * 48 + 0x20, &boneftf.scale3d, sizeof(Vector3));
    
    Ue4Matrix bonematrix = transformToMatrix(boneftf);
    
    return matrixToVector(matrixMulti(bonematrix, actormatrix));
}

bool getBone2d(MinimalViewInfo pov,ImVec2 screen, uintptr_t human, uintptr_t bones, int part,ImVec2 &buf) {

    Vector3 newmatrix = getBone(human, bones, part);

    if(part == 5){newmatrix.z += 10;}

    buf = worldToScreen(newmatrix, pov, screen);

    return buf.x != 0 && buf.y != 0;
}

struct BonesData {
    ImVec2 head;
    ImVec2 pit;
    ImVec2 pelvis;
    ImVec2 lcollar;
    ImVec2 rcollar;
    ImVec2 lelbow;
    ImVec2 relbow;
    ImVec2 lwrist;
    ImVec2 rwrist;
    ImVec2 lthigh;
    ImVec2 rthigh;
    ImVec2 lknee;
    ImVec2 rknee;
    ImVec2 lankle;
    ImVec2 rankle;
ImVec2 a2;
ImVec2 a3;
ImVec2 a6;
ImVec2 a7;
ImVec2 a8;
ImVec2 a9;
ImVec2 a10;
ImVec2 a13;
ImVec2 a14;
ImVec2 a15;
ImVec2 a16;
ImVec2 a17;
ImVec2 a18;
ImVec2 a19;
ImVec2 a20;
ImVec2 a21;
ImVec2 a22;
ImVec2 a23;
ImVec2 a24;
ImVec2 a25;
ImVec2 a26;
ImVec2 a27;
ImVec2 a28;
ImVec2 a29;
ImVec2 a30;
ImVec2 a31;
ImVec2 a34;
ImVec2 a35;
ImVec2 a36;
ImVec2 a37;
ImVec2 a38;
ImVec2 a39;
ImVec2 a40;
ImVec2 a41;
ImVec2 a42;
ImVec2 a43;
ImVec2 a44;
ImVec2 a45;
ImVec2 a46;
ImVec2 a47;
ImVec2 a48;
ImVec2 a49;
ImVec2 a50;
ImVec2 a51;
ImVec2 a55;
ImVec2 a59;
ImVec2 a60;
ImVec2 a61;
ImVec2 a64;
ImVec2 a65;
ImVec2 a66;
ImVec2 a67;
ImVec2 a68;
ImVec2 a69;
ImVec2 a70;
};

float Get2dDistance(const ImVec2& p1, const ImVec2& p2) {
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    return std::sqrt(dx * dx + dy * dy);
}


long TeamID_Color;
long TeamID_Color2;
        
int alpha1 = 255;
int alpha2 = 130;

void teamColor(int teamNumber){

                    switch (teamNumber) {

                        case NULL:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 1:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 2:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 3:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 4:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 5:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 6:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 7:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 8:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 9:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 10:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 11:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 12:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 13:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 14:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 15:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 16:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 17:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 18:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 19:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 20:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 21:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 22:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 23:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 24:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 25:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 26:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 27:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 28:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 29:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 30:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 31:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 32:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 33:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 34:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 35:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 36:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 37:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 38:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 39:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 40:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 41:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 42:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 43:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 44:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 45:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 46:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 47:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 48:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 49:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 50:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 51:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 52:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 53:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 54:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 55:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 56:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 57:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 58:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 59:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 60:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 61:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 62:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 63:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 64:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 65:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 66:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 67:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 68:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 69:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 70:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 71:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 72:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 73:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 74:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 75:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 76:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 77:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 78:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 79:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 80:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 81:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 82:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 83:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 84:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 85:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 86:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 87:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 88:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 89:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 90:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 91:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 92:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 93:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 94:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 95:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 96:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 97:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 98:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 99:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 100:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 101:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 102:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 103:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 104:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 105:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 106:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 107:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 108:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 109:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 110:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 111:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 112:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 113:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 114:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 115:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 116:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 117:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 118:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 119:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 120:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 121:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 122:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 123:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 124:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 125:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 126:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 127:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 128:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 129:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 130:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 131:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 132:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 133:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 134:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 135:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 136:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 137:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 138:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 139:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 140:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            //red(140)
                            
                        case 141:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 142:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 143:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 144:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 145:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 146:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 147:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 148:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 149:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 150:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 151:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 152:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 153:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 154:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 155:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 156:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 157:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 158:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 159:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 160:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 161:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 162:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 163:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 164:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 165:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 166:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 167:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 168:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 169:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 170:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 171:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 172:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 173:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 174:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 175:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 176:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 177:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 178:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 179:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 180:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 181:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 182:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 183:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 184:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 185:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 186:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 187:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 188:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 189:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 190:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 191:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 192:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 193:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 194:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 195:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 196:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 197:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 198:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 199:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 200:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 201:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 202:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 203:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 204:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 205:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 206:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 207:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 208:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 209:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 210:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 211:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 212:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 213:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 214:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 215:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 216:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 217:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 218:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 219:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 220:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 221:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 222:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 223:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 224:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 225:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 226:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 227:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 228:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 229:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 230:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 231:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 232:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 233:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 234:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 235:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 236:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 237:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 238:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 239:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 240:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 241:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 242:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 243:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 244:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 245:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                            
                        case 246:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            //TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 247:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color =IM_COL32(test1, test2, test3, alpha1);
                            //TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color2 =IM_COL32(test4, test5, test6, alpha2);
                            break;
                            
                        case 248:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            //TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 249:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 250:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 251:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 252:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            //TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                            
                        case 253:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            //TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 254:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color =IM_COL32(test1, test2, test3, alpha1);
                            //TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color2 =IM_COL32(test4, test5, test6, alpha2);
                            break;
                            
                        case 255:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            //TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 256:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 257:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 258:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 259:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            //TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 260:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            //TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 261:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color =IM_COL32(test1, test2, test3, alpha1);
                            //TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color2 =IM_COL32(test4, test5, test6, alpha2);
                            break;
                            
                        case 262:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            //TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 263:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 264:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 265:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 266:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            //TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 267:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            //TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 268:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color =IM_COL32(test1, test2, test3, alpha1);
                            //TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color2 =IM_COL32(test4, test5, test6, alpha2);
                            break;
                            
                        case 269:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            //TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 270:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 271:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 272:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 273:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            //TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 274:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            //TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 275:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color =IM_COL32(test1, test2, test3, alpha1);
                            //TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color2 =IM_COL32(test4, test5, test6, alpha2);
                            break;
                            
                        case 276:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            //TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 277:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 278:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 279:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 280:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            //TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 281:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            //TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 282:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color =IM_COL32(test1, test2, test3, alpha1);
                            //TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color2 =IM_COL32(test4, test5, test6, alpha2);
                            break;
                            
                        case 283:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            //TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 284:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 285:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 286:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 287:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            //TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 288:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            //TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 289:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color =IM_COL32(test1, test2, test3, alpha1);
                            //TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color2 =IM_COL32(test4, test5, test6, alpha2);
                            break;
                            
                        case 290:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            //TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 291:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 292:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 293:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 294:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            //TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 295:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            //TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 296:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color =IM_COL32(test1, test2, test3, alpha1);
                            //TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color2 =IM_COL32(test4, test5, test6, alpha2);
                            break;
                            
                        case 297:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            //TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 298:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 299:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 300:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 301:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            //TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 302:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            //TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 303:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color =IM_COL32(test1, test2, test3, alpha1);
                            //TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color2 =IM_COL32(test4, test5, test6, alpha2);
                            break;
                            
                        case 304:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            //TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 305:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 306:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 307:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 308:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            //TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 309:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            //TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 310:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color =IM_COL32(test1, test2, test3, alpha1);
                            //TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color2 =IM_COL32(test4, test5, test6, alpha2);
                            break;
                            
                        case 311:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            //TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 312:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 313:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 314:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 315:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            //TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 316:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            //TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 317:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color =IM_COL32(test1, test2, test3, alpha1);
                            //TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color2 =IM_COL32(test4, test5, test6, alpha2);
                            break;
                            
                        case 318:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            //TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 319:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 320:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 321:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 322:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            //TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 323:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            //TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 324:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color =IM_COL32(test1, test2, test3, alpha1);
                            //TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color2 =IM_COL32(test4, test5, test6, alpha2);
                            break;
                            
                        case 325:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            //TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 326:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 327:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 328:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 329:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            //TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 330:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            //TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 331:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color =IM_COL32(test1, test2, test3, alpha1);
                            //TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color2 =IM_COL32(test4, test5, test6, alpha2);
                            break;
                            
                        case 332:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            //TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 333:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 334:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 335:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 336:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            //TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                            /////
                        case 337:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 338:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 339:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            //TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 340:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color =IM_COL32(test1, test2, test3, alpha1);
                            //TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color2 =IM_COL32(test4, test5, test6, alpha2);
                            break;
                            
                        case 341:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            //TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 342:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 343:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 345:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 346:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            //TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 347:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            //TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 348:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color =IM_COL32(test1, test2, test3, alpha1);
                            //TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color2 =IM_COL32(test4, test5, test6, alpha2);
                            break;
                            
                        case  349:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            //TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 350:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 351:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 352:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 353:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            //TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 354:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            //TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 355:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color =IM_COL32(test1, test2, test3, alpha1);
                            //TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color2 =IM_COL32(test4, test5, test6, alpha2);
                            break;
                            
                        case 356:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            //TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 357:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 358:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 359:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 360:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            //TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 361:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            //TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 362:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color =IM_COL32(test1, test2, test3, alpha1);
                            //TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color2 =IM_COL32(test4, test5, test6, alpha2);
                            break;
                            
                        case 363:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            //TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 364:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 365:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 366:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 367:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            //TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 368:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            //TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 369:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color =IM_COL32(test1, test2, test3, alpha1);
                            //TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color2 =IM_COL32(test4, test5, test6, alpha2);
                            break;
                            
                        case 370:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            //TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 371:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 372:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 373:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 374:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            //TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 375:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            //TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 376:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color =IM_COL32(test1, test2, test3, alpha1);
                            //TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color2 =IM_COL32(test4, test5, test6, alpha2);
                            break;
                            
                        case 377:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            //TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 378:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 379:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 380:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 381:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            //TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 382:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            //TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 383:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color =IM_COL32(test1, test2, test3, alpha1);
                            //TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color2 =IM_COL32(test4, test5, test6, alpha2);
                            break;
                            
                        case 384:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            //TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 385:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 386:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 387:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 388:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            //TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 389:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 390:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 391:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 392:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 393:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 394:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 395:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 396:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 397:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 398:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 399:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            //TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 400:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color =IM_COL32(test1, test2, test3, alpha1);
                            //TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            //    TeamID_Color2 =IM_COL32(test4, test5, test6, alpha2);
                            break;
                            
                        case 401:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            //TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 402:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 403:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 404:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            //TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 405:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            //TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 993:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 994:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                            
                        case 995:
                            TeamID_Color =IM_COL32(200, 185, 40, alpha1);
                            TeamID_Color2 =IM_COL32(185, 180, 70, alpha2);
                            break;
                            
                        case 996:
                            TeamID_Color =IM_COL32(100, 150, 70, alpha1);
                            TeamID_Color2 =IM_COL32(100, 190, 80, alpha2);
                            break;
                            
                        case 997:
                            TeamID_Color =IM_COL32(60, 165, 105, alpha1);
                            TeamID_Color2 =IM_COL32(70, 160, 100, alpha2);
                            break;
                            
                        case 998:
                            TeamID_Color =IM_COL32(75, 110, 150, alpha1);
                            TeamID_Color2 =IM_COL32(150, 190, 225, alpha2);
                            break;
                            
                        case 999:
                            TeamID_Color =IM_COL32(80, 60, 140, alpha1);
                            TeamID_Color2 =IM_COL32(150, 100, 220, alpha2);
                            break;
                            
                        case 1000:
                            TeamID_Color =IM_COL32(135, 60, 130, alpha1);
                            TeamID_Color2 =IM_COL32(150, 50, 160, alpha2);
                            break;
                            
                        case 1001:
                            TeamID_Color =IM_COL32(140, 70, 60, alpha1);
                            TeamID_Color2 =IM_COL32(200, 80, 80, alpha2);
                            break;
                    }

}

/*
struct Box3D {
    ImVec2 s1;
    ImVec2 s2;
    ImVec2 s3;
    ImVec2 s4;
    ImVec2 s5;
    ImVec2 s6;
    ImVec2 s7;
    ImVec2 s8;
};

Box3D GetBox3D(MinimalViewInfo pov, Vector3 origin, Vector3 extends, ImVec2 screenPos) {
    origin.x -= extends.x / 2.f;
    origin.y -= extends.y / 2.f;
    origin.z -= extends.z / 2.f;
    
    
    Vector3 one = origin;
    Vector3 two = origin; two.x += extends.x;
    Vector3 three = origin; three.x += extends.x; three.y += extends.y;
    Vector3 four = origin; four.y += extends.y;
    
    Vector3 five = one; five.z += extends.z;
    Vector3 six = two; six.z += extends.z;
    Vector3 seven = three; seven.z += extends.z;
    Vector3 eight = four; eight.z += extends.z;
    
    ImVec2 s1, s2, s3, s4, s5, s6, s7, s8;
    
    s1 = worldToScreen(one, pov,screenPos);
    s2 = worldToScreen(two, pov,screenPos);
    s3 = worldToScreen(three, pov,screenPos);
    s4 = worldToScreen(four, pov,screenPos);
    s5 = worldToScreen(five, pov,screenPos);
    s6 = worldToScreen(six, pov,screenPos);
    s7 = worldToScreen(seven, pov,screenPos);
    s8 = worldToScreen(eight, pov,screenPos);
    
    Box3D Box3D;
    Box3D.s1 = s1;
    Box3D.s2 = s2;
    Box3D.s3 = s3;
    Box3D.s4 = s4;
    Box3D.s5 = s5;
    Box3D.s6 = s6;
    Box3D.s7 = s7;
    Box3D.s8 = s8;
    return Box3D;
}
*/

struct Box3D {
    ImVec2 s[8];  
};


Vector3 RotateYaw(Vector3 v, float yaw) {
    float yawNormalized = abs(yaw);
    float rad = yawNormalized * (M_PI / 180.0f);  
    float cosYaw = cos(rad);
    float sinYaw = sin(rad);

    return Vector3(
        v.x * cosYaw - v.y * sinYaw,
        v.x * sinYaw + v.y * cosYaw,
        v.z
    );
}

Box3D GetBox3D(MinimalViewInfo pov, Vector3 origin, Vector3 extends, ImVec2 screenPos, Vector3 rotation) {

    origin -= extends * 0.50000f;

    Vector3 points[8] = {
        origin,
        origin + Vector3(extends.x, 0, 0),
        origin + Vector3(extends.x, extends.y, 0),
        origin + Vector3(0, extends.y, 0),
        origin + Vector3(0, 0, extends.z),
        origin + Vector3(extends.x, 0, extends.z),
        origin + Vector3(extends.x, extends.y, extends.z),
        origin + Vector3(0, extends.y, extends.z),
    };

    Box3D box;
    for (int i = 0; i < 8; ++i) {
        points[i] = RotateYaw(points[i] - origin, rotation.y) + origin;
        box.s[i] = worldToScreen(points[i], pov, screenPos);
    }

    return box;
}

struct RectBox3D {
    ImVec2 p[32];
};

RectBox3D GetRectBox3D(MinimalViewInfo pov, Vector3 origin, Vector3 extends, ImVec2 screenPos) {
    origin -= extends * 0.50000f;

    Vector3 points[32] = {
        origin,
        origin + Vector3(0, 0, 40),
        origin + Vector3(0, 25, 0),
        origin + Vector3(25, 0, 0),
        origin + Vector3(extends.x, 0, 0),
        origin + Vector3(extends.x, 0, 40),
        origin + Vector3(extends.x, 25, 0),
        origin + Vector3(extends.x - 25, 0, 0),
        origin + Vector3(extends.x, extends.y, 0),
        origin + Vector3(extends.x, extends.y, 40),
        origin + Vector3(extends.x, extends.y - 25, 0),
        origin + Vector3(extends.x - 25, extends.y, 0),
        origin + Vector3(0, extends.y, 0),
        origin + Vector3(0, extends.y, 40),
        origin + Vector3(0, extends.y - 25, 0),
        origin + Vector3(25, extends.y, 0),
        origin + Vector3(0, 0, extends.z),
        origin + Vector3(0, 0, extends.z - 40),
        origin + Vector3(0, 25, extends.z),
        origin + Vector3(25, 0, extends.z),
        origin + Vector3(extends.x, 0, extends.z),
        origin + Vector3(extends.x, 0, extends.z - 40),
        origin + Vector3(extends.x, 25, extends.z),
        origin + Vector3(extends.x - 25, 0, extends.z),
        origin + Vector3(extends.x, extends.y, extends.z),
        origin + Vector3(extends.x, extends.y, extends.z - 40),
        origin + Vector3(extends.x, extends.y - 25, extends.z),
        origin + Vector3(extends.x - 25, extends.y, extends.z),
        origin + Vector3(0, extends.y, extends.z),
        origin + Vector3(0, extends.y, extends.z - 40),
        origin + Vector3(0, extends.y - 25, extends.z),
        origin + Vector3(25, extends.y, extends.z),
    };

    RectBox3D box;
    for (int i = 0; i < 32; ++i) {
        box.p[i] = worldToScreen(points[i], pov, screenPos);
    }

    return box;

}


struct Circle3D {
    ImVec2 top[12]; 
    ImVec2 halftop[12]; 
    ImVec2 center[12]; 
    ImVec2 halfbot[12]; 
    ImVec2 bottom[12]; 
};

Circle3D GetCircle3D(MinimalViewInfo pov, Vector3 origin, float radius, ImVec2 screenPos) {
    Circle3D circle;

    const float angleStep = 2.0f * M_PI / 12.0f;

    for (int i = 0; i < 12; ++i) {
        float angle = i * angleStep; 
        Vector3 point = origin;
        point.x += radius * cos(angle); 
        point.y += radius * sin(angle); 

        Vector3 point1 = { origin.x, origin.y, origin.z + 180 };
        point1.x += radius * 0.5 * cos(angle); 
        point1.y += radius * 0.5 * sin(angle); 

        Vector3 point3 = { origin.x, origin.y, origin.z + 90 };
        point3.x += radius * 0.85 * cos(angle); 
        point3.y += radius * 0.85 * sin(angle); 

        Vector3 point2 = { origin.x, origin.y, origin.z - 180 };
        point2.x += radius * 0.5 * cos(angle); 
        point2.y += radius * 0.5 * sin(angle); 

        Vector3 point4 = { origin.x, origin.y, origin.z - 90 };
        point4.x += radius * 0.85 * cos(angle); 
        point4.y += radius * 0.85 * sin(angle); 

        circle.center[i] = worldToScreen(point, pov, screenPos);
        circle.top[i] = worldToScreen(point1, pov, screenPos);
        circle.halftop[i] = worldToScreen(point3, pov, screenPos);
        circle.bottom[i] = worldToScreen(point2, pov, screenPos);
        circle.halfbot[i] = worldToScreen(point4, pov, screenPos);
    }

    return circle;
}

struct Circle2D {
    ImVec2 center[36]; 
    ImVec2 half[36]; 
    ImVec2 max[36]; 
};

Circle2D GetCircle2D(MinimalViewInfo pov, Vector3 origin, float radius, ImVec2 screenPos) {
    Circle2D circle;

    const float angleStep = 2.0f * M_PI / 36.0f;

    for (int i = 0; i < 36; ++i) {
        float angle = i * angleStep; 
        Vector3 point = origin;
        point.x += radius * 0.3 * cos(angle); 
        point.y += radius * 0.3 * sin(angle); 

        Vector3 point1 = origin;
        point1.x += radius * 0.7 * cos(angle); 
        point1.y += radius * 0.7 * sin(angle); 

        Vector3 point2 = origin;
        point2.x += radius * cos(angle); 
        point2.y += radius * sin(angle); 


        circle.center[i] = worldToScreen(point, pov, screenPos);
        circle.half[i] = worldToScreen(point1, pov, screenPos);
        circle.max[i] = worldToScreen(point2, pov, screenPos);

    }

    return circle;
}

const char *Vehicles[] = {"BRDM", "Dacia", "MiniBus", "Mirado", "Motorcycle", "MotorcycleCart", "Rony", "Scooter", "Snowbike", "Snowmobile", "VH_Tuk", "UAZ", "Buggy", "UTV", "Bigfoot", "CoupeRB", "ATV", "Motorglider"};

bool isVehicle(const char *name){
    if(strstr(name, "BRDM")){
        return true;
    }else if(strstr(name, "Dacia")){
        return true;
    }else if(strstr(name, "MiniBus")){
        return true;
    }else if(strstr(name, "Mirado")){
        return true;
    }else if(strstr(name, "Motorcycle")){
        return true;
    }else if(strstr(name, "MotorcycleCart")){
        return true;
    }else if(strstr(name, "Rony")){
        return true;
    }else if(strstr(name, "Scooter")){
        return true;
    }else if(strstr(name, "Snowbike")){
        return true;
    }else if(strstr(name, "Snowmobile")){
        return true;
    }else if(strstr(name, "VH_Tuk")){
        return true;
    }else if(strstr(name, "UAZ")){
        return true;
    }else if(strstr(name, "Buggy")){
        return true;
    }else if(strstr(name, "UTV")){
        return true;
    }else if(strstr(name, "Bigfoot")){
        return true;
    }else if(strstr(name, "CoupeRB")){
        return true;
    }else if(strstr(name, "ATV")){
        return true;
    }else if(strstr(name, "Motoglider")){
        return true;
    }else{
        return false;
    }
}

struct Loot {
    int type;
    std::string name;    
};

bool isLoot(const char* name, Loot &loot){
  if(ESP.Loot){
    if(strstr(name, "BP_Rifle_M416_Wrapper_C") && ESP.LootGun && ESP.m416){
        loot = {3, "M416"};
        return true;
    }else if(strstr(name, "BP_Rifle_M762_Wrapper_C") && ESP.LootGun && ESP.m762){
        loot = {3, "M762"};
        return true;
    }else if(strstr(name, "BP_Rifle_AUG_Wrapper_C") && ESP.LootGun && ESP.aug){
        loot = {3, "AUG"};
        return true;
    }else if(strstr(name, "BP_Rifle_Groza_Wrapper_C") && ESP.LootGun && ESP.groza){
        loot = {3, "Groza"};
        return true;
    }else if(strstr(name, "BP_Rifle_SCAR_Wrapper_C") && ESP.LootGun && ESP.scar){
        loot = {3, "SCAR"};
        return true;
    }else if(strstr(name, "BP_Rifle_ACE32_Wrapper_C") && ESP.LootGun && ESP.ace32){
        loot = {3, "ACE32"};
        return true;
    }else if(strstr(name, "BP_Rifle_AKM_Wrapper_C") && ESP.LootGun && ESP.akm){
        loot = {3, "AKM"};
        return true;
    }else if(strstr(name, "BP_Ammo_50BMG_Pickup_C") && ESP.Bullets && ESP.bmg50){
        loot = {2, "50BMG"};
        return true;
    }else if(strstr(name, "BP_Ammo_556mm_Pickup_C") && ESP.Bullets && ESP.mm556){
        loot = {2, "5.56mm"};
        return true;
    }else if(strstr(name, "BP_Ammo_762mm_Pickup_C") && ESP.Bullets && ESP.mm762){
        loot = {2, "7.62mm"};
        return true;
    }else if(strstr(name, "BP_Ammo_9mm_Pickup_C") && ESP.Bullets && ESP.mm9){
        loot = {1, "9mm"};
        return true;
    }else if(strstr(name, "BP_Ammo_45ACP_Pickup_C") && ESP.Bullets && ESP.acp45){
        loot = {1, "45ACP"};
        return true;
    }else if(strstr(name, "BP_Ammo_12Guage_Pickup_C") && ESP.Bullets && ESP.guage12){
        loot = {1, "12 Guage"};
        return true;
    }else if(strstr(name, "PickUp_BP_Bag_Lv3") && ESP.Backpack && ESP.Backpack3){
        loot = {3, "Backpack Lv3"};
        return true;
    }else if(strstr(name, "PickUp_BP_Bag_Lv2") && ESP.Backpack && ESP.Backpack2){
        loot = {2, "Backpack Lv2"};
        return true;
    }else if(strstr(name, "PickUp_BP_Bag_Lv1") && ESP.Backpack && ESP.Backpack1){
        loot = {1, "Backpack Lv1"};
        return true;
    }else if(strstr(name, "PickUp_BP_Armor_Lv3_C") && ESP.Armor && ESP.Armor3){
        loot = {3, "Armor Lv3"};
        return true;
    }else if(strstr(name, "PickUp_BP_Armor_Lv2_C") && ESP.Armor && ESP.Armor2){
        loot = {2, "Armor Lv2"};
        return true;
    }else if(strstr(name, "PickUp_BP_Armor_Lv1_C") && ESP.Armor && ESP.Armor1){
        loot = {1, "Armor Lv1"};
        return true;
    }else if(strstr(name, "PickUp_BP_Helmet_Lv3_C") && ESP.Helmet && ESP.Helmet3){
        loot = {3, "Helmet Lv3"};
        return true;
    }else if(strstr(name, "PickUp_BP_Helmet_Lv2_C") && ESP.Helmet && ESP.Helmet2){
        loot = {2, "Helmet Lv2"};
        return true;
    }else if(strstr(name, "PickUp_BP_Helmet_Lv1_C") && ESP.Helmet && ESP.Helmet1){
        loot = {1, "Helmet Lv1"};
        return true;
    }
  }
    return false;
}

bool isDeadBox(const char* name){
    if(strstr(name, "PickUpList") || strstr(name, "AirdropList")){
        return true;
    }  
    return false;
}

struct DeadBoxInfo{
    int type;
    std::string name;    
};

bool isDeadBoxInfo(int id, DeadBoxInfo &info){
    if (id == 601006) {
        info.name = "[DRUG]MED KIT";
        info.type = 1;
        return true;
    } else if (id == 601005) {
        info.name = "[DRUG]FIRST AID KIT";
        info.type = 1;
        return true;
    } else if (id == 601001) {
        info.name = "[DRUG]DRINK";
        info.type = 1;
        return true;
    } else if (id == 601002) {
        info.name = "[DRUG]ADRENALINE";
        info.type = 1;
        return true;
    } else if (id == 601003) {
        info.name = "[DRUG]PAINKILLER";
        info.type = 1;
        return true;
    } else if (id == 303001) {
        info.name = "[BULLET]5.56mm";
        info.type = 2;
        return true;
    } else if (id == 302001) {
        info.name = "[BULLET]7.62mm";
        info.type = 2;
        return true;
    } else if (id == 306001) {
        info.name = "[BULLET]MAGNUM";
        info.type = 2;
        return true;
    } else if (id == 308001) {
        info.name = "[BULLET]50BMG";
        info.type = 2;
        return true;
    } else if (id == 503002) {
        info.name = "[ARMOR]ARMOR Lv2";
        info.type = 2;
        return true;
    } else if (id == 503003) {
        info.name = "[ARMOR]ARMOR Lv3";
        info.type = 3;
        return true;
    } else if (id == 501002) {
        info.name = "[ARMOR]BACKPACK Lv2";
        info.type = 2;
        return true;
    } else if (id == 501006) {
        info.name = "[ARMOR]BACKPACK Lv3";
        info.type = 3;
        return true;
    } else if (id == 502002) {
        info.name = "[ARMOR]HELMET Lv2";
        info.type = 2;
        return true;
    } else if (id == 501003) {
        info.name = "[ARMOR]HELMET Lv3";
        info.type = 3;
        return true;
    } else if (id == 501106) {
        info.name = "[ARMOR]BACKPACK Lv6";
        info.type = 6;
        return true;
    } else if (id == 502106) {
        info.name = "[ARMOR]HELMET Lv6";
        info.type = 6;
        return true;
    } else if (id == 502109) {
        info.name = "[ARMOR]HELMET COBRA Lv6";
        info.type = 6;
        return true;
    } else if (id == 502112) {
        info.name = "[ARMOR]HELMET STEEL FRONT Lv6";
        info.type = 6;
        return true;
    } else if (id == 503106) {
        info.name = "[ARMOR]VEST Lv6";
        info.type = 6;
        return true;
    } else if (id == 503106) {
        info.name = "[ARMOR]VEST COBRA Lv6";
        info.type = 6;
        return true;
    } else if (id == 503112) {
        info.name = "[ARMOR]VEST STEEL FRONT Lv6";
        info.type = 6;
        return true;
    } 



    return false;
}

