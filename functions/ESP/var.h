#import "functions/ESP/struct.h"

const char *aimbotModeText[] = {"Scope", "Fire", "Scope & Fire", "Scope on snipers. Fire on rifles."};

const char *aimbotPartsText[] = {"Head", "Neck", "Body"};

const char *espStylesText[] = {"Zenin", "CS GO"};

ImVec2 testBackAngle;

Vector3 GrenadeExtends;
float GrenadeRadius;

uintptr_t ObjectAddr;
uintptr_t coordAddr;
//Vector3 objectCoord;
uintptr_t goodsListArray;
//std::string className;
std::string AlivePlayers;
std::string RealPlayers;
std::string AliveTeams;
std::string MemInfo;
std::string CPUInfo;
std::string playerControllerClassName;
std::string cameraManagerClassName;
Vector3 selfCoord;
int objectDistance;
int team;
int TeamID;
int ListX = 7;
int goodsListCount;
bool bDead;
bool enabledAimbot;
bool bIsAI;
ImVec2 screenSize;

int playerCount = 0, robotCount = 0;

float asize = 0.7f;
float test1 =0;
float test2 =0;
float test3 =0;

float Nheight = 55;
float Nwidth = 40;
float Dheight = 32.24;
float Dwidth = 93;

int usedMem = 0;
int totalMem = 0;
double userUsage = 0;
double systemUsage = 0;
double idleUsage = 0;

float rate;
MinimalViewInfo pov;
Loot loot;
DeadBoxInfo BoxInfo;

std::vector<EnemyInfo> enemyInfo;
std::vector<std::string> ClassNames;

uintptr_t meshAddr = 0;
uintptr_t humanAddr = 0;
uintptr_t boneAddr = 0;