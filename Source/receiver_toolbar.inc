// Verified call sites extend the native toolbar and digit rendering in ImGui
// window. Frequency editing, tuning-mode button and SNR meter remain native.
// The verified logo call is suppressed only while Essential is enabled.
// Only the loaded core's call operands change; original bytes are restored on unload.

struct StereoSample { float left,right; };
static void* g_audio_target=0;
static u64 g_audio_peak_state=0;
static const u64 AUDIO_EPOCH_MASK=0xFFFFFFFE00000000ull;
static const u64 AUDIO_PACKET=0x100000000ull;
static void** g_audio_run_slot=0;
static bool g_audio_hooked=false;
struct AudioRunHook { void**slot;void*original;void*owner;const char*module; };
static AudioRunHook g_audio_hooks[2]={};
static int g_audio_hook_count=0;
static float g_rf_needle=0,g_audio_needle=0;
static double g_meter_time=0;
static float g_last_audio_db=-120;
static double g_audio_seen=0;

static void idle_audio_meter(){__atomic_store_n(&g_audio_target,(void*)0,__ATOMIC_RELEASE);}
static void ensure_audio_volume_hook(void*volume);

static int text_compare(const char*a,const char*b){
    while(*a&&*a==*b){a++;b++;}return (int)(u8)*a-(int)(u8)*b;
}
// MSVC map node layout: links at 0/8/16, nil at 25, string key at 32, pointer at 64.
// SinkManager::streams is at 0xA0, confirmed by bindStream/saveStreamConfig.
static void* selected_audio_stream(){
    u8*head=*(u8**)(core_sink_manager+0xA0);if(!head)return 0;
    u8*node=*(u8**)(head+8);const char*name=string_chars(wf_selected_name());
    for(int limit=0;node&&node!=head&&!node[25]&&limit<128;limit++){
        int comparison=text_compare(name,string_chars(node+32));
        if(!comparison)return *(void**)(node+64);
        node=*(u8**)(node+(comparison<0?0:16));
    }
    return 0;
}
// The native Mute button uses Volume::getMuted/setMuted and persists the
// selected stream through ConfigManager. Use those exact same APIs for M.
static bool native_toggle_selected_mute(){
    void*stream=selected_audio_stream();
    if(!stream)return false;
    // SinkManager::Stream::volumeAjust, verified on the matching core build.
    void*volume=(u8*)stream+0x250;
    bool old=audio_getMuted(volume);
    audio_setMuted(volume,!old);
    config_acquire(core_config_manager);
    CoreString streamName;
    core_stringCopy(&streamName,wf_selected_name());
    sink_saveStreamConfig(core_sink_manager,&streamName);
    config_release(core_config_manager,true);
    return true;
}
static void publish_audio_target(){
    void*stream=selected_audio_stream();void*target=stream?(u8*)stream+0x250:0;
    if(target)ensure_audio_volume_hook(target);
    if(__atomic_load_n(&g_audio_target,__ATOMIC_ACQUIRE)!=target){
        // Publish a new generation while no target is visible. A finishing packet
        // from the previous receiver cannot enter this receiver's meter.
        __atomic_store_n(&g_audio_target,(void*)0,__ATOMIC_RELEASE);
        u64 previous=__atomic_load_n(&g_audio_peak_state,__ATOMIC_ACQUIRE);
        __atomic_store_n(&g_audio_peak_state,((previous&AUDIO_EPOCH_MASK)+(1ull<<33))&AUDIO_EPOCH_MASK,__ATOMIC_RELEASE);
        __atomic_store_n(&g_audio_target,target,__ATOMIC_RELEASE);
        g_audio_needle=0;g_last_audio_db=-120;g_audio_seen=0;
    }
}
static void measure_audio_packet(void*volume,const StereoSample*samples,int count){
    u64 epoch=__atomic_load_n(&g_audio_peak_state,__ATOMIC_ACQUIRE)&AUDIO_EPOCH_MASK;
    if(volume!=__atomic_load_n(&g_audio_target,__ATOMIC_ACQUIRE)||!samples||count<=0||count>1000000)return;
    float peak=0;
    for(int i=0;i<count;i++){
        float a=samples[i].left,b=samples[i].right;if(a<0)a=-a;if(b<0)b=-b;
        if(a>peak&&a<1e8f)peak=a;if(b>peak&&b<1e8f)peak=b;
    }
    if(peak>16)peak=16;union { float value;u32 bits; }v={peak};
    u64 previous=__atomic_load_n(&g_audio_peak_state,__ATOMIC_ACQUIRE);
    while((previous&AUDIO_EPOCH_MASK)==epoch){
        u32 oldPeak=(u32)previous;u64 next=epoch|AUDIO_PACKET|(oldPeak>v.bits?oldPeak:v.bits);
        if(__atomic_compare_exchange_n(&g_audio_peak_state,&previous,next,false,__ATOMIC_RELEASE,__ATOMIC_ACQUIRE))break;
    }
}
static int hook_audio_volume_run(void*volume){
    // Radio constructs its own Volume vtable/run() in radio.dll. Calling the
    // core copy unconditionally misses that worker; retain each native run.
    void**slot=*(void***)volume+3;
    int(*run)(void*)=audio_volume_run;
    int hooks=__atomic_load_n(&g_audio_hook_count,__ATOMIC_ACQUIRE);
    for(int i=0;i<hooks;i++)if(g_audio_hooks[i].slot==slot){run=(int(*)(void*))g_audio_hooks[i].original;break;}
    int count=run(volume);
    // Volume::out is at 0xA8; after successful swap its completed samples are in
    // readBuf at 0xB8. This worker cannot overwrite them until its next run().
    if(count>0)measure_audio_packet(volume,*(const StereoSample**)((u8*)volume+0xB8),count);
    return count;
}
static bool add_audio_run_hook(void**slot,void*original,void*owner,const char*module){
    int count=__atomic_load_n(&g_audio_hook_count,__ATOMIC_ACQUIRE);
    for(int i=0;i<count;i++)if(g_audio_hooks[i].slot==slot)return true;
    if(count>=2||*slot!=original)return false;
    g_audio_hooks[count]={slot,original,owner,module};
    __atomic_store_n(&g_audio_hook_count,count+1,__ATOMIC_RELEASE);
    if(!replace_import(slot,(void*)hook_audio_volume_run)){
        __atomic_store_n(&g_audio_hook_count,count,__ATOMIC_RELEASE);return false;
    }
    g_audio_hooked=true;return true;
}
static void ensure_audio_volume_hook(void*volume){
    void**table=*(void***)volume;if(!table)return;
    if(table[3]==(void*)hook_audio_volume_run)return;
    u8*radio=(u8*)GetModuleHandleA("radio.dll");if(!radio)return;
    // Verified Radio module from SDR++ 1.3.2 nightly (2026-11-18).
    // Volume vtable at 0x2E450, run at 0x1C260. Exact module checks below.
    if(*(unsigned short*)radio!=0x5A4D)return;
    u32 pe=*(u32*)(radio+0x3C);
    if(pe>=4096||*(u32*)(radio+pe)!=0x4550||*(u32*)(radio+pe+8)!=0x6AFE1732||
       *(u32*)(radio+pe+24+56)!=0x42000||table!=(void**)(radio+0x2E450))return;
    add_audio_run_hook(table+3,(void*)(radio+0x1C260),radio,"radio.dll");
}
static bool install_audio_meter(){
    if(g_audio_hooked)return true;
    void*module=GetModuleHandleA("sdrpp_core.dll");if(!module)return false;
    void**table=(void**)GetProcAddress(module,"??_7Volume@audio@dsp@@6B@");
    if(!table||table[3]!=(void*)audio_volume_run)return false;
    g_audio_run_slot=table+3;
    if(!add_audio_run_hook(g_audio_run_slot,(void*)audio_volume_run,module,"sdrpp_core.dll")){g_audio_run_slot=0;return false;}
    return true;
}
static void collect_streams(u8*node,u8*head,void**out,int&count,int capacity,int depth=0){
    if(!node||node==head||node[25]||depth>64||count>=capacity)return;
    collect_streams(*(u8**)node,head,out,count,capacity,depth+1);
    if(count<capacity)out[count++]=*(void**)(node+64);
    collect_streams(*(u8**)(node+16),head,out,count,capacity,depth+1);
}
static void restore_audio_meter(){
    if(!g_audio_hooked)return;
    // Stop/join running sink workers before restoring the shared run vtable, so
    // no thread can still return through this DLL when it is unloaded.
    int capacity=(int)*(u64*)(core_sink_manager+0xA8);if(capacity<1)capacity=1;
    void**streams=(void**)malloc((usize)capacity*(sizeof(void*)+1));
    if(!streams){
        // If workers cannot be joined during an out-of-memory unload, keep this
        // module mapped. It is safer than leaving worker return addresses unmapped.
        void*module=0;GetModuleHandleExA(5,(const char*)hook_audio_volume_run,&module);return;
    }
    bool*running=(bool*)(streams+capacity);int count=0;
    u8*head=*(u8**)(core_sink_manager+0xA0);
    if(head)collect_streams(*(u8**)(head+8),head,streams,count,capacity);
    for(int i=0;i<count;i++){
        running[i]=streams[i]&&*((u8*)streams[i]+0x4D8);
        if(running[i])audio_stream_stop(streams[i]);
    }
    int hooks=__atomic_load_n(&g_audio_hook_count,__ATOMIC_ACQUIRE);
    for(int i=0;i<hooks;i++){
        AudioRunHook&hook=g_audio_hooks[i];
        if(GetModuleHandleA(hook.module)==hook.owner&&*hook.slot==(void*)hook_audio_volume_run)
            replace_import(hook.slot,hook.original);
    }
    __atomic_store_n(&g_audio_hook_count,0,__ATOMIC_RELEASE);
    __atomic_store_n(&g_audio_target,(void*)0,__ATOMIC_RELEASE);
    for(int i=0;i<count;i++)if(running[i])audio_stream_start(streams[i]);
    free(streams);g_audio_hooked=false;g_audio_run_slot=0;
}
static float amplitude_db(float amplitude){
    if(!(amplitude>0))return -120;
    union { float value;u32 bits; }v={amplitude};
    int exponent=(int)((v.bits>>23)&255)-127;
    v.bits=(v.bits&0x7FFFFFu)|(127u<<23);
    // log(m) via the rapidly converging atanh series, m in [1,2).
    double t=(v.value-1.0)/(v.value+1.0),power=t,sum=t;
    for(int n=3;n<=13;n+=2){power*=t*t;sum+=power/n;}
    float db=(float)(6.020599913279624*exponent+17.37177927613007*sum);
    return db<-120?-120:db;
}
struct MeterReading { float rfDb,audioDb;bool rfValid,audioValid; };
static MeterReading update_meter_reading(){
    publish_audio_target();MeterReading result={-120,-120,false,false};double now=ui_GetTime();
    u64 samples=__atomic_load_n(&g_audio_peak_state,__ATOMIC_ACQUIRE);
    while(!__atomic_compare_exchange_n(&g_audio_peak_state,&samples,samples&AUDIO_EPOCH_MASK,false,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE)){}
    if(window_isPlaying(core_main_window)&&selected_vfo()){
        double span=g_last_args.highFreq-g_last_args.lowFreq;
        if(g_args_frame>=ui_GetFrameCount()-1&&span>0){float fraction=(float)((selected_frequency()-g_last_args.lowFreq)/span);
            if(fraction>=0&&fraction<=1)result.rfValid=signal_at_fraction(fraction,result.rfDb)&&result.rfDb>=-1200&&result.rfDb<=1200;
        }
        if(samples&AUDIO_PACKET){
            union { u32 bits;float value; }peak={(u32)samples};g_last_audio_db=amplitude_db(peak.value);g_audio_seen=now;
        }
        result.audioValid=__atomic_load_n(&g_audio_target,__ATOMIC_ACQUIRE)&&g_audio_seen>0&&now-g_audio_seen<0.5;
        if(result.audioValid)result.audioDb=g_last_audio_db;
    }else{g_audio_seen=0;g_last_audio_db=-120;}
    double dt=now-g_meter_time;if(dt<=0||dt>0.5)dt=1.0/60;g_meter_time=now;
    float rfTarget=result.rfValid?clamp01((result.rfDb+120)/120):0;
    float audioTarget=result.audioValid?clamp01((result.audioDb+60)/60):0;
    g_rf_needle+=(rfTarget-g_rf_needle)*(float)(dt/(dt+(rfTarget>g_rf_needle?0.04:0.28)));
    g_audio_needle+=(audioTarget-g_audio_needle)*(float)(dt/(dt+(audioTarget>g_audio_needle?0.035:0.30)));
    return result;
}
static float meter_sin(float x){
    float x2=x*x;return x*(1-x2/6+x2*x2/120-x2*x2*x2/5040+x2*x2*x2*x2/362880-x2*x2*x2*x2*x2/39916800);
}
static float meter_cos(float x){
    float x2=x*x;return 1-x2/2+x2*x2/24-x2*x2*x2/720+x2*x2*x2*x2/40320-x2*x2*x2*x2*x2/3628800;
}
static ImVec2 dial_point(const ImVec2&pos,float width,float height,float fraction,float length){
    float angle=-2.70f+2.26f*fraction;
    return ImVec2(pos.x+width*0.5f+meter_cos(angle)*width*0.47f*length,
                  pos.y+height*0.96f+meter_sin(angle)*height*0.67f*length);
}
static void draw_needle_meter(const char*label,float needle,float db,bool valid,float width,float scale){
    ImVec2 pos=ui_GetCursorScreenPos();float height=40*scale;ImDrawList*dl=ui_GetWindowDrawList();
    draw_AddRectFilled(dl,pos,ImVec2(pos.x+width,pos.y+height),lcd_palette().frame,scale,0);
    draw_AddRectFilled(dl,ImVec2(pos.x+scale,pos.y+scale),ImVec2(pos.x+width-scale,pos.y+height-scale),0xFFCCDED5u,0,0);
    ImVec2 previous=dial_point(pos,width,height,0,1);
    for(int i=1;i<=30;i++){
        float f=(float)i/30;ImVec2 point=dial_point(pos,width,height,f,1);
        draw_AddLine(dl,previous,point,i>24?0xFF4D4BCCu:lcd_palette().rim,scale);previous=point;
    }
    for(int i=0;i<=10;i++){
        float f=(float)i/10;draw_AddLine(dl,dial_point(pos,width,height,f,1),dial_point(pos,width,height,f,i%5?0.87f:0.78f),
                                     i>8?0xFF4D4BCCu:lcd_palette().rim,scale);
    }
    draw_AddLine(dl,ImVec2(pos.x+width*0.5f,pos.y+height*0.96f),dial_point(pos,width,height,clamp01(needle),0.86f),0xFF3D3CBCu,1.6f*scale);
    draw_AddTextSized(dl,0,10*scale,ImVec2(pos.x+5*scale,pos.y+2*scale),lcd_palette().divider,label,0,0,0);
    char value[40];if(valid){gain_number(db,true,false,value,sizeof(value));c_append(value,sizeof(value)," dBFS");}else c_copy(value,sizeof(value),"-- dBFS");
    ImVec2 valueSize=ui_CalcTextSize(value,0,false,-1);
    float font=ui_GetFontSize();if(!(font>0))font=13*scale;
    float textWidth=valueSize.x*(10*scale)/font;
    if(width>=96*scale)draw_AddTextSized(dl,0,10*scale,ImVec2(pos.x+width-textWidth-5*scale,pos.y+2*scale),lcd_palette().divider,value,0,0,0);
    ui_Dummy(ImVec2(width,height));
}
static void toolbar_tooltip(const char*text){if(ui_IsItemHovered(0)){ui_BeginTooltip();ui_TextUnformatted(text,0);ui_EndTooltip();}}
static float toolbar_scale(){float scale=core_ui_scale;return scale>=0.5f&&scale<=4?scale:1;}
static ImVec2 g_volume_toolbar_origin={};static float g_volume_toolbar_width=0;static int g_volume_toolbar_frame=-1;
static float toolbar_volume_width(float original){
    if(!g_supported||!g_enabled)return original;
    float scale=toolbar_scale(),window=ui_GetWindowSize().x/scale;
    float limit=(window>=2000?248:window>=1600?168:window>=1300?128:100)*scale;
    return original<limit?original:limit;
}
static void hook_toolbar_volume(void*manager,void*name,void*prefix,float width,float height,int border,bool sameLine){
    // When Essential is disabled, forward every original argument unchanged.
    // In particular, neither width, icon height, style nor slider Y position
    // may be modified in the native SDR++ toolbar.
    if(!g_supported||!g_enabled){
        g_volume_toolbar_frame=-1;g_volume_toolbar_width=0;
        sink_showVolumeSlider(manager,name,prefix,width,height,border,sameLine);
        return;
    }
    // Both native by-value strings are forwarded once; the core owns their
    // destruction. Only the slider's width changes with available space.
    float w=toolbar_volume_width(width);
    g_volume_toolbar_origin=ui_GetCursorPos();g_volume_toolbar_width=w;g_volume_toolbar_frame=ui_GetFrameCount();
    // The native SinkManager function draws BOTH the Mute button and Volume
    // slider. Its height argument is ALSO used as the Mute icon size. Passing
    // 15px here (V17-V21) therefore shrunk Mute instead of only the slider.
    // Keep Mute at the same 32px icon size as the built-in Play button.
    float scale=toolbar_scale();
    ui_PushStyleVarVec(11,ImVec2(4*scale,0));
    sink_showVolumeSlider(manager,name,prefix,w,32*scale,border,sameLine);
    ui_PopStyleVar(1);
    // Native Volume is already a single ImGui item; show its current role
    // without putting a label on top of the slider itself.
    // The selected native Stream stores the live linear volume at +0x4DC
    // (the same float passed by SDR++ to ImGui::SliderFloat). Reading it
    // after the native slider call makes the percentage update every frame,
    // including while the user drags the knob.
    char volumeTip[64];c_copy(volumeTip,sizeof(volumeTip),l10n("Volume: "));
    void*stream=selected_audio_stream();
    if(stream){
        float level=*(float*)((u8*)stream+0x4DC);
        if(level>=0.0f&&level<=1.0f){
            int percent=(int)(level*100.0f+0.5f);
            char digits[16];u64str((u64)percent,digits,sizeof(digits));
            c_append(volumeTip,sizeof(volumeTip),digits);
            c_append(volumeTip,sizeof(volumeTip),"%");
        }else c_append(volumeTip,sizeof(volumeTip),"--");
    }else c_append(volumeTip,sizeof(volumeTip),"--");
    toolbar_tooltip(volumeTip);
}
// Called ONLY from the 2 native SinkManager Volume Slider callsites.
// Preserve the 32px Mute button while lifting only its adjacent slider.
static void hook_volume_slider_vertical(float nativeY){
    ui_SetCursorPosY((g_supported&&g_enabled)?nativeY-7*toolbar_scale():nativeY);
}
struct FrequencyMetrics { int digitWidth,dotWidth,digitHeight;float glyphHeight; };
static FrequencyMetrics frequency_metrics(){
    static FrequencyMetrics metrics={};static int frame=-1;static float scale=0;static void*font=0;
    if(frame!=ui_GetFrameCount()||scale!=core_ui_scale||font!=core_big_font){
        ui_PushFont(core_big_font);ImVec2 digit=ui_CalcTextSize("0",0,false,-1),dot=ui_CalcTextSize(".",0,false,-1);ui_PopFont();
        metrics={(int)digit.x,(int)dot.x,(int)digit.y,digit.y};frame=ui_GetFrameCount();scale=core_ui_scale;font=core_big_font;
    }
    return metrics;
}
static u32 server_text_color(){
    // Connection state keeps semantic colours; the *connected* IP/port LCD
    // follows the user-selected five-colour Essential palette.
    if(g_server.status==SERVER_CONNECTING)return 0xFF66DCF3u;
    if(g_server.status==SERVER_ERROR)return 0xFF8080FFu;
    if(g_server.editing||g_server.dirty)return lcd_palette().dim;
    if(g_server.status==SERVER_CONNECTED)return lcd_palette().glow;
    return lcd_palette().dim;
}
// Two-line Source controls: compact Connect + MSPS on top, full-width
// receiver selector below. The dropdown uses the same deferred hardware
// transition as the Essential sidebar and the native device combo.
static float toolbar_rate_width(float scale){
    return (ui_GetWindowSize().x<1600*scale?80:96)*scale;
}
static void toolbar_rate(float scale,const ImVec2&origin,float top=8){
    bool valid=g_rate.valid&&c_equal(g_rate.source,string_chars(core_selected_source));
    int selected=valid?g_rate.selected:0;
    ui_SetCursorPos(ImVec2(ui_GetCursorPos().x,origin.y+top*scale));
    ui_SetNextItemWidth(toolbar_rate_width(scale));
    ui_BeginDisabled(*(bool*)core_main_window||!valid);
    if(ui_Combo("##essential_toolbar_msps",&selected,valid?g_rate.items:"--\0",-1)&&valid)request_source_rate(selected);
    ui_EndDisabled();toolbar_tooltip("MSPS: czestotliwosc probkowania aktywnego Source");
    ui_SameLine(0,6*scale);ui_SetCursorPos(ImVec2(ui_GetCursorPos().x,origin.y));
}
static void toolbar_device_dropdown(const ImVec2&pos,float width){
    bool valid=g_device.valid&&g_device.count>0&&
        c_equal(g_device.source,string_chars(core_selected_source));
    int selected=valid?g_device.selected:0;
    ui_SetCursorPos(pos);ui_SetNextItemWidth(width);
    // Switching while playing is intentionally allowed; the deferred request
    // stops the stream, selects the receiver and resumes only if needed.
    ui_BeginDisabled(*(bool*)core_main_window||!valid||g_deferred_device.ready||g_source_request.kind!=0);
    if(ui_Combo("##essential_toolbar_device",&selected,valid?g_device.items:"--\0",-1)&&
       valid&&selected>=0&&selected<g_device.count&&selected!=g_device.selected)
        defer_device_change(g_device.key,selected);
    ui_EndDisabled();
    toolbar_tooltip(valid?g_device.description:"Wybrane Source nie udostepnia listy odbiornikow SDR");
}
static void toolbar_local_device(float scale,const ImVec2&origin){
    const char*source=string_chars(core_selected_source);
    const char*device=c_equal(g_device.source,source)&&g_device.description[0]?g_device.description:source;
    float width=(ui_GetWindowSize().x>=1600*scale?184:128)*scale;
    ImVec2 pos=ui_GetCursorScreenPos();pos.y+=8*scale;ImDrawList*dl=ui_GetWindowDrawList();
    draw_lcd_glass(dl,pos,ImVec2(pos.x+width,pos.y+27*scale),scale);
    float clip[4]={pos.x+5*scale,pos.y,pos.x+width-5*scale,pos.y+27*scale};
    draw_AddTextSized(dl,0,13*scale,ImVec2(pos.x+6*scale,pos.y+5*scale),lcd_palette().ink,device,0,0,clip);
    ui_SetCursorPos(ImVec2(origin.x,origin.y+8*scale));ui_Dummy(ImVec2(width,27*scale));toolbar_tooltip(device);
    ui_SameLine(0,8*scale);ui_SetCursorPos(ImVec2(ui_GetCursorPos().x,origin.y));
}
static void toolbar_local_source_controls(float scale,const ImVec2&origin){
    toolbar_local_device(scale,origin);
    float x=ui_GetCursorPos().x;
    float width=(toolbar_rate_width(scale)>96*scale?toolbar_rate_width(scale):96*scale);
    // The local Source also gets a receiver selector under its sample rate.
    ui_SetCursorPos(ImVec2(x,origin.y));ui_Dummy(ImVec2(width,51*scale));
    ui_SetCursorPos(ImVec2(x,origin.y));toolbar_rate(scale,origin,1);
    toolbar_device_dropdown(ImVec2(x,origin.y+28*scale),width);
    ui_SameLine(0,6*scale);ui_SetCursorPos(ImVec2(ui_GetCursorPos().x,origin.y));
}
static void toolbar_server(){
    refresh_gain_reading();server_prepare_source();server_observe_transport();server_sync_address();float scale=toolbar_scale();ImVec2 origin=ui_GetCursorPos();
    if(!server_selected()){toolbar_local_source_controls(scale,origin);return;}
    server_sample_download();bool connected=g_server.observed&&g_server.connected,locked=*(bool*)core_main_window;
    bool compact=ui_GetWindowSize().x<1600*scale;
    float fieldWidth=ui_CalcTextSize(g_server.address,0,false,-1).x+16*scale;
    float minField=(connected?96:128)*scale,maxField=(compact?184:224)*scale;
    if(fieldWidth<minField)fieldWidth=minField;if(fieldWidth>maxField)fieldWidth=maxField;
    const double values[]={g_server.download,g_server.upload};char lines[2][64];float textWidths[2];
    float font=ui_GetFontSize();if(!(font>0))font=16*scale;
    for(int i=0;i<2;i++){
        if(!connected||!g_server.transferReady)c_copy(lines[i],sizeof(lines[i]),"--");
        else{
            double bits=values[i]*8;bool mega=bits>=1048576;char value[32];
            gain_number(bits/(mega?1048576:1024),true,false,value,sizeof(value));
            c_copy(lines[i],sizeof(lines[i]),value);c_append(lines[i],sizeof(lines[i]),mega?" Mbit/s":" kbit/s");
        }
        textWidths[i]=ui_CalcTextSize(lines[i],0,false,-1).x*(13*scale)/font;
    }
    float widerText=textWidths[0]>textWidths[1]?textWidths[0]:textWidths[1];
    float rowWidth=2*widerText+40*scale;
    if(fieldWidth<rowWidth)fieldWidth=rowWidth;
    ImVec2 p=ui_GetCursorScreenPos();ImDrawList*dl=ui_GetWindowDrawList();
    draw_lcd_glass(dl,p,ImVec2(p.x+fieldWidth,p.y+44*scale),scale);
    // The complete LCD chassis is one click target. Editing is done in a
    // separate two-field popup, never inline while the stream is running.
    const char*address=g_server.address[0]?g_server.address:"IP:port";
    float addressClip[4]={p.x+6*scale,p.y+2*scale,p.x+fieldWidth-6*scale,p.y+23*scale};
    draw_AddTextSized(dl,0,13*scale,ImVec2(p.x+6*scale,p.y+4*scale),server_text_color(),address,0,0,addressClip);
    for(int i=0;i<2;i++){
        // Vector arrows do not require extra glyphs in the user's font atlas.
        float middle=p.x+fieldWidth*0.5f;
        float x=middle+(i?5:-5)*scale,y=p.y+31*scale,direction=i?-1.0f:1.0f;
        ImVec2 tip(x,y+4*scale*direction);
        draw_AddLine(dl,ImVec2(x,y-4*scale*direction),tip,lcd_palette().ink,1.4f*scale);
        draw_AddLine(dl,ImVec2(x-3*scale,y+scale*direction),tip,lcd_palette().ink,1.4f*scale);
        draw_AddLine(dl,ImVec2(x+3*scale,y+scale*direction),tip,lcd_palette().ink,1.4f*scale);
        float textX=i?middle+14*scale:middle-14*scale-textWidths[0];
        float clip[4]={textX,p.y+23*scale,textX+textWidths[i]+scale,p.y+41*scale};
        draw_AddTextSized(dl,0,13*scale,ImVec2(textX,p.y+25*scale),lcd_palette().ink,lines[i],0,0,clip);
    }
    ui_SetCursorPos(origin);
    ui_BeginDisabled(locked||!g_server.hostEditable||g_server.pending);
    bool editEndpoint=ui_InvisibleButton("##essential_server_endpoint_click",ImVec2(fieldWidth,44*scale),0);
    ui_EndDisabled();
    if(editEndpoint){endpoint_dialog_open();ui_OpenPopup("##essential_endpoint_popup",0);}
    toolbar_tooltip(g_server.error[0]?g_server.error:"Kliknij, aby zmienic adres IP i port. Strzalki: transfer DL/UL.");
    // Popup stays bound to the same source; closing it with ESC or outside
    // the popup has exactly the same no-op semantics as Anuluj.
    if(g_endpoint_dialog.open&&!c_equal(g_endpoint_dialog.source,string_chars(core_selected_source)))endpoint_dialog_cancel();
    if(g_endpoint_dialog.open)ui_SetNextWindowSize(ImVec2(318*scale,0),1);
    if(ui_BeginPopup("##essential_endpoint_popup",0)){
        ui_TextUnformatted("Adres IP / host",0);
        ui_SetNextItemWidth(284*scale);
        ui_InputText("##essential_endpoint_host",g_endpoint_dialog.host,sizeof(g_endpoint_dialog.host),0,0,0);
        ui_TextUnformatted("Port TCP",0);
        ui_SetNextItemWidth(128*scale);
        ui_InputInt("##essential_endpoint_port",&g_endpoint_dialog.port,0,0,0);
        if(g_endpoint_dialog.error[0])ui_TextUnformatted(g_endpoint_dialog.error,0);
        if(ui_Button("Zapisz##essential_endpoint_save",ImVec2(112*scale,0))){
            if(endpoint_dialog_save()){
                // Save to the native Source configuration. When already connected,
                // reconnect to the new endpoint and restore the previous Play state.
                if(g_server.dirty){
                    if(g_server.connected){
                        queue_server_connection(true);
                    }else{
                        server_native_action(0,g_endpoint_dialog.host,g_endpoint_dialog.port);
                        if(c_equal(g_server.host,g_endpoint_dialog.host)&&g_server.port==g_endpoint_dialog.port){
                            g_server.dirty=false;server_sync_address();
                        }
                    }
                }
                ui_CloseCurrentPopup();
            }
        }
        ui_SameLine(0,10*scale);
        if(ui_Button("Anuluj##essential_endpoint_cancel",ImVec2(112*scale,0))){
            endpoint_dialog_cancel();ui_CloseCurrentPopup();
        }
        ui_EndPopup();
    }else if(g_endpoint_dialog.open){endpoint_dialog_cancel();}
    ui_SetCursorPos(origin);ui_Dummy(ImVec2(fieldWidth,44*scale));
    ui_SameLine(0,6*scale);
    float groupX=ui_GetCursorPos().x;
    float buttonWidth=82*scale,rateWidth=toolbar_rate_width(scale);
    float groupWidth=buttonWidth+6*scale+rateWidth;
    // Reserve the full two-row region before drawing interactive widgets,
    // so no later Dummy overlays their hit rectangles.
    ui_SetCursorPos(ImVec2(groupX,origin.y));ui_Dummy(ImVec2(groupWidth,51*scale));
    ui_SetCursorPos(ImVec2(groupX,origin.y+1*scale));
    ui_BeginDisabled(locked||g_server.pending);
    bool clicked=ui_Button(connected?"Disconnect##essential_server_connect":"Connect##essential_server_connect",ImVec2(buttonWidth,23*scale));
    ui_EndDisabled();toolbar_tooltip(g_server.error[0]?g_server.error:connected?"Rozlacz aktywny klient sieciowy":"Polacz aktualny klient sieciowy");
    if(clicked)queue_server_connection(!connected);
    ui_SetCursorPos(ImVec2(groupX+buttonWidth+6*scale,origin.y));toolbar_rate(scale,origin,1);
    toolbar_device_dropdown(ImVec2(groupX,origin.y+28*scale),groupWidth);
    ui_SameLine(0,6*scale);ui_SetCursorPos(ImVec2(ui_GetCursorPos().x,origin.y));
}
static void hook_toolbar_before_play(float offset,float spacing){ui_SameLine(offset,spacing);if(g_supported&&g_enabled)toolbar_server();}
static void toolbar_gain(){
    refresh_gain_reading();float scale=toolbar_scale();
    ImVec2 origin=ui_GetCursorPos();float window=ui_GetWindowSize().x;
    // Preserve the user's Paint layout: volume above gain, RTL/Tuner AGC stacked
    // to their right. No extra gain row is created horizontally.
    bool hasVolume=g_volume_toolbar_frame==ui_GetFrameCount()&&g_volume_toolbar_width>50*scale;
    // Native SinkManager width includes its square Mute button, internal
    // padding, and SameLine spacing. Gain begins after that button and has
    // exactly the available slider width, never the full combined width.
    // Measured against the V22 screenshot: native Volume starts 4 px to the
    // right of Gain and ends 10 px farther right.  Calibrate both edges,
    // leaving Mute and the native Volume geometry unchanged.
    float muteWithSpacing=50*scale;
    float sliderWidth=hasVolume?g_volume_toolbar_width-40*scale:140*scale;
    if(sliderWidth<45*scale)sliderWidth=45*scale;
    float labelsWidth=(ui_CalcTextSize("Tuner AGC",0,false,-1).x+28*scale);
    float available=window-origin.x;
    if(available<labelsWidth+55*scale)return;
    bool locked=*(bool*)core_main_window;
    bool rtlAuto=g_gain.rtlAuto,rtlAvailable=g_gain.rtlAgcKey[0]!=0&&c_equal(g_gain.source,string_chars(core_selected_source));
    ui_SetCursorPos(ImVec2(origin.x,origin.y+1*scale));
    ui_BeginDisabled(locked||!rtlAvailable);
    if(ui_Checkbox("RTL AGC##essential_toolbar_rtl",&rtlAuto)){
        char key[256];c_copy(key,sizeof(key),g_gain.rtlAgcKey);
        if(request_source_value(4,key,0,rtlAuto)){process_source_request();refresh_gain_reading();}
    }
    ui_EndDisabled();toolbar_tooltip(rtlAvailable?"RTL AGC aktywnego Source":"Aktywne Source nie udostepnia RTL AGC");
    bool automatic=g_gain.agcValue,agcAvailable=g_gain.agcKey[0]!=0&&c_equal(g_gain.source,string_chars(core_selected_source));
    ui_SetCursorPos(ImVec2(origin.x,origin.y+29*scale));
    ui_BeginDisabled(locked||!agcAvailable);
    if(ui_Checkbox(g_gain.agcMode||(!starts_with(g_gain.agcKey,"Tuner AGC")&&agcAvailable)?"AGC##essential_toolbar":"Tuner AGC##essential_toolbar",&automatic)){
        char key[256];c_copy(key,sizeof(key),g_gain.agcKey);
        if(request_source_value(4,key,0,automatic)){process_source_request();refresh_gain_reading();}
    }
    ui_EndDisabled();toolbar_tooltip(agcAvailable?"Automatyczny gain aktywnego Source":"Aktywne Source nie udostepnia kontrolki AGC");
    GainValue reading={};GainPosition range=gain_position();int selected=range.selected;
    if(selected>=0)reading=g_gain.values[selected];
    int maximum=range.maximum,position=range.position;
    float sliderX=hasVolume?g_volume_toolbar_origin.x+muteWithSpacing:origin.x;
    // Lower row begins beneath the upper native Volume control.  Never
    // center a 32px-high Gain widget over the volume row.
    float gainTop=(hasVolume?g_volume_toolbar_origin.y:origin.y)+30*scale;
    ui_SetCursorPos(ImVec2(sliderX,gainTop));
    ui_SetNextItemWidth(sliderWidth);
    ui_PushStyleVarVec(11,ImVec2(4*scale,1*scale));
    ui_BeginDisabled(locked||selected<0||reading.blocked||g_gain.tunerAuto||g_gain.otherAuto||!maximum);
    // Slider has exactly the same width as Volume; show the value on
    // hover, not a long caption that is drawn over the slider and its knob.
    if(ui_SliderInt("##essential_toolbar_gain",&position,0,maximum,"",0)){
        if(request_source_value(3,reading.key,reading.minimum+position*range.signedStep)){
            process_source_request();refresh_gain_reading();
        }
    }
    ui_EndDisabled();ui_PopStyleVar(1);
    char gainTip[192]="Gain: ";char value[80];
    if(!gain_caption(value,sizeof(value)))c_copy(value,sizeof(value),"--");
    c_append(gainTip,sizeof(gainTip),value);
    c_append(gainTip,sizeof(gainTip),g_gain.tunerAuto||g_gain.otherAuto?" (AGC aktywne)":" (reczne sterowanie)");
    toolbar_tooltip(gainTip);
    ui_SetCursorPos(ImVec2(origin.x+labelsWidth+4*scale,origin.y));
}
// 2 independent manual UTC offsets, no DST or automatic Polish time selection.
static void clock_hhmmss(int offset,char*out){
    WinSystemTime now={};GetSystemTime(&now);
    int seconds=(int)now.hour*3600+(int)now.minute*60+(int)now.second+offset*3600;
    seconds=(seconds%86400+86400)%86400;
    int values[]={seconds/3600,(seconds/60)%60,seconds%60};
    for(int i=0;i<3;i++){
        out[3*i]=(char)('0'+values[i]/10);out[3*i+1]=(char)('0'+values[i]%10);
        if(i<2)out[3*i+2]=':';
    }out[8]=0;
}
static void toolbar_clock(int index,float width,float scale,float top){
    ImVec2 origin=ui_GetCursorPos(),pos=ui_GetCursorScreenPos();ImDrawList*dl=ui_GetWindowDrawList();
    float height=49*scale;
    draw_lcd_glass(dl,pos,ImVec2(pos.x+width,pos.y+height),scale);
    char zone[24]="UTC";int offset=g_clock_offsets[index];c_append(zone,sizeof(zone),offset>=0?"+":"-");
    char digits[16];u64str((u64)(offset>=0?offset:-offset),digits,sizeof(digits));c_append(zone,sizeof(zone),digits);
    draw_lcd_text(dl,index?"ZONE 2":"ZONE 1",ImVec2(pos.x+6*scale,pos.y+5*scale),9*scale,lcd_palette().dim,width*0.42f);
    draw_lcd_text(dl,zone,ImVec2(pos.x+width-43*scale,pos.y+5*scale),9*scale,lcd_palette().ink,35*scale);
    // No dropdown chevron: a click on the clock panel still opens UTC choices.
    char time[9];clock_hhmmss(offset,time);
    // Ordinary SDR++ UI font contains reliable colon glyphs.  The previous
    // bigFont lacks a usable colon and rendered the time as 8 run-together
    // digits; fit all eight characters and center the entire HH:MM:SS.
    ImVec2 measured=ui_CalcTextSize(time,0,false,-1);
    float nativeHeight=ui_GetFontSize();
    if(nativeHeight<1)nativeHeight=13*scale;
    float targetHeight=19*scale;
    float maxWidth=width-9*scale;
    float renderedWidth=measured.x*targetHeight/nativeHeight;
    if(renderedWidth>maxWidth){targetHeight*=maxWidth/renderedWidth;renderedWidth=maxWidth;}
    draw_AddTextSized(dl,0,targetHeight,
       ImVec2(pos.x+(width-renderedWidth)*0.5f,pos.y+21*scale),
       lcd_palette().glow,time,0,0,0);
    ui_SetCursorPos(origin);
    if(ui_InvisibleButton(index?"##essential_clock2":"##essential_clock1",ImVec2(width,height),0)){
        g_tuning_input_suppressed_through=ui_GetFrameCount()+2;
        ui_OpenPopup(index?"##essential_clock_popup2":"##essential_clock_popup1",0);
    }
    if(ui_BeginPopup(index?"##essential_clock_popup2":"##essential_clock_popup1",0)){
        for(int off=-12;off<=14;off++){
            char label[24]="UTC";c_append(label,sizeof(label),off>=0?"+":"-");
            char n[16];u64str((u64)(off>=0?off:-off),n,sizeof(n));c_append(label,sizeof(label),n);
            if(ui_Selectable(label,g_clock_offsets[index]==off,0,ImVec2())){
                g_clock_offsets[index]=off;settings_changed();
                g_tuning_input_suppressed_through=ui_GetFrameCount()+3;
                ui_CloseCurrentPopup();
            }
        }
        ui_EndPopup();
    }
    ui_SetCursorPos(ImVec2(origin.x+width+5*scale,top));
}
static void toolbar_clocks(float scale){
    ImVec2 start=ui_GetCursorPos();float window=ui_GetWindowSize().x;
    FrequencyMetrics m=frequency_metrics();
    float freqWidth=12*m.digitWidth+3*m.dotWidth+25*scale;
    // Preserve frequency, history, receiver LCD and meters. Clocks shrink first.
    float reserve=freqWidth+680*scale;
    float room=window-start.x-reserve;
    if(room<155*scale)return;
    float width=(room-5*scale)/2;
    if(width>142*scale)width=142*scale;
    if(width<75*scale)return;
    toolbar_clock(0,width,scale,start.y);
    toolbar_clock(1,width,scale,start.y);
}
static bool g_frequency_lcd_active=false;
static float g_toolbar_bottom=0;static int g_toolbar_bottom_frame=-1;
static void hook_toolbar_columns(int count,const char*id,bool border){
    if(g_supported&&g_enabled&&c_equal(id,"WindowColumns")&&g_toolbar_bottom_frame==ui_GetFrameCount()){
        ImVec2 p=ui_GetCursorPos();if(p.y<g_toolbar_bottom)ui_SetCursorPos(ImVec2(p.x,g_toolbar_bottom));
    }
    ui_Columns(count,id,border);
}
static void hook_frequency_lcd_text(ImDrawList*dl,const ImVec2&pos,u32 color,const char*text,const char*end){
    if(!g_supported||!g_enabled||!g_frequency_lcd_active||!text||!text[0]||text[1]||
       ((text[0]<'0'||text[0]>'9')&&text[0]!='.')){draw_AddText(dl,pos,color,text,end);return;}
    float font=ui_GetFontSize(),height=font*0.78f;ImVec2 size=ui_CalcTextSize(text,end,false,-1);
    ImVec2 origin(pos.x+(size.x-height*(text[0]=='.'?0.095f:0.48f))*0.5f,pos.y+(font-height)*0.5f);
    draw_receiver_digit(dl,text[0],origin,height,lcd_palette().ink);
}
static void hook_toolbar_frequency(void*frequency){
    if(!g_supported||!g_enabled){frequency_draw(frequency);return;}
    toolbar_gain();float scale=toolbar_scale();toolbar_clocks(scale);ImVec2 origin=ui_GetCursorPos();
    ui_SetCursorPos(ImVec2(origin.x,origin.y+6*scale));
    // Measure with the native selector's own font and integer digit geometry.
    // Styling leaves its hit regions, wheel, digit edits and frequency intact.
    FrequencyMetrics metrics=frequency_metrics();
    ImVec2 pos=ui_GetCursorScreenPos(),content=ui_GetWindowContentRegionMin();
    int digitHeight=metrics.digitHeight,digitWidth=metrics.digitWidth,dotWidth=metrics.dotWidth;
    float lift=(float)(int)(15*scale);if(lift<15*scale)lift+=1;
    pos.x+=content.x;pos.y-=metrics.glyphHeight*0.5f-lift-5;
    float width=12*digitWidth+3*dotWidth+15*scale,height=digitHeight+12*scale;
    ImDrawList*dl=ui_GetWindowDrawList();ImVec2 lo(pos.x-5*scale,pos.y-4*scale),hi(pos.x+width,pos.y+height-4*scale);
    draw_lcd_glass(dl,lo,hi,scale);
    g_toolbar_bottom=hi.y-ui_GetWindowPos().y+6*scale;g_toolbar_bottom_frame=ui_GetFrameCount();
    ui_PushTextColor(0,lcd_palette().ink);g_frequency_lcd_active=true;
    // The native selector consumes keys/characters under the mouse even while
    // another field is active. Keep IP (and other text edits) out of tuning.
    bool locked=*(bool*)core_main_window;
    if(ui_IsAnyItemActive())*(bool*)core_main_window=true;
    frequency_draw(frequency);
    *(bool*)core_main_window=locked;
    g_frequency_lcd_active=false;ui_PopStyleColor(1);
    // Native FrequencySelect::ItemSize excludes its content-region offset;
    // SameLine ignores its final SetCursorPosX. Reserve the real chassis width
    // here, including a 4 px spacer before the native theme's ItemSpacing.
    float barHeight=g_toolbar_bottom-origin.y;
    ui_SetCursorPos(origin);ui_Dummy(ImVec2(content.x+width+4*scale,barHeight));
}
static void receiver_information(char*filter,usize fs,char*modulation,usize ms,char*power,usize ps,const MeterReading&reading){
    void*v=selected_vfo();char value[48];
    c_copy(filter,fs,l10n("Filtr: "));if(v)format_filter(vfo_bandwidth(v),value,sizeof(value));else c_copy(value,sizeof(value),"--");c_append(filter,fs,value);
    c_copy(modulation,ms,l10n("Modulacja: "));c_append(modulation,ms,modulation_name(selected_modulation()));
    c_copy(power,ps,l10n("Moc sygnalu: "));if(reading.rfValid)format_dbfs(reading.rfDb,value,sizeof(value));else c_copy(value,sizeof(value),"---.- dBFS");c_append(power,ps,value);
}
#include "receiver_actions.inc"
static void format_tuning_step(double hz,char*out,usize size){
    if(!(hz>=1&&hz<=2147483647.0)){c_copy(out,size,"-- Hz");return;}
    const char*unit=" Hz";double value=hz;
    if(hz>=1000000){value=hz/1000000;unit=" MHz";}
    else if(hz>=1000){value=hz/1000;unit=" kHz";}
    if(hz<1000)u64str((u64)(hz+0.5),out,size);
    else{
        gain_number(value,true,false,out,size);usize n=c_len(out);
        if(n>=2&&out[n-2]=='.'&&out[n-1]=='0')out[n-2]=0;
    }
    c_append(out,size,unit);
}
static void receiver_lcd(const char*filter,const char*modulation,const char*power,float width,float scale){
    ImVec2 origin=ui_GetCursorPos(),pos=ui_GetCursorScreenPos();ImDrawList*dl=ui_GetWindowDrawList();float height=56*scale;
    if(height<44)height=44;
    float labelHeight=13*scale,valueHeight=20*scale;
    if(labelHeight<13)labelHeight=13;if(valueHeight<16)valueHeight=16;
    float labelY=6*scale,valueY=28*scale;if(labelY<4)labelY=4;if(valueY<22)valueY=22;
    // The same dark green chassis and illuminated readout as Frequency entry.
    draw_lcd_glass(dl,pos,ImVec2(pos.x+width,pos.y+height),scale);
    char step[40];void*v=selected_vfo();format_tuning_step(v?vfo_snap(v):0,step,sizeof(step));
    const char*values[]={filter+c_len(l10n("Filtr: ")),modulation+c_len(l10n("Modulacja: ")),step,power+c_len(l10n("Moc sygnalu: "))};
    const char*labels[]={l10n("FILTR"),l10n("MODULACJA"),l10n("KROK"),l10n("MOC RF")};
    float preferred[4],total=0,starts[5]={0};
    for(int i=0;i<4;i++){
        float labelWidth=lcd_text_width(labels[i],labelHeight),valueWidth=lcd_text_width(values[i],valueHeight,true);
        preferred[i]=(labelWidth>valueWidth?labelWidth:valueWidth)+14*scale;total+=preferred[i];
    }
    for(int i=0;i<4;i++)starts[i+1]=starts[i]+width*preferred[i]/total;
    float bottom=pos.y-ui_GetWindowPos().y+height+6*scale;
    if(bottom>g_toolbar_bottom)g_toolbar_bottom=bottom;
    {
        for(int i=0;i<4;i++){
            if(i)draw_AddLine(dl,ImVec2(pos.x+starts[i],pos.y+7*scale),ImVec2(pos.x+starts[i],pos.y+height-7*scale),lcd_palette().divider,scale);
            float available=starts[i+1]-starts[i]-12*scale;
            draw_lcd_text(dl,labels[i],ImVec2(pos.x+starts[i]+6*scale,pos.y+labelY),labelHeight,lcd_palette().dim,available);
            draw_lcd_text(dl,values[i],ImVec2(pos.x+starts[i]+6*scale,pos.y+valueY),valueHeight,lcd_palette().ink,available,true);
        }
    }
    bool editable=radio_interface_ready()&&!*(bool*)core_main_window;
    ui_BeginDisabled(!editable);
    ui_SetCursorPos(origin);
    if(ui_InvisibleButton("##essential_toolbar_filter",ImVec2(starts[1],height),0))open_toolbar_filter();
    scroll_toolbar_receiver(0);
    ui_SetCursorPos(ImVec2(origin.x+starts[1],origin.y));
    if(ui_InvisibleButton("##essential_toolbar_modulation",ImVec2(starts[2]-starts[1],height),0)){g_tuning_input_suppressed_through=ui_GetFrameCount()+2;ui_OpenPopup("##essential_modulation_popup",0);}
    scroll_toolbar_receiver(1);
    ui_SetCursorPos(ImVec2(origin.x+starts[2],origin.y));
    if(ui_InvisibleButton("##essential_toolbar_step",ImVec2(starts[3]-starts[2],height),0))open_toolbar_step();
    scroll_toolbar_receiver(2);
    ui_EndDisabled();ui_SetCursorPos(origin);ui_Dummy(ImVec2(width,height));
    if(ui_IsItemHovered(0)){
        char exactStep[64];c_copy(exactStep,sizeof(exactStep),l10n("Krok: "));
        if(v){char digits[32];u64str((u64)(vfo_snap(v)+0.5),digits,sizeof(digits));c_append(exactStep,sizeof(exactStep),digits);c_append(exactStep,sizeof(exactStep)," Hz");}
        else c_append(exactStep,sizeof(exactStep),"--");
        ui_BeginTooltip();ui_TextUnformatted(filter,0);ui_TextUnformatted(modulation,0);ui_TextUnformatted(exactStep,0);ui_TextUnformatted(power,0);ui_EndTooltip();
    }
    toolbar_receiver_popups();
}
static void hook_toolbar_snr(float snr,const ImVec2&size){
    if(!g_supported||!g_enabled){ui_SNRMeter(snr,size);return;}
    float scale=core_ui_scale;if(!(scale>=0.5f&&scale<=4))scale=1;
    ImVec2 original=ui_GetCursorPos(),window=ui_GetWindowSize(),windowPos=ui_GetWindowPos();
    ImVec2 centerEnd=ui_GetItemRectMax();
    float left=centerEnd.x-windowPos.x+8*scale,right=window.x-18*scale;
    float room=right-left,gap=(window.x<1300*scale?4:window.x<1400*scale?6:8)*scale;
    float buttonWidth=(window.x<1300*scale?20:window.x<1400*scale?24:30)*scale,buttons=2*buttonWidth+gap;
    float minNative=(window.x>=1600*scale?150:window.x>=1400*scale?90:window.x>=1300*scale?60:40)*scale;
    if(room<buttons+minNative+2*gap){
        if(room>8*scale){float nativeWidth=lcd_pixel(room);ui_SetCursorPos(ImVec2(right-nativeWidth,original.y));ui_SetNextItemWidth(nativeWidth);ui_SNRMeter(snr,size);}
        return;
    }
    history_poll();MeterReading reading=update_meter_reading();
    ui_SetCursorPos(ImVec2(left,original.y-5*scale));
    bool locked=*(bool*)core_main_window;
    ui_BeginDisabled(locked||!history_back_available());
    if(ui_Button("<##essential_history_back",ImVec2(buttonWidth,buttonWidth)))history_move(-1);
    ui_EndDisabled();toolbar_tooltip("Wstecz: czestotliwosc, modulacja i filtr (Ctrl+Z)");
    ui_SameLine(0,gap);ui_BeginDisabled(locked||g_history.cursor+1>=g_history.count);
    if(ui_Button(">##essential_history_forward",ImVec2(buttonWidth,buttonWidth)))history_move(1);
    ui_EndDisabled();toolbar_tooltip("Naprzod: czestotliwosc, modulacja i filtr");
    float after=left+buttons+gap;
    float meterWidth=room>780*scale?124*scale:room>570*scale?104*scale:room>390*scale?84*scale:62*scale;
    float minInformation=320*scale;
    if(room-buttons-2*meterWidth-4*gap-minNative<minInformation)meterWidth=62*scale;
    // Keep native controls and history usable when the two AGC checkboxes make
    // a narrow toolbar too short for both analogue faces. Never overlap them.
    bool informationFits=right-after-2*gap-minNative>=minInformation;
    if(room<buttons+2*meterWidth+4*gap+minNative||
       (room-buttons-2*meterWidth-4*gap-minNative<minInformation&&informationFits)){
        float infoRoom=right-after-2*gap-minNative;
        if(infoRoom>=minInformation){
            if(infoRoom>500*scale)infoRoom=500*scale;
            char filter[64],modulation[64],power[64];receiver_information(filter,sizeof(filter),modulation,sizeof(modulation),power,sizeof(power),reading);
            ui_SameLine(0,gap);ui_SetCursorPos(ImVec2(after,original.y-5*scale));
            receiver_lcd(filter,modulation,power,infoRoom,scale);after+=infoRoom+gap;
        }
        float nativeWidth=lcd_pixel(right-after);
        ui_SameLine(0,gap);ui_SetCursorPos(ImVec2(right-nativeWidth,original.y));
        ui_SetNextItemWidth(nativeWidth);ui_SNRMeter(snr,size);return;
    }
    float informationWidth=room-buttons-2*meterWidth-4*gap-minNative;
    if(informationWidth>500*scale)informationWidth=500*scale;
    if(informationWidth<minInformation)informationWidth=0;
    if(informationWidth>0){
        char filter[64],modulation[64],power[64];receiver_information(filter,sizeof(filter),modulation,sizeof(modulation),power,sizeof(power),reading);
        ui_SameLine(0,gap);ui_SetCursorPos(ImVec2(after,original.y-5*scale));
        receiver_lcd(filter,modulation,power,informationWidth,scale);after+=informationWidth+gap;
    }
    // Reserve the native meter on the far right and pack RF/AUDIO immediately to its left.
    float available=right-after-2*meterWidth-2*gap;
    float width=available;if(width<minNative)width=minNative;
    // ImGui floors CalcItemWidth; round once and anchor the integer width to
    // the right edge, including fractional 80% scale positions.
    width=lcd_pixel(width);
    float audioX=right-width-gap-meterWidth,rfX=audioX-gap-meterWidth;
    ui_SameLine(0,gap);ui_SetCursorPos(ImVec2(rfX,original.y-5*scale));
    draw_needle_meter("RF",g_rf_needle,reading.rfDb,reading.rfValid,meterWidth,scale);
    toolbar_tooltip("RF: moc przy czestotliwosci czerwonego markera, dBFS");
    ui_SameLine(0,gap);ui_SetCursorPos(ImVec2(audioX,original.y-5*scale));
    draw_needle_meter("AUDIO",g_audio_needle,reading.audioDb,reading.audioValid,meterWidth,scale);
    toolbar_tooltip("AUDIO: szczytowy poziom wybranego odbiornika po regulacji glosnosci, dBFS");
    ui_SameLine(0,gap);ui_SetCursorPos(ImVec2(right-width,original.y));ui_SetNextItemWidth(width);
    ui_SNRMeter(snr,size);
}
static bool toolbar_image_with_tip(const char*tip,void*texture,const ImVec2&size,const ImVec2&uv0,const ImVec2&uv1,int padding,const ImVec4&background,const ImVec4&tint){
    bool clicked=ui_ImageButton(texture,size,uv0,uv1,padding,background,tint);
    if(g_supported&&g_enabled)toolbar_tooltip(tip);
    return clicked;
}
static bool hook_toolbar_play(void*t,const ImVec2&s,const ImVec2&a,const ImVec2&b,int p,const ImVec4&bg,const ImVec4&tint){
    return toolbar_image_with_tip("Play: uruchom odbior",t,s,a,b,p,bg,tint);
}
static bool hook_toolbar_stop(void*t,const ImVec2&s,const ImVec2&a,const ImVec2&b,int p,const ImVec4&bg,const ImVec4&tint){
    return toolbar_image_with_tip("Stop: zatrzymaj odbior",t,s,a,b,p,bg,tint);
}
static bool hook_toolbar_mute(void*t,const ImVec2&s,const ImVec2&a,const ImVec2&b,int p,const ImVec4&bg,const ImVec4&tint){
    // Mouse clicks remain entirely native. M toggles the selected audio stream
    // directly in keyboard_frame; no per-frame synthetic clicks here.
    return toolbar_image_with_tip("Mute: wycisz / wlacz dzwiek",t,s,a,b,p,bg,tint);
}
// The tuning icon is now a selector, not an action. Changing mode must NOT
// move the selected VFO, its Hz, the zoom or the current display centre.
static const char* toolbar_tuning_name(){
    return g_tuning_mode==1?"Center":g_tuning_mode==2?"Free Fixed":"Free Tuning";
}
static void toolbar_tuning_menu(){
    if(!ui_BeginPopup("##essential_tuning_popup",0))return;
    const char*labels[]={"Free Tuning","Center","Free Fixed"};
    for(int i=0;i<3;i++){
        if(ui_Selectable(l10n(labels[i]),g_tuning_mode==i,0,ImVec2())){
            g_tuning_mode=i;
            *(int*)(core_main_window+0xFC)=i==1?0:1;
            core_waterfall[0x71]=i==1?1:0;
            cancel_tuning_gestures();
            g_tuning_input_suppressed_through=ui_GetFrameCount()+3;
            g_fixed_marker_hz=selected_frequency();
            panel_changed();
            ui_CloseCurrentPopup();
        }
    }
    ui_EndPopup();
}
static bool toolbar_tuning_selector(void*t,const ImVec2&s,const ImVec2&a,const ImVec2&b,int p,const ImVec4&bg,const ImVec4&tint){
    // Use original SDR++ Normal Tuning texture for every state; identical icon.
    void*icon=core_normal_tuning_icon?core_normal_tuning_icon:t;
    const bool clicked=toolbar_image_with_tip(toolbar_tuning_name(),icon,s,a,b,p,bg,tint);
    if(clicked&&g_supported&&g_enabled){
        g_tuning_input_suppressed_through=ui_GetFrameCount()+3;
        ui_OpenPopup("##essential_tuning_popup",0);
    }
    if(g_supported&&g_enabled)toolbar_tuning_menu();
    return false; // native SDR++ must not toggle Center on button press
}
static bool hook_toolbar_center_on(void*t,const ImVec2&s,const ImVec2&a,const ImVec2&b,int p,const ImVec4&bg,const ImVec4&tint){
    return toolbar_tuning_selector(t,s,a,b,p,bg,tint);
}
static bool hook_toolbar_center_off(void*t,const ImVec2&s,const ImVec2&a,const ImVec2&b,int p,const ImVec4&bg,const ImVec4&tint){
    return toolbar_tuning_selector(t,s,a,b,p,bg,tint);
}
static bool hook_toolbar_logo(void*texture,const ImVec2&size,const ImVec2&uv0,const ImVec2&uv1,int padding,const ImVec4&background,const ImVec4&tint){
    if(g_supported&&g_enabled)return false;
    return ui_ImageButton(texture,size,uv0,uv1,padding,background,tint);
}

struct ToolbarCall { u8*address;u8 original[5],patched[5]; };
static const int TOOLBAR_HOOK_COUNT=18;
static ToolbarCall g_toolbar_calls[TOOLBAR_HOOK_COUNT]={};
static void* g_toolbar_thunks=0;
static int g_toolbar_expected_active=0;
static bool write_code(u8*destination,const u8*bytes,usize size){
    u32 protection=0;if(!VirtualProtect(destination,size,0x40,&protection))return false;
    for(usize i=0;i<size;i++)destination[i]=bytes[i];
    FlushInstructionCache(GetCurrentProcess(),destination,size);u32 ignored=0;VirtualProtect(destination,size,protection,&ignored);return true;
}
static bool bytes_equal(const u8*a,const u8*b,usize size){for(usize i=0;i<size;i++)if(a[i]!=b[i])return false;return true;}
static bool restore_toolbar_hooks(){
    bool safe=true;
    for(int i=TOOLBAR_HOOK_COUNT-1;i>=0;i--){ToolbarCall&call=g_toolbar_calls[i];
        if(call.address&&bytes_equal(call.address,call.patched,5)){
            if(!write_code(call.address,call.original,5)){
                safe=false;continue; // retain the patch record and its live thunk
            }
        }
        call={};
    }
    if(safe&&g_toolbar_thunks){
        if(VirtualFree(g_toolbar_thunks,0,0x8000))g_toolbar_thunks=0;
        else safe=false;
    }
    if(safe)g_toolbar_expected_active=0;
    return safe;
}
static bool install_toolbar_hooks(){
    if(g_toolbar_thunks){
        // Fast path: an already complete installation needs no extra writes.
        int active=0,patched=0;
        for(int i=0;i<TOOLBAR_HOOK_COUNT;i++)if(g_toolbar_calls[i].address){
            active++;
            if(bytes_equal(g_toolbar_calls[i].address,g_toolbar_calls[i].patched,5))patched++;
        }
        if(active&&active==g_toolbar_expected_active&&active==patched)return true;
        // A previous partial unhook must be resolved before re-installation.
        if(!restore_toolbar_hooks())return false;
    }
    u8*core=(u8*)GetModuleHandleA("sdrpp_core.dll");if(!core)return false;
    // Four MainWindow image buttons and the native SinkManager Mute icon.
    // Each verified call is patched only when all five original bytes match.
    const u32 offsets[]={0x56d83,0x570e1,0x56a45,0x8e6d7,0x8e750,0x153bd0,0x66768,0x573f1,0x57b68,0x56d63,0x571a9,0x161126,0x16148a,
        0x56ad3,0x56ba6,0x56e0c,0x56ee1,0x160e86};
    const u8 expected[TOOLBAR_HOOK_COUNT][5]={{0xE8,0xB8,0x76,0x03,0x00},{0xE8,0x9A,0xBF,0x03,0x00},{0xE8,0xE6,0x52,0x06,0x00},{0xE8,0x14,0x95,0x05,0x00},
        {0xE8,0x9B,0x94,0x05,0x00},{0xE8,0xCB,0x15,0xFC,0xFF},{0xE8,0x33,0xEA,0x0A,0x00},{0xE8,0x5A,0x61,0x0A,0x00},
        {0xE8,0xE3,0x59,0x0A,0x00},{0xE8,0x08,0x9E,0x10,0x00},{0xE8,0x12,0x11,0x0C,0x00},{0xE8,0x45,0xB9,0xF5,0xFF},
        {0xE8,0xE1,0xB5,0xF5,0xFF},{0xE8,0xE8,0x17,0x0C,0x00},{0xE8,0x15,0x17,0x0C,0x00},{0xE8,0xAF,0x14,0x0C,0x00},
        {0xE8,0xDA,0x13,0x0C,0x00},{0xE8,0x35,0x74,0xFB,0xFF}};
    // Optional tooltip/mute callsites are VERIFIED INDIVIDUALLY. In V34 a
    // mismatch at *any* of the five sites disabled the Mute keyboard hook,
    // even when the mute callsite itself matched.
    const int BASE_HOOK_COUNT=13;
    for(int i=0;i<BASE_HOOK_COUNT;i++)if(!bytes_equal(core+offsets[i],expected[i],5))return false;
    bool siteEnabled[TOOLBAR_HOOK_COUNT]={};
    int activeCount=0;
    for(int i=0;i<TOOLBAR_HOOK_COUNT;i++){
        siteEnabled[i]=(i<BASE_HOOK_COUNT)||bytes_equal(core+offsets[i],expected[i],5);
        if(siteEnabled[i])activeCount++;
    }
    // rel32 calls require nearby thunks. Allocation granularity is 64 KiB.
    usize base=(usize)core&~(usize)0xFFFF;
    for(usize distance=0x900000;distance<0x70000000&&!g_toolbar_thunks;distance+=0x10000){
        g_toolbar_thunks=VirtualAlloc((void*)(base+distance),4096,0x3000,4);
        if(!g_toolbar_thunks&&base>distance)g_toolbar_thunks=VirtualAlloc((void*)(base-distance),4096,0x3000,4);
    }
    if(!g_toolbar_thunks)return false;
    g_toolbar_expected_active=activeCount;
    void*hooks[]={(void*)hook_toolbar_frequency,(void*)hook_toolbar_snr,(void*)hook_toolbar_before_play,
        (void*)hook_frequency_lcd_text,(void*)hook_frequency_lcd_text,(void*)hook_native_source_combo,(void*)hook_ui_combo,(void*)hook_toolbar_columns,(void*)hook_toolbar_columns,(void*)hook_toolbar_volume,(void*)hook_toolbar_logo,(void*)hook_volume_slider_vertical,(void*)hook_volume_slider_vertical,
        (void*)hook_toolbar_stop,(void*)hook_toolbar_play,(void*)hook_toolbar_center_on,(void*)hook_toolbar_center_off,(void*)hook_toolbar_mute};
    u8*thunks=(u8*)g_toolbar_thunks;
    for(int i=0;i<TOOLBAR_HOOK_COUNT;i++){
        if(!siteEnabled[i])continue;
        u8*thunk=thunks+16*i;thunk[0]=0xFF;thunk[1]=0x25;
        for(int j=2;j<6;j++)thunk[j]=0;u64 address=(u64)(usize)hooks[i];
        for(int j=0;j<8;j++)thunk[j+6]=(u8)(address>>(8*j));
        ToolbarCall&call=g_toolbar_calls[i];call.address=core+offsets[i];
        for(int j=0;j<5;j++)call.original[j]=expected[i][j];
        long long delta=(long long)(usize)thunk-(long long)(usize)(call.address+5);
        if(delta<(-2147483647LL-1)||delta>2147483647LL){restore_toolbar_hooks();return false;}
        call.patched[0]=0xE8;int displacement=(int)delta;
        for(int j=0;j<4;j++)call.patched[j+1]=(u8)((u32)displacement>>(8*j));
    }
    u32 old=0;if(!VirtualProtect(thunks,4096,0x20,&old)){restore_toolbar_hooks();return false;}
    FlushInstructionCache(GetCurrentProcess(),thunks,4096);
    for(int i=0;i<TOOLBAR_HOOK_COUNT;i++)if(siteEnabled[i]&&
        !write_code(g_toolbar_calls[i].address,g_toolbar_calls[i].patched,5)){
        restore_toolbar_hooks();return false;
    }
    return true;
}
