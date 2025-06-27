//Require standard library
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <Foundation/Foundation.h>
#include <iostream>
#include <fstream>
//Imgui library
#import "Esp/CaptainHook.h"
#import "Esp/ImGuiDrawView.h"
#import "IMGUI/imgui.h"
#import "IMGUI/imgui_impl_metal.h"
#import "IMGUI/imgui-fokusi.h"
#import "IMGUI/imgui_internal.h"
//Patch library
#import "utils/NakanoIchika.h"
#import "utils/NakanoNino.h"
#import "utils/NakanoMiku.h"
#import "utils/NakanoYotsuba.h" 
#include "utils/oxorany_include.h"
#import "utils/dobby.h"
#import "utils/base64.h"
#import "IMGUI/stb_image.h"
#import "utils/Vector3.h"
//some another helpers
#import "variables.h"
#import "utils/crossoffsets.h"
#import "Stream/HeeeNoScreenShotView.h"
ImFont *font;
ImFont *LootFont;
ImFont *ESPFont;
#import "functions/ESP/main.h"
#import "font.h"
#import "font2.h"

#define kWidth  [UIScreen mainScreen].bounds.size.width
#define kHeight [UIScreen mainScreen].bounds.size.height
#define kScale [UIScreen mainScreen].scale

#define GUN_BASE64(device, textureVar, base64String) \
    NSData *textureVar##Data = [[NSData alloc] initWithBase64EncodedString:base64String options:NSDataBase64DecodingIgnoreUnknownCharacters]; \
    textureVar = [self loadImageTexture:device :(void*)[textureVar##Data bytes] :[textureVar##Data length]];


@interface ImGuiDrawView () <MTKViewDelegate>
@property (nonatomic, strong) id <MTLDevice> device;
@property (nonatomic, strong) id <MTLCommandQueue> commandQueue;
@property (strong, nonatomic) UIWindow *window;
@end

@implementation ImGuiDrawView

long gWorld() {
    OffsetValues offsetsForBundle = [OffsetsManager getOffsetsForBundleID:[[NSBundle mainBundle] bundleIdentifier]];
    //return reinterpret_cast<long(__fastcall*)(long)>((long)_dyld_get_image_vmaddr_slide(1) + offsetsForBundle.gWorldFun)((long)_dyld_get_image_vmaddr_slide(1) + offsetsForBundle.gWorldData);
    return reinterpret_cast<long(__fastcall*)(long)>(getRealOffset(offsetsForBundle.gWorldFun))(getRealOffset(offsetsForBundle.gWorldData));
}

long gName() {
    OffsetValues offsetsForBundle = [OffsetsManager getOffsetsForBundleID:[[NSBundle mainBundle] bundleIdentifier]];
    //return reinterpret_cast<long(__fastcall*)(long)>((long)_dyld_get_image_vmaddr_slide(1) + offsetsForBundle.gNameFun)((long)_dyld_get_image_vmaddr_slide(1) + offsetsForBundle.gNameData);
    return reinterpret_cast<long(__fastcall*)(long)>(getRealOffset(offsetsForBundle.gNameFun))(getRealOffset(offsetsForBundle.gNameData));
}

- (instancetype)initWithNibName:(nullable NSString *)nibNameOrNil bundle:(nullable NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];

    _device = MTLCreateSystemDefaultDevice();
    _commandQueue = [_device newCommandQueue];

    if (!self.device) abort();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.Fonts->AddFontFromMemoryTTF(Asap_Light_ttf, Asap_Light_ttf_len, 60.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());

    LootFont = io.Fonts->AddFontFromMemoryTTF(Asap_SemiBold_ttf, Asap_SemiBold_ttf_len, 30.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());

    /*ImVector<ImWchar> ranges;
    ImFontGlyphRangesBuilder builder;
    builder.AddRanges(io.Fonts->GetGlyphRangesGreek());
    builder.AddRanges(io.Fonts->GetGlyphRangesKorean());
    builder.AddRanges(io.Fonts->GetGlyphRangesChineseFull());
    builder.AddRanges(io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    builder.AddRanges(io.Fonts->GetGlyphRangesJapanese());
    builder.AddRanges(io.Fonts->GetGlyphRangesCyrillic());
    builder.AddRanges(io.Fonts->GetGlyphRangesThai());
    builder.AddRanges(io.Fonts->GetGlyphRangesVietnamese());
    builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
    builder.BuildRanges(&ranges);

    ESPFont = io.Fonts->AddFontFromFileTTF("/System/Library/Fonts/CoreUI/SFUI.ttf", 45.0f, NULL, ranges.Data);*/


    ImGui::StyleColorsDark();
    
    ImGui_ImplMetal_Init(_device);

    [self initImageTexture:_device];

    return self;
}

+ (void)showChange:(BOOL)open
{
    userInteraction = open;
}

- (MTKView *)mtkView
{
    return (MTKView *)self.view;
}

- (void)loadView
{
    CGFloat w = [UIApplication sharedApplication].windows[0].rootViewController.view.frame.size.width;
    CGFloat h = [UIApplication sharedApplication].windows[0].rootViewController.view.frame.size.height;
    self.view = [[MTKView alloc] initWithFrame:CGRectMake(0, 0, w * 2, h * 2)];
}

HeeeNoScreenShotView *noScreenShotView;

- (void)viewDidLoad {

    [super viewDidLoad];
    
    self.mtkView.device = self.device;
    self.mtkView.delegate = self;
    self.mtkView.clearColor = MTLClearColorMake(0, 0, 0, 0);
    self.mtkView.backgroundColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:0];
    self.mtkView.clipsToBounds = YES;

    noScreenShotView = [[HeeeNoScreenShotView alloc] initWithFrame:CGRectMake(0,0,0,0)];
    [noScreenShotView setUserInteractionEnabled:NO];
    [noScreenShotView addSubview:self.mtkView];
    [[[[UIApplication sharedApplication] windows]lastObject] addSubview:noScreenShotView];

dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
    screenSize = ImVec2(kWidth/2, kHeight/2);
    enemy = new enemy_t();
    me = new me_t();
    Veh = new Veh_t();
    MapLoot = new MapLoot_t();
    DeadBoxLoot = new DeadBoxLoot_t();

    OpenURL(NSENC("https://t.me/hehipa"));
    HexIpa->init();

    sleep(5);
    OffsetValues offsetsForBundle = [OffsetsManager getOffsetsForBundleID:[[NSBundle mainBundle] bundleIdentifier]];
    DecryptActors = decltype(DecryptActors)(getRealOffset(offsetsForBundle.ActorsOffset));
    pthread_t readStaticDataThread;
    pthread_create(&readStaticDataThread, nullptr, readStaticData, nullptr);
    pthread_t AIMThread;
    pthread_create(&AIMThread, nullptr, silenceAimbot, nullptr);

});

}

#pragma mark - Interaction

- (void)updateIOWithTouchEvent:(UIEvent *)event
{
    UITouch *anyTouch = event.allTouches.anyObject;
    CGPoint touchLocation = [anyTouch locationInView:self.view];
    ImGuiIO &io = ImGui::GetIO();
    io.MousePos = ImVec2(touchLocation.x, touchLocation.y);

    BOOL hasActiveTouch = NO;
    for (UITouch *touch in event.allTouches)
    {
        if (touch.phase != UITouchPhaseEnded && touch.phase != UITouchPhaseCancelled)
        {
            hasActiveTouch = YES;
            break;
        }
    }
    io.MouseDown[0] = hasActiveTouch;
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    [self updateIOWithTouchEvent:event];
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    [self updateIOWithTouchEvent:event];
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    [self updateIOWithTouchEvent:event];
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    [self updateIOWithTouchEvent:event];
}

#pragma mark - MTKViewDelegate

float high = 72.14, low = 21.71, w1 = -40.76, w2 = 92.08;

- (void)drawInMTKView:(MTKView*)view
{
    
    ImGuiIO& io = ImGui::GetIO();
    //io.DisplaySize.x = view.bounds.size.width;
    //io.DisplaySize.y = view.bounds.size.height;

    io.DisplaySize.x = kWidth * 2;
    io.DisplaySize.y = kHeight * 2;

static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{

    ImGuiIO & io = ImGui::GetIO();
    //io.DisplaySize = ImVec2(kWidth, kHeight);
    ImGui::SetNextWindowPos(ImVec2(kWidth / 2, kHeight / 2), 0, ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x/2, io.DisplaySize.y/2));

    ImGuiStyle* style = &ImGui::GetStyle();

    style->WindowPadding            = ImVec2(15, 15);
    style->WindowRounding           = 12.0f;
    style->FramePadding             = ImVec2(10, 5);
    style->FrameRounding            = 12.0f;
    style->ItemSpacing              = ImVec2(5, 4);
    style->ItemInnerSpacing         = ImVec2(7, 5);
    style->IndentSpacing            = 15.0f;
    style->ScrollbarSize            = 24.0f;
    style->ScrollbarRounding        = 9.0f;
    style->GrabMinSize              = 16.0f; 
    style->GrabRounding             = 3.0f; 
    });

    CGFloat framebufferScale = view.window.screen.scale ?: UIScreen.mainScreen.scale;
    io.DisplayFramebufferScale = ImVec2(framebufferScale, framebufferScale);
    io.DeltaTime = 1 / float(view.preferredFramesPerSecond ?: 120);
    
    id<MTLCommandBuffer> commandBuffer = [self.commandQueue commandBuffer];
    
        
        if (userInteraction == true) {
            [self.view setUserInteractionEnabled:YES];
        } else if (userInteraction == false) {
            [self.view setUserInteractionEnabled:NO];
        }

        MTLRenderPassDescriptor* renderPassDescriptor = view.currentRenderPassDescriptor;
        if (renderPassDescriptor != nil)
        {
            id <MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
            [renderEncoder pushDebugGroup:NSENC("ImGui Jane")];

            ImGui_ImplMetal_NewFrame(renderPassDescriptor);
            ImGui::NewFrame();
            
            font = ImGui::GetFont();
            font->Scale = 20.f / font->FontSize;
            
            CGFloat x = (([UIApplication sharedApplication].windows[0].rootViewController.view.frame.size.width) - 360) / 2;
            CGFloat y = (([UIApplication sharedApplication].windows[0].rootViewController.view.frame.size.height) - 300) / 2;
            
            ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x/4, io.DisplaySize.y/4), ImGuiCond_FirstUseEver);
            
            if (userInteraction)
            {                
               

ImGui::Begin(oxorany("HEXIPA FREE TELEGRAM @hehipa @jailbreaked"), &userInteraction);

if (ImGui::BeginTabBar(oxorany("HEXIPA"), ImGuiTabBarFlags_NoTooltip|ImGuiTabBarFlags_Reorderable)){
            

if(ImGui::BeginTabItem(ENCRYPT("Debug"))){

ImGui::SliderFloat("high", &high, 0.0f, 200.0f, "%.2f");
ImGui::SliderFloat("low", &low, 0.0f, 200.0f, "%.2f");
ImGui::SliderFloat("w1", &w1, -100.0f, 100.0f, "%.2f");
ImGui::SliderFloat("w2", &w2, 0.0f, 200.0f, "%.2f");

ImGui::Text("libAddr: 0x%llx", libAddr);

ImGui::Text("Screen: x:%.f, y:%.f", io.DisplaySize.x, io.DisplaySize.y);

ImGui::Text("AddControllerYawInput: 0x%llx", AddControllerYawInput);
ImGui::Text("AddControllerPitchInputOffset: 0x%llx", AddControllerPitchInput);
ImGui::Text("AddControllerRollInputOffset: 0x%llx", AddControllerRollInput);

ImGui::Text("weaponAddr: 0x%llx", weaponAddr);

ImGui::Text("Size: %i",playerDataList.size());
ImGui::Text("Size: %i",RealplayerDataList.size());
ImGui::Text("cameraManagerClassName: %s",cameraManagerClassName.c_str());
ImGui::Text("Gworld: 0x%llx",GWorldAddr);
ImGui::Text("Gname: 0x%llx",GNameAddr);

ImGui::Text("NetDriver: 0x%llx",me->NetDriver);
ImGui::Text("ServerConnection: 0x%llx",me->ServerConnection);
ImGui::Text("PlayerController: 0x%llx",me->PlayerController);
ImGui::Text("SelfAddr: 0x%llx",me->Pointer);
ImGui::Text("CameraManager: 0x%llx", me->Camera);
ImGui::Text("ObjectArray: 0x%llx", me->ObjectArray);
ImGui::Text("ObjectCount: %i", me->ObjectCount);
ImGui::Text("meshAddr: 0x%llx", meshAddr);
ImGui::Text("humanAddr: 0x%llx", humanAddr);
ImGui::Text("boneAddr: 0x%llx", boneAddr);

for (StaticPlayerData playerData:playerDataList) {
     ImGui::Text("Name: %s, coord: 0x%llx, team: %i, pos: x: %.3f, y: %.3f", playerData.name.c_str(), playerData.coordAddr, playerData.team, playerData.pos.x, playerData.pos.y);     

}

for(int i = 0; i < ClassNames.size(); i++){
ImGui::Text("Id: %i Name: %s", i, ClassNames[i].c_str());
}


ImGui::EndTabItem();

}


if(ImGui::BeginTabItem(ENCRYPT("AIM"))){

ImGui::Checkbox(ENCRYPT("Start AIM"), &AIM.Start);
if(AIM.Start){
ImGui::Checkbox(ENCRYPT("Ignore Knocked"), &AIM.IgnoreKnocked);ImGui::SameLine();
ImGui::Checkbox(ENCRYPT("Ignore Bots"), &AIM.IgnoreBots);ImGui::SameLine();
ImGui::Checkbox(ENCRYPT("Visible check"), &AIM.VisibleCheck);

ImGui::SliderFloat("Speed", &AIM.Speed, 0.0f, 1.0f, "%.2f");



ImGui::Combo("Mode", &AIM.Mode, aimbotModeText, IM_ARRAYSIZE(aimbotModeText));
ImGui::Combo("Part", &AIM.AimbotParts, aimbotPartsText, IM_ARRAYSIZE(aimbotPartsText));

ImGui::SliderFloat("Radius", &AIM.AimbotRadius, 0.0f, ([UIScreen mainScreen].bounds.size.height * [UIScreen mainScreen].nativeScale) / 2, "%.0f");


}




ImGui::EndTabItem();

}

if(ImGui::BeginTabItem(ENCRYPT("ESP"))){

ImGui::Checkbox(ENCRYPT("Draw"), &ESP.Draw);ImGui::SameLine();
if(ImGui::Checkbox(ENCRYPT("Vote Vladimir Putin"), &ESP.Stream)){
    if(!ESP.Stream){
        ESP.Stream = true;
    }
}


if(ESP.Draw){
//ImGui::Combo("ESP Styles", &ESP.Style, espStylesText, IM_ARRAYSIZE(espStylesText));
ImGui::Checkbox(ENCRYPT("Line"), &ESP.Line);ImGui::SameLine();
ImGui::Checkbox(ENCRYPT("Info"), &ESP.Info);ImGui::SameLine();
ImGui::Checkbox(ENCRYPT("Arrows"), &ESP.Arrows);ImGui::SameLine();
ImGui::Checkbox(ENCRYPT("Match info"), &ESP.MatchInfo);
if(ESP.Info){
    ImGui::Checkbox(ENCRYPT("Distance"), &ESP.Distance);ImGui::SameLine();
    ImGui::Checkbox(ENCRYPT("Bones"), &ESP.Bones);ImGui::SameLine();
    ImGui::Checkbox(ENCRYPT("Name"), &ESP.Name);ImGui::SameLine();
    ImGui::Checkbox(ENCRYPT("Box"), &ESP.Box);ImGui::SameLine();
    ImGui::Checkbox(ENCRYPT("Hp"), &ESP.Hp);
}
}

/*
ImGui::SliderFloat("X", &GrenadeExtends.x, -1500.0f, 1500.0f, "%.2f");
ImGui::SliderFloat("Y", &GrenadeExtends.y, -1500.0f, 1500.0f, "%.2f");
ImGui::SliderFloat("Z", &GrenadeExtends.z, -1500.0f, 1500.0f, "%.2f");
*/



ImGui::EndTabItem();

}
if(ImGui::BeginTabItem(ENCRYPT("Items ESP"))){

ImGui::Checkbox(ENCRYPT("Vehicles ESP"), &ESP.Vehicles);ImGui::SameLine();
ImGui::Checkbox(ENCRYPT("Grenades warning"), &ESP.Grenade);
ImGui::Checkbox(ENCRYPT("Loot ESP"), &ESP.Loot);

if(ESP.Loot){
    ImGui::Checkbox(ENCRYPT("Guns"), &ESP.LootGun);ImGui::SameLine();  
    ImGui::Checkbox(ENCRYPT("Bullets"), &ESP.Bullets);
    ImGui::Checkbox(ENCRYPT("Armor"), &ESP.Armor);ImGui::SameLine();  
    ImGui::Checkbox(ENCRYPT("Helmets"), &ESP.Helmet);ImGui::SameLine();  
    ImGui::Checkbox(ENCRYPT("Backpacks"), &ESP.Backpack);
    ImGui::Checkbox(ENCRYPT("Gun Settings"), &ESP.LootGunSettings);
    if(ESP.LootGunSettings){
        ImGui::Begin(ENCRYPT("Loot settings"));
        {
          if(ESP.LootGun){
            ImGui::Checkbox(ENCRYPT("M416"), &ESP.m416);ImGui::SameLine();  
            ImGui::Checkbox(ENCRYPT("M762"), &ESP.m762);ImGui::SameLine();
            ImGui::Checkbox(ENCRYPT("AUG"), &ESP.aug);ImGui::SameLine();
            ImGui::Checkbox(ENCRYPT("AKM"), &ESP.akm);
            ImGui::Checkbox(ENCRYPT("Groza"), &ESP.groza);ImGui::SameLine();
            ImGui::Checkbox(ENCRYPT("SCAR"), &ESP.scar);ImGui::SameLine();
            ImGui::Checkbox(ENCRYPT("ACE32"), &ESP.ace32);
          }

          ImGui::Dummy(ImVec2(0.0f, 5.0f)); 

          if(ESP.Bullets){
            ImGui::Checkbox(ENCRYPT("5.56 mm"), &ESP.mm556);ImGui::SameLine();  
            ImGui::Checkbox(ENCRYPT("7.62 mm"), &ESP.mm762);ImGui::SameLine();  
            ImGui::Checkbox(ENCRYPT("9 mm"), &ESP.mm9);ImGui::SameLine();  
            ImGui::Checkbox(ENCRYPT("50 BMG"), &ESP.bmg50);
            ImGui::Checkbox(ENCRYPT("45 ACP"), &ESP.acp45);ImGui::SameLine();  
            ImGui::Checkbox(ENCRYPT("12 Guage"), &ESP.guage12);
          }

          ImGui::Dummy(ImVec2(0.0f, 5.0f)); 
      
          if(ESP.Armor){
              ImGui::Checkbox(ENCRYPT("Armor Lv1"), &ESP.Armor1);ImGui::SameLine();
              ImGui::Checkbox(ENCRYPT("Armor Lv2"), &ESP.Armor2);ImGui::SameLine();
              ImGui::Checkbox(ENCRYPT("Armor Lv3"), &ESP.Armor3);
          }

          ImGui::Dummy(ImVec2(0.0f, 5.0f)); 

          if(ESP.Helmet){
              ImGui::Checkbox(ENCRYPT("Helmet Lv1"), &ESP.Helmet1);ImGui::SameLine();
              ImGui::Checkbox(ENCRYPT("Helmet Lv2"), &ESP.Helmet2);ImGui::SameLine();
              ImGui::Checkbox(ENCRYPT("Helmet Lv3"), &ESP.Helmet3);
          }

          ImGui::Dummy(ImVec2(0.0f, 5.0f)); 

          if(ESP.Backpack){
              ImGui::Checkbox(ENCRYPT("Backpack Lv1"), &ESP.Backpack1);ImGui::SameLine();
              ImGui::Checkbox(ENCRYPT("Backpack Lv2"), &ESP.Backpack2);ImGui::SameLine();
              ImGui::Checkbox(ENCRYPT("Backpack Lv3"), &ESP.Backpack3);
          }

        ImGui::End();
        }
    }
}

ImGui::EndTabItem();
}

}

ImGui::End();


            } 


            ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
            ImVec2 textSize = ImGui::CalcTextSize("TG @HEHIPA");
            draw_list->AddText(font, 20.0f, ImVec2(io.DisplaySize.x/4 - textSize.x/2, 30), IM_COL32(255, 0, 0, 255), ENC("TG @HEHIPA"));

            GWorldAddr = gWorld();
            GNameAddr = gName();

            drawesp();

            ImGui::Render();
            ImDrawData* draw_data = ImGui::GetDrawData();
            ImGui_ImplMetal_RenderDrawData(draw_data, commandBuffer, renderEncoder);
          
            [renderEncoder popDebugGroup];
            [renderEncoder endEncoding];

            [commandBuffer presentDrawable:view.currentDrawable];
        }

        [commandBuffer commit];
}

void OpenURL(NSString *urlString) {
    NSURL *url = [NSURL URLWithString:urlString];
    if ([[UIApplication sharedApplication] canOpenURL:url]) {
        [[UIApplication sharedApplication] openURL:url options:@{} completionHandler:^(BOOL success) {
            if (success) {
                NSLog(NSENC("Ссылка открыта успешно."));
            } else {
                NSLog(NSENC("Не удалось открыть ссылку."));
            }
        }];
    } else {
        NSLog(NSENC("Невозможно открыть ссылку: %@"), urlString);
    }
}



id<MTLTexture> M416Texture;
id<MTLTexture> AKMTexture;
id<MTLTexture> ACE32Texture;
id<MTLTexture> AUGTexture;
id<MTLTexture> FAMASTexture;
id<MTLTexture> G36CTexture;
id<MTLTexture> GROZATexture;
id<MTLTexture> BADGERTexture;
id<MTLTexture> M16A4Texture;
id<MTLTexture> M762Texture;
id<MTLTexture> MUTANTTexture;
id<MTLTexture> QBZTexture;
id<MTLTexture> SCARTexture;

id<MTLTexture> AWMTexture;
id<MTLTexture> AMRTexture;
id<MTLTexture> M24Texture;
id<MTLTexture> KAR98KTexture;
id<MTLTexture> MOSINTexture;
id<MTLTexture> DSRTexture;

id<MTLTexture> CrossbowTexture;
id<MTLTexture> SKSTexture;
id<MTLTexture> MINI14Texture;
id<MTLTexture> MK14Texture;
id<MTLTexture> MK12Texture;
id<MTLTexture> SLRTexture;
id<MTLTexture> QBUTexture;


-(void)initImageTexture: (id<MTLDevice>)device {
    GUN_BASE64(device, M416Texture, M416Base64);
    GUN_BASE64(device, AKMTexture, AKMBase64);
    GUN_BASE64(device, ACE32Texture, ACE32Base64);
    GUN_BASE64(device, AUGTexture, AUGBase64);
    GUN_BASE64(device, FAMASTexture, FAMASBase64);
    GUN_BASE64(device, G36CTexture, G36CBase64);
    GUN_BASE64(device, GROZATexture, GROZABase64);
    GUN_BASE64(device, BADGERTexture, BADGERBase64);
    GUN_BASE64(device, M16A4Texture, M16A4Base64);
    GUN_BASE64(device, M762Texture, M762Base64);
    GUN_BASE64(device, MUTANTTexture, MUTANTBase64);
    GUN_BASE64(device, QBZTexture, QBZBase64);
    GUN_BASE64(device, SCARTexture, SCARBase64);

    GUN_BASE64(device, AWMTexture, AWMBase64);
    GUN_BASE64(device, AMRTexture, AMRBase64);
    GUN_BASE64(device, M24Texture, M24Base64);
    GUN_BASE64(device, KAR98KTexture, KAR98KBase64);
    GUN_BASE64(device, MOSINTexture, MOSINBase64);
    GUN_BASE64(device, DSRTexture, DSRBase64);

    GUN_BASE64(device, CrossbowTexture, CrossbowBase64);
    GUN_BASE64(device, SKSTexture, SKSBase64);
    GUN_BASE64(device, MINI14Texture, MINI14Base64);
    GUN_BASE64(device, MK14Texture, MK14Base64);
    GUN_BASE64(device, MK12Texture, MK12Base64);
    GUN_BASE64(device, SLRTexture, SLRBase64);
    GUN_BASE64(device, QBUTexture, QBUBase64);

}

-(id<MTLTexture>)loadImageTexture:(id<MTLDevice>)device :(void*) imageData :(size_t) fileDataSize {
    int width, height;
    unsigned char *pixels = stbi_load_from_memory((stbi_uc const *)imageData, (int)fileDataSize, &width, &height, NULL, 4);

    MTLTextureDescriptor *textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                                 width:(NSUInteger)width
                                                                                                height:(NSUInteger)height
                                                                                             mipmapped:NO];
    textureDescriptor.usage = MTLTextureUsageShaderRead;
    textureDescriptor.storageMode = MTLStorageModeShared;
    id<MTLTexture> texture = [device newTextureWithDescriptor:textureDescriptor];
    [texture replaceRegion:MTLRegionMake2D(0, 0, (NSUInteger)width, (NSUInteger)height) mipmapLevel:0 withBytes:pixels bytesPerRow:(NSUInteger)width * 4];
    
    return texture;
}

void drawesp(){

    ImDrawList* bg = ImGui::GetBackgroundDrawList();

    readFrameData(screenSize, RealplayerDataList, RealgrenadeDataList, RealvehicleDataList, ReallootDataList);

    if(RealplayerDataList.size() == 0){ playerCount = 0; robotCount = 0; }

    string PlayersCount = "[ Players: " + to_string(playerCount) + " Bots: " + to_string(robotCount) + " ]";
    ImVec2 textSize = ImGui::CalcTextSize(PlayersCount.c_str());
    bg->AddText(font, 17.5f, {kWidth/2 - textSize.x/2.3, 50}, IM_COL32(255, 0, 0, 255),PlayersCount.c_str());

            if(ESP.MatchInfo){
                AlivePlayers = "Alive players: " + to_string(me->AlivePlayers);
                bg->AddText(font, 20.0f, ImVec2(70, kHeight - 70), IM_COL32(255, 255, 255, 255), AlivePlayers.c_str());
                RealPlayers = "Real players: " + to_string(me->RealPlayers);
                bg->AddText(font, 20.0f, ImVec2(70, kHeight - 100), IM_COL32(255, 255, 255, 255), RealPlayers.c_str());
                AliveTeams = "Alive teams: " + to_string(me->AliveTeams);
                bg->AddText(font, 20.0f, ImVec2(70, kHeight - 130), IM_COL32(255, 255, 255, 255), AliveTeams.c_str());
                MemInfo = "Memory: " + to_string(usedMem) + "mb/" + to_string(totalMem) + "mb";
                bg->AddText(font, 20.0f, ImVec2(70, kHeight - 160), IM_COL32(255, 255, 255, 255), MemInfo.c_str());
                CPUInfo = "CPU: " + to_string(userUsage) + "%";
                bg->AddText(font, 20.0f, ImVec2(70, kHeight - 190), IM_COL32(255, 255, 255, 255), CPUInfo.c_str());

            }

  
  if(ESP.Draw){
    if(AIM.Start){
        bg->AddCircle(ImVec2(kWidth / 2, kHeight / 2), AIM.AimbotRadius, ImColor(0, 255, 255), 0, 0.5f);
        if(tmpAim.x != 0 && tmpAim.y != 0 && TragetVis && TestTarget != 0){
            ImColor linecol;
            if(!TragetVis){
                linecol = ImColor(255,0,0);
            }else{
                linecol = ImColor(0,255,0);
            }
            bg->AddLine(tmpAim, {kWidth/2, kHeight/2}, linecol);
        }
    }
    int tmpplayerCount = 0, tmprobotCount = 0;
    for(PlayerData playerData:RealplayerDataList){
        if (playerData.robot) {
            tmprobotCount += 1;
        } else {
            tmpplayerCount += 1;
        }

        playerCount = tmpplayerCount;
        robotCount = tmprobotCount;

      if(playerData.screen.x > 0 && playerData.screen.x < kWidth && playerData.screen.y > 0 && playerData.screen.y < kHeight){          

        if(ESP.Line){
            ImColor linecol;
          if(!playerData.robot){
            if(!playerData.visibility){
                linecol = ImColor(255,0,0);
            }else{
                linecol = ImColor(0,255,0);
            }
          }else{
            if(!playerData.visibility){
                linecol = ImColor(0,120,120);
            }else{
                linecol = ImColor(165,165,165);
            }
          }
            
            bg->AddLine(playerData.screen, {kWidth/ 2, kHeight / 10}, linecol);

        }

        if(ESP.Info){
            teamColor(playerData.team);
            bg->AddRectFilled({playerData.bonesData.head.x - 95 * asize, playerData.bonesData.head.y - 58 *asize+test2}, {playerData.bonesData.head.x + 95*asize+test3, playerData.bonesData.head.y - 34 * asize},  TeamID_Color2, 0, 0);
            bg->AddRectFilled({playerData.bonesData.head.x - 95 * asize, playerData.bonesData.head.y - 58*asize+test2}, {playerData.bonesData.head.x - 45*asize+test3, playerData.bonesData.head.y - 34 * asize}, TeamID_Color, 0, 0);

            if(ESP.Hp){
                 rate = 1.0f * playerData.hp / playerData.hpMax;
                 ImColor HpColor;
                 if(rate >= 0.75){
                     HpColor = ImColor(0,255,0,180);
                 }else if(rate >= 0.5 && rate < 0.75){
                     HpColor = ImColor(200,200,20,180);
                 }else if(rate >= 0.25 && rate < 0.5){
                     HpColor = ImColor(200,100,20,180);
                 }else if(rate >= 0 && rate < 0.25){
                     HpColor = ImColor(255,0,0,180);
                 }else{
                     HpColor = ImColor(255,255,255,180);
                 }        
                 bg->AddRectFilled({playerData.bonesData.head.x - 95 * asize, playerData.bonesData.head.y - 34*asize+test2}, {(playerData.bonesData.head.x - 68) + (192 * asize) * rate, playerData.bonesData.head.y - 31 * asize}, HpColor, 0, 0);
            }
            if(ESP.Distance){
                string dist = "[" + to_string(playerData.distance) + "m]";
                ImVec2 textSize = ImGui::CalcTextSize(dist.c_str());
                DrawText(bg, font, 12.5f, ImVec2(playerData.bonesData.head.x - textSize.x / 3, playerData.bonesData.lankle.y + 15), ImColor(255, 255, 255, 255), dist.c_str(), true);
            }
            if(ESP.Name){
                const char* name;
                if(playerData.robot){
                    name = "BOT";
                }else{
                    name = playerData.name.c_str();
                }
                float fontSize = 12.5f;
                if(strlen(name) >= 10){
                    fontSize = 10.0f;
                }else if(strlen(name) >= 12){
                    fontSize = 9.0f;
                }

                ImVec2 textSize = ImGui::CalcTextSize(name);

                bg->AddText(LootFont, fontSize, ImVec2(playerData.bonesData.head.x - Nwidth * asize, playerData.bonesData.head.y - Nheight * asize), IM_COL32(255, 255, 255, 255),name);
            }
            if(ESP.Info){
                std::string s;
                auto textSize = ImGui::CalcTextSize(s.c_str(), 0, 30);

                if (playerData.team == 0) {
                    s += std::to_string(playerData.team);
                    bg->AddText(font,17.5f,ImVec2(playerData.bonesData.head.x - 93 * asize - (playerData.bonesData.head.x / 2), playerData.bonesData.head.y - 59 * asize),IM_COL32(0, 0, 0, 60),s.c_str());
                } else if (playerData.team < 10) {
                    s += " 0";
                    s += std::to_string(playerData.team);
                    bg->AddText(font,17.5f,ImVec2(playerData.bonesData.head.x - 87 * asize - (textSize.x / 2),playerData.bonesData.head.y - 59 * asize),IM_COL32(255, 255, 255, 255),s.c_str());
                } else if (playerData.team < 100) {
                    s += " ";
                    s += std::to_string(playerData.team);
                    bg->AddText(font,17.5f,ImVec2(playerData.bonesData.head.x - 87 * asize - (textSize.x / 2),playerData.bonesData.head.y - 59 * asize),IM_COL32(255, 255, 255, 255),s.c_str());
                } else {
                    s += std::to_string(playerData.team);
                    bg->AddText(font,17.5f,ImVec2(playerData.bonesData.head.x - 87 * asize - 1 - (textSize.x / 2),playerData.bonesData.head.y - 59 * asize),IM_COL32(255, 255, 255, 255),s.c_str());
                }

            }
            if(ESP.Box){

                /*float boxHeight = 2.5 * (playerData.bonesData.pelvis.y - playerData.bonesData.head.y);
                float boxWidth = boxHeight * 0.6f;
                Box4Line(bg, 0.5f, playerData.bonesData.head.x - (boxWidth / 2), playerData.bonesData.head.y, boxWidth, boxHeight, ImColor(0,0,0));*/

                RectBox3D boxData = GetRectBox3D(pov, playerData.pos, Vector3(80,80,180), screenSize);

                DrawRectBox3D(bg, boxData);

                id<MTLTexture>CurrentTexture = getCurrentWeapon(playerData.weaponName);               

                bg->AddImage((__bridge ImTextureID)CurrentTexture, {playerData.bonesData.head.x - w1 * asize, playerData.bonesData.head.y - high *asize+test2}, {playerData.bonesData.head.x + w2 *asize+test3, playerData.bonesData.head.y - low * asize}, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));


            }

        }
        if(ESP.Bones){
          ImColor bonecol;

          if(!playerData.robot){
            if(!playerData.visibility){
                bonecol = ImColor(255,0,0);
            }else{
                bonecol = ImColor(0,255,0);
            }
          }else{
            if(!playerData.visibility){
                bonecol = ImColor(165,165,165);
            }else{
                bonecol = ImColor(255,255,0);
            }
          }

          if(!(AIM.IgnoreBots && playerData.robot)){

            bg->AddLine(playerData.bonesData.head, playerData.bonesData.pit, bonecol, 1.0f);
            bg->AddLine(playerData.bonesData.pelvis, playerData.bonesData.a2, bonecol, 1.0f);
            bg->AddLine(playerData.bonesData.a2, playerData.bonesData.a3, bonecol, 1.0f);
            bg->AddLine(playerData.bonesData.a3, playerData.bonesData.pit, bonecol, 1.0f);

            bg->AddLine(playerData.bonesData.pit, playerData.bonesData.lcollar, bonecol, 1.0f);
            bg->AddLine(playerData.bonesData.lcollar, playerData.bonesData.lelbow, bonecol, 1.0f);
            bg->AddLine(playerData.bonesData.lelbow, playerData.bonesData.lwrist, bonecol, 1.0f);
                    
            bg->AddLine(playerData.bonesData.pit, playerData.bonesData.rcollar, bonecol, 1.0f);
            bg->AddLine(playerData.bonesData.rcollar, playerData.bonesData.relbow, bonecol, 1.0f);
            bg->AddLine(playerData.bonesData.relbow, playerData.bonesData.rwrist, bonecol, 1.0f); 
                    
            bg->AddLine(playerData.bonesData.pelvis, playerData.bonesData.lthigh, bonecol, 1.0f);
            bg->AddLine(playerData.bonesData.lthigh, playerData.bonesData.lknee, bonecol, 1.0f);
            bg->AddLine(playerData.bonesData.lknee, playerData.bonesData.lankle, bonecol, 1.0f);
                    
            bg->AddLine(playerData.bonesData.pelvis, playerData.bonesData.rthigh, bonecol, 1.0f);
            bg->AddLine(playerData.bonesData.rthigh, playerData.bonesData.rknee, bonecol, 1.0f);
            bg->AddLine(playerData.bonesData.rknee, playerData.bonesData.rankle, bonecol, 1.0f);
          }

        }
      }

        if(ESP.Arrows){ 
          if(playerData.screen.x < 0 || playerData.screen.x > kWidth || playerData.screen.y < 0 || playerData.screen.y > kHeight){          
            ImVec2 backAngle = rotateCoord(playerData.angle,ImVec2(150, 0));

            testBackAngle = backAngle;

                ImVec2 triangle1;
                triangle1 = rotateCoord(playerData.angle - 90, ImVec2(6, 0));
                triangle1.x += kWidth / 2 + backAngle.x;
                triangle1.y += kHeight / 2 + backAngle.y;

                ImVec2 triangle;
                triangle = rotateCoord(playerData.angle, ImVec2(25, 0));
                triangle.x += kWidth / 2 + backAngle.x;
                triangle.y += kHeight / 2 + backAngle.y;

                ImVec2 triangle2;
                triangle2 = rotateCoord(playerData.angle + 90, ImVec2(6, 0));
                triangle2.x += kWidth / 2 + backAngle.x;
                triangle2.y += kHeight / 2 + backAngle.y;                   

          ImColor arrowcol1;
          ImColor arrowcol2;
            
          if(!playerData.robot){
            if(!playerData.visibility){
                arrowcol1 = ImColor(255,0,0);
                arrowcol2 = ImColor(165,0,0);
            }else{
                arrowcol1 = ImColor(0,255,0);
                arrowcol2 = ImColor(30,89,69);
            }
          }else{
            if(!playerData.visibility){
                arrowcol1 = ImColor(165,165,165);
                arrowcol2 = ImColor(71,74,81);
            }else{
                arrowcol1 = ImColor(255,255,0);
                arrowcol2 = ImColor(166,166,17);
            }
          }


            bg->AddTriangleFilled(triangle1, triangle, triangle2, arrowcol1);
            bg->AddTriangle(triangle1, triangle, triangle2, arrowcol2);
          }
        }

    }
    if(ESP.Grenade){
        for(PlayerData grenadeData:RealgrenadeDataList){
            if(grenadeData.distance < 40){
                ImVec2 textS = ImGui::CalcTextSize("!NEAR GRENADE!");
                bg->AddText(LootFont, 23.0f, ImVec2(kWidth/2 - textS.x/2, kHeight / 7), IM_COL32(255, 255, 0, 255), "!NEAR GRENADE!");
            }

            string grenadeText = grenadeData.name;
            ImVec2 textSize = ImGui::CalcTextSize(grenadeText.c_str());
            bg->AddText(LootFont, 12.0f, {grenadeData.screen.x - textSize.x/2, grenadeData.screen.y}, ImColor(255,0,0,255), grenadeText.c_str());

            if(grenadeData.name == "[SMOKE]"){
                Circle3D CircleData = GetCircle3D(pov, grenadeData.pos, 550, screenSize);
                DrawCircle3d(bg, CircleData);
            }else if(grenadeData.name == "[GRENADE]"){
                Circle2D CircleData = GetCircle2D(pov, grenadeData.pos, 385, screenSize);
                DrawCircle2d(bg, CircleData);
            }else {
                Circle2D CircleData = GetCircle2D(pov, grenadeData.pos, 385, screenSize);
                DrawCircle2d(bg, CircleData);
            }

        }
    }
    if(ESP.Vehicles){
        for(PlayerData vehicleData:RealvehicleDataList){
             std::string s = "[" + vehicleData.name + " " + std::to_string(vehicleData.distance) + "m" + "]";
             ImVec2 textSize = ImGui::CalcTextSize(s.c_str());

             //bg->AddText(LootFont, 13.5f, {vehicleData.screen.x - textSize.x/2, vehicleData.screen.y}, ImColor(0,255,255,255), s.c_str()); 
             DrawText(bg, LootFont, 12.0f, {vehicleData.screen.x - textSize.x/2, vehicleData.screen.y}, ImColor(255,255,255), s.c_str(), true);  
                    
        }
    }
    if(ESP.Loot){
        for(PlayerData lootData:ReallootDataList){

            ImColor LootColor;

            if(lootData.type == 1){
                LootColor = ImColor(255,255,255,255);
            }else if(lootData.type == 2){
                LootColor = ImColor(0,255,0,255);
            }else if(lootData.type == 3){
                LootColor = ImColor(30,144,255,255);
            }else if(lootData.type == 4){
                LootColor = ImColor(128,0,0,255);
            }else if(lootData.type == 5){
                LootColor = ImColor(255,0,255,255);
            }else if(lootData.type == 6){
                LootColor = ImColor(255,100,0,255);
            }else if(lootData.type == 7){
                LootColor = ImColor(255,215,0,255);
            }

            std::string LootStr = "[" + lootData.name + " " + std::to_string(lootData.distance) + "m" + "]";

            bg->AddText(LootFont, 10.0f, lootData.screen, LootColor, LootStr.c_str());      
        }
    }
  }
}

id<MTLTexture> getCurrentWeapon(std::string name){
     if(name.c_str() == "M416"){
         return M416Texture;
     }else if(strstr(name.c_str(), "AKM")){
         return AKMTexture;
     }else if(strstr(name.c_str(), "ACE32")){
         return ACE32Texture;
     }else if(strstr(name.c_str(), "AUG")){
         return AUGTexture;
     }else if(strstr(name.c_str(), "FAMAS")){
         return FAMASTexture;
     }else if(strstr(name.c_str(), "G36C")){
         return G36CTexture;
     }else if(strstr(name.c_str(), "GROZA")){
         return GROZATexture;
     }else if(strstr(name.c_str(), "BADGER")){
         return BADGERTexture;
     }else if(strstr(name.c_str(), "M16A4")){
         return M16A4Texture;
     }else if(strstr(name.c_str(), "M762")){
         return M762Texture;
     }else if(strstr(name.c_str(), "MUTANT")){
         return MUTANTTexture;
     }else if(strstr(name.c_str(), "QBZ")){
         return QBZTexture;
     }else if(strstr(name.c_str(), "SCAR")){
         return SCARTexture;
     }else if(strstr(name.c_str(), "AWM")){
         return AWMTexture;
     }else if(strstr(name.c_str(), "AMR")){
         return AMRTexture;
     }else if(strstr(name.c_str(), "M24")){
         return M24Texture;
     }else if(strstr(name.c_str(), "Kar98k")){
         return KAR98KTexture;
     }else if(strstr(name.c_str(), "Mosin")){
         return MOSINTexture;
     }else if(strstr(name.c_str(), "DSR")){
         return DSRTexture;
     }else if(strstr(name.c_str(), "SKS")){
         return SKSTexture;
     }else if(strstr(name.c_str(), "Crossbow")){
         return CrossbowTexture;
     }else if(strstr(name.c_str(), "SLR")){
         return SLRTexture;
     }else if(strstr(name.c_str(), "Mini14")){
         return MINI14Texture;
     }else if(strstr(name.c_str(), "Mk14")){
         return MK14Texture;
     }else if(strstr(name.c_str(), "Mk12")){
         return MK12Texture;
     }else if(strstr(name.c_str(), "QBU")){
         return QBUTexture;
     }else {
         return M416Texture;
     }
}



- (void)mtkView:(MTKView*)view drawableSizeWillChange:(CGSize)size
{
    
}


@end
