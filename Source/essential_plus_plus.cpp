// SPDX-License-Identifier: GPL-3.0-only
// Essential++ for SDR++. Community source release.
// SDR++ Essential V40 for verified Windows x64 SDR++ 1.3.2 nightly (2026-11-18).
// Ctrl+wheel anchors the green preview; Ctrl+Up/Down center the selected red VFO.
// Green cursor marker snaps to selected VFO snap interval and previews current VFO bandwidth/reference.
// V11: modifier arrows, sidebar toggle and native source gain/sample-rate shortcuts.
// This bridge intentionally uses the same verified MSVC ABI as V7. Core-owned std::strings
// are copied with the core's constructor before calls taking them by value (callee destroys).

#ifdef ESSENTIAL_TEST
#define __declspec(x)
#define __cdecl
#define __stdcall
#define CORE_ALIAS(sym)
#else
#define CORE_ALIAS(sym) __asm__(sym)
#endif
#define MOD_EXPORT extern "C" __declspec(dllexport)
#define DLLIMPORT extern "C" __declspec(dllimport)

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef decltype(sizeof(0)) usize;

extern "C" void* __cdecl malloc(usize);
extern "C" void __cdecl free(void*);

static usize c_len(const char* s){usize n=0;if(s)while(s[n])n++;return n;}
static void c_copy(char*d,usize cap,const char*s){if(!cap)return;usize i=0;if(s)for(;i+1<cap&&s[i];i++)d[i]=s[i];d[i]=0;}
static void c_append(char*d,usize cap,const char*s){usize n=c_len(d);if(n<cap)c_copy(d+n,cap-n,s);}
static void u64str(u64 v,char*out,usize cap){char t[32];int n=0;do{t[n++]=(char)('0'+(v%10));v/=10;}while(v&&n<31);int j=0;while(n&&j+1<(int)cap)out[j++]=t[--n];out[j]=0;}
static void frac5(u32 v,char*out){out[0]=(char)('0'+((v/10000)%10));out[1]=(char)('0'+((v/1000)%10));out[2]=(char)('0'+((v/100)%10));out[3]=(char)('0'+((v/10)%10));out[4]=(char)('0'+(v%10));out[5]=0;}
static void frac1(u32 v,char*out){out[0]=(char)('0'+(v%10));out[1]=0;}

struct ImVec2 { float x,y; ImVec2(float X=0,float Y=0):x(X),y(Y){} };
struct ImVec4 { float x,y,z,w; };
typedef __builtin_va_list NativeVaList;
struct ImDrawList;

namespace ModuleManager {
    struct ModuleInfo_t { const char* name; const char* description; const char* author; const int versionMajor; const int versionMinor; const int versionBuild; const int maxInstances; };
    class Instance { public: virtual ~Instance(){}; virtual void postInit()=0; virtual void enable()=0; virtual void disable()=0; virtual bool isEnabled()=0; };
}

struct InputHandlerArgs {
    ImVec2 fftRectMin;
    ImVec2 fftRectMax;
    ImVec2 freqScaleRectMin;
    ImVec2 freqScaleRectMax;
    ImVec2 waterfallRectMin;
    ImVec2 waterfallRectMax;
    double lowFreq;
    double highFreq;
    double freqToPixelRatio;
    double pixelToFreqRatio;
};
struct InputEventHandler { void (*handler)(InputHandlerArgs,void*); void* ctx; };
struct RawVector { InputEventHandler** first; InputEventHandler** last; InputEventHandler** cap; };

DLLIMPORT u8 core_sink_manager[] CORE_ALIAS("sinkManager");
DLLIMPORT float core_ui_scale CORE_ALIAS("uiScale");
DLLIMPORT void* core_big_font CORE_ALIAS("bigFont");
DLLIMPORT void* core_normal_tuning_icon CORE_ALIAS("?NORMAL_TUNING@icons@@3PEAXEA");
DLLIMPORT void ui_PushFont(void*) CORE_ALIAS("?PushFont@ImGui@@YAXPEAUImFont@@@Z");
DLLIMPORT void ui_PopFont() CORE_ALIAS("?PopFont@ImGui@@YAXXZ");
DLLIMPORT ImVec2 ui_GetWindowContentRegionMin() CORE_ALIAS("?GetWindowContentRegionMin@ImGui@@YA?AUImVec2@@XZ");
DLLIMPORT void frequency_draw(void*) CORE_ALIAS("?draw@FrequencySelect@@QEAAXXZ");
DLLIMPORT void ui_SNRMeter(float,const ImVec2&) CORE_ALIAS("?SNRMeter@ImGui@@YAXMAEBUImVec2@@@Z");
DLLIMPORT bool ui_ImageButton(void*,const ImVec2&,const ImVec2&,const ImVec2&,int,const ImVec4&,const ImVec4&) CORE_ALIAS("?ImageButton@ImGui@@YA_NPEAXAEBUImVec2@@11HAEBUImVec4@@2@Z");
DLLIMPORT float ui_GetFontSize() CORE_ALIAS("?GetFontSize@ImGui@@YAMXZ");
DLLIMPORT float ui_CalcItemWidth() CORE_ALIAS("?CalcItemWidth@ImGui@@YAMXZ");
DLLIMPORT ImVec2 ui_GetItemRectMax() CORE_ALIAS("?GetItemRectMax@ImGui@@YA?AUImVec2@@XZ");
DLLIMPORT void draw_AddTextSized(ImDrawList*,const void*,float,const ImVec2&,u32,const char*,const char*,float,const void*) CORE_ALIAS("?AddText@ImDrawList@@QEAAXPEBUImFont@@MAEBUImVec2@@IPEBD2MPEBUImVec4@@@Z");

DLLIMPORT ImVec2 ui_GetCursorPos() CORE_ALIAS("?GetCursorPos@ImGui@@YA?AUImVec2@@XZ");
DLLIMPORT ImVec2 ui_GetCursorScreenPos() CORE_ALIAS("?GetCursorScreenPos@ImGui@@YA?AUImVec2@@XZ");
DLLIMPORT void ui_SameLine(float,float) CORE_ALIAS("?SameLine@ImGui@@YAXMM@Z");
DLLIMPORT void ui_Columns(int,const char*,bool) CORE_ALIAS("?Columns@ImGui@@YAXHPEBD_N@Z");
DLLIMPORT void ui_TextColored(const ImVec4&,const char*,...) CORE_ALIAS("?TextColored@ImGui@@YAXAEBUImVec4@@PEBDZZ");
DLLIMPORT void ui_TextColoredV(const ImVec4&,const char*,NativeVaList) CORE_ALIAS("?TextColoredV@ImGui@@YAXAEBUImVec4@@PEBDPEAD@Z");
DLLIMPORT void ui_BeginDisabled(bool) CORE_ALIAS("?BeginDisabled@ImGui@@YAX_N@Z");
DLLIMPORT void ui_EndDisabled() CORE_ALIAS("?EndDisabled@ImGui@@YAXXZ");
DLLIMPORT bool ui_IsItemHovered(int) CORE_ALIAS("?IsItemHovered@ImGui@@YA_NH@Z");
DLLIMPORT bool ui_IsWindowHovered(int) CORE_ALIAS("?IsWindowHovered@ImGui@@YA_NH@Z");
DLLIMPORT int audio_volume_run(void*) CORE_ALIAS("?run@Volume@audio@dsp@@UEAAHXZ");
DLLIMPORT void audio_stream_start(void*) CORE_ALIAS("?start@Stream@SinkManager@@QEAAXXZ");
DLLIMPORT void audio_stream_stop(void*) CORE_ALIAS("?stop@Stream@SinkManager@@QEAAXXZ");
DLLIMPORT void sink_showVolumeSlider(void*,void*,void*,float,float,int,bool) CORE_ALIAS("?showVolumeSlider@SinkManager@@QEAAXV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@0MMH_N@Z");
// Call the native audio mute functions and persist through SinkManager, not via
// an optional ImageButton patch. All four exports are verified in SDR++ core.
DLLIMPORT bool audio_getMuted(void*) CORE_ALIAS("?getMuted@Volume@audio@dsp@@QEAA_NXZ");
DLLIMPORT void audio_setMuted(void*,bool) CORE_ALIAS("?setMuted@Volume@audio@dsp@@QEAAX_N@Z");
DLLIMPORT void sink_saveStreamConfig(void*,void*) CORE_ALIAS("?saveStreamConfig@SinkManager@@AEAAXV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z");
DLLIMPORT void config_acquire(void*) CORE_ALIAS("?acquire@ConfigManager@@QEAAXXZ");
DLLIMPORT void config_release(void*,bool) CORE_ALIAS("?release@ConfigManager@@QEAAX_N@Z");
struct WinSystemTime { unsigned short year,month,dayOfWeek,day,hour,minute,second,milliseconds; };
DLLIMPORT void __stdcall GetSystemTime(WinSystemTime*);
DLLIMPORT void* __stdcall VirtualAlloc(void*,usize,u32,u32);
DLLIMPORT int __stdcall VirtualFree(void*,usize,u32);
DLLIMPORT int __stdcall FlushInstructionCache(void*,const void*,usize);
DLLIMPORT u8 core_waterfall[] CORE_ALIAS("waterfall");
DLLIMPORT u8 core_main_window[] CORE_ALIAS("mainWindow");
DLLIMPORT u8 core_mod_com[] CORE_ALIAS("modComManager");
DLLIMPORT u8 core_menu[] CORE_ALIAS("menu");
DLLIMPORT u8 core_config_manager[] CORE_ALIAS("configManager");
DLLIMPORT u8 core_source_manager[] CORE_ALIAS("sourceManager");
DLLIMPORT u8 core_vfo_manager[] CORE_ALIAS("vfoManager");
DLLIMPORT void manager_setOffset(void*,void*,double) CORE_ALIAS("?setOffset@VFOManager@@QEAAXV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@N@Z");
DLLIMPORT u8 core_selected_source[] CORE_ALIAS("?selectedSource@sourcemenu@@3V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@A");
DLLIMPORT void source_showMenu(void*) CORE_ALIAS("?showSelectedMenu@SourceManager@@QEAAXXZ");
DLLIMPORT void source_menuDraw(void*) CORE_ALIAS("?draw@sourcemenu@@YAXPEAX@Z");
DLLIMPORT void source_start(void*) CORE_ALIAS("?start@SourceManager@@QEAAXXZ");
DLLIMPORT void source_stop(void*) CORE_ALIAS("?stop@SourceManager@@QEAAXXZ");
DLLIMPORT void ui_PushItemFlag(int,bool) CORE_ALIAS("?PushItemFlag@ImGui@@YAXH_N@Z");
DLLIMPORT void ui_PopItemFlag() CORE_ALIAS("?PopItemFlag@ImGui@@YAXXZ");
DLLIMPORT bool ui_InvisibleButton(const char*,const ImVec2&,int) CORE_ALIAS("?InvisibleButton@ImGui@@YA_NPEBDAEBUImVec2@@H@Z");
DLLIMPORT void ui_OpenPopup(const char*,int) CORE_ALIAS("?OpenPopup@ImGui@@YAXPEBDH@Z");
DLLIMPORT bool ui_BeginPopup(const char*,int) CORE_ALIAS("?BeginPopup@ImGui@@YA_NPEBDH@Z");
DLLIMPORT bool ui_IsMouseClicked(int,bool) CORE_ALIAS("?IsMouseClicked@ImGui@@YA_NH_N@Z");
DLLIMPORT bool ui_IsMouseDown(int) CORE_ALIAS("?IsMouseDown@ImGui@@YA_NH@Z");
DLLIMPORT void ui_StartMouseMovingWindow(void*) CORE_ALIAS("?StartMouseMovingWindow@ImGui@@YAXPEAUImGuiWindow@@@Z");
DLLIMPORT bool ui_BeginPopupModal(const char*,bool*,int) CORE_ALIAS("?BeginPopupModal@ImGui@@YA_NPEBDPEA_NH@Z");
DLLIMPORT bool ui_Selectable(const char*,bool,int,const ImVec2&) CORE_ALIAS("?Selectable@ImGui@@YA_NPEBD_NHAEBUImVec2@@@Z");
DLLIMPORT void ui_EndPopup() CORE_ALIAS("?EndPopup@ImGui@@YAXXZ");
DLLIMPORT void ui_CloseCurrentPopup() CORE_ALIAS("?CloseCurrentPopup@ImGui@@YAXXZ");
DLLIMPORT void ui_SetKeyboardFocusHere(int) CORE_ALIAS("?SetKeyboardFocusHere@ImGui@@YAXH@Z");
DLLIMPORT bool sm_InputText(const char*,char*,usize,int) CORE_ALIAS("?InputText@SmGui@@YA_NPEBDPEAD_KH@Z");
DLLIMPORT bool sm_InputInt(const char*,int*,int,int,int) CORE_ALIAS("?InputInt@SmGui@@YA_NPEBDPEAHHHH@Z");
DLLIMPORT bool sm_Button(const char*,ImVec2) CORE_ALIAS("?Button@SmGui@@YA_NPEBDUImVec2@@@Z");
DLLIMPORT double wf_getBandwidth(void*) CORE_ALIAS("?getBandwidth@WaterFall@ImGui@@QEAANXZ");
DLLIMPORT double wf_getViewBandwidth(void*) CORE_ALIAS("?getViewBandwidth@WaterFall@ImGui@@QEAANXZ");
DLLIMPORT double wf_getViewOffset(void*) CORE_ALIAS("?getViewOffset@WaterFall@ImGui@@QEAANXZ");
DLLIMPORT double wf_getCenterFrequency(void*) CORE_ALIAS("?getCenterFrequency@WaterFall@ImGui@@QEAANXZ");
DLLIMPORT void wf_setViewBandwidth(void*,double) CORE_ALIAS("?setViewBandwidth@WaterFall@ImGui@@QEAAXN@Z");
DLLIMPORT void wf_setFullWaterfallUpdate(void*,bool) CORE_ALIAS("?setFullWaterfallUpdate@WaterFall@ImGui@@QEAAX_N@Z");
DLLIMPORT void wf_updateWaterfallFb(void*) CORE_ALIAS("?updateWaterfallFb@WaterFall@ImGui@@AEAAXXZ");
DLLIMPORT bool core_fullWaterfallUpdate CORE_ALIAS("?fullWaterfallUpdate@displaymenu@@3_NA");
DLLIMPORT void wf_setViewOffset(void*,double) CORE_ALIAS("?setViewOffset@WaterFall@ImGui@@QEAAXN@Z");
DLLIMPORT void vfo_setOffset(void*,double) CORE_ALIAS("?setOffset@WaterfallVFO@ImGui@@QEAAXN@Z");
DLLIMPORT void window_setZoomSlider(void*,float) CORE_ALIAS("?setViewBandwidthSlider@MainWindow@@QEAAXM@Z");
DLLIMPORT bool window_isPlaying(void*) CORE_ALIAS("?isPlaying@MainWindow@@QEAA_NXZ");
DLLIMPORT void window_setPlayState(void*,bool) CORE_ALIAS("?setPlayState@MainWindow@@QEAAX_N@Z");
DLLIMPORT void core_tune(int,void*,double) CORE_ALIAS("?tune@tuner@@YAXHV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@N@Z");
DLLIMPORT void* core_stringCopy(void*,const void*) CORE_ALIAS("??0?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QEAA@AEBV01@@Z");
DLLIMPORT void* core_stringFromText(void*,const char*) CORE_ALIAS("??0?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QEAA@QEBD@Z");
DLLIMPORT void core_stringDestroy(void*) CORE_ALIAS("??1?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QEAA@XZ");
DLLIMPORT void menu_register(void*,void*,void(*)(void*),void*,ModuleManager::Instance*) CORE_ALIAS("?registerEntry@Menu@@QEAAXV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@P6AXPEAX@Z1PEAVInstance@ModuleManager@@@Z");
DLLIMPORT void menu_remove(void*,void*) CORE_ALIAS("?removeEntry@Menu@@QEAAXV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z");
DLLIMPORT bool com_exists(void*,void*) CORE_ALIAS("?interfaceExists@ModuleComManager@@QEAA_NV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z");
DLLIMPORT void* com_moduleName(void*,void*,void*) CORE_ALIAS("?getModuleName@ModuleComManager@@QEAA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V23@@Z");
DLLIMPORT bool com_call(void*,void*,int,void*,void*) CORE_ALIAS("?callInterface@ModuleComManager@@QEAA_NV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@HPEAX1@Z");

// std::map<std::string, ImGui::WaterfallVFO*>::operator[] from this exact SDR++ build.
// We pass the map object and the already-existing selectedVFO std::string object; no STL construction is needed in this module.
DLLIMPORT void** wf_vfo_index(void*,void*) CORE_ALIAS("??A?$map@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PEAVWaterfallVFO@ImGui@@U?$less@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PEAVWaterfallVFO@ImGui@@@std@@@2@@std@@QEAAAEAPEAVWaterfallVFO@ImGui@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@1@@Z");

DLLIMPORT void ui_SetClipboardText(const char*) CORE_ALIAS("?SetClipboardText@ImGui@@YAXPEBD@Z");
DLLIMPORT ImVec2 ui_GetMousePos() CORE_ALIAS("?GetMousePos@ImGui@@YA?AUImVec2@@XZ");
DLLIMPORT int ui_GetMouseCursor() CORE_ALIAS("?GetMouseCursor@ImGui@@YAHXZ");
DLLIMPORT void ui_SetMouseCursor(int) CORE_ALIAS("?SetMouseCursor@ImGui@@YAXH@Z");
DLLIMPORT void* ui_GetIO() CORE_ALIAS("?GetIO@ImGui@@YAAEAUImGuiIO@@XZ");
DLLIMPORT ImDrawList* ui_GetForegroundDrawList() CORE_ALIAS("?GetForegroundDrawList@ImGui@@YAPEAUImDrawList@@XZ");
DLLIMPORT ImVec2 ui_CalcTextSize(const char*,const char*,bool,float) CORE_ALIAS("?CalcTextSize@ImGui@@YA?AUImVec2@@PEBD0_NM@Z");
DLLIMPORT void draw_AddRectFilled(ImDrawList*,const ImVec2&,const ImVec2&,u32,float,int) CORE_ALIAS("?AddRectFilled@ImDrawList@@QEAAXAEBUImVec2@@0IMH@Z");
DLLIMPORT void draw_AddText(ImDrawList*,const ImVec2&,u32,const char*,const char*) CORE_ALIAS("?AddText@ImDrawList@@QEAAXAEBUImVec2@@IPEBD1@Z");
DLLIMPORT void draw_AddLine(ImDrawList*,const ImVec2&,const ImVec2&,u32,float) CORE_ALIAS("?AddLine@ImDrawList@@QEAAXAEBUImVec2@@0IM@Z");
DLLIMPORT void draw_PushClipRect(ImDrawList*,ImVec2,ImVec2,bool) CORE_ALIAS("?PushClipRect@ImDrawList@@QEAAXUImVec2@@0_N@Z");
DLLIMPORT void draw_PopClipRect(ImDrawList*) CORE_ALIAS("?PopClipRect@ImDrawList@@QEAAXXZ");
DLLIMPORT float* wf_acquireLatestFFT(void*,int&) CORE_ALIAS("?acquireLatestFFT@WaterFall@ImGui@@QEAAPEAMAEAH@Z");
DLLIMPORT void wf_releaseLatestFFT(void*) CORE_ALIAS("?releaseLatestFFT@WaterFall@ImGui@@QEAAXXZ");
DLLIMPORT short __stdcall GetAsyncKeyState(int);
DLLIMPORT void* __stdcall GetForegroundWindow();
DLLIMPORT u32 __stdcall GetWindowThreadProcessId(void*,u32*);
DLLIMPORT u32 __stdcall GetCurrentProcessId();
DLLIMPORT void* __stdcall GetModuleHandleA(const char*);
DLLIMPORT void* __stdcall GetCurrentProcess();
DLLIMPORT int __stdcall K32EnumProcessModules(void*,void**,u32,u32*);
DLLIMPORT void* __stdcall GetProcAddress(void*,const char*);
DLLIMPORT int __stdcall GetModuleHandleExA(u32,const char*,void**);
DLLIMPORT int __stdcall VirtualProtect(void*,usize,u32,u32*);
DLLIMPORT int __stdcall MessageBoxA(void*,const char*,const char*,u32);
DLLIMPORT int __stdcall MultiByteToWideChar(u32,u32,const char*,int,unsigned short*,int);
DLLIMPORT void* __stdcall CreateFileW(const unsigned short*,u32,u32,void*,u32,u32,void*);
DLLIMPORT u32 __stdcall GetFileSize(void*,u32*);
DLLIMPORT int __stdcall ReadFile(void*,void*,u32,u32*,void*);
DLLIMPORT int __stdcall WriteFile(void*,const void*,u32,u32*,void*);
DLLIMPORT int __stdcall FlushFileBuffers(void*);
DLLIMPORT int __stdcall CloseHandle(void*);
DLLIMPORT int __stdcall MoveFileExW(const unsigned short*,const unsigned short*,u32);
DLLIMPORT int __stdcall DeleteFileW(const unsigned short*);
DLLIMPORT u32 __stdcall GetLastError();
DLLIMPORT void* __stdcall FindFirstFileW(const unsigned short*,void*);
DLLIMPORT int __stdcall FindNextFileW(void*,void*);
DLLIMPORT int __stdcall FindClose(void*);

struct KeyData { bool Down; float DownDuration; float DownDurationPrev; float AnalogValue; };
struct ContextHook { u32 HookId; int Type; u32 Owner; void (*Callback)(void*,ContextHook*); void* UserData; };
struct CharQueue { int Size,Capacity; unsigned short* Data; };
DLLIMPORT void* ui_GetContext() CORE_ALIAS("?GetCurrentContext@ImGui@@YAPEAUImGuiContext@@XZ");
DLLIMPORT const char* ui_GetVersion() CORE_ALIAS("?GetVersion@ImGui@@YAPEBDXZ");
DLLIMPORT int ui_GetFrameCount() CORE_ALIAS("?GetFrameCount@ImGui@@YAHXZ");
DLLIMPORT double ui_GetTime() CORE_ALIAS("?GetTime@ImGui@@YANXZ");
DLLIMPORT u32 ui_AddHook(void*,const ContextHook*) CORE_ALIAS("?AddContextHook@ImGui@@YAIPEAUImGuiContext@@PEBUImGuiContextHook@@@Z");
DLLIMPORT void ui_RemoveHook(void*,u32) CORE_ALIAS("?RemoveContextHook@ImGui@@YAXPEAUImGuiContext@@I@Z");
DLLIMPORT KeyData* ui_GetKeyData(int) CORE_ALIAS("?GetKeyData@ImGui@@YAPEAUImGuiKeyData@@H@Z");
DLLIMPORT bool ui_IsKeyPressed(int,bool) CORE_ALIAS("?IsKeyPressed@ImGui@@YA_NH_N@Z");
DLLIMPORT bool ui_IsAnyItemActive() CORE_ALIAS("?IsAnyItemActive@ImGui@@YA_NXZ");
DLLIMPORT bool ui_IsPopupOpen(u32,int) CORE_ALIAS("?IsPopupOpen@ImGui@@YA_NIH@Z");
DLLIMPORT void ui_BeginTooltip() CORE_ALIAS("?BeginTooltip@ImGui@@YAXXZ");
// BeginTooltipEx(OverridePrevious=1) hides the native Frequency Manager tooltip
// without modifying ImGui internals or other tooltip producers.
DLLIMPORT void ui_BeginTooltipEx(int,int) CORE_ALIAS("?BeginTooltipEx@ImGui@@YAXHH@Z");
DLLIMPORT void ui_EndTooltip() CORE_ALIAS("?EndTooltip@ImGui@@YAXXZ");
DLLIMPORT void ui_TextUnformatted(const char*,const char*) CORE_ALIAS("?TextUnformatted@ImGui@@YAXPEBD0@Z");
DLLIMPORT void ui_Separator() CORE_ALIAS("?Separator@ImGui@@YAXXZ");
DLLIMPORT void ui_PushTextColor(int,u32) CORE_ALIAS("?PushStyleColor@ImGui@@YAXHI@Z");
DLLIMPORT void ui_PopStyleColor(int) CORE_ALIAS("?PopStyleColor@ImGui@@YAXH@Z");
DLLIMPORT bool ui_BeginTable(const char*,int,int,const ImVec2&,float) CORE_ALIAS("?BeginTable@ImGui@@YA_NPEBDHHAEBUImVec2@@M@Z");
DLLIMPORT void ui_EndTable() CORE_ALIAS("?EndTable@ImGui@@YAXXZ");
DLLIMPORT void ui_TableSetupColumn(const char*,int,float,u32) CORE_ALIAS("?TableSetupColumn@ImGui@@YAXPEBDHMI@Z");
DLLIMPORT void ui_TableHeadersRow() CORE_ALIAS("?TableHeadersRow@ImGui@@YAXXZ");
DLLIMPORT void ui_TableNextRow(int,float) CORE_ALIAS("?TableNextRow@ImGui@@YAXHM@Z");
DLLIMPORT bool ui_TableNextColumn() CORE_ALIAS("?TableNextColumn@ImGui@@YA_NXZ");
DLLIMPORT void ui_SetNextItemWidth(float) CORE_ALIAS("?SetNextItemWidth@ImGui@@YAXM@Z");
DLLIMPORT void ui_PushID(int) CORE_ALIAS("?PushID@ImGui@@YAXH@Z");
DLLIMPORT void ui_PopID() CORE_ALIAS("?PopID@ImGui@@YAXXZ");
DLLIMPORT bool ui_InputText(const char*,char*,usize,int,int(*)(void*),void*) CORE_ALIAS("?InputText@ImGui@@YA_NPEBDPEAD_KHP6AHPEAUImGuiInputTextCallbackData@@@ZPEAX@Z");
DLLIMPORT bool ui_InputTextWithHint(const char*,const char*,char*,usize,int,int(*)(void*),void*) CORE_ALIAS("?InputTextWithHint@ImGui@@YA_NPEBD0PEAD_KHP6AHPEAUImGuiInputTextCallbackData@@@ZPEAX@Z");
DLLIMPORT bool ui_InputInt(const char*,int*,int,int,int) CORE_ALIAS("?InputInt@ImGui@@YA_NPEBDPEAHHHH@Z");
DLLIMPORT bool ui_IsItemActive() CORE_ALIAS("?IsItemActive@ImGui@@YA_NXZ");
DLLIMPORT bool ui_InputFloat(const char*,float*,float,float,const char*,int) CORE_ALIAS("?InputFloat@ImGui@@YA_NPEBDPEAMMM0H@Z");
DLLIMPORT bool ui_Combo(const char*,int*,const char*,int) CORE_ALIAS("?Combo@ImGui@@YA_NPEBDPEAH0H@Z");
DLLIMPORT bool ui_IsItemDeactivatedAfterEdit() CORE_ALIAS("?IsItemDeactivatedAfterEdit@ImGui@@YA_NXZ");
DLLIMPORT bool ui_Button(const char*,const ImVec2&) CORE_ALIAS("?Button@ImGui@@YA_NPEBDAEBUImVec2@@@Z");
DLLIMPORT bool ui_Checkbox(const char*,bool*) CORE_ALIAS("?Checkbox@ImGui@@YA_NPEBDPEA_N@Z");
DLLIMPORT bool ui_BeginChild(const char*, const ImVec2&, bool, int) CORE_ALIAS("?BeginChild@ImGui@@YA_NPEBDAEBUImVec2@@_NH@Z");
DLLIMPORT void ui_EndChild() CORE_ALIAS("?EndChild@ImGui@@YAXXZ");
DLLIMPORT bool ui_Begin(const char*,bool*,int) CORE_ALIAS("?Begin@ImGui@@YA_NPEBDPEA_NH@Z");
DLLIMPORT void ui_End() CORE_ALIAS("?End@ImGui@@YAXXZ");
DLLIMPORT void* ui_GetCurrentWindow() CORE_ALIAS("?GetCurrentWindow@ImGui@@YAPEAUImGuiWindow@@XZ");
DLLIMPORT void ui_BringWindowToDisplayFront(void*) CORE_ALIAS("?BringWindowToDisplayFront@ImGui@@YAXPEAUImGuiWindow@@@Z");
DLLIMPORT void ui_SetNextWindowPos(const ImVec2&,int,const ImVec2&) CORE_ALIAS("?SetNextWindowPos@ImGui@@YAXAEBUImVec2@@H0@Z");
DLLIMPORT void ui_SetWindowPos(const ImVec2&,int) CORE_ALIAS("?SetWindowPos@ImGui@@YAXAEBUImVec2@@H@Z");
DLLIMPORT void ui_SetNextWindowSize(const ImVec2&,int) CORE_ALIAS("?SetNextWindowSize@ImGui@@YAXAEBUImVec2@@H@Z");
DLLIMPORT ImVec2 ui_GetWindowPos() CORE_ALIAS("?GetWindowPos@ImGui@@YA?AUImVec2@@XZ");
DLLIMPORT ImVec2 ui_GetWindowSize() CORE_ALIAS("?GetWindowSize@ImGui@@YA?AUImVec2@@XZ");
DLLIMPORT ImDrawList* ui_GetWindowDrawList() CORE_ALIAS("?GetWindowDrawList@ImGui@@YAPEAUImDrawList@@XZ");
DLLIMPORT void ui_SetCursorPos(const ImVec2&) CORE_ALIAS("?SetCursorPos@ImGui@@YAXAEBUImVec2@@@Z");
DLLIMPORT void ui_SetCursorPosY(float) CORE_ALIAS("?SetCursorPosY@ImGui@@YAXM@Z");
DLLIMPORT void ui_Dummy(const ImVec2&) CORE_ALIAS("?Dummy@ImGui@@YAXAEBUImVec2@@@Z");
DLLIMPORT void ui_PushStyleVarFloat(int,float) CORE_ALIAS("?PushStyleVar@ImGui@@YAXHM@Z");
DLLIMPORT void ui_PushStyleVarVec(int,const ImVec2&) CORE_ALIAS("?PushStyleVar@ImGui@@YAXHAEBUImVec2@@@Z");
DLLIMPORT void ui_PopStyleVar(int) CORE_ALIAS("?PopStyleVar@ImGui@@YAXH@Z");
DLLIMPORT void ui_SetColumnWidth(int,float) CORE_ALIAS("?SetColumnWidth@ImGui@@YAXHM@Z");
DLLIMPORT ImVec2 ui_GetMousePos() CORE_ALIAS("?GetMousePos@ImGui@@YA?AUImVec2@@XZ");
DLLIMPORT bool ui_VSliderFloat(const char*,const ImVec2&,float*,float,float,const char*,int) CORE_ALIAS("?VSliderFloat@ImGui@@YA_NPEBDAEBUImVec2@@PEAMMM0H@Z");
DLLIMPORT void ui_NewLine() CORE_ALIAS("?NewLine@ImGui@@YAXXZ");
DLLIMPORT void ui_TextUnformatted(const char*,const char*) CORE_ALIAS("?TextUnformatted@ImGui@@YAXPEBD0@Z");
DLLIMPORT void ui_SetCursorPosX(float) CORE_ALIAS("?SetCursorPosX@ImGui@@YAXM@Z");
DLLIMPORT void wf_onResize(void*) CORE_ALIAS("?onResize@WaterFall@ImGui@@AEAAXXZ");
DLLIMPORT void wf_updateAllVFOs(void*,bool) CORE_ALIAS("?updateAllVFOs@WaterFall@ImGui@@AEAAX_N@Z");
DLLIMPORT float wf_getFFTMin(void*) CORE_ALIAS("?getFFTMin@WaterFall@ImGui@@QEAAMXZ");
DLLIMPORT float wf_getFFTMax(void*) CORE_ALIAS("?getFFTMax@WaterFall@ImGui@@QEAAMXZ");
DLLIMPORT void wf_setWaterfallMin(void*,float) CORE_ALIAS("?setWaterfallMin@WaterFall@ImGui@@QEAAXM@Z");
DLLIMPORT void wf_setWaterfallMax(void*,float) CORE_ALIAS("?setWaterfallMax@WaterFall@ImGui@@QEAAXM@Z");
DLLIMPORT float wf_getWaterfallMin(void*) CORE_ALIAS("?getWaterfallMin@WaterFall@ImGui@@QEAAMXZ");
DLLIMPORT float wf_getWaterfallMax(void*) CORE_ALIAS("?getWaterfallMax@WaterFall@ImGui@@QEAAMXZ");
DLLIMPORT u8 core_iq_frontend[] CORE_ALIAS("iqFrontEnd");
DLLIMPORT int core_display_fft_rate CORE_ALIAS("?fftRate@displaymenu@@3HA");
DLLIMPORT void frontend_setFFTRate(void*,double) CORE_ALIAS("?setFFTRate@IQFrontEnd@@QEAAXN@Z");
DLLIMPORT void display_updateFFTSpeeds() CORE_ALIAS("?updateFFTSpeeds@displaymenu@@YAXXZ");

DLLIMPORT bool ui_SliderInt(const char*,int*,int,int,const char*,int) CORE_ALIAS("?SliderInt@ImGui@@YA_NPEBDPEAHHH0H@Z");
DLLIMPORT bool sm_Combo(const char*,int*,const char*,int) CORE_ALIAS("?Combo@SmGui@@YA_NPEBDPEAH0H@Z");
DLLIMPORT bool sm_Checkbox(const char*,bool*) CORE_ALIAS("?Checkbox@SmGui@@YA_NPEBDPEA_N@Z");
DLLIMPORT bool sm_SliderInt(const char*,int*,int,int,int,int) CORE_ALIAS("?SliderInt@SmGui@@YA_NPEBDPEAHHHW4FormatString@1@H@Z");
DLLIMPORT bool sm_SliderFloat(const char*,float*,float,float,int,int) CORE_ALIAS("?SliderFloat@SmGui@@YA_NPEBDPEAMMMW4FormatString@1@H@Z");
DLLIMPORT bool sm_SliderFloatSteps(const char*,float*,float,float,float,int) CORE_ALIAS("?SliderFloatWithSteps@SmGui@@YA_NPEBDPEAMMMMW4FormatString@1@@Z");
DLLIMPORT void sm_DrawListDraw(void*,void*,void*,bool*) CORE_ALIAS("?draw@DrawList@SmGui@@QEAAXAEAV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEAUDrawListElem@2@AEA_N@Z");

static bool g_supported=false, g_enabled=true;
static bool c_equal(const char*a,const char*b){if(!a||!b)return false;while(*a&&*a==*b){a++;b++;}return *a==*b;}
struct alignas(8) CoreString { u8 bytes[32]; };
static_assert(sizeof(CoreString)==32&&alignof(CoreString)==8,"MSVC x64 string ABI");
static_assert(sizeof(KeyData)==16&&sizeof(ContextHook)==32&&sizeof(CharQueue)==16,"ImGui 1.87 x64 ABI");
static_assert(sizeof(InputHandlerArgs)==80,"Waterfall input ABI");
static const char* string_chars(const void*p){return *(const u64*)((const u8*)p+24)>=16?*(const char* const*)p:(const char*)p;}
#include "localization.inc"
static bool check_core_abi(){
#ifdef ESSENTIAL_TEST
    return true;
#else
    u8*p=(u8*)GetModuleHandleA("sdrpp_core.dll");
    if(!p||*(unsigned short*)p!=0x5A4D)return false;
    u32 pe=*(u32*)(p+0x3C);
    return pe<4096 && *(u32*)(p+pe)==0x4550 && *(u32*)(p+pe+8)==0x6AFE16EB &&
           *(u32*)(p+pe+24+56)==0x819000 && c_equal(ui_GetVersion(),"1.87");
#endif
}

// Verified from this exact build's WaterFall layout/disassembly.
static bool* wf_input_handled(){ return (bool*)(core_waterfall+0x70); }
static RawVector* wf_input_vec(){ return (RawVector*)(core_waterfall+0x78); }
static void* wf_vfo_map(){ return (void*)(core_waterfall+0x20); }
static void* wf_selected_name(){ return (void*)(core_waterfall+0x30); }
static u64 wf_selected_name_size(){ return *(u64*)(core_waterfall+0x40); }

// Verified WaterfallVFO field offsets from setBandwidth/setReference/setSnapInterval disassembly.
static double vfo_bandwidth(void*p){return p?*(double*)((u8*)p+0x20):0.0;}
static double vfo_snap(void*p){return p?*(double*)((u8*)p+0x28):0.0;}
static int vfo_reference(void*p){return p?*(int*)((u8*)p+0x30):1;}
static void* selected_vfo(){
    u64 n=wf_selected_name_size();
    if(n==0 || n>255)return 0;
    void**slot=wf_vfo_index(wf_vfo_map(),wf_selected_name());
    return slot?*slot:0;
}

static bool radio_interface_ready(){
    if(!selected_vfo())return false;
    CoreString name;core_stringCopy(&name,wf_selected_name());
    if(!com_exists(core_mod_com,&name))return false; // by-value name is destroyed by the core
    CoreString module;core_stringCopy(&name,wf_selected_name());
    com_moduleName(core_mod_com,&module,&name);
    bool radio=c_equal(string_chars(&module),"radio");
    core_stringDestroy(&module);
    return radio;
}
static bool radio_command(int code,void*in,void*out){
    CoreString name;core_stringCopy(&name,wf_selected_name());
    return com_call(core_mod_com,&name,code,in,out);
}
static int selected_modulation(){
    int mode=-1;
    if(radio_interface_ready())radio_command(0,0,&mode);
    return mode;
}
// UI order is also the requested 1..8 mapping; it differs from Radio's enum order.
static const int MODE_ORDER[8]={0,1,2,6,4,3,5,7};
static int mode_index(int mode){for(int i=0;i<8;i++)if(MODE_ORDER[i]==mode)return i;return -1;}
static bool set_modulation(int mode){
    if(mode_index(mode)<0||!radio_interface_ready())return false;
    int current=-1;if(!radio_command(0,0,&current))return false;
    if(current==mode)return true; // avoid rebuilding an unchanged DSP demodulator
    if(!radio_command(1,&mode,0))return false;
    current=-1;return radio_command(0,0,&current)&&current==mode;
}
static void cycle_modulation(int direction){
    int index=mode_index(selected_modulation());
    if(index>=0)set_modulation(MODE_ORDER[(index+direction+8)%8]);
}
static const char* modulation_name(int mode){
    static const char* names[]={"NFM","WFM","AM","DSB","USB","CW","LSB","RAW"};
    return mode>=0&&mode<8?names[mode]:"---";
}
static int g_tuning_mode=-1;
// 0 = no info popup, 1 = text, 2 = text + meter; Left-Alt cycles these.
static int g_marker_info_mode=0;
static bool g_fixed_pan_pending=false,g_fixed_pan_force=false;
static bool g_free_pan_pending=false,g_free_pan_force=false;
static double g_fixed_marker_hz=0.0;
static double selected_frequency(){
    void*v=selected_vfo();
    return v?wf_getCenterFrequency(core_waterfall)+*(double*)v:0.0;
}
static void tune_frequency(double hz){
    if(!selected_vfo()||!(hz==hz))return;
    if(hz<0.0)hz=0.0;if(hz>999999999999.0)hz=999999999999.0;
    // MainWindow's selected tuning mode, verified in the V7 core's draw() disassembly.
    int mode=*(int*)(core_main_window+0xFC);if(mode<0||mode>3)mode=1;
    CoreString name;core_stringCopy(&name,wf_selected_name());
    core_tune(mode,&name,hz); // handles DSP/source retuning, also across the visible window
    // Native normalTuning has a boundary branch which treats a filter's centre
    // as its reference frequency. For LSB/USB that shifts the carrier by BW/2.
    // Finish at the requested carrier through VFOManager so the DSP and marker
    // agree, even after a modulation/filter change or a hardware retune.
    if(selected_frequency()!=hz){
        core_stringCopy(&name,wf_selected_name());
        manager_setOffset(core_vfo_manager,&name,hz-wf_getCenterFrequency(core_waterfall));
    }
    if(g_tuning_mode==2)g_fixed_marker_hz=hz;
}
static void change_frequency(double step){tune_frequency(selected_frequency()+step);}
static void change_filter(double step){
    void*v=selected_vfo();if(!v||!radio_interface_ready())return;
    float bw=(float)(vfo_bandwidth(v)+step);
    // The radio interface clamps to the active demodulator's limits and respects a locked filter.
    radio_command(3,&bw,0);
}

#include "source_controls.inc"
#include "radio_controls.inc"

static bool in_rect(const ImVec2&p,const ImVec2&a,const ImVec2&b){return p.x>=a.x&&p.x<=b.x&&p.y>=a.y&&p.y<=b.y;}
static float clamp01(float x){if(x<0)return 0;if(x>1)return 1;return x;}
static double dabs(double x){return x<0?-x:x;}
static double round_nearest(double x){return x>=0.0?(double)((long long)(x+0.5)):(double)((long long)(x-0.5));}
static double positive_sqrt(double x){if(x<=0.0)return 0.0;double y=1.0;for(int i=0;i<20;i++)y=0.5*(y+x/y);return y;}
static float slider_from_bandwidth(double view,double whole){
    if(whole<=1000.0)return 1.0f;
    double r=(view-1000.0)/(whole-1000.0);if(r<0.0)r=0.0;if(r>1.0)r=1.0;
    return (float)positive_sqrt(r);
}
static void sync_zoom_slider(){
    window_setZoomSlider(core_main_window,slider_from_bandwidth(wf_getViewBandwidth(core_waterfall),wf_getBandwidth(core_waterfall)));
}
// Preserve the actual visible span in Hz and its absolute center. Core resets
// the Waterfall view to full width when a source/rate is reinitialized.
static float g_slider_snapshot=1.0f;
struct ZoomTransition {
    bool pending,applied;
    double span,center,deadline,lastWhole,began;
    int stableFrames;bool sawBandwidthChange;
};
static ZoomTransition g_zoom_transition={};
struct ZoomPreference { bool valid;double span,center; };
static ZoomPreference g_zoom_preference={};
static void zoom_record_preference(){
    double whole=wf_getBandwidth(core_waterfall),span=wf_getViewBandwidth(core_waterfall);
    if(!(whole>0&&span>0&&span<whole*0.999)){g_zoom_preference={};return;}
    g_zoom_preference={true,span,wf_getCenterFrequency(core_waterfall)+wf_getViewOffset(core_waterfall)};
}
static void zoom_prepare_source_transition(){
    if(g_zoom_transition.pending)return;
    double whole=wf_getBandwidth(core_waterfall),span=wf_getViewBandwidth(core_waterfall);
    if(!(whole>0&&span>0))return;
    if(span<whole*0.999)zoom_record_preference();
    if(!g_zoom_preference.valid)return;
    g_zoom_transition={};g_zoom_transition.pending=true;
    g_zoom_transition.span=g_zoom_preference.span;
    g_zoom_transition.center=g_zoom_preference.center;
    g_zoom_transition.began=ui_GetTime();
    g_zoom_transition.deadline=ui_GetTime()+3.0;
    g_zoom_transition.lastWhole=whole;
}
static void zoom_finish_source_transition(){
    // Keep the snapshot through asynchronous USB/remote reconfiguration.
    if(g_zoom_transition.pending){g_zoom_transition.stableFrames=0;g_zoom_transition.applied=false;}
}
static void zoom_restore_frame(){
    ZoomTransition&z=g_zoom_transition;
    if(!z.pending)return;
    double now=ui_GetTime();if(now>z.deadline){z={};return;}
    double whole=wf_getBandwidth(core_waterfall),span=wf_getViewBandwidth(core_waterfall);
    if(!(whole>0&&span>0))return;
    double minSpan=whole<20?whole:20;
    double target=z.span<whole?z.span:whole;
    if(target<minSpan)target=minSpan;
    double center=wf_getCenterFrequency(core_waterfall);
    double offset=z.center-center;
    double maxOffset=(whole-target)*0.5;
    if(offset>maxOffset)offset=maxOffset;
    if(offset<-maxOffset)offset=-maxOffset;
    double currentOffset=wf_getViewOffset(core_waterfall);
    bool reset=dabs(span-target)>target*0.0001+1.0||dabs(currentOffset-offset)>2.0;
    bool bandwidthChanged=dabs(whole-z.lastWhole)>1.0;
    if(bandwidthChanged)z.sawBandwidthChange=true;
    if(reset||bandwidthChanged||!z.applied){
        wf_setViewBandwidth(core_waterfall,target);
        wf_setViewOffset(core_waterfall,offset);
        sync_zoom_slider();
        g_slider_snapshot=*(float*)(core_main_window+0x9C);
        z.applied=true;z.stableFrames=0;
    }else if(z.stableFrames<30)z.stableFrames++;
    z.lastWhole=whole;
    if(z.stableFrames>=18&&(z.sawBandwidthChange||now-z.began>1.5))z={};
}

// Verified WaterfallVFO hit-test geometry offsets from this exact build's updateDrawingVars disassembly.
static ImVec2 vfo_vec2(void* p, usize off){ return p ? *(ImVec2*)((u8*)p+off) : ImVec2(); }
static bool selected_vfo_edge_hover(const ImVec2& m){
    void* v=selected_vfo(); if(!v) return false;
    return in_rect(m,vfo_vec2(v,0x84),vfo_vec2(v,0x8C)) ||
           in_rect(m,vfo_vec2(v,0x94),vfo_vec2(v,0x9C)) ||
           in_rect(m,vfo_vec2(v,0xA4),vfo_vec2(v,0xAC)) ||
           in_rect(m,vfo_vec2(v,0xB4),vfo_vec2(v,0xBC));
}
static bool selected_vfo_body_hover(const ImVec2& m){
    void* v=selected_vfo(); if(!v) return false;
    return in_rect(m,vfo_vec2(v,0x44),vfo_vec2(v,0x4C)) ||
           in_rect(m,vfo_vec2(v,0x64),vfo_vec2(v,0x6C));
}

// The carrier is only a thin line. Treat the *entire selected filter passband*
// as a grab surface in both spectrum and waterfall, independently of the
// native VFO's cached hover rectangles and its reference mode (USB/LSB/CW).
// Keep the native bandwidth resize grips ahead of this hit test.
static bool selected_vfo_passband_hit(const InputHandlerArgs& a,const ImVec2& mouse){
    void* v=selected_vfo();if(!v)return false;
    if(!(in_rect(mouse,a.fftRectMin,a.fftRectMax)||
         in_rect(mouse,a.waterfallRectMin,a.waterfallRectMax)))return false;
    const double bw=vfo_bandwidth(v),carrier=selected_frequency();
    if(!(bw>0.0)||!(a.freqToPixelRatio>0.0)||!(a.highFreq>a.lowFreq))return false;
    const int ref=vfo_reference(v);
    double lower=carrier,upper=carrier;
    if(ref==0)upper+=bw;              // REF_LOWER: USB-style filter
    else if(ref==2)lower-=bw;         // REF_UPPER: LSB-style filter
    else {lower-=bw*0.5;upper+=bw*0.5;} // REF_CENTER
    float x1=a.fftRectMin.x+(float)((lower-a.lowFreq)*a.freqToPixelRatio);
    float x2=a.fftRectMin.x+(float)((upper-a.lowFreq)*a.freqToPixelRatio);
    if(x2<x1){float tmp=x1;x1=x2;x2=tmp;}
    // Preserve a practical hit target even if the bandwidth is subpixel wide.
    const float minHalf=5.0f;
    if(x2-x1<minHalf*2.0f){const float mid=(x1+x2)*0.5f;x1=mid-minHalf;x2=mid+minHalf;}
    return mouse.x>=x1&&mouse.x<=x2;
}

static float get_mouse_wheel(){u8*io=(u8*)ui_GetIO();return *(float*)(io+0xD14);}
static void clear_mouse_wheel(){u8*io=(u8*)ui_GetIO();*(float*)(io+0xD14)=0.0f;}

static void format_frequency5(double hz,char*out,usize cap){
    bool neg=hz<0.0;if(neg)hz=-hz;
    // Exactly five digits after decimal point in MHz. 1 unit = 10 Hz.
    u64 q=(u64)(hz/10.0+0.5);u64 whole=q/100000ull;u32 frac=(u32)(q%100000ull);
    char a[32],f[8];u64str(whole,a,sizeof(a));frac5(frac,f);out[0]=0;if(neg)c_append(out,cap,"-");c_append(out,cap,a);c_append(out,cap,".");c_append(out,cap,f);c_append(out,cap," MHz");
}

static void format_dbfs(float db,char*out,usize cap){
    if(!(db==db) || db<-999.0f || db>999.0f){c_copy(out,cap,"---.- dBFS");return;}
    bool neg=db<0.0f;float a=neg?-db:db;u64 q=(u64)(a*10.0f+0.5f);
    u64 whole=q/10ull;u32 frac=(u32)(q%10ull);char n[32],f[4];u64str(whole,n,sizeof(n));frac1(frac,f);
    out[0]=0;if(neg)c_append(out,cap,"-");c_append(out,cap,n);c_append(out,cap,".");c_append(out,cap,f);c_append(out,cap," dBFS");
}

static void format_filter(double hz,char*out,usize cap){
    if(!(hz>=0.0)||hz>1.0e12){c_copy(out,cap,"---kHz");return;}
    u64 q=(u64)(hz/100.0+0.5);char whole[32],fraction[4];
    u64str(q/10,whole,sizeof(whole));frac1((u32)(q%10),fraction);
    out[0]=0;c_append(out,cap,whole);c_append(out,cap,".");c_append(out,cap,fraction);c_append(out,cap,"kHz");
}

struct FrequencyPreset { u64 frequency;int mode;u32 filter_hz; };
struct PresetEditor { char frequency[13];float filter_khz;bool invalid_frequency;bool invalid_filter; };
struct FrequencyEntry { char text[23];int length;bool overflow;char target[256];int unit=2; };
static FrequencyEntry g_entry={};
static FrequencyPreset g_presets[10];
static PresetEditor g_preset_editors[10];
static unsigned short g_config_path[1024],g_config_temp[1024],g_preset_search_path[1024];
static unsigned short g_settings_path[1024],g_settings_temp[1024];
static bool g_config_ready=false,g_config_dirty=false,g_settings_dirty=false,g_config_error=false,g_load_error=false,g_menu_bound=false;
static bool g_legacy_settings_embedded=false;
static bool g_empty_preset_defaults_migrated=false;
static double g_save_after=0.0;
// Only an occupied slot number may request deletion. Confirmation is separate
// from all editing fields; cancellation has no side effects.
static int g_preset_delete_candidate=-1;
static const double HOLD_SECONDS=0.6;
static const u64 MAX_FREQUENCY=999999999999ull;
static const u32 MAX_FILTER=1000000000u;
static void clear_entry();
static void set_enabled(bool enabled);
static bool parse_uint(const char*&p,u64 limit,u64&value){
    value=0;int count=0;
    while(*p>='0'&&*p<='9'){
        u64 digit=(u64)(*p-'0');if(value>limit/10||(value==limit/10&&digit>limit%10))return false;
        value=value*10+digit;p++;count++;
    }
    return count>0;
}
static bool parse_frequency(const char*text,u64&hz){
    if(!*text){hz=0;return true;}const char*p=text;
    return parse_uint(p,MAX_FREQUENCY,hz)&&!*p;
}
static void refresh_preset_editor(int i){
    if(g_presets[i].frequency)u64str(g_presets[i].frequency,g_preset_editors[i].frequency,13);
    else g_preset_editors[i].frequency[0]=0; // Deleted/empty slot displays blank Hz.
    g_preset_editors[i].filter_khz=(float)g_presets[i].filter_hz/1000.0f;
    g_preset_editors[i].invalid_frequency=g_preset_editors[i].invalid_filter=false;
}
static void refresh_preset_editors(){
    for(int i=0;i<10;i++)refresh_preset_editor(i);
}
static FrequencyPreset default_preset(int i){
    (void)i;
    // All empty quick-access slots use NFM and 0 Hz until configured.
    return {0,MODE_ORDER[0],0};
}
static void reset_presets(){
    for(int i=0;i<10;i++)g_presets[i]=default_preset(i);
    refresh_preset_editors();
}
static bool temp_path(const unsigned short*dest,unsigned short*tmp,usize capacity){
    usize n=0;while(n+5<capacity && dest[n]){tmp[n]=dest[n];n++;}
    if(dest[n] || n+5>=capacity)return false;
    tmp[n++]='.';tmp[n++]='t';tmp[n++]='m';tmp[n++]='p';tmp[n]=0;
    return true;
}
static bool build_config_paths(){
    // Resolve near SDR++ config.json, including custom --root directories.
    const void*path=core_config_manager+0x10;
    u64 length=*(const u64*)((const u8*)path+16);
    if(!length||length>880)return false;
    const char*original=string_chars(path);usize parent=0;
    for(usize i=0;i<(usize)length;i++)if(original[i]=='/'||original[i]=='\\')parent=i+1;
    if(!parent)return false;
    char directory[1024];for(usize i=0;i<parent;i++)directory[i]=original[i];directory[parent]=0;
    char file[1024];c_copy(file,sizeof(file),directory);
    c_append(file,sizeof(file),"essential_frequency_shortcuts.ini");
    if(!MultiByteToWideChar(0,0,file,-1,g_config_path,1024))return false;
    if(!temp_path(g_config_path,g_config_temp,1024))return false;
    c_copy(file,sizeof(file),directory);
    c_append(file,sizeof(file),"essential_settings.ini");
    if(!MultiByteToWideChar(0,0,file,-1,g_settings_path,1024))return false;
    if(!temp_path(g_settings_path,g_settings_temp,1024))return false;
    c_copy(file,sizeof(file),directory);
    c_append(file,sizeof(file),"*frequency_shortcuts.ini");
    if(!MultiByteToWideChar(0,0,file,-1,g_preset_search_path,1024))return false;
    return true;
}
static int g_clock_offsets[2]={0,1};
// 0 = always visible, 1 = overlay on hover, 2 = hidden.
static int g_left_panel=0,g_right_panel=0;
static int g_colour_map=0; // 0 Green, 1 Blue, 2 Red, 3 Orange, 4 Magenta.
// Three positions of the ORIGINAL top-bar tuning button: Free, Center, Free Fixed.
// -1 means adopt SDR++'s current state after its first GUI initialization.

static int g_speed=20,g_contrast=100; // Contrast 100 is native SDR++ mapping.
static double g_speed_apply_after=0,g_speed_last_applied=-100;
static float g_speed_slider=20.0f,g_contrast_slider=100.0f;
static bool g_contrast_needs_apply=false;
static bool g_speed_pending=true,g_speed_custom=false;
static bool g_panel_hooked=false;
static bool g_panel_dirty=false;
static void request_panel_resize(){
    // Force native WaterFall::draw to recompute allocations/geometry on next frame.
    *(float*)(core_waterfall+0x3d09e4)=-999999.0f;
}
static void settings_changed(){g_settings_dirty=true;g_save_after=ui_GetTime()+0.75;}
static void panel_changed(){g_panel_dirty=true;settings_changed();}

static int g_tuning_input_suppressed_through=-1;
static int g_v40_last_popup_frame=-1000;
static usize serialize_presets(char*out,usize capacity){
    if(g_entry.unit<0||g_entry.unit>3)return 0;
    c_copy(out,capacity,"EssentialFrequencyShortcuts=1\n");
    for(int i=0;i<10;i++){
        char row[64],n[32];row[0]=(char)('0'+i);row[1]='=';row[2]=0;
        u64str(g_presets[i].frequency,n,sizeof(n));c_append(row,sizeof(row),n);c_append(row,sizeof(row),",");
        u64str((u64)g_presets[i].mode,n,sizeof(n));c_append(row,sizeof(row),n);c_append(row,sizeof(row),",");
        u64str(g_presets[i].filter_hz,n,sizeof(n));c_append(row,sizeof(row),n);c_append(row,sizeof(row),"\n");
        if(c_len(out)+c_len(row)>=capacity)return 0;c_append(out,capacity,row);
    }
    return c_len(out);
}
static usize serialize_settings(char*out,usize capacity){
    if(g_entry.unit<0||g_entry.unit>3)return 0;
    c_copy(out,capacity,"EssentialSettings=1\n");
    for(int i=0;i<2;i++){
        char row[32];c_copy(row,sizeof(row),i?"ClockZone2=":"ClockZone1=");
        // Saved as a positive index 0..26: UTC-12 .. UTC+14.
        char n[16];u64str((u64)(g_clock_offsets[i]+12),n,sizeof(n));
        c_append(row,sizeof(row),n);c_append(row,sizeof(row),"\n");
        if(c_len(out)+c_len(row)>=capacity)return 0;c_append(out,capacity,row);
    }
    char unit[16]="EntryUnit=";unit[10]=(char)('0'+g_entry.unit);unit[11]='\n';unit[12]=0;
    if(c_len(out)+c_len(unit)>=capacity)return 0;c_append(out,capacity,unit);
    const char*names[]={"LeftPanel=","RightPanel=","Speed=","Contrast=","SpeedSet=","ColourMap=","Language="};
    const int values[]={g_left_panel,g_right_panel,g_speed,g_contrast,g_speed_custom?1:0,g_colour_map,g_language};
    for(int i=0;i<7;i++){
        char row[64],n[20];c_copy(row,sizeof(row),names[i]);u64str((u64)values[i],n,sizeof(n));
        c_append(row,sizeof(row),n);c_append(row,sizeof(row),"\n");
        if(c_len(out)+c_len(row)>=capacity)return 0;c_append(out,capacity,row);
    }
    {
        char row[32],n[16];c_copy(row,sizeof(row),"MarkerInfoMode=");
        u64str((u64)g_marker_info_mode,n,sizeof(n));c_append(row,sizeof(row),n);
        c_append(row,sizeof(row),"\n");
        if(c_len(out)+c_len(row)>=capacity)return 0;c_append(out,capacity,row);
    }
    if(g_tuning_mode>=0 && g_tuning_mode<=2){
        char row[32],n[16];c_copy(row,sizeof(row),"TuneMode=");
        u64str((u64)g_tuning_mode,n,sizeof(n));c_append(row,sizeof(row),n);
        c_append(row,sizeof(row),"\n");
        if(c_len(out)+c_len(row)>=capacity)return 0;c_append(out,capacity,row);
    }
    return c_len(out);
}
static bool deserialize_presets(const char*text,bool compatible=false){
    const char*p=text;
    if(compatible){
        // Accept the historical format based on its structural suffix, not
        // on a retired product name. Reject unknown binary/oversized headers.
        const char*suffix="FrequencyShortcuts=1";
        const char*line=text;int length=0;
        while(line[length]&&line[length]!='\r'&&line[length]!='\n'&&length<96)length++;
        int suffixLen=(int)c_len(suffix);
        if(length<=suffixLen||length>=96)return false;
        for(int i=0;i<length-suffixLen;i++)if(line[i]<33||line[i]>126)return false;
        for(int i=0;i<suffixLen;i++)if(line[length-suffixLen+i]!=suffix[i])return false;
        p+=length;
    }else{
        const char*header="EssentialFrequencyShortcuts=1";
        for(usize i=0;header[i];i++)if(*p++!=header[i])return false;
    }
    if(*p=='\r')p++;if(*p++!='\n')return false;
    FrequencyPreset parsed[10]={};bool seen[10]={};int rows=0,unit=2;bool unitSeen=false;int zones[2]={0,1};bool zoneSeen[2]={false,false};
    const char* names[]={"LeftPanel=","RightPanel=","Speed=","Contrast=","SpeedSet=","ColourMap=","Brightness="};
    int panelValues[6]={0,0,20,100,0,0};bool panelSeen[7]={};
    int savedTuningMode=-1;bool tuneModeSeen=false;
    int savedMarkerMode=0;bool markerModeSeen=false;
    bool embedded=false;
    while(*p){
        if(starts_with(p,"MarkerInfoMode=")){
            if(markerModeSeen)return false;embedded=true;
            p+=15;u64 mode=0;if(!parse_uint(p,2,mode))return false;
            if(*p=='\r')p++;if(*p&&*p++!='\n')return false;
            savedMarkerMode=(int)mode;markerModeSeen=true;continue;
        }
        if(starts_with(p,"TuneMode=")){
            if(tuneModeSeen)return false;embedded=true;
            p+=9;u64 mode=0;if(!parse_uint(p,2,mode))return false;
            if(*p=='\r')p++;if(*p&&*p++!='\n')return false;
            savedTuningMode=(int)mode;tuneModeSeen=true;continue;
        }
        int option=-1;
        for(int i=0;i<7;i++)if(starts_with(p,names[i])){option=i;break;}
        if(option>=0){
            if(panelSeen[option])return false;embedded=true;p+=c_len(names[option]);u64 value=0;
            const u64 maxima[]={2,2,60,200,1,4,200};
            if(!parse_uint(p,maxima[option],value))return false;
            if(option==2 && value<1)return false;
            if(*p=='\r')p++;if(*p&&*p++!='\n')return false;
            if(option!=6)panelValues[option]=(int)value; // Ignore old Brightness overlay setting.
            panelSeen[option]=true;continue;
        }
        int zi=starts_with(p,"ClockZone1=")?0:starts_with(p,"ClockZone2=")?1:-1;
        if(zi>=0){
            if(zoneSeen[zi])return false;embedded=true;p+=11;u64 value=0;
            if(!parse_uint(p,26,value))return false;
            if(*p=='\r')p++;if(*p&&*p++!='\n')return false;
            zones[zi]=(int)value-12;zoneSeen[zi]=true;continue;
        }
        if(starts_with(p,"EntryUnit=")){
            if(unitSeen)return false;embedded=true;p+=10;u64 value=0;
            if(!parse_uint(p,3,value))return false;
            if(*p=='\r')p++;if(*p&&*p++!='\n')return false;
            unit=(int)value;unitSeen=true;continue;
        }
        if(*p<'0'||*p>'9')return false;int index=*p++-'0';
        if(seen[index]||*p++!='=')return false;
        u64 hz,mode,width;
        if(!parse_uint(p,MAX_FREQUENCY,hz)||*p++!=',')return false;
        if(!parse_uint(p,7,mode)||*p++!=',')return false;
        if(!parse_uint(p,MAX_FILTER,width))return false; // A blank slot may have a zero-width filter.
        if(*p=='\r')p++;if(*p&&*p++!='\n')return false;
        parsed[index]={hz,(int)mode,(u32)width};seen[index]=true;rows++;
    }
    if(rows!=10)return false;
    // Migrate only vacant legacy slots. Never change a stored frequency,
    // modulation or bandwidth belonging to an occupied preset.
    bool migrateEmpty=false;
    for(int i=0;i<10;i++){
        if(parsed[i].frequency==0 && (parsed[i].mode!=MODE_ORDER[0] || parsed[i].filter_hz!=0)){
            parsed[i]=default_preset(i);
            migrateEmpty=true;
        }
    }
    for(int i=0;i<10;i++)g_presets[i]=parsed[i];g_entry.unit=unit;g_clock_offsets[0]=zones[0];g_clock_offsets[1]=zones[1];
    g_empty_preset_defaults_migrated=migrateEmpty;
    g_left_panel=panelValues[0];g_right_panel=panelValues[1];g_speed=panelValues[2];g_contrast=panelValues[3];g_colour_map=panelValues[5];
    g_tuning_mode=savedTuningMode;
    g_marker_info_mode=savedMarkerMode;
    g_speed_slider=(float)g_speed;g_contrast_slider=(float)g_contrast;
    g_contrast_needs_apply=true;g_speed_pending=true;g_speed_custom=panelValues[4]!=0;
    g_legacy_settings_embedded=embedded;
    refresh_preset_editors();return true;
}
// Settings are stored separately. Parse independently and commit only on success.
static bool deserialize_settings(const char*text){
    const char*header="EssentialSettings=1";const char*p=text;
    for(usize i=0;header[i];i++)if(*p++!=header[i])return false;
    if(*p=='\r')p++;if(*p++!='\n')return false;
    const char*names[]={"ClockZone1=","ClockZone2=","EntryUnit=","LeftPanel=","RightPanel=", "Speed=","Contrast=","SpeedSet=","ColourMap=","MarkerInfoMode=","TuneMode=","Language="};
    const u64 maxima[]={26,26,3,2,2,60,200,1,4,2,2,3};
    int values[]={g_clock_offsets[0]+12,g_clock_offsets[1]+12,g_entry.unit,g_left_panel,g_right_panel,g_speed,g_contrast,g_speed_custom?1:0,g_colour_map,g_marker_info_mode,g_tuning_mode,g_language};
    bool seen[12]={};
    while(*p){
        int index=-1;
        for(int i=0;i<12;i++)if(starts_with(p,names[i])){index=i;break;}
        if(index<0||seen[index])return false;
        seen[index]=true;p+=c_len(names[index]);u64 val=0;
        if(!parse_uint(p,maxima[index],val))return false;
        if(index==5&&val<1)return false;
        if(*p=='\r')p++;if(*p&&*p++!='\n')return false;
        values[index]=(int)val;
    }
    g_clock_offsets[0]=values[0]-12;g_clock_offsets[1]=values[1]-12;
    g_entry.unit=values[2];g_left_panel=values[3];g_right_panel=values[4];
    g_speed=values[5];g_contrast=values[6];g_speed_custom=values[7]!=0;
    g_colour_map=values[8];g_marker_info_mode=values[9];
    if(seen[10])g_tuning_mode=values[10];
    g_language=values[11];
    g_speed_slider=(float)g_speed;g_contrast_slider=(float)g_contrast;
    g_speed_pending=true;g_contrast_needs_apply=true;
    return true;
}
// WIN32_FIND_DATAW is 592 bytes; its cFileName[260] starts at byte 44.
// A byte buffer avoids introducing the Windows SDK into this freestanding DLL.
static bool read_preset_file(const unsigned short*path,char*text,usize capacity,bool*missing=nullptr){
    if(missing)*missing=false;
    void*f=CreateFileW(path,0x80000000u,1,0,3,0x80,0);
    if(f==(void*)(long long)-1){
        u32 error=GetLastError();
        if(missing)*missing=(error==2||error==3);
        return false;
    }
    u32 high=0,size=GetFileSize(f,&high),read=0;
    bool ok=!high&&size>0&&size<capacity&&ReadFile(f,text,size,&read,0)&&read==size;
    CloseHandle(f);
    if(ok){text[size]=0;for(u32 i=0;i<size;i++)if(!text[i])ok=false;}
    return ok;
}
struct PresetSearchBuffers {
    alignas(4) u8 findData[592];
    unsigned short path[1024];
    char content[2048];
};
static bool import_previous_presets(){
    auto*b=(PresetSearchBuffers*)malloc(sizeof(PresetSearchBuffers));
    if(!b)return false;
    void*handle=FindFirstFileW(g_preset_search_path,b->findData);
    if(handle==(void*)(long long)-1){free(b);return false;}
    // Search the same directory as config.json; never walk subdirectories.
    usize parent=0;
    while(g_preset_search_path[parent]&&parent<1020)parent++;
    while(parent&&g_preset_search_path[parent-1]!='/'&&g_preset_search_path[parent-1]!='\\')parent--;
    bool imported=false;int attempts=0;
    do{
        if(++attempts>128)break;
        if((*(u32*)b->findData)&0x10)continue; // skip directories
        const unsigned short*name=(const unsigned short*)(b->findData+44);
        usize length=0;while(length<260&&name[length])length++;
        if(!length||length>=260||parent+length>=1024)continue;
        for(usize i=0;i<parent;i++)b->path[i]=g_preset_search_path[i];
        for(usize i=0;i<length;i++)b->path[parent+i]=name[i];
        b->path[parent+length]=0;
        bool same=true;for(usize i=0;i<=parent+length;i++)if(b->path[i]!=g_config_path[i]){same=false;break;}
        if(same)continue;
        if(read_preset_file(b->path,b->content,sizeof(b->content))&&deserialize_presets(b->content,true)){
            imported=true;break;
        }
    }while(FindNextFileW(handle,b->findData));
    FindClose(handle);free(b);
    return imported;
}
static void load_presets(){
    reset_presets();g_entry.unit=2;g_clock_offsets[0]=0;g_clock_offsets[1]=1;
    g_left_panel=g_right_panel=0;g_speed=20;g_contrast=100;g_colour_map=0;g_language=0;g_tuning_mode=-1;g_marker_info_mode=0;
    g_speed_slider=20.0f;g_contrast_slider=100.0f;g_contrast_needs_apply=true;
    g_speed_pending=true;g_speed_last_applied=-100;g_speed_custom=false;
    g_config_ready=build_config_paths();
    g_config_dirty=g_settings_dirty=g_config_error=g_load_error=g_legacy_settings_embedded=false;
    g_empty_preset_defaults_migrated=false;
    if(!g_config_ready){g_load_error=true;return;}
    char content[2048];
    bool missingPresets=false;
    if(read_preset_file(g_config_path,content,sizeof(content),&missingPresets)){
        if(!deserialize_presets(content)){g_load_error=true;return;}
    }else{
        if(!missingPresets){g_load_error=true;return;}
        if(import_previous_presets())g_config_dirty=true;
    }
    bool missingSettings=false;
    if(read_preset_file(g_settings_path,content,sizeof(content),&missingSettings)){
        if(!deserialize_settings(content)){g_load_error=true;return;}
        // V37 migration: an older shortcuts file may still contain settings.
        if(g_legacy_settings_embedded)g_config_dirty=true;
    }else{
        if(!missingSettings){g_load_error=true;return;}
        // Persist settings FIRST. Until that succeeds, do not rewrite a
        // legacy combined file, so failed migration cannot lose preferences.
        g_settings_dirty=true;
        if(g_legacy_settings_embedded)g_config_dirty=true;
    }
    if(g_empty_preset_defaults_migrated)g_config_dirty=true;
    if(g_config_dirty||g_settings_dirty)g_save_after=0;
}
static bool write_config_atomic(const unsigned short*dst,const unsigned short*tmp,const char*text,usize size){
    if(!size)return false;
    u32 written=0;void*f=CreateFileW(tmp,0x40000000u,0,0,2,0x80,0);
    bool ok=f!=(void*)(long long)-1;
    if(ok){ok=WriteFile(f,text,(u32)size,&written,0)&&written==size&&FlushFileBuffers(f);if(!CloseHandle(f))ok=false;}
    if(ok)ok=MoveFileExW(tmp,dst,9)!=0;
    if(!ok)DeleteFileW(tmp);
    return ok;
}
static bool save_settings(){
    if(!g_config_ready){g_config_ready=build_config_paths();if(!g_config_ready){g_config_error=true;return false;}}
    char text[1024];usize size=serialize_settings(text,sizeof(text));
    bool ok=write_config_atomic(g_settings_path,g_settings_temp,text,size);
    g_config_error=!ok;
    if(ok)g_settings_dirty=false;
    else g_save_after=(ui_GetContext()?ui_GetTime():0.0)+5.0;
    return ok;
}
static bool save_presets(){
    if(!g_config_ready){g_config_ready=build_config_paths();if(!g_config_ready){g_config_error=true;return false;}}
    // Do not rewrite a legacy combined file until settings are safely stored.
    if(g_settings_dirty && !save_settings())return false;
    char text[1024];usize size=serialize_presets(text,sizeof(text));
    bool ok=write_config_atomic(g_config_path,g_config_temp,text,size);
    g_config_error=!ok;
    if(ok){g_config_dirty=false;g_legacy_settings_embedded=false;g_load_error=false;}
    else g_save_after=(ui_GetContext()?ui_GetTime():0.0)+5.0;
    return ok;
}
static void preset_changed(){g_config_dirty=true;g_save_after=ui_GetTime()+0.75;}
static void delete_preset(int slot){
    if(slot<0||slot>=10)return;
    // A deleted shortcut becomes a genuinely blank slot, including its
    // modulation and filter: 0 Hz / NFM / 0 Hz. Use this single function
    // from both the sidebar confirmation and the waterfall PPM dialog.
    const FrequencyPreset&old=g_presets[slot];
    const bool changed=old.frequency!=0 || old.mode!=MODE_ORDER[0] || old.filter_hz!=0;
    g_presets[slot]=default_preset(slot);
    refresh_preset_editor(slot); // Immediately update the visible table.
    if(changed)preset_changed(); // Persist the reset in shortcuts.ini.
}
static void save_presets_if_due(){
    if((!g_config_dirty&&!g_settings_dirty)||ui_GetTime()<g_save_after)return;
    if(g_panel_dirty && (GetAsyncKeyState(0x01)&0x8000))return;
    // Write new settings first; if it fails, preserve the entire legacy file.
    if(g_settings_dirty&&!save_settings())return;
    if(g_config_dirty&&!save_presets())return;
    g_panel_dirty=false;
}
static void save_all_config(){
    if(g_settings_dirty && !save_settings())return;
    if(g_config_dirty)save_presets();
}
static bool commit_preset_frequency(int i){
    u64 hz;bool valid=parse_frequency(g_preset_editors[i].frequency,hz);
    g_preset_editors[i].invalid_frequency=!valid;
    if(valid){
        if(hz==0){
            // Clearing the frequency field is also deletion: reset the
            // previous modulation and bandwidth, not just its frequency.
            delete_preset(i);
        }else if(g_presets[i].frequency!=hz){
            g_presets[i].frequency=hz;
            preset_changed();
        }
    }
    return valid;
}
static bool commit_preset_filter(int i){
    double hz=(double)g_preset_editors[i].filter_khz*1000.0;
    bool valid=hz>=0.0&&hz<=MAX_FILTER;g_preset_editors[i].invalid_filter=!valid;
    if(valid){u32 rounded=(u32)(hz+0.5);if(g_presets[i].filter_hz!=rounded){g_presets[i].filter_hz=rounded;preset_changed();}}
    return valid;
}
static void apply_preset(int digit){
    const FrequencyPreset&p=g_presets[digit];
    if(!p.frequency||!set_modulation(p.mode))return;
    // A demodulator switch loads its own bandwidth; apply the user's width afterwards.
    // Zero means "use the demodulator's native/default filter width".
    // Do not send a zero-Hz bandwidth command to SDR++.
    if(p.filter_hz){
        float width=(float)p.filter_hz;
        if(!radio_command(3,&width,0))return;
    }
    clear_entry();tune_frequency((double)p.frequency);
}
// Headerless Essential++ dialogs: drag the narrow, unused top border.
// Do not add a synthetic title, button or invisible overlay over UI controls.
// The border sits above the first ImGui item and does not intercept Combo clicks.
static void move_headerless_window_from_top_border(){
    if(!ui_IsWindowHovered(0)||!ui_IsMouseClicked(0,false))return;
    const float scale=core_ui_scale>0.0f?core_ui_scale:1.0f;
    const ImVec2 p=ui_GetWindowPos(),s=ui_GetWindowSize(),m=ui_GetMousePos();
    if(m.x>=p.x+3.0f*scale && m.x<=p.x+s.x-3.0f*scale &&
       m.y>=p.y+1.0f*scale && m.y<=p.y+7.0f*scale)
        ui_StartMouseMovingWindow(ui_GetCurrentWindow());
}
static void shortcut_menu(void*){
    if(!g_supported)return;
    static const char* choicesByLanguage[4]={
        "Show\0Show on hover\0Hide\0\0",
        "Anzeigen\0Bei Beruehrung\0Ausblenden\0\0",
        "Mostrar\0Al pasar cursor\0Ocultar\0\0",
        "Pokaz\0Pokaz po najechaniu\0Ukryj\0\0"
    };
    const int safeLanguage=(unsigned)g_language<4u?g_language:0;
    const char*choices=choicesByLanguage[safeLanguage];
    // Fixed labels at left; dropdowns fill the remaining width at right.
    ui_TextUnformatted("Lewy panel",0);ui_SameLine(0,8.0f*core_ui_scale);
    ui_SetNextItemWidth(-1.0f);
    if(ui_Combo("##essential_left_panel",&g_left_panel,choices,-1)){request_panel_resize();panel_changed();}
    ui_TextUnformatted("Prawy panel",0);ui_SameLine(0,8.0f*core_ui_scale);
    ui_SetNextItemWidth(-1.0f);
    if(ui_Combo("##essential_right_panel",&g_right_panel,choices,-1)){panel_changed();}
    // Tuning is deliberately controlled ONLY by the original top toolbar button.
    // Keep old TuneMode=2 values in the preferences for seamless V31 migration.
    ui_TextUnformatted("Colour map",0);ui_SameLine(0,8.0f*core_ui_scale);
    ui_SetNextItemWidth(-1.0f);
    static const char* mapNames[4]={
        "Green\0Blue\0Red\0Orange\0Magenta\0\0",
        "Gruen\0Blau\0Rot\0Orange\0Magenta\0\0",
        "Verde\0Azul\0Rojo\0Naranja\0Magenta\0\0",
        "Zielony\0Niebieski\0Czerwony\0Pomaranczowy\0Purpurowy\0\0"
    };
    if(ui_Combo("##essential_colour_map",&g_colour_map,mapNames[safeLanguage],-1)){panel_changed();}
    ui_TextUnformatted("Language",0);ui_SameLine(0,8.0f*core_ui_scale);
    ui_SetNextItemWidth(-1.0f);
    if(ui_Combo("##essential_language",&g_language,
        "English\0Deutsch\0Español\0Polski\0\0",-1))settings_changed();
    // The receiver selector lives only on the toolbar (second row).
    // Do not duplicate it in the Essential sidebar.
    ui_TextUnformatted("Szybki dostep (kliknij numer, aby usunac):",0);
    int request_delete=-1;
    if(ui_BeginTable("##essential_presets",4,64|128|256|512|1024|32768,ImVec2(0,0),0)){
        ui_TableSetupColumn("Nr",16,22,0);
        ui_TableSetupColumn("Hz",8,1.8f,0);
        ui_TableSetupColumn("Mod",8,1.0f,0);
        ui_TableSetupColumn("Filtr",8,1.2f,0);ui_TableHeadersRow();
        const char* modes="NFM\0WFM\0AM\0LSB\0USB\0DSB\0CW\0RAW\0\0";
        for(int i=0;i<10;i++){
            ui_PushID(i);ui_TableNextRow(0,0);ui_TableNextColumn();
            char digit[2]={(char)('0'+i),0};
            if(g_presets[i].frequency){
                // Only the number itself is a deletion control, not the whole row.
                if(ui_Selectable(digit,false,0,ImVec2()))request_delete=i;
            }else ui_TextUnformatted(digit,0);
            ui_TableNextColumn();ui_SetNextItemWidth(-1);
            bool enter=ui_InputText("##hz",g_preset_editors[i].frequency,13,1|32,0,0);
            if(enter||ui_IsItemDeactivatedAfterEdit())commit_preset_frequency(i);
            ui_TableNextColumn();ui_SetNextItemWidth(-1);int index=mode_index(g_presets[i].mode);
            if(ui_Combo("##mode",&index,modes,-1)&&index>=0&&index<8){g_presets[i].mode=MODE_ORDER[index];preset_changed();}
            ui_TableNextColumn();ui_SetNextItemWidth(-1);
            enter=ui_InputFloat("##filter",&g_preset_editors[i].filter_khz,0,0,"%.1fkHz",32);
            if(enter||ui_IsItemDeactivatedAfterEdit())commit_preset_filter(i);
            ui_PopID();
        }
        ui_EndTable();
    }
    // Open after PopID and EndTable to keep the popup's ImGui ID in the
    // sidebar's scope. It is a true modal: no accidental deletion on click.
    if(request_delete>=0){
        g_preset_delete_candidate=request_delete;
        ui_OpenPopup("Usun szybki dostep##essential_delete_confirm",0);
    }
    if(ui_BeginPopupModal("Usun szybki dostep##essential_delete_confirm",nullptr,1|2|32|64)){
        move_headerless_window_from_top_border();
        int slot=g_preset_delete_candidate;
        if(slot>=0&&slot<10&&g_presets[slot].frequency){
            char question[112],digit[4],hz[24];
            u64str((u64)slot,digit,sizeof(digit));
            u64str(g_presets[slot].frequency,hz,sizeof(hz));
            c_copy(question,sizeof(question),l10n("Usunac szybki dostep nr "));
            c_append(question,sizeof(question),digit);
            c_append(question,sizeof(question),"?");
            ui_TextUnformatted(question,nullptr);
            c_copy(question,sizeof(question),l10n("Czestotliwosc: "));
            c_append(question,sizeof(question),hz);
            c_append(question,sizeof(question)," Hz");
            ui_TextUnformatted(question,nullptr);
            ui_TextUnformatted("Zniknie czestotliwosc, modulacja i filtr tego numeru.",nullptr);
            if(ui_Button("Tak##essential_delete_yes",ImVec2(90,0))){
                delete_preset(slot);
                g_preset_delete_candidate=-1;
                ui_CloseCurrentPopup();
            }
            ui_SameLine(0,12.0f*core_ui_scale);
            if(ui_Button("Nie##essential_delete_no",ImVec2(90,0))){
                g_preset_delete_candidate=-1;
                ui_CloseCurrentPopup();
            }
        }else{
            g_preset_delete_candidate=-1;
            ui_CloseCurrentPopup();
        }
        ui_EndPopup();
    }
}
static void promote_shortcut_menu(){
    // Menu::order is the leading MSVC vector. Verified registerEntry code uses
    // 40-byte MenuOption_t objects: a 32-byte string followed by the open flag.
    u8*first=*(u8**)core_menu;u8*last=*(u8**)(core_menu+8);
    if(!first||!last||last<first)return;
    usize bytes=(usize)(last-first);if(bytes%40||bytes/40>1024)return;
    for(usize index=0;index<bytes/40;index++){
        u8*item=first+index*40;
        if(!c_equal(string_chars(item),"Essential"))continue;
        CoreString name;core_stringCopy(&name,item);
        for(usize i=index;i>0;i--){
            u8*to=first+i*40;u8*from=to-40;
            core_stringDestroy(to);core_stringCopy(to,from);to[32]=from[32];
        }
        core_stringDestroy(first);core_stringCopy(first,&name);first[32]=1;
        core_stringDestroy(&name);return;
    }
}
static ModuleManager::Instance* g_essential_menu_instance=0;
static void bind_menu(){
    if(!g_supported||g_menu_bound)return;
    CoreString name;core_stringFromText(&name,"Essential");
    // SDR++ draws the enable checkbox in the section header when an Instance is supplied.
    menu_register(core_menu,&name,shortcut_menu,0,g_essential_menu_instance);g_menu_bound=true;
    promote_shortcut_menu();
}
static void unbind_menu(){
    if(!g_menu_bound)return;
    CoreString name;core_stringFromText(&name,"Essential");menu_remove(core_menu,&name);g_menu_bound=false;
}

static bool signal_at_fraction(float frac,float&db){
    int width=0;float*fft=wf_acquireLatestFFT(core_waterfall,width);
    if(!fft || width<=0){if(fft)wf_releaseLatestFFT(core_waterfall);return false;}
    int idx=(int)(frac*(float)(width-1)+0.5f);if(idx<0)idx=0;if(idx>=width)idx=width-1;
    float best=fft[idx];
    if(idx>0 && fft[idx-1]>best)best=fft[idx-1];
    if(idx+1<width && fft[idx+1]>best)best=fft[idx+1];
    wf_releaseLatestFFT(core_waterfall);db=best;return true;
}

struct CursorPreview { double freq; double bw; double snap; int ref; bool hasVfo; float frac; };
static CursorPreview make_preview(const InputHandlerArgs&args,const ImVec2&mouse){
    CursorPreview p={0,0,1,1,false,0};
    float w=args.fftRectMax.x-args.fftRectMin.x;if(w<=1.0f)return p;
    float rawFrac=clamp01((mouse.x-args.fftRectMin.x)/w);
    double rawFreq=args.lowFreq+(args.highFreq-args.lowFreq)*(double)rawFrac;
    void*v=selected_vfo();
    if(v){
        p.hasVfo=true;p.bw=vfo_bandwidth(v);p.snap=vfo_snap(v);p.ref=vfo_reference(v);
        if(p.snap<0.000001)p.snap=1.0;
        p.freq=round_nearest(rawFreq/p.snap)*p.snap;
    }else{p.freq=rawFreq;p.snap=1.0;p.bw=0.0;p.ref=1;}
    double span=args.highFreq-args.lowFreq;
    p.frac=span>0.0?clamp01((float)((p.freq-args.lowFreq)/span)):rawFrac;
    return p;
}

static void draw_preview_rect(ImDrawList*dl,const InputHandlerArgs&args,const CursorPreview&pv,float markerX,const ImVec2&r0,const ImVec2&r1){
    if(!dl)return;
    if(pv.hasVfo && pv.bw>0.0){
        double lo=pv.freq,hi=pv.freq;
        if(pv.ref==0){hi=pv.freq+pv.bw;}
        else if(pv.ref==2){lo=pv.freq-pv.bw;}
        else {lo=pv.freq-pv.bw*0.5;hi=pv.freq+pv.bw*0.5;}
        if(lo<args.lowFreq)lo=args.lowFreq;if(hi>args.highFreq)hi=args.highFreq;
        if(hi>lo){
            float x0=args.fftRectMin.x+(float)((lo-args.lowFreq)*args.freqToPixelRatio);
            float x1=args.fftRectMin.x+(float)((hi-args.lowFreq)*args.freqToPixelRatio);
            draw_AddRectFilled(dl,ImVec2(x0,r0.y),ImVec2(x1,r1.y),0x1400FF00u,0.0f,0);
            float innerPad=(x1-x0)*0.08f;if(innerPad>4.0f)innerPad=4.0f;
            if(x1-x0>innerPad*2.0f)draw_AddRectFilled(dl,ImVec2(x0+innerPad,r0.y),ImVec2(x1-innerPad,r1.y),0x0C00FF00u,0.0f,0);
            draw_AddLine(dl,ImVec2(x0,r0.y),ImVec2(x0,r1.y),0x3000FF00u,1.0f);
            draw_AddLine(dl,ImVec2(x1,r0.y),ImVec2(x1,r1.y),0x3000FF00u,1.0f);
        }
    }
    draw_AddLine(dl,ImVec2(markerX,r0.y),ImVec2(markerX,r1.y),0x9000FF00u,1.35f);
}

// ImGui 1.87 context hooks let us consume keys before FrequencySelect/MainWindow read them.
// Their complete state is restored after EndFrame, so holding arrows retains native repeat timing.
enum { K_LEFT=513,K_RIGHT=514,K_UP=515,K_DOWN=516,K_BACKSPACE=523,K_SPACE=524,K_ENTER=525,K_ESCAPE=526,K_CTRL_LEFT=527,K_CTRL_RIGHT=531,K_ALT_LEFT=529,K_ALT_RIGHT=533,
       K_MENU=535,K_TOP0=536,K_Z=571,K_GRAVE=594,K_M=558,K_NUM0=600,K_DECIMAL=610,K_DIVIDE=611,K_MULTIPLY=612,K_SUBTRACT=613,K_ADD=614,K_NUMENTER=615,
       K_CTRL_MOD=641,K_ALT_MOD=643,K_KEY_COUNT=645 };
struct SavedKey { int key;KeyData data; };
// Includes the grave key and a one-frame pulse for the core's native sidebar shortcut.
static SavedKey g_saved_keys[40];static int g_saved_key_count=0;
static bool g_entry_control_owned[4]={};
struct DigitGesture { bool armed,fired;double started;char target[256]; };
static DigitGesture g_digit_gestures[10]={};
static const char ENTRY_UNITS[]="Hz\0kHz\0MHz\0GHz\0";
static const u64 ENTRY_FACTORS[]={1,1000,1000000,1000000000};
static ImVec2 g_entry_rect_min,g_entry_rect_max;
static bool g_entry_rect_valid=false;
static InputHandlerArgs g_last_args;
static int g_args_frame=-1;

struct ControlTap { bool wasDown,armed,used;char target[256]; };
static ControlTap g_control_tap={};
struct AltTap { bool wasDown,armed,used; };
static AltTap g_alt_tap={};
static void cancel_alt_tap(bool down=false){g_alt_tap={};g_alt_tap.wasDown=down;}
static void alt_tap_frame(bool down,bool control,bool shift,bool super){
    if(down&&!g_alt_tap.wasDown){g_alt_tap.armed=true;g_alt_tap.used=false;}
    if(down&&g_alt_tap.armed){
        u8*io=(u8*)ui_GetIO();
        g_alt_tap.used=g_alt_tap.used||control||shift||super||g_entry.length||get_mouse_wheel()!=0||*(float*)(io+0xD18)!=0;
        const int buttons[]={1,2,4,5,6};
        for(int button:buttons)if(GetAsyncKeyState(button)&0x8000)g_alt_tap.used=true;
        for(int key=512;key<K_KEY_COUNT&&!g_alt_tap.used;key++){
            KeyData*k=ui_GetKeyData(key);
            if(key!=K_ALT_LEFT&&key!=K_ALT_RIGHT&&key!=K_ALT_MOD&&k&&k->Down)g_alt_tap.used=true;
        }
    }
    if(!down&&g_alt_tap.wasDown&&g_alt_tap.armed&&!g_alt_tap.used&&!control&&!shift&&!super&&!g_entry.length){
        g_marker_info_mode=(g_marker_info_mode+1)%3;
        settings_changed();
    }
    g_alt_tap.wasDown=down;if(!down){g_alt_tap.armed=false;g_alt_tap.used=false;}
}
static void cancel_control_tap(bool down=false){g_control_tap={};g_control_tap.wasDown=down;}
static void control_tap_frame(bool down,bool shift,bool alt,bool super){
    if(down&&!g_control_tap.wasDown){
        g_control_tap.armed=selected_vfo()!=0;g_control_tap.used=false;
        c_copy(g_control_tap.target,sizeof(g_control_tap.target),string_chars(wf_selected_name()));
    }
    if(down&&g_control_tap.armed&&!g_control_tap.used){
        u8*io=(u8*)ui_GetIO();
        g_control_tap.used=shift||alt||super||g_entry.length||get_mouse_wheel()!=0||*(float*)(io+0xD18)!=0;
        const int mouseButtons[]={1,2,4,5,6};
        for(int button:mouseButtons)if(GetAsyncKeyState(button)&0x8000)g_control_tap.used=true;
        // The named-key range and modifier keys are verified against this core's ImGui 1.87.
        for(int key=512;key<K_KEY_COUNT&&!g_control_tap.used;key++)
            if(key!=K_CTRL_LEFT&&key!=K_CTRL_RIGHT&&key!=K_CTRL_MOD&&ui_GetKeyData(key)->Down)g_control_tap.used=true;
    }
    if(!down&&g_control_tap.wasDown&&g_control_tap.armed&&!g_control_tap.used&&
        !shift&&!alt&&!super&&!g_entry.length&&selected_vfo()&&
        c_equal(g_control_tap.target,string_chars(wf_selected_name()))){
        // The same one-shot hardware/VFO centring used by the native toolbar
        // button. Keep the user's subsequent tuning mode and zoom unchanged.
        double frequency=selected_frequency();CoreString name;
        core_stringCopy(&name,wf_selected_name());core_tune(0,&name,frequency);
    }
    g_control_tap.wasDown=down;
    if(!down){g_control_tap.armed=false;g_control_tap.used=false;}
}
static void zoom_view(int direction,int steps,double anchor,float fraction){
    g_zoom_transition={}; // explicit user zoom wins over a pending restore
    double old=wf_getViewBandwidth(core_waterfall),whole=wf_getBandwidth(core_waterfall);
    if(!(old>0)||!(whole>0))return;
    if(steps<1)steps=1;if(steps>8)steps=8;
    double next=old;for(int i=0;i<steps;i++)next*=direction>0?0.80:1.25;
    double minimum=whole<20?whole:20;if(next<minimum)next=minimum;if(next>whole)next=whole;
    double offset=anchor+(0.5-(double)fraction)*next-wf_getCenterFrequency(core_waterfall);
    wf_setViewBandwidth(core_waterfall,next);wf_setViewOffset(core_waterfall,offset);
    sync_zoom_slider();g_slider_snapshot=*(float*)(core_main_window+0x9C);
    zoom_record_preference();
}
static void keyboard_zoom(int direction){
    // Center on the selected red VFO; without a VFO retain the displayed center.
    double anchor=selected_vfo()?selected_frequency():
        wf_getCenterFrequency(core_waterfall)+wf_getViewOffset(core_waterfall);
    zoom_view(direction,1,anchor,0.5f);
}
static void* g_hook_context=0;
static u32 g_keyboard_hook=0,g_tooltip_hook=0,g_restore_hook=0;
static bool bind_keyboard();
static void clear_entry(){g_entry.length=0;g_entry.text[0]=0;g_entry.overflow=false;g_entry.target[0]=0;g_entry_rect_valid=false;}
static int entry_decimal(){for(int i=0;i<g_entry.length;i++)if(g_entry.text[i]=='.')return i;return -1;}
static bool entry_value(u64&hz){
    hz=0;if(!g_entry.length||g_entry.overflow||g_entry.unit<0||g_entry.unit>3)return false;
    u64 integer=0,fraction=0;int decimal=entry_decimal(),count=0;
    for(int i=0;i<g_entry.length;i++){
        if(g_entry.text[i]=='.')continue;
        if(decimal>=0&&i>decimal){fraction=fraction*10+(u64)(g_entry.text[i]-'0');count++;}
        else integer=integer*10+(u64)(g_entry.text[i]-'0');
    }
    u64 divisor=1;for(int i=0;i<count;i++)divisor*=10;
    u64 factor=ENTRY_FACTORS[g_entry.unit];
    if(integer>MAX_FREQUENCY/factor)return false;
    u64 scaled=fraction*factor;if(scaled%divisor)return false; // frequency resolution is 1 Hz
    hz=integer*factor+scaled/divisor;return hz<=MAX_FREQUENCY;
}
static void append_digit(int digit){
    int decimal=entry_decimal(),count=decimal<0?g_entry.length:g_entry.length-decimal-1;
    if(count>=(decimal<0?12:9)||g_entry.length>=22){g_entry.overflow=true;return;}
    if(!g_entry.length)c_copy(g_entry.target,sizeof(g_entry.target),string_chars(wf_selected_name()));
    g_entry.text[g_entry.length++]=(char)('0'+digit);g_entry.text[g_entry.length]=0;
}
static void append_decimal(){
    if(entry_decimal()>=0)return;
    if(!g_entry.length)append_digit(0);
    g_entry.text[g_entry.length++]='.';g_entry.text[g_entry.length]=0;
}
static bool key_down(int key){KeyData*k=ui_GetKeyData(key);return k&&k->Down;}
static void cancel_digit_gestures(){for(int i=0;i<10;i++)g_digit_gestures[i].armed=false;}
static void cancel_shortcut_ownership(){
    cancel_digit_gestures();for(int i=0;i<4;i++)g_entry_control_owned[i]=false;
}
static void consume_key(int key){
    KeyData*k=ui_GetKeyData(key);if(!k||!k->Down||g_saved_key_count>=(int)(sizeof(g_saved_keys)/sizeof(g_saved_keys[0])))return;
    g_saved_keys[g_saved_key_count].key=key;g_saved_keys[g_saved_key_count++].data=*k;
    k->Down=false;k->DownDuration=-1.0f;k->DownDurationPrev=-1.0f;
}
static void pulse_native_menu(){
    KeyData*k=ui_GetKeyData(K_MENU);
    if(!k||k->Down||g_saved_key_count>=40)return;
    g_saved_keys[g_saved_key_count].key=K_MENU;g_saved_keys[g_saved_key_count++].data=*k;
    k->Down=true;k->DownDuration=0;k->DownDurationPrev=-1;k->AnalogValue=1;
}
static void restore_keys(){
    for(int i=0;i<g_saved_key_count;i++){KeyData*k=ui_GetKeyData(g_saved_keys[i].key);if(k)*k=g_saved_keys[i].data;}
    g_saved_key_count=0;
}
#include "receiver_history.inc"

static void idle_audio_meter();
static bool install_toolbar_hooks();
static bool restore_toolbar_hooks();
static bool install_audio_meter();
static bool install_panel_hooks();
static bool restore_panel_hooks();
static void finish_lightweight_slider_drag(bool force=false);
static void restore_native_waterfall_levels();
static void restore_audio_meter();
static bool native_toggle_selected_mute();
static void set_enabled(bool enabled){
    if(g_enabled==enabled)return;
    g_fixed_pan_pending=false;g_fixed_pan_force=false;
    cancel_alt_tap();cancel_control_tap();history_reset();
    if(!enabled){
        // Undo every reversible modification, not merely hide our drawings.
        // The next frame is rendered by the unmodified native SDR++ calls.
        finish_lightweight_slider_drag(true);
        restore_native_waterfall_levels();
        g_enabled=false;
        idle_audio_meter();clear_entry();cancel_shortcut_ownership();restore_keys();
        finish_source_request();g_zoom_transition={};
        restore_panel_hooks();request_panel_resize();restore_toolbar_hooks();restore_audio_meter();restore_source_imports();
    }else{
        g_enabled=true;
        if(g_supported){
            install_toolbar_hooks();install_audio_meter();install_source_imports();install_panel_hooks();
            g_speed_pending=g_speed_custom;
            g_speed_apply_after=ui_GetTime()+0.25; // SDR++ also refreshes FFT when re-enabled.
            g_contrast_needs_apply=true;
            sync_zoom_slider();
        }
    }
}
static bool our_window_focused(){
    void*w=GetForegroundWindow();if(!w)return false;
    u32 pid=0;GetWindowThreadProcessId(w,&pid);return pid==GetCurrentProcessId();
}
static void number_row_frame(bool down[10]){
    double now=ui_GetTime();const char*target=string_chars(wf_selected_name());
    for(int i=0;i<10;i++){
        int key=K_TOP0+i;DigitGesture&g=g_digit_gestures[i];down[i]=key_down(key);
        if(g.armed&&!c_equal(g.target,target))g.armed=false;
        if(ui_IsKeyPressed(key,false)){
            g.armed=true;g.fired=false;g.started=now;c_copy(g.target,sizeof(g.target),target);
        }
        if(g.armed){
            double elapsed=now-g.started;
            if(elapsed<0)g.armed=false;
            else if(!g.fired&&elapsed>=HOLD_SECONDS){g.fired=true;apply_preset(i);}
            if(!down[i]){
                if(g.armed&&!g.fired){
                    if(i>=1&&i<=8)set_modulation(MODE_ORDER[i-1]);
                    else cycle_modulation(i==0?1:-1);
                }
                g.armed=false;
            }
        }
        consume_key(key);
    }
}
// These declarations are defined after the toolbar include, below.
// Defer costly tuner calls beyond WaterFall::draw()'s FFT lock, but perform
// them at the START of the next frame, before native waterfall/VFO rendering.
static void process_fixed_pan();
static void process_free_pan();
static void process_passband_drag();
static void process_filter_wheel();
static bool v34_context_open();
static void keyboard_frame(void*,ContextHook*){
    restore_keys();
    if(!g_supported||!g_enabled){cancel_control_tap();cancel_alt_tap();cancel_shortcut_ownership();return;}
    process_fixed_pan();
    process_free_pan();
    process_passband_drag();
    process_filter_wheel();
    history_poll();
    u8*io=(u8*)ui_GetIO();
    if(!our_window_focused()){clear_entry();cancel_control_tap(io[0xD1C]!=0);cancel_alt_tap(io[0xD1E]!=0);cancel_shortcut_ownership();return;}
    // Mute is independent of VFO, popup and optional icon hooks. Do not
    // intercept typing into an active input control, or modifier shortcuts.
    const bool canMute=!(ui_IsAnyItemActive()||io[0xCA]||
        (GetAsyncKeyState(0x11)&0x8000)||(GetAsyncKeyState(0x12)&0x8000)||
        (GetAsyncKeyState(0x10)&0x8000)||(GetAsyncKeyState(0x5B)&0x8000)||
        (GetAsyncKeyState(0x5C)&0x8000));
    if(canMute&&ui_IsKeyPressed(K_M,false))native_toggle_selected_mute();
    if(canMute)consume_key(K_M);
    if(v34_context_open()){cancel_shortcut_ownership();return;}
    if(*(bool*)core_main_window || io[0xC9] || io[0xCA] || ui_IsAnyItemActive() || ui_IsPopupOpen(0,384)){
        cancel_control_tap(io[0xD1C]!=0);cancel_alt_tap(io[0xD1E]!=0);cancel_shortcut_ownership();return;
    }
    bool control=io[0xD1C]!=0,shift=io[0xD1D]!=0,alt=io[0xD1E]!=0,super=io[0xD1F]!=0;
    control_tap_frame(control,shift,alt,super);
    alt_tap_frame(alt,control,shift,super);
    // Backtick and Shift+backtick (~) both use the physical key below Esc.
    // Pulse the native Menu key so SDR++ also saves its sidebar visibility setting.
    if(!control&&!alt&&!super){
        if(ui_IsKeyPressed(K_GRAVE,false))pulse_native_menu();
        if(key_down(K_GRAVE)){
            consume_key(K_GRAVE);
            CharQueue*q=(CharQueue*)(io+0x3740);int remaining=0;
            if(q->Data)for(int i=0;i<q->Size;i++)if(q->Data[i]!='`'&&q->Data[i]!='~')q->Data[remaining++]=q->Data[i];
            q->Size=remaining;
        }
    }
    int modifiers=(int)control+(int)shift+(int)alt+(int)super;
    if(modifiers){
        cancel_shortcut_ownership();
        if(modifiers!=1||super)return; // conflicting modifier combinations stay native
        if(control){
            if(!g_entry.length&&ui_IsKeyPressed(K_Z,false))history_move(-1);
            if(!g_entry.length)consume_key(K_Z);
            if(ui_IsKeyPressed(K_UP,true))keyboard_zoom(1);
            if(ui_IsKeyPressed(K_DOWN,true))keyboard_zoom(-1);
            if(selected_vfo()){
                if(ui_IsKeyPressed(K_RIGHT,true))change_frequency(100000.0);
                if(ui_IsKeyPressed(K_LEFT,true))change_frequency(-100000.0);
                consume_key(K_LEFT);consume_key(K_RIGHT);
            }
            consume_key(K_UP);consume_key(K_DOWN);return;
        }
        if(ui_IsKeyPressed(K_UP,true))request_source_step(shift?1:2,1);
        if(ui_IsKeyPressed(K_DOWN,true))request_source_step(shift?1:2,-1);
        consume_key(K_UP);consume_key(K_DOWN);
        if(selected_vfo()){
            if(ui_IsKeyPressed(K_RIGHT,true))change_frequency(shift?10000.0:100.0);
            if(ui_IsKeyPressed(K_LEFT,true))change_frequency(shift?-10000.0:-100.0);
            consume_key(K_RIGHT);consume_key(K_LEFT);
        }
        return;
    }
    // Use the same source lifecycle and play lock as the native Play/Stop button.
    // Playback does not require a selected VFO; held Space must never repeat.
    if(ui_IsKeyPressed(K_SPACE,false)){
        bool playing=window_isPlaying(core_main_window);
        if(playing||!core_main_window[1])window_setPlayState(core_main_window,!playing);
    }
    consume_key(K_SPACE);
    if(!selected_vfo()){clear_entry();cancel_shortcut_ownership();return;}
    if(g_entry.length&&!c_equal(g_entry.target,string_chars(wf_selected_name())))clear_entry();
    bool topDown[10];number_row_frame(topDown);

    if(ui_IsKeyPressed(K_RIGHT,true))change_frequency(1000.0);
    if(ui_IsKeyPressed(K_LEFT,true))change_frequency(-1000.0);
    if(ui_IsKeyPressed(K_UP,true))change_filter(100.0);
    if(ui_IsKeyPressed(K_DOWN,true))change_filter(-100.0);
    if(ui_IsKeyPressed(K_ADD,true))change_frequency(1000.0);
    if(ui_IsKeyPressed(K_SUBTRACT,true))change_frequency(-1000.0);
    if(ui_IsKeyPressed(K_DIVIDE,true))change_frequency(-100.0);
    if(ui_IsKeyPressed(K_MULTIPLY,true))change_frequency(100.0);

    bool pressed[10],seen[10]={},down[10];bool decimalDown=key_down(K_DECIMAL),decimalPressed=ui_IsKeyPressed(K_DECIMAL,false),decimalSeen=false;
    bool anyDigits=decimalDown;
    for(int i=0;i<10;i++){pressed[i]=ui_IsKeyPressed(K_NUM0+i,false);down[i]=key_down(K_NUM0+i);anyDigits=anyDigits||down[i]||topDown[i];}
    CharQueue*q=(CharQueue*)(io+0x3740);
    if(anyDigits&&q->Data&&q->Size>0){
        int remaining=0;
        for(int i=0;i<q->Size;i++){
            unsigned short c=q->Data[i];int digit=(int)c-'0';
            if(digit>=0&&digit<=9&&(down[digit]||topDown[digit])){
                if(pressed[digit]&&!seen[digit]){append_digit(digit);seen[digit]=true;}
                // Top-row shortcuts and numpad input must never leak into native FrequencySelect.
            }else if((c=='.'||c==',')&&decimalDown){
                if(decimalPressed&&!decimalSeen){append_decimal();decimalSeen=true;}
            }else q->Data[remaining++]=c;
        }
        q->Size=remaining;
    }
    // Also handle keyboard backends that report keypad keys without character events.
    for(int i=0;i<10;i++)if(pressed[i]&&!seen[i])append_digit(i);
    if(decimalPressed&&!decimalSeen)append_decimal();

    bool editing=g_entry.length>0;
    if(g_entry.length&&ui_IsKeyPressed(K_BACKSPACE,true)){
        g_entry.text[--g_entry.length]=0;g_entry.overflow=false;
    }
    if(g_entry.length&&ui_IsKeyPressed(K_ESCAPE,false))clear_entry();
    bool enter=ui_IsKeyPressed(K_ENTER,false)||ui_IsKeyPressed(K_NUMENTER,false);
    bool hadEntry=g_entry.length>0;
    u64 enteredHz=0;
    if(hadEntry&&enter&&entry_value(enteredHz)){clear_entry();tune_frequency((double)enteredHz);}

    const int steps[]={K_LEFT,K_RIGHT,K_UP,K_DOWN,K_ADD,K_SUBTRACT,K_DIVIDE,K_MULTIPLY};
    for(int i=0;i<8;i++)consume_key(steps[i]);
    for(int i=0;i<10;i++)consume_key(K_NUM0+i);
    consume_key(K_DECIMAL);
    const int controls[]={K_ENTER,K_NUMENTER,K_BACKSPACE,K_ESCAPE};
    for(int i=0;i<4;i++){
        if(!key_down(controls[i]))g_entry_control_owned[i]=false;
        else if(editing||hadEntry||g_entry.length)g_entry_control_owned[i]=true;
        if(g_entry_control_owned[i])consume_key(controls[i]);
    }
}


#include "receiver_lcd.inc"
static void draw_receiver_digit(ImDrawList*dl,char digit,const ImVec2&pos,float height,u32 color,float horizontalScale=1){
    float width=height*0.48f,thick=height*0.095f,gap=height*0.045f,half=height*0.5f;
    if(digit=='.'){
        draw_AddRectFilled(dl,ImVec2(pos.x,pos.y+height-thick),ImVec2(pos.x+thick*horizontalScale,pos.y+height),color,0,0);return;
    }
    if(digit<'0'||digit>'9')return;
    static const u8 masks[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
    ImVec2 lo[]={ImVec2(thick,gap),ImVec2(width-thick,thick+gap),ImVec2(width-thick,half+gap),
        ImVec2(thick,height-thick-gap),ImVec2(0,half+gap),ImVec2(0,thick+gap),ImVec2(thick,half-thick*0.5f)};
    ImVec2 hi[]={ImVec2(width-thick,thick+gap),ImVec2(width,half-gap),ImVec2(width,height-thick-gap),
        ImVec2(width-thick,height-gap),ImVec2(thick,height-thick-gap),ImVec2(thick,half-gap),ImVec2(width-thick,half+thick*0.5f)};
    for(int segment=0;segment<7;segment++){
        u32 ink=(masks[digit-'0']&(1<<segment))?color:lcd_palette().off;
        draw_AddRectFilled(dl,ImVec2(pos.x+lo[segment].x*horizontalScale,pos.y+lo[segment].y),
            ImVec2(pos.x+hi[segment].x*horizontalScale,pos.y+hi[segment].y),ink,0,0);
    }
}
static void draw_needle_meter(const char*,float,float,bool,float,float);
static void show_marker_tooltip(const CursorPreview&pv,bool red){
    if(!g_marker_info_mode)return;
    char frequency[64],power[32],filter[48],filterLine[64],modeLine[64];
    format_frequency5(pv.freq,frequency,sizeof(frequency));
    float db=0.0f;
    bool valid=signal_at_fraction(pv.frac,db)&&db>=-1200&&db<=1200;
    if(valid)format_dbfs(db,power,sizeof(power));else c_copy(power,sizeof(power),"---.- dBFS");
    format_filter(pv.bw,filter,sizeof(filter));
    c_copy(filterLine,sizeof(filterLine),l10n("Filtr: "));c_append(filterLine,sizeof(filterLine),filter);
    c_copy(modeLine,sizeof(modeLine),l10n("Modulacja: "));c_append(modeLine,sizeof(modeLine),modulation_name(selected_modulation()));
    float scale=core_ui_scale;if(!(scale>=0.5f&&scale<=4))scale=1;
    // Fit the actual readouts, including device gain, rather than reserving
    // an empty fixed-width column. The optional dial adds its own column.
    char gain[80];bool hasGain=gain_caption(gain,sizeof(gain));
    float font=ui_GetFontSize();if(!(font>0))font=16*scale;
    float textWidth=ui_CalcTextSize(frequency,0,false,-1).x*(15*scale)/font+18*scale;
    const char*rows[]={power,filterLine,modeLine,gain};
    for(int i=0;i<(hasGain?4:3);i++){
        float w=ui_CalcTextSize(rows[i],0,false,-1).x*(14*scale)/font;
        if(w>textWidth)textWidth=w;
    }
    float titleWidth=ui_CalcTextSize(red?"ESSENTIAL  /  RX":l10n("ESSENTIAL  /  PODGLAD"),0,false,-1).x*(9*scale)/font;
    if(titleWidth>textWidth)textWidth=titleWidth;
    float textColumn=textWidth+24*scale;
    float width=textColumn+(g_marker_info_mode==2?96*scale:0);
    ui_PushStyleVarVec(2,ImVec2(12*scale,12*scale));ui_PushStyleVarFloat(3,8*scale);ui_PushStyleVarFloat(4,1*scale);
    ui_PushTextColor(0,0xFFDDEBE0u);ui_PushTextColor(2,0xFF1C2219u);ui_PushTextColor(4,0xFF1C2219u);ui_PushTextColor(5,0xFF425B4Bu);
    // Native Waterfall uses ImGui's shared ##Tooltip_00 for its "Radio" label.
    // A distinct no-input overlay prevents both tooltips from writing to one window.
    ImVec2 mouse=ui_GetMousePos();u8*io=(u8*)ui_GetIO();
    ImVec2 display=io?*(ImVec2*)(io+8):ImVec2(1920,1080);
    float height=(hasGain?166:146)*scale;
    // Native SDR++ shows its small "Radio" tooltip next to the red marker.
    // Leave a full tooltip-height gap before our larger RX preview; if there
    // is no room below, put the preview above with the same separation.
    float x=mouse.x+22*scale;
    float y=mouse.y+(red?43:24)*scale;
    if(display.x>width+8*scale&&x+width>display.x-8*scale)
        x=mouse.x-width-22*scale;
    if(display.y>height+8*scale&&y+height>display.y-8*scale)
        y=mouse.y-height-30*scale;
    if(x<8*scale)x=8*scale;
    if(y<8*scale)y=8*scale;
    ui_SetNextWindowPos(ImVec2(x,y),1,ImVec2());
    ui_SetNextWindowSize(ImVec2(width,0),1);
    ui_Begin("##Essential++ Marker Info",0,1|2|4|8|64|256|512|4096|33554432);
    ui_BringWindowToDisplayFront(ui_GetCurrentWindow());
    ImVec2 pos=ui_GetWindowPos();ImDrawList*dl=ui_GetWindowDrawList();u32 accent=red?0xFF8080FFu:0xFFA7F5BFu;
    draw_AddTextSized(dl,0,9*scale,ImVec2(pos.x+12*scale,pos.y+10*scale),lcd_palette().dim,red?"ESSENTIAL  /  RX":l10n("ESSENTIAL  /  PODGLAD"),0,0,0);
    draw_AddRectFilled(dl,ImVec2(pos.x+12*scale,pos.y+27*scale),ImVec2(pos.x+textColumn-12*scale,pos.y+55*scale),0xFF10160Du,4*scale,0);
    draw_AddTextSized(dl,0,15*scale,ImVec2(pos.x+21*scale,pos.y+33*scale),accent,frequency,0,0,0);
    for(int i=0;i<(hasGain?4:3);i++)draw_AddTextSized(dl,0,14*scale,ImVec2(pos.x+12*scale,pos.y+(65+i*20)*scale),i==0?accent:0xFFDDEBE0u,rows[i],0,0,0);
    if(g_marker_info_mode==2){ui_SetCursorPos(ImVec2(textColumn-6*scale,68*scale));draw_needle_meter("RF",valid?clamp01((db+120)/120):0,db,valid,90*scale,scale);}
    float bottom=(hasGain?148:128)*scale;
    draw_AddLine(dl,ImVec2(pos.x+12*scale,pos.y+bottom),ImVec2(pos.x+width-12*scale,pos.y+bottom),0xFF304738u,scale);
    ui_SetCursorPos(ImVec2(12*scale,bottom+2*scale));ui_Dummy(ImVec2(width-24*scale,1*scale));
    ui_End();ui_PopStyleColor(4);ui_PopStyleVar(3);
}
static void draw_receiver_number(ImDrawList*dl,const char*text,const ImVec2&pos,float available,u32 color){
    float units=0;for(const char*p=text;*p;p++)units+=*p=='.'?0.22f:0.62f;
    float height=46;if(units*height>available&&units>0)height=available/units;
    float x=pos.x,y=pos.y+(46-height)*0.5f;
    for(const char*p=text;*p;p++){
        draw_receiver_digit(dl,*p,ImVec2(x,y),height,color);x+=height*(*p=='.'?0.22f:0.62f);
    }
    draw_AddLine(dl,ImVec2(x+3,y+height*0.1f),ImVec2(x+3,y+height*0.9f),0x8090F2B0u,1.5f);
}
static bool draw_entry_banner(const InputHandlerArgs&args){
    if(!g_entry.length){g_entry_rect_valid=false;return false;}
    ui_PushStyleVarVec(2,ImVec2(18,16));ui_PushStyleVarFloat(3,2);ui_PushStyleVarFloat(4,1);
    ui_PushStyleVarFloat(9,1);ui_PushStyleVarVec(11,ImVec2(10,8));ui_PushStyleVarFloat(12,1);
    const int colors[]={0,2,4,5,7,8,9,21,22,23,24,25,26};
    const LcdPalette&lc=lcd_palette();
    const u32 palette[]={lc.ink,lc.glass,lc.substrate,lc.frame,lc.frame,
        lc.divider,lc.rim,lc.frame,lc.divider,lc.rim,lc.frame,lc.divider,lc.rim};
    for(int i=0;i<13;i++)ui_PushTextColor(colors[i],palette[i]);
    ui_SetNextWindowPos(ImVec2(args.fftRectMin.x+12,args.fftRectMin.y+12),1,ImVec2());
    ui_SetNextWindowSize(ImVec2(440,224),1);
    bool visible=ui_Begin("Frequency entry##Essential++",0,1|2|4|8|16|32|256|4096|524288);
    // Keep the entry above full-screen Main without taking keyboard focus or
    // covering its unit dropdown (or another active popup).
    if(!ui_IsPopupOpen(0,384))ui_BringWindowToDisplayFront(ui_GetCurrentWindow());
    if(visible){
        ImVec2 p=ui_GetWindowPos();ImDrawList*dl=ui_GetWindowDrawList();
        draw_AddText(dl,ImVec2(p.x+18,p.y+18),lc.ink,"ESSENTIAL",0);
        draw_AddText(dl,ImVec2(p.x+18,p.y+42),lc.dim,"WPISZ CZESTOTLIWOSC",0);
        draw_AddRectFilled(dl,ImVec2(p.x+380,p.y+16),ImVec2(p.x+422,p.y+40),lc.frame,1,0);
        draw_AddText(dl,ImVec2(p.x+394,p.y+21),lc.glow,"RX",0);
        draw_lcd_glass(dl,ImVec2(p.x+18,p.y+68),ImVec2(p.x+422,p.y+158),1);
        draw_AddText(dl,ImVec2(p.x+322,p.y+78),lc.dim,"JEDNOSTKA",0);
        ui_SetCursorPos(ImVec2(322,98));ui_SetNextItemWidth(82);
        if(ui_Combo("##essential_entry_unit",&g_entry.unit,ENTRY_UNITS,-1)){settings_changed();save_settings();}
        u64 hz=0;
        bool valid=entry_value(hz);
        draw_receiver_number(dl,g_entry.text,ImVec2(p.x+34,p.y+89),264,valid?lc.glow:0xFF9292FFu);
        draw_AddLine(dl,ImVec2(p.x+24,p.y+156),ImVec2(p.x+416,p.y+156),lc.frame,1);
        draw_AddText(dl,ImVec2(p.x+18,p.y+174),valid?lc.dim:0xFF9292FFu,
            valid?"GOTOWE DO ZATWIERDZENIA":"POPRAW WPIS: pelne Hz, do 999999999999 Hz",0);
        draw_AddText(dl,ImVec2(p.x+18,p.y+203),lc.soft,"ENTER  zatwierdz",0);
        draw_AddText(dl,ImVec2(p.x+162,p.y+203),lc.soft,"BACKSPACE  usun",0);
        draw_AddText(dl,ImVec2(p.x+320,p.y+203),lc.soft,"ESC  anuluj",0);
        ui_SetCursorPos(ImVec2(18,220));ui_Dummy(ImVec2(1,1));
    }
    g_entry_rect_min=ui_GetWindowPos();ImVec2 size=ui_GetWindowSize();
    g_entry_rect_max=ImVec2(g_entry_rect_min.x+size.x,g_entry_rect_min.y+size.y);g_entry_rect_valid=true;
    bool hovered=in_rect(ui_GetMousePos(),g_entry_rect_min,g_entry_rect_max);ui_End();
    ui_PopStyleColor(13);ui_PopStyleVar(6);return hovered;
}
// The toolbar may change tuning modes while the last mouse gesture is ending.
// Cancel the gesture without moving the VFO or SDR centre.
static void cancel_tuning_gestures();
#include "receiver_toolbar.inc"
#include "panel_controls.inc"
static bool v34_context_open();
#include "v38_context.inc"

static bool g_left_was_down=false;
static bool g_right_control_drag=false;
static bool g_pan_candidate=false;
static bool g_pan_started_on_marker=false;
static bool g_panning=false;
static bool g_native_gesture=false;
static bool g_native_resize=false;
static ImVec2 g_pan_start_mouse;
static double g_pan_start_offset=0.0;
static double g_pan_start_center=0.0;
static double g_pan_locked_hz=0.0;
static double g_click_frequency=0.0;
static const float PAN_THRESHOLD=4.0f;
#include "fixed_pan_logic.inc"
// Captured at mouse-down; never derive the pan origin from an already-panned frame.
static double g_pan_hz_per_pixel=1.0;
static double g_pan_latest_dx=0.0;
static double g_pan_last_hardware_change=-100.0;
static double g_free_pan_dx=0.0,g_free_pan_last_hw=-100.0;
static char g_pan_vfo_name[256]={};

// Direct passband dragging. This owns the gesture from mouse-down through
// release, even if the cursor leaves the highlighted filter. Frequency is
// measured from the original pointer position: grabbing near either side
// must never jump the carrier under the cursor on the first frame.
struct PassbandDrag {
    bool captured=false,dragging=false,pending=false,finalPending=false;
    float mouseStartX=0;
    double initialHz=0,hzPerPixel=1,snapHz=1,targetHz=0,lastRetune=-100;
    char vfoName[256]={};
};
static PassbandDrag g_passband_drag={};
static double g_filter_wheel_pending=0.0;
static void process_filter_wheel(){
    if(g_filter_wheel_pending==0.0)return;
    double delta=g_filter_wheel_pending;g_filter_wheel_pending=0.0;
    if(g_supported&&g_enabled&&selected_vfo()&&radio_interface_ready())change_filter(delta);
}
static void cancel_tuning_gestures(){
    g_pan_candidate=g_panning=g_native_gesture=g_native_resize=false;
    g_passband_drag.captured=g_passband_drag.dragging=false;
    g_passband_drag.pending=g_passband_drag.finalPending=false;
    g_fixed_pan_pending=g_fixed_pan_force=false;
    g_free_pan_pending=g_free_pan_force=false;
    g_filter_wheel_pending=0.0;
}
static void queue_passband_drag(const ImVec2& mouse,bool finish){
    PassbandDrag& d=g_passband_drag;
    const double dx=(double)mouse.x-(double)d.mouseStartX;
    if(!d.dragging && dabs(dx)>=(double)PAN_THRESHOLD)d.dragging=true;
    if(d.dragging){
        const double desired=d.initialHz+dx*d.hzPerPixel;
        const double snap=d.snapHz>0.000001?d.snapHz:1.0;
        d.targetHz=round_nearest(desired/snap)*snap;
        if(d.targetHz<0.0)d.targetHz=0.0;
        if(d.targetHz>999999999999.0)d.targetHz=999999999999.0;
        d.pending=true;
        if(finish)d.finalPending=true;
    }
}
static void process_passband_drag(){
    PassbandDrag&d=g_passband_drag;
    if(!d.pending||!g_supported||!g_enabled)return;
    if(!selected_vfo()||!c_equal(d.vfoName,string_chars(wf_selected_name()))){
        d.pending=false;d.finalPending=false;return;
    }
    const double oldHz=selected_frequency();
    if(dabs(oldHz-d.targetHz)<0.5){d.pending=false;d.finalPending=false;return;}
    // Avoid hammering the tuner when the drag reaches a hardware edge or in
    // native Center mode; within the current SDR bandwidth VFO movement is
    // handled by SDR++'s original tuning path without extra FFT work.
    const double center=wf_getCenterFrequency(core_waterfall);
    const double whole=wf_getBandwidth(core_waterfall);
    const double halfFilter=vfo_bandwidth(selected_vfo())*0.5;
    const bool hardwareLikely=*(int*)(core_main_window+0xFC)==0 ||
        d.targetHz-halfFilter<center-whole*0.5 ||
        d.targetHz+halfFilter>center+whole*0.5;
    const double now=ui_GetTime();
    if(!d.finalPending&&hardwareLikely&&now-d.lastRetune<1.0/15.0)return;
    tune_frequency(d.targetHz);
    if(hardwareLikely)d.lastRetune=now;
    d.pending=false;d.finalPending=false;
}
static void queue_fixed_pan(double deltaX,bool onRelease){
    g_pan_latest_dx=deltaX;
    g_fixed_pan_pending=true;
    if(onRelease)g_fixed_pan_force=true;
}
// Called once per ImGui end-of-frame, OUTSIDE WaterFall::draw()'s FFT mutex.
// Retuning from the waterfall input callback itself previously blocked drawing,
// and postponing ALL hardware retunes until mouse-up made full-zoom panning inert.
static void process_fixed_pan(){
    if(!g_fixed_pan_pending || !g_supported || !g_enabled || g_tuning_mode!=2)return;
    if(!selected_vfo()||!c_equal(g_pan_vfo_name,string_chars(wf_selected_name()))){
        g_fixed_pan_pending=false;g_fixed_pan_force=false;return;
    }
    const double whole=wf_getBandwidth(core_waterfall);
    const double view=wf_getViewBandwidth(core_waterfall);
    if(!(whole>0.0&&view>0.0&&g_pan_hz_per_pixel>0.0)){
        g_fixed_pan_pending=false;g_fixed_pan_force=false;return;
    }
    const double now=ui_GetTime();
    const double oldCenter=wf_getCenterFrequency(core_waterfall);
    FixedPanPlan requested=plan_fixed_pan(g_pan_start_center,g_pan_start_offset,
        g_pan_locked_hz,whole,view,g_pan_hz_per_pixel,g_pan_latest_dx,oldCenter,true);
    const bool centerNeedsRetune=fixed_abs(requested.hardwareCenterHz-oldCenter)>
        g_pan_hz_per_pixel*0.40;
    // Maximum 15 SDR hardware retunes per second while dragging; always apply
    // the final one on mouse release. Pure view-offset pans remain unthrottled.
    const bool mayRetune=g_fixed_pan_force ||
        !centerNeedsRetune || now-g_pan_last_hardware_change>=1.0/15.0;
    FixedPanPlan p=mayRetune?requested:plan_fixed_pan(g_pan_start_center,g_pan_start_offset,
        g_pan_locked_hz,whole,view,g_pan_hz_per_pixel,g_pan_latest_dx,oldCenter,false);
    if(p.hardwareCenterHz<0.0 || p.hardwareCenterHz>999999999999.0){
        g_fixed_pan_pending=false;g_fixed_pan_force=false;return;
    }
    if(centerNeedsRetune&&mayRetune){
        CoreString name;core_stringCopy(&name,wf_selected_name());
        core_tune(4,&name,p.hardwareCenterHz); // native IQ-only center retune
        g_pan_last_hardware_change=now;
    }
    const double hwNow=wf_getCenterFrequency(core_waterfall);
    if(fixed_abs(wf_getViewOffset(core_waterfall)-p.viewOffsetHz)>
       g_pan_hz_per_pixel*0.02)
        wf_setViewOffset(core_waterfall,p.viewOffsetHz);
    const double currentMarker=selected_frequency();
    if(fixed_abs(currentMarker-p.markerHz)>0.25){
        CoreString name;core_stringCopy(&name,wf_selected_name());
        // Unlike WaterfallVFO::setOffset, VFOManager also updates the DSP VFO.
        manager_setOffset(core_vfo_manager,&name,p.markerHz-hwNow);
    }
    g_fixed_marker_hz=p.markerHz;
    // Keep this request alive until a throttled hardware retune can catch up.
    g_fixed_pan_pending=centerNeedsRetune&&!mayRetune;
    if(!g_fixed_pan_pending)g_fixed_pan_force=false;
}

// Unpinned Free Tuning: move the SDR tuning centre, not the zoomed viewport.
// SDR++ processes selected-frequency updates through its native IQ tuner.
// Hardware retuning is deferred until after WaterFall::draw releases its lock.
static void queue_free_pan(double deltaX,bool onRelease){
    g_free_pan_dx=deltaX;g_free_pan_pending=true;
    if(onRelease)g_free_pan_force=true;
}
static void process_free_pan(){
    if(!g_free_pan_pending||!g_supported||!g_enabled||g_tuning_mode!=0)return;
    if(!selected_vfo()||!c_equal(g_pan_vfo_name,string_chars(wf_selected_name()))){
        g_free_pan_pending=g_free_pan_force=false;return;
    }
    const double desired=g_pan_start_center-g_free_pan_dx*g_pan_hz_per_pixel;
    if(!(desired>=0.0&&desired<=999999999999.0)){
        g_free_pan_pending=g_free_pan_force=false;return;
    }
    const double current=wf_getCenterFrequency(core_waterfall);
    const double now=ui_GetTime();
    // No need to re-send an identical frequency on mouse-up. Ignore tiny
    // sub-pixel changes while dragging to reduce USB/TCP tuning traffic.
    const double freqChange=dabs(desired-current);
    if(freqChange<0.5||(!g_free_pan_force&&freqChange<=g_pan_hz_per_pixel*0.50)){
        g_free_pan_pending=g_free_pan_force=false;return;
    }
    // 30 Hz ceiling for hardware retunes, final mouse-up always applied.
    if(!g_free_pan_force&&now-g_free_pan_last_hw<1.0/30.0)return;
    CoreString name;core_stringCopy(&name,wf_selected_name());
    core_tune(4,&name,desired); // SDR++ native IQ-only hardware centre update
    g_free_pan_last_hw=now;
    g_free_pan_pending=g_free_pan_force=false;
}

// Render the selected passband on the waterfall only when Essential consumes
// the mouse input. Native SDR++ suppresses its waterfall VFO preview when
// inputHandled=true. IMPORTANT: use WaterfallVFO's *already calculated* drawing
// geometry, rather than projecting selected_frequency() again after the frame.
// The native spectrum marker and this overlay now share identical X positions.
// O(1): one translucent rectangle and one line, with no FFT/history processing.
static void draw_captured_passband(const InputHandlerArgs&args){
    if(!(g_passband_drag.captured||g_pan_candidate||g_panning))return;
    void*v=selected_vfo();if(!v)return;
    const ImVec2 boxMin=vfo_vec2(v,0x64); // WaterfallVFO::wfRectMin
    const ImVec2 boxMax=vfo_vec2(v,0x6C); // WaterfallVFO::wfRectMax
    const ImVec2 lineMin=vfo_vec2(v,0x74); // WaterfallVFO::wfLineMin
    if(!(boxMin.x==boxMin.x&&boxMax.x==boxMax.x&&lineMin.x==lineMin.x))return;
    if(boxMax.x<boxMin.x||args.waterfallRectMax.x<=args.waterfallRectMin.x)return;
    ImDrawList*dl=ui_GetForegroundDrawList();if(!dl)return;
    draw_PushClipRect(dl,args.waterfallRectMin,args.waterfallRectMax,true);
    draw_AddRectFilled(dl,ImVec2(boxMin.x,args.waterfallRectMin.y),
        ImVec2(boxMax.x,args.waterfallRectMax.y),0x32FFFFFFu,0.0f,0);
    if(lineMin.x>=args.waterfallRectMin.x&&lineMin.x<=args.waterfallRectMax.x)
        draw_AddLine(dl,ImVec2(lineMin.x,args.waterfallRectMin.y),
            ImVec2(lineMin.x,args.waterfallRectMax.y),0xFF0000FFu,1.0f);
    draw_PopClipRect(dl);
}

static void tooltip_frame(void*,ContextHook*){
    // Remember that a dropdown was open BEFORE the click that dismisses it.
    // A closed popup cannot otherwise be detected on the mouse-up frame.
    if(g_supported&&g_enabled&&ui_IsPopupOpen(0,384))
        g_v40_last_popup_frame=ui_GetFrameCount();
    if(g_supported&&g_enabled && ui_GetFrameCount()>10){
        if(g_tuning_mode<0)g_tuning_mode=(*(int*)(core_main_window+0xFC)==0)?1:0;
        if(g_tuning_mode>=0){
            const int nativeMode=g_tuning_mode==1?0:1;
            if(*(int*)(core_main_window+0xFC)!=nativeMode)
                *(int*)(core_main_window+0xFC)=nativeMode;
            const u8 singleClick=g_tuning_mode==1?1:0;
            if(core_waterfall[0x71]!=singleClick)core_waterfall[0x71]=singleClick;
        }
    }
    if(g_supported&&g_enabled)history_poll();
    if(g_supported)save_presets_if_due();
    process_source_request();
    process_deferred_rate();
    process_deferred_device();
    // Pan and VFO drag are committed at NewFramePost, before native SDR++
    // draws the spectrum and waterfall. EndFramePre must NOT retune the source:
    // otherwise an already-drawn FFT marker and our overlay use different Hz.
    // Apply slider changes from the per-frame GUI callback; V29 previously
    // declared this update function but never called it, so Speed was inert.
    apply_deferred_panel_sliders();
    if(g_supported&&g_enabled)process_server_connection();
    if(!g_supported||!g_enabled||!our_window_focused())return;
    refresh_gain_reading();
    // Native SDR++ zoom now owns the VSliderFloat -> viewBandwidth mapping.
    // Reapplying a different zoom formula after its callback caused snap-to-end
    // and expensive full waterfall rebuilds. Only OBSERVE it here.
    float nativeSlider=*(float*)(core_main_window+0x9C);
    if(!g_zoom_transition.pending && nativeSlider!=g_slider_snapshot){
        g_slider_snapshot=nativeSlider;
        zoom_record_preference();
    }
    zoom_restore_frame();
    if(g_args_frame!=ui_GetFrameCount())return;
    // Erase ONLY the native white LEFT border, covering its subpixel fringe.
    // The user explicitly wants the bottom border to remain unchanged.
    {const InputHandlerArgs &a=g_last_args; ImDrawList*dl=ui_GetForegroundDrawList();
     if(dl&&a.waterfallRectMax.x>a.waterfallRectMin.x+100 && a.waterfallRectMax.y>a.fftRectMin.y+100){
       float x=a.fftRectMin.x;
       draw_AddRectFilled(dl,ImVec2(x-2.25f,a.fftRectMin.y),
          ImVec2(x+1.5f,a.waterfallRectMax.y),0xFF080808u,0.0f,0);
     }}
    const InputHandlerArgs&args=g_last_args;
    draw_captured_passband(args);
    raise_native_right_panel();
    v34_draw_context();
    if(v34_context_open() && g_v34_context.on_bookmark){
        // The native Frequency Manager draws its hover tooltip before this
        // EndFramePre hook. Override that tooltip for as long as our context
        // menu/form is active. Keep the replacement empty and transparent,
        // so no black tooltip rectangle remains above the PPM menu.
        ui_PushStyleVarVec(2,ImVec2(0,0));
        ui_PushTextColor(4,0u); // ImGuiCol_PopupBg
        ui_PushTextColor(5,0u); // ImGuiCol_Border
        ui_BeginTooltipEx(1,0); // ImGuiTooltipFlags_OverridePrevious
        ui_EndTooltip();
        ui_PopStyleColor(2);
        ui_PopStyleVar(1);
    }
    if(!v34_context_open() && !g_entry.length && !v34_mouse_over_context(ui_GetMousePos())){
        V38BookmarkHit label={};
        if(v38_bookmark_hit(args,ui_GetMousePos(),true,label)){
            ui_SetMouseCursor(7); // ImGuiMouseCursor_Hand
            return; // Native Frequency Manager handles tooltip and tune.
        }
    }
    // Hide the green marker as soon as the cursor enters our floating menu,
    // edit form or slot list; the preview cannot show through the form.
    if(v34_mouse_over_context(ui_GetMousePos()))return;
    const bool menuPreview=(g_v34_context.stage==1);
    if(!menuPreview && draw_entry_banner(args))return;
    if((*(bool*)core_main_window && g_v34_context.stage!=5)||ui_IsPopupOpen(0,384))return;
    ImVec2 mouse=menuPreview?g_v34_context.origin:ui_GetMousePos();
    // Green marker and its tooltip must disappear over the right sliders.
    if(mouse_over_right_controls(mouse,args))return;
    bool overFFT=in_rect(mouse,args.fftRectMin,args.fftRectMax),overWF=in_rect(mouse,args.waterfallRectMin,args.waterfallRectMax);
    if(!overFFT&&!overWF)return;
    double span=args.highFreq-args.lowFreq;if(span<=0)return;
    void*v=selected_vfo();
    bool red=v&&(selected_vfo_passband_hit(args,mouse)||selected_vfo_edge_hover(mouse));
    if(v){
        float x=args.fftRectMin.x+(float)((selected_frequency()-args.lowFreq)*args.freqToPixelRatio);
        if(x>=args.fftRectMin.x&&x<=args.fftRectMax.x&&dabs(mouse.x-x)<=4.0)red=true;
    }
    if(red){
        CursorPreview pv={selected_frequency(),vfo_bandwidth(v),vfo_snap(v),vfo_reference(v),true,0};
        pv.frac=clamp01((float)((pv.freq-args.lowFreq)/span));
        show_marker_tooltip(pv,true);
        return; // no green marker and no green tooltip anywhere over the selected red VFO
    }
    // Left Alt controls whether the green preview is visible at all. Persist
    // the 0/1/2 state with the rest of the Essential settings.
    if(g_marker_info_mode==0)return;
    bool leftDown=(GetAsyncKeyState(0x01)&0x8000)!=0;
    // Do not leave a stale preview over the VFO during mouse gestures or during a resize.
    if(leftDown||g_pan_candidate||g_panning||g_native_gesture||g_native_resize)return;
    float nativeLeft=*(float*)(core_waterfall+0x90);
    float nativeRight=*(float*)(core_waterfall+0x98);
    if(dabs((double)(nativeLeft-args.fftRectMin.x))>2.5||dabs((double)(nativeRight-args.fftRectMax.x))>2.5)return;
    if(!(args.highFreq>args.lowFreq) || !(args.freqToPixelRatio>0.0))return;
    CursorPreview pv=make_preview(args,mouse);
    float x=args.fftRectMin.x+(float)((pv.freq-args.lowFreq)*args.freqToPixelRatio);
    if(x<args.fftRectMin.x)x=args.fftRectMin.x;if(x>args.fftRectMax.x)x=args.fftRectMax.x;
    ImDrawList*dl=ui_GetForegroundDrawList();
    // The draw list is global; clip each preview to its own plot to prevent
    // green lines spilling into the scale, outer borders or control overlays.
    draw_PushClipRect(dl,args.fftRectMin,args.fftRectMax,true);
    draw_preview_rect(dl,args,pv,x,args.fftRectMin,args.fftRectMax);
    draw_PopClipRect(dl);
    draw_PushClipRect(dl,args.waterfallRectMin,args.waterfallRectMax,true);
    draw_preview_rect(dl,args,pv,x,args.waterfallRectMin,args.waterfallRectMax);
    draw_PopClipRect(dl);
    // A tooltip would cover the active context menu; the green line stays.
    if(!menuPreview)show_marker_tooltip(pv,false);
}
static void restore_frame(void*,ContextHook*){restore_keys();}
static bool bind_keyboard(){
    void*ctx=ui_GetContext();if(!ctx)return false;
    if(ctx==g_hook_context&&g_keyboard_hook)return true;
    g_hook_context=ctx;
    ContextHook h={0,1,0,keyboard_frame,0};g_keyboard_hook=ui_AddHook(ctx,&h);
    h.Type=2;h.Callback=tooltip_frame;g_tooltip_hook=ui_AddHook(ctx,&h);
    h.Type=3;h.Callback=restore_frame;g_restore_hook=ui_AddHook(ctx,&h);
    return g_keyboard_hook&&g_tooltip_hook&&g_restore_hook;
}
static void unbind_keyboard(){
    if(g_hook_context&&g_hook_context==ui_GetContext()){
        restore_keys();
        if(g_keyboard_hook)ui_RemoveHook(g_hook_context,g_keyboard_hook);
        if(g_tooltip_hook)ui_RemoveHook(g_hook_context,g_tooltip_hook);
        if(g_restore_hook)ui_RemoveHook(g_hook_context,g_restore_hook);
    }
    g_hook_context=0;g_keyboard_hook=g_tooltip_hook=g_restore_hook=0;clear_entry();cancel_control_tap();cancel_shortcut_ownership();
}

static bool g_v36_block_context_click_until_up=false;
static bool g_v40_dismiss_click_until_release=false;
static bool g_v40_middle_was_down=false;
// Frequency copied under MMB is quantised like the existing green marker.
static void v40_copy_frequency(double hz,int mode){
    if(!(hz>=0.0&&hz<1000000000000.0))return;
    const u64 rounded=(u64)(hz+0.5);
    const u64 integer=rounded/1000;u32 frac=(u32)(rounded%1000);
    char out[96],digits[32];u64str(integer,digits,sizeof(digits));
    c_copy(out,sizeof(out),digits);
    if(frac){
        char decimal[5]={'.',(char)('0'+frac/100),(char)('0'+(frac/10)%10),(char)('0'+frac%10),0};
        for(int i=3;i>0&&decimal[i]=='0';i--)decimal[i]=0;
        c_append(out,sizeof(out),decimal);
    }
    c_append(out,sizeof(out),"kHz ");
    c_append(out,sizeof(out),modulation_name(mode));
    ui_SetClipboardText(out);
}

static void input_callback(InputHandlerArgs args,void*){
    if(!g_supported||!g_enabled){g_v40_middle_was_down=false;return;}
    bind_keyboard();
    g_last_args=args;g_args_frame=ui_GetFrameCount();
    ImVec2 mouse=ui_GetMousePos();
    const bool leftDown=(GetAsyncKeyState(0x01)&0x8000)!=0;
    const bool rightDown=(GetAsyncKeyState(0x02)&0x8000)!=0;
    const bool rightPressed=rightDown&&!g_v34_right_was_down;
    g_v34_right_was_down=rightDown;
    // A click that dismissed the menu must not retune on either mouse-down or
    // mouse-up, regardless of native input-handler ordering.
    if(g_v36_block_context_click_until_up){
        g_left_was_down=leftDown;
        *wf_input_handled()=true;
        if(!leftDown)g_v36_block_context_click_until_up=false;
        return;
    }
    const bool overFFT=in_rect(mouse,args.fftRectMin,args.fftRectMax);
    const bool overWF=in_rect(mouse,args.waterfallRectMin,args.waterfallRectMax);
    const bool overPlot=(overFFT||overWF) && !mouse_over_right_controls(mouse,args);
    // MMB: copy tuned VFO Hz from its grey filter, otherwise cursor Hz.
    // Never tune, pan or alter the red/green markers.
    const bool middleDown=(GetAsyncKeyState(0x04)&0x8000)!=0;
    if(middleDown&&!g_v40_middle_was_down && overPlot &&
       !v34_context_open() && !ui_IsPopupOpen(0,384)){
        double hz=selected_vfo_passband_hit(args,mouse) ? selected_frequency()
            : make_preview(args,mouse).freq;
        if(selected_vfo())v40_copy_frequency(hz,selected_modulation());
        *wf_input_handled()=true;
    }
    g_v40_middle_was_down=middleDown;
    // The same click that closes a toolbar Combo must NOT tune on release.
    if(g_v40_dismiss_click_until_release){
        g_left_was_down=leftDown;*wf_input_handled()=true;
        if(!leftDown){g_v40_dismiss_click_until_release=false;g_v40_last_popup_frame=-1000;}
        return;
    }
    if(overPlot && leftDown && !g_left_was_down &&
       (ui_IsPopupOpen(0,384) || ui_GetFrameCount()-g_v40_last_popup_frame<=1)){
        g_v40_dismiss_click_until_release=true;
        cancel_tuning_gestures();g_left_was_down=true;
        *wf_input_handled()=true;return;
    }
    V38BookmarkHit v38Label={};
    const bool overBookmarkLabel=overFFT && v38_bookmark_hit(args,mouse,true,v38Label);
    if(rightPressed&&(overFFT||overWF)&&!mouse_over_right_controls(mouse,args)){
        cancel_tuning_gestures();
        V38BookmarkHit bookmark=v38Label;
        // Preserve the established FFT PPM behavior. On the waterfall only,
        // resolve the bookmark by the selected VFO's grey passband and an
        // EXACT match between its tuned frequency and the saved bookmark.
        // The yellow label / carrier hit test is FFT-only, never waterfall.
        const bool onBookmark=overBookmarkLabel ||
            (overFFT && (v38_bookmark_hit(args,mouse,false,bookmark) ||
                         v39_bookmark_selected_passband_hit(args,mouse,bookmark))) ||
            (overWF && v39_bookmark_selected_passband_hit(args,mouse,bookmark));
        v34_open_context(args,mouse,onBookmark?&bookmark:nullptr);
        *wf_input_handled()=true;return;
    }
    if(g_v34_context.stage==1&&leftDown&&!g_left_was_down&&(overFFT||overWF)){
        const bool onMenu=g_v34_context.window_valid&&
            in_rect(mouse,g_v34_context.window_pos,
                ImVec2(g_v34_context.window_pos.x+g_v34_context.window_size.x,
                       g_v34_context.window_pos.y+g_v34_context.window_size.y));
        if(!onMenu){
            g_v34_context.stage=0;
            g_v36_block_context_click_until_up=true;
            cancel_tuning_gestures();g_left_was_down=true;
            *wf_input_handled()=true;return;
        }
    }
    // Native Frequency Manager owns a click anywhere in the label rectangle.
    // Essential must never convert its left/right ends into a pan gesture.
    // Leave inputHandled untouched so the native module can tune once, with
    // its saved mode and bandwidth. Never steal an already active drag.
    if(overBookmarkLabel && !v34_context_open() && !g_passband_drag.captured &&
       !g_pan_candidate && !g_panning && !g_native_resize){
        g_left_was_down=leftDown;
        return;
    }
    // Continue an already-owned VFO gesture before checking other UI panels.
    // This is important when the pointer passes outside the highlighted band.
    const bool pointerDown=leftDown;
    if(g_passband_drag.captured){
        queue_passband_drag(mouse,!pointerDown);
        *wf_input_handled()=true;
        if(!pointerDown){
            g_passband_drag.captured=false;
            g_passband_drag.dragging=false;
        }
        g_left_was_down=pointerDown;
        return;
    }
    const bool rightPanelHit=mouse_over_right_controls(mouse,args);
    const bool buttonDown=(GetAsyncKeyState(0x01)&0x8000)!=0;
    if(rightPanelHit&&buttonDown&&!g_left_was_down)g_right_control_drag=true;
    if(rightPanelHit||g_right_control_drag){
        // No frequency changes from slider clicks, drags or releases.
        g_pan_candidate=g_panning=g_native_gesture=g_native_resize=false;
        g_fixed_pan_pending=false;g_fixed_pan_force=false;
        g_free_pan_pending=g_free_pan_force=false;
        g_left_was_down=buttonDown;
        *wf_input_handled()=true;
        if(!buttonDown)g_right_control_drag=false;
        return;
    }
    bool inputPopup=v34_context_open()||ui_IsPopupOpen(0,384)||ui_GetFrameCount()<=g_tuning_input_suppressed_through;
    if(inputPopup){
        g_pan_candidate=false;g_panning=false;g_native_gesture=false;g_native_resize=false;
        g_fixed_pan_pending=false;g_fixed_pan_force=false;
        g_free_pan_pending=g_free_pan_force=false;
        g_left_was_down=(GetAsyncKeyState(0x01)&0x8000)!=0;
        *wf_input_handled()=true;return;
    }
    if(g_entry.length&&(ui_IsPopupOpen(0,384)||(g_entry_rect_valid&&in_rect(mouse,g_entry_rect_min,g_entry_rect_max)))){
        *wf_input_handled()=true;return; // clicks on the unit selector must never tune the VFO
    }
    bool overScale=in_rect(mouse,args.freqScaleRectMin,args.freqScaleRectMax);
    if(v34_context_open()){*wf_input_handled()=true;return;}
    bool edgeHover=selected_vfo_edge_hover(mouse);
    bool bodyHover=selected_vfo_body_hover(mouse);
    bool passbandHover=selected_vfo_passband_hit(args,mouse);

    // Claim the selected VFO over the ENTIRE filter bandwidth, not merely the
    // red carrier line. Resize handles take precedence, keeping bandwidth
    // adjustments identical to native SDR++. Click without movement is inert.
    if(leftDown&&!g_left_was_down&&passbandHover&&!edgeHover){
        PassbandDrag&d=g_passband_drag;
        d.captured=true;d.dragging=false;d.pending=false;d.finalPending=false;
        d.mouseStartX=mouse.x;d.initialHz=selected_frequency();
        d.hzPerPixel=args.pixelToFreqRatio>0.0?args.pixelToFreqRatio:1.0;
        d.snapHz=vfo_snap(selected_vfo());d.targetHz=d.initialHz;
        d.lastRetune=-100.0;
        c_copy(d.vfoName,sizeof(d.vfoName),string_chars(wf_selected_name()));
        g_pan_candidate=g_panning=g_native_gesture=g_native_resize=false;
        g_fixed_pan_pending=g_fixed_pan_force=false;
        g_left_was_down=true;
        *wf_input_handled()=true;
        return;
    }

    // Decide ownership only on the initial mouse-down. Red VFO body and bandwidth grips
    // always stay native SDR++ gestures. Empty FFT/waterfall background becomes our pan gesture.
    if(leftDown && !g_left_was_down && (overFFT||overWF||(g_tuning_mode==2&&overScale))){
        g_fixed_pan_pending=false;g_fixed_pan_force=false;
        g_pan_started_on_marker=bodyHover;
        if(edgeHover){
            g_native_gesture=true; g_native_resize=true;
            g_pan_candidate=false; g_panning=false;
        }
        else if(g_tuning_mode==1){
            // Centre keeps SDR++ native pointer processing. Only Free/Fixed
            // spectrum background pans are owned by Essential.
            g_native_gesture=true; g_native_resize=false;
            g_pan_candidate=false; g_panning=false;
        }
        else{
            g_native_gesture=false; g_native_resize=false;
            g_pan_candidate=true; g_panning=false; g_pan_start_mouse=mouse;
            g_pan_start_offset=wf_getViewOffset(core_waterfall);
            g_pan_start_center=wf_getCenterFrequency(core_waterfall);
            g_pan_locked_hz=selected_frequency();
            g_pan_hz_per_pixel=args.pixelToFreqRatio>0.0?args.pixelToFreqRatio:1.0;
            c_copy(g_pan_vfo_name,sizeof(g_pan_vfo_name),string_chars(wf_selected_name()));
            g_pan_last_hardware_change=-100.0;
            float w=args.fftRectMax.x-args.fftRectMin.x;
            float f=w>1.0f?clamp01((mouse.x-args.fftRectMin.x)/w):0.5f;
            g_click_frequency=args.lowFreq+(args.highFreq-args.lowFreq)*(double)f;
        }
    }

    // The green preview disappears immediately over a bandwidth grip and remains hidden
    // throughout a native bandwidth-resize drag, even if the mouse leaves the narrow grip.
    // Markers/tooltips are rendered by EndFramePre, after native tooltip and VFO geometry updates.

    if(leftDown && g_pan_candidate && !g_native_gesture){
        float dx=mouse.x-g_pan_start_mouse.x;
        if(!g_panning && dabs((double)dx)>=PAN_THRESHOLD)g_panning=true;
        if(g_panning){
            if(g_tuning_mode==0){
                queue_free_pan((double)dx,false);
            }else if(g_tuning_mode==2){
                // SDR# pinned mode: pan immediately, keep the marker at the
                // original Hz until it hits the visible edge, THEN edge-tune.
                // Hardware moves are deferred outside the waterfall lock.
                queue_fixed_pan((double)dx,false);
            }else{
                const double newOffset=g_pan_start_offset-(double)dx*args.pixelToFreqRatio;
                wf_setViewOffset(core_waterfall,newOffset);
            }
        }
        // Background gestures must be blocked immediately so native SDR++ doesn't move
        // the red VFO before we know whether this becomes a click or a pan.
        *wf_input_handled()=true;
    }

    if(!leftDown && g_left_was_down){
        if(g_pan_candidate && !g_native_gesture){
            // Essential owns the release too: native SDR++ must not interpret
            // the mouse-up as a second click-to-tune operation.
            *wf_input_handled()=true;
            if(!g_panning){
                // Clicking the existing red VFO body must NOT alter its Hz.
                // Empty-background clicks are deliberate click-to-tune actions.
                void*v=selected_vfo();
                if(v && !(g_tuning_mode==2 && g_pan_started_on_marker)){
                    double snap=vfo_snap(v);if(snap<0.000001)snap=1.0;
                    double freq=round_nearest(g_click_frequency/snap)*snap;
                    CoreString name;core_stringCopy(&name,wf_selected_name());
                    manager_setOffset(core_vfo_manager,&name,freq-wf_getCenterFrequency(core_waterfall));
                    if(g_tuning_mode==2)g_fixed_marker_hz=freq;
                }
            } else if(g_tuning_mode==0){
                queue_free_pan((double)(mouse.x-g_pan_start_mouse.x),true);
            } else if(g_tuning_mode==2){
                // Finish at the ACTUAL mouse-up position; do not snap back to
                // the frozen display center from mouse-down.
                queue_fixed_pan((double)(mouse.x-g_pan_start_mouse.x),true);
            }
        }
        g_pan_candidate=false; g_panning=false;g_pan_started_on_marker=false;
        g_native_gesture=false; g_native_resize=false;
    }
    g_left_was_down=leftDown;

    float wheel=get_mouse_wheel();if(wheel==0.0f)return;
    // Wheel on the selected receive passband adjusts the native Radio bandwidth.
    // Defer the actual radio call to NewFramePost, outside the WaterFall lock.
    const bool ctrlWheel=(GetAsyncKeyState(0x11)&0x8000)!=0;
    if(!ctrlWheel && (overFFT||overWF) && passbandHover && radio_interface_ready()) {
        // 0.1 kHz per tick adjusts ONLY the demodulator filter bandwidth.
        g_filter_wheel_pending += 100.0*(double)wheel;
        if(g_filter_wheel_pending>5000000.0)g_filter_wheel_pending=5000000.0;
        if(g_filter_wheel_pending< -5000000.0)g_filter_wheel_pending=-5000000.0;
        // Consume the wheel delta, NOT the entire waterfall input event.
        // inputHandled=true makes native WaterFall skip the grey VFO passband
        // on wheel frames, producing the on/off flash shown in the V39 video.
        // The native renderer still needs this frame; wheel is already zero,
        // so SDR++ cannot interpret it as its own zoom command.
        clear_mouse_wheel();return;
    }
    if(!ctrlWheel)return;
    if(!overFFT&&!overWF&&!overScale)return;
    clear_mouse_wheel();

    if(args.fftRectMax.x-args.fftRectMin.x<=1.0f)return;
    CursorPreview preview=make_preview(args,mouse);
    int steps=(int)(wheel>0.0f?wheel:-wheel);if(steps<1)steps=1;if(steps>8)steps=8;
    zoom_view(wheel>0?1:-1,steps,preview.freq,preview.frac);*wf_input_handled()=true;
}

static InputEventHandler g_handler={input_callback,0};static bool g_bound=false;
static bool bind_input(){if(g_bound)return true;RawVector*v=wf_input_vec();usize n=(v->first&&v->last)?(usize)(v->last-v->first):0;usize cap=(v->first&&v->cap)?(usize)(v->cap-v->first):0;if(n>=cap){usize nc=cap?cap*2:1;InputEventHandler**p=(InputEventHandler**)malloc(nc*sizeof(InputEventHandler*));if(!p)return false;for(usize i=0;i<n;i++)p[i]=v->first[i];if(v->first)free(v->first);v->first=p;v->last=p+n;v->cap=p+nc;}*v->last++=&g_handler;g_bound=true;return true;}
static void unbind_input(){if(!g_bound)return;RawVector*v=wf_input_vec();if(v->first&&v->last){for(InputEventHandler**p=v->first;p<v->last;p++){if(*p==&g_handler){for(InputEventHandler**q=p;q+1<v->last;q++)*q=*(q+1);v->last--;break;}}}g_bound=false;}

class DummyInstance:public ModuleManager::Instance{
public:
    DummyInstance(){g_enabled=true;}
    ~DummyInstance()override{set_enabled(false);unbind_menu();if(g_config_dirty||g_settings_dirty)save_all_config();clear_entry();cancel_shortcut_ownership();restore_keys();}
    void postInit()override{if(g_supported)bind_keyboard();}
    void enable()override{set_enabled(true);}
    void disable()override{set_enabled(false);}
    bool isEnabled()override{return g_enabled&&g_supported;}
};
__declspec(align(16)) static u8 g_dummy_buf[128];static DummyInstance* g_dummy=0;
#ifndef ESSENTIAL_TEST
void* operator new(usize,void*p) noexcept{return p;}void operator delete(void*) noexcept{}void operator delete(void*,usize) noexcept{}
#endif
extern "C" { int _fltused=0; }

MOD_EXPORT const ModuleManager::ModuleInfo_t _INFO_={"Essential++","LCD frequency toolbar, bookmarks, localisation and shortcuts","TheMisterEQ / OpenAI",40,0,0,0};
MOD_EXPORT void _INIT_(){
    g_supported=check_core_abi();
    if(!g_supported){MessageBoxA(0,"Essential V39 requires SDR++ Windows x64 1.3.2 nightly (Nov 18, 2026).","Essential",0x30);return;}
    load_presets();
    if(g_tuning_mode>=0){
        *(int*)(core_main_window+0xFC)=g_tuning_mode==1?0:1;
        core_waterfall[0x71]=(g_tuning_mode==1)?1:0;
    }
    if(!g_speed_custom){
        int initial=core_display_fft_rate;
        if(initial<1)initial=1;if(initial>60)initial=60;
        g_speed=initial;g_speed_slider=(float)initial;g_speed_pending=false;
    }
    if(!g_dummy)g_dummy=new((void*)g_dummy_buf) DummyInstance();
    g_essential_menu_instance=g_dummy;
    bind_menu();bind_input();bind_keyboard();sync_zoom_slider();
    install_toolbar_hooks();install_audio_meter();install_panel_hooks();
    // Restore the saved custom FFT rate once the host's initial setup settles.
    if(g_speed_custom){g_speed_pending=true;g_speed_apply_after=ui_GetTime()+0.5;}
    g_contrast_needs_apply=true;
}
MOD_EXPORT ModuleManager::Instance* _CREATE_INSTANCE_(void*){
    if(!g_dummy)g_dummy=new((void*)g_dummy_buf) DummyInstance();
    return g_dummy;
}
MOD_EXPORT void _DELETE_INSTANCE_(ModuleManager::Instance*){if(g_dummy){g_dummy->~DummyInstance();g_dummy=0;}}
MOD_EXPORT void _END_(){
    v39_restore_embedded_editor_hooks();
    // Ensure that unloading the module restores native SDR++ waterfall levels.
    restore_native_waterfall_levels();
    g_fixed_pan_pending=false;g_fixed_pan_force=false;
    g_free_pan_pending=g_free_pan_force=false;
    g_enabled=false;
    idle_audio_meter();
    if(!restore_panel_hooks()){void*module=0;GetModuleHandleExA(5,(const char*)patched_waterfall_max,&module);}
    if(!restore_toolbar_hooks()){
        // A protection failure must not leave an unloaded DLL as a live CALL
        // target. Pin it until process exit; the native code is still restored
        // on the next clean SDR++ start without this module.
        void*module=0;GetModuleHandleExA(5,(const char*)hook_toolbar_volume,&module);
    }
    restore_audio_meter();
    history_reset();finish_source_request();restore_source_imports();
    if(g_config_dirty||g_settings_dirty)save_all_config();
    unbind_menu();unbind_keyboard();if(g_bound)unbind_input();g_supported=false;
}
