// Source modules expose gain and sample rate through their existing SmGui widgets.
// Redirect only their matching imports in memory; leave the DLL files untouched.
// Returning a widget change runs the source's normal hardware/configuration callback.
// SDR++ Server receives the same DrawList diff as a normal remote-widget edit.

static bool our_window_focused();
static void zoom_prepare_source_transition();
static void zoom_finish_source_transition();
static bool g_source_hidden_visit=false;
struct SourceRequest { int kind,direction;bool done,resume;char source[256];char key[256];float value;bool automatic;char host[1024];int port,rateHz;bool setEndpoint,resumePrimed; };
static SourceRequest g_source_request={};
static bool starts_with(const char*s,const char*prefix){
    if(!s)return false;while(*prefix)if(*s++!=*prefix++)return false;return true;
}
static bool contains(const char*s,const char*part){
    if(!s)return false;for(;*s;s++)if(starts_with(s,part))return true;return false;
}
struct GainValue { char key[256],caption[16],text[40];bool blocked;int control;float value,minimum,maximum,step; };
struct GainReading { char source[256];GainValue values[4];int count,frame;bool tunerAuto,rtlAuto,otherAuto;char agcKey[256],rtlAgcKey[256];bool agcValue,agcMode; };
static GainReading g_gain={};
static bool g_gain_refresh=false;
static bool g_numeric_rate_apply=false;
static double g_gain_next_probe=0,g_gain_next_hooks=0;
static void begin_gain_sample(bool force=false){
    const char*source=string_chars(core_selected_source);int frame=ui_GetFrameCount();
    if(force||g_gain.frame!=frame||!c_equal(g_gain.source,source)){
        g_gain={};g_gain.frame=frame;c_copy(g_gain.source,sizeof(g_gain.source),source);
    }
}
static bool gain_display_widget(const char*label){
    return starts_with(label,"##_rtlsdr_gain_")||starts_with(label,"##_gain_select_")||
        starts_with(label,"##_airspy_sens_gain_")||starts_with(label,"##_airspy_lin_gain_")||
        starts_with(label,"##_airspy_lna_gain_")||starts_with(label,"##_airspy_mix_gain_")||
        starts_with(label,"##_airspy_vga_gain_")||starts_with(label,"##_hackrf_lna_")||
        starts_with(label,"##_hackrf_vga_")||starts_with(label,"##_pluto_gain__")||
        starts_with(label,"##_gain_sel_")||starts_with(label,"##sdrplay_gain")||starts_with(label,"##sdrplay_lna_gain");
}
static void observe_gain_agc(const char*label,bool value){
    if(!g_supported||!g_enabled||!label)return;
    bool tuner=starts_with(label,"Tuner AGC")||contains(label,"tuner_agc");
    bool rtl=starts_with(label,"RTL AGC")||contains(label,"_rtl_agc_");
    bool other=starts_with(label,"AGC")||starts_with(label,"IF AGC")||starts_with(label,"LNA AGC")||
        starts_with(label,"Mixer AGC")||contains(label,"sdrplay_agc")||contains(label,"_agc_sel_");
    if(!tuner&&!rtl&&!other)return;
    begin_gain_sample();
    if(tuner)g_gain.tunerAuto=value;else if(rtl)g_gain.rtlAuto=value;else g_gain.otherAuto=g_gain.otherAuto||value;
    if(rtl)c_copy(g_gain.rtlAgcKey,sizeof(g_gain.rtlAgcKey),label);
    if(!rtl&&(tuner||!g_gain.agcKey[0])){
        c_copy(g_gain.agcKey,sizeof(g_gain.agcKey),label);g_gain.agcValue=value;g_gain.agcMode=false;
    }
}
static void observe_gain_mode(const char*label,int value){
    if(!g_supported||!g_enabled||!starts_with(label,"##_pluto_gainmode_select_"))return;
    begin_gain_sample();g_gain.otherAuto=value!=0;
    c_copy(g_gain.agcKey,sizeof(g_gain.agcKey),label);g_gain.agcValue=value!=0;g_gain.agcMode=true;
}
static void gain_number(double value,bool decimal,bool db,char*out,usize size){
    out[0]=0;if(!(value>=-1000000&&value<=1000000)){c_copy(out,size,"--");return;}
    bool negative=value<0;if(negative)value=-value;
    u64 scaled=(u64)(value*(decimal?10:1)+0.5);char digits[32];
    if(negative)c_append(out,size,"-");u64str(decimal?scaled/10:scaled,digits,sizeof(digits));c_append(out,size,digits);
    if(decimal){char fraction[2]={(char)('0'+scaled%10),0};c_append(out,size,".");c_append(out,size,fraction);}
    if(db)c_append(out,size," dB");
}
static void observe_gain(const char*label,double value,int maximum,bool decimal,bool db,const char*literal,bool blocked){
    if(!g_supported||!g_enabled||!gain_display_widget(label))return;
    begin_gain_sample();int index=0;
    for(;index<g_gain.count;index++)if(c_equal(g_gain.values[index].key,label))break;
    if(index>=4)return;if(index==g_gain.count)g_gain.count++;
    GainValue&reading=g_gain.values[index];reading={};reading.blocked=blocked;
    c_copy(reading.key,sizeof(reading.key),label);
    if(starts_with(label,"##sdrplay_gain")){c_copy(reading.caption,sizeof(reading.caption),"IF GR");db=true;}
    else if(contains(label,"lna_"))c_copy(reading.caption,sizeof(reading.caption),"LNA");
    else if(contains(label,"vga_"))c_copy(reading.caption,sizeof(reading.caption),"VGA");
    else if(contains(label,"mix_gain_"))c_copy(reading.caption,sizeof(reading.caption),"MIX");
    else if(contains(label,"sens_gain_"))c_copy(reading.caption,sizeof(reading.caption),"SENS");
    else if(contains(label,"lin_gain_"))c_copy(reading.caption,sizeof(reading.caption),"LIN");
    else if(starts_with(label,"##_gain_sel_")){
        const char*name=label+c_len("##_gain_sel_");usize sourceLength=c_len(g_gain.source);
        if(starts_with(name,g_gain.source))name+=sourceLength;
        c_copy(reading.caption,sizeof(reading.caption),name);db=true;
    }
    if(starts_with(label,"##_hackrf_")||starts_with(label,"##_pluto_gain__"))db=true;
    if(literal&&*literal&&contains(literal,"dB")&&!contains(literal,"%")){
        c_copy(reading.text,sizeof(reading.text),literal);return;
    }
    gain_number(value,decimal,db,reading.text,sizeof(reading.text));
    // An RTL gain index is not a measured dB value. Remote RTL menus expose only
    // the selected index; preserve that distinction until a source supplies dB.
    if(!db&&(starts_with(label,"##_rtlsdr_gain_")||starts_with(label,"##_gain_select_"))){
        char limit[24];u64str(maximum>0?(u64)maximum:0,limit,sizeof(limit));
        c_append(reading.text,sizeof(reading.text),"/");c_append(reading.text,sizeof(reading.text),limit);
        c_append(reading.text,sizeof(reading.text)," krok");
    }
}
static bool source_request_matches(int kind);
static void observe_gain_control(const char*label,float value,float minimum,float maximum,float step,int control){
    if(!g_supported||!g_enabled||!gain_display_widget(label))return;
    for(int i=0;i<g_gain.count;i++)if(c_equal(g_gain.values[i].key,label)){
        GainValue&g=g_gain.values[i];g.value=value;g.minimum=minimum;g.maximum=maximum;g.step=step;g.control=control;return;
    }
}
static bool apply_agc_checkbox(const char*label,bool*v,bool disabled){
    if(!source_request_matches(4)||!v||disabled||!c_equal(label,g_source_request.key))return false;
    g_source_request.done=true;bool next=g_source_request.automatic;
    if(*v==next)return false;*v=next;return true;
}
static bool apply_agc_mode(const char*label,int*v,bool disabled){
    if(!source_request_matches(4)||!v||disabled||!c_equal(label,g_source_request.key))return false;
    g_source_request.done=true;int next=g_source_request.automatic?1:0;
    if(((*v)!=0)==g_source_request.automatic)return false;*v=next;return true;
}
static bool source_request_matches(int kind){
    return g_supported&&g_enabled&&g_source_request.kind==kind&&!g_source_request.done&&
        c_equal(g_source_request.source,string_chars(core_selected_source));
}
static bool widget_disabled(){
    u8*ctx=(u8*)ui_GetContext();
    // ImGui 1.87 CurrentItemFlags; checked by ABI probe and actual PushItemFlag export.
    return !ctx||(*(u32*)(ctx+0x3ED8)&4)!=0;
}
#include "network_transport.inc"
#include "server_connection.inc"
static bool rate_widget(const char*label){
    return contains(label,"_sr_")||contains(label,"_sr_sel_")||contains(label,"_sr_select_")||
        starts_with(label,"##sdrplay_sr")||contains(label,"sample_rate")||contains(label,"samplerate")||
        contains(label,"_source_sr")||contains(label,"samp_rate");
}
struct RateReading { char source[256],key[256],items[8192];int selected,count,numericHz;bool numeric,valid; };
static RateReading g_rate={};
struct DeviceReading { char source[256],description[512],key[256],items[8192];int selected,count;bool valid; };
static DeviceReading g_device={};
struct DeferredRate { char source[256];int index,hz;bool ready; };
static DeferredRate g_deferred_rate={};
struct DeferredDevice { char source[256],key[256];int index;bool ready; };
static DeferredDevice g_deferred_device={};
static bool device_widget(const char*l){
    return l&&(contains(l,"dev_sel")||contains(l,"device_select")||contains(l,"device_sel"));
}
static void defer_device_change(const char*key,int index){
    if(!g_supported||!g_enabled||!device_widget(key)||index<0||index>255)return;
    g_deferred_device={};
    c_copy(g_deferred_device.source,sizeof(g_deferred_device.source),string_chars(core_selected_source));
    c_copy(g_deferred_device.key,sizeof(g_deferred_device.key),key);
    g_deferred_device.index=index;g_deferred_device.ready=true;
}
static bool g_remote_rate_draw=false;
static void defer_rate_change(int index,int hz=0){
    g_deferred_rate={};c_copy(g_deferred_rate.source,sizeof(g_deferred_rate.source),string_chars(core_selected_source));
    g_deferred_rate.index=index;g_deferred_rate.hz=hz;g_deferred_rate.ready=true;
}
static void observe_device(const char*l,int selected,const char*items,usize limit=8192){
    if(!g_supported||!g_enabled||!l||!items||selected<0||selected>255||!device_widget(l))return;
    usize at=0;int count=0;
    while(at<limit&&items[at]&&count<256){
        while(at<limit&&items[at])at++;
        if(at>=limit||at+2>sizeof(g_device.items))return;
        at++;count++;
    }
    if(selected>=count||at>=limit||at+1>=sizeof(g_device.items))return;
    g_device={};g_device.selected=selected;g_device.count=count;g_device.valid=true;
    c_copy(g_device.source,sizeof(g_device.source),string_chars(core_selected_source));
    c_copy(g_device.key,sizeof(g_device.key),l);
    for(usize i=0;i<at;i++)g_device.items[i]=items[i];
    g_device.items[at]=g_device.items[at+1]=0;
    const char*p=g_device.items;
    for(int i=0;i<selected;i++)p+=c_len(p)+1;
    c_copy(g_device.description,sizeof(g_device.description),p);
}
static void observe_rate(const char*l,int selected,const char*items,usize limit=8192){
    if(!g_supported||!g_enabled||!rate_widget(l)||!items||selected<0)return;
    usize at=0;int count=0;
    while(at<limit&&items[at]&&count<256){while(at<limit&&items[at])at++;if(at>=limit||at+2>sizeof(g_rate.items))return;at++;count++;}
    if(selected>=count||at>=limit||at+1>=sizeof(g_rate.items))return;
    g_rate={};g_rate.selected=selected;g_rate.count=count;g_rate.valid=true;
    c_copy(g_rate.source,sizeof(g_rate.source),string_chars(core_selected_source));c_copy(g_rate.key,sizeof(g_rate.key),l);
    for(usize i=0;i<at;i++)g_rate.items[i]=items[i];g_rate.items[at]=g_rate.items[at+1]=0;
}
static bool device_edit_enabled(const char*l){
    return g_supported&&g_enabled&&device_widget(l)&&!*(bool*)core_main_window&&
        (window_isPlaying(core_main_window)||g_source_lifecycle.started||source_request_matches(6));
}
static bool rate_edit_enabled(const char*l){
    return g_supported&&g_enabled&&rate_widget(l)&&!*(bool*)core_main_window&&
        (window_isPlaying(core_main_window)||g_source_lifecycle.started||(g_remote_rate_draw&&source_request_matches(2)));
}
static bool gain_widget(const char*label){
    static const char*prefixes[]={"##_rtlsdr_gain_","##_gain_select_","##_airspy_sens_gain_",
        "##_airspy_lin_gain_","##_airspy_lna_gain_","##_hackrf_lna_","##_pluto_gain__",
        "##_gain_sel_","##sdrplay_gain"};
    for(const char*p:prefixes)if(starts_with(label,p))return true;
    return false;
}
static int primary_gain_control(){
    // Match Shift+Up/Down, including SDRplay IF GR rather than its LNA widget.
    for(int pass=0;pass<2;pass++)for(int i=0;i<g_gain.count;i++){
        const GainValue&value=g_gain.values[i];
        if(value.control&&value.minimum!=value.maximum&&(pass||gain_widget(value.key)))return i;
    }
    return -1;
}
struct GainPosition { int selected,position,maximum;float signedStep; };
static GainPosition gain_position(){
    GainPosition result={primary_gain_control(),0,0,0};
    if(result.selected<0)return result;
    const GainValue&value=g_gain.values[result.selected];
    result.signedStep=value.maximum>=value.minimum?value.step:-value.step;
    if(!(result.signedStep!=0))return result;
    float maximum=(value.maximum-value.minimum)/result.signedStep;
    if(!(maximum>=0&&maximum<=1000000))return result;
    result.maximum=(int)(maximum+0.5f);
    result.position=(int)((value.value-value.minimum)/result.signedStep+0.5f);
    if(result.position<0)result.position=0;
    if(result.position>result.maximum)result.position=result.maximum;
    return result;
}
static bool gain_caption(char*out,usize size,bool=false){
    const char*source=string_chars(core_selected_source);if(!*source)return false;
    if(!c_equal(g_gain.source,source)){c_copy(out,size,"-- Gain");return true;}
    if(g_gain.tunerAuto||g_gain.otherAuto){c_copy(out,size,"AUTO Gain");return true;}
    GainPosition range=gain_position();
    if(range.selected<0||!range.maximum){c_copy(out,size,g_gain.rtlAuto?"AUTO Gain":"-- Gain");return true;}
    char limit[32];u64str((u64)range.position,out,size);c_append(out,size,"/");
    u64str((u64)range.maximum,limit,sizeof(limit));c_append(out,size,limit);c_append(out,size," Gain");
    if(g_gain.rtlAuto)c_append(out,size," (RTL AGC)");
    return true;
}
static double rate_hz(const char*s,usize size){
    usize i=0;while(i<size&&s[i]==' ')i++;double value=0,decimal=0.1;bool digits=false,dot=false;
    for(;i<size;i++){
        char c=s[i];if(c>='0'&&c<='9'){digits=true;if(dot){value+=(c-'0')*decimal;decimal*=0.1;}else value=value*10+c-'0';}
        else if(c=='.'&&!dot)dot=true;else break;
    }
    while(i<size&&s[i]==' ')i++;
    if(i<size){if(s[i]=='M'||s[i]=='m')value*=1e6;else if(s[i]=='K'||s[i]=='k')value*=1e3;else if(s[i]=='G'||s[i]=='g')value*=1e9;}
    return digits&&value>0&&value<=1e12?value:0;
}
static int next_rate(const char*items,usize limit,int selected,int direction){
    if(!items||selected<0)return selected;
    double rates[256];int count=0;usize at=0;
    while(at<limit&&items[at]&&count<256){
        usize begin=at;while(at<limit&&items[at])at++;if(at==limit)return selected;
        rates[count++]=rate_hz(items+begin,at-begin);at++;
    }
    if(selected>=count||rates[selected]<=0)return selected;
    int best=selected;double current=rates[selected];
    for(int i=0;i<count;i++){
        if(direction>0&&rates[i]>current&&(best==selected||rates[i]<rates[best]))best=i;
        if(direction<0&&rates[i]>0&&rates[i]<current&&(best==selected||rates[i]>rates[best]))best=i;
    }
    return best;
}
static bool apply_device_widget(const char*label,int*v,bool disabled){
    if(!source_request_matches(6)||!device_widget(label)||!v||disabled||
       !c_equal(label,g_source_request.key))return false;
    g_source_request.done=true;
    if(*v==g_source_request.value)return false;
    *v=(int)g_source_request.value;return true;
}
static bool apply_rate_widget(const char*label,int*v,const char*items,usize size,bool disabled){
    if(!source_request_matches(2)||!rate_widget(label)||disabled||!v)return false;
    int next=g_source_request.direction?next_rate(items,size,*v,g_source_request.direction):(int)g_source_request.value;
    if(next<0||(!g_source_request.direction&&next>=g_rate.count))return false;g_source_request.done=true;
    if(next==*v)return false;*v=next;return true;
}
static bool apply_gain_int(const char*label,int*v,int minimum,int maximum,bool disabled){
    bool absolute=source_request_matches(3)&&c_equal(label,g_source_request.key);
    if((!absolute&&(!source_request_matches(1)||!gain_widget(label)))||disabled||!v)return false;
    int low=minimum<maximum?minimum:maximum,high=minimum>maximum?minimum:maximum;
    int step=g_source_request.direction*(maximum>=minimum?1:-1);
    long long next=absolute?(long long)g_source_request.value:(long long)*v+step;if(next<low)next=low;if(next>high)next=high;
    g_source_request.done=true;if(next==*v)return false;*v=(int)next;return true;
}
static bool apply_gain_float(const char*label,float*v,float minimum,float maximum,float step,bool disabled){
    bool absolute=source_request_matches(3)&&c_equal(label,g_source_request.key);
    if((!absolute&&(!source_request_matches(1)||!gain_widget(label)))||disabled||!v)return false;
    if(!(step>0)||!(minimum==minimum)||!(maximum==maximum)||!(*v==*v))return false;
    float low=minimum<maximum?minimum:maximum,high=minimum>maximum?minimum:maximum;
    float next=*v+g_source_request.direction*step*(maximum>=minimum?1:-1);
    if(absolute){
        float requested=g_source_request.value;if(!(requested==requested))return false;
        float signedStep=maximum>=minimum?step:-step;
        float units=(requested-minimum)/signedStep;
        next=minimum+(float)(long long)(units+0.5f)*signedStep;
    }
    if(next<low)next=low;if(next>high)next=high;
    g_source_request.done=true;if(next==*v)return false;*v=next;return true;
}
static bool hook_sm_combo(const char*l,int*v,const char*items,int height){
    int before=v?*v:-1;
    bool enabled=rate_edit_enabled(l)||device_edit_enabled(l),disabled=widget_disabled();if(enabled)ui_PushItemFlag(4,false);
    bool changed=sm_Combo(l,v,items,height);if(enabled)ui_PopItemFlag();
    if(changed&&rate_widget(l)&&v&&!g_source_request.kind){defer_rate_change(*v);*v=before;changed=false;}
    if(changed&&g_supported&&g_enabled&&device_widget(l)&&v&&!g_source_request.kind){defer_device_change(l,*v);*v=before;changed=false;}
    if(v){observe_rate(l,*v,items);observe_device(l,*v,items);}
    changed=apply_agc_mode(l,v,disabled)||changed;
    if(v)observe_gain_mode(l,*v);
    return apply_device_widget(l,v,disabled&&!enabled)||apply_rate_widget(l,v,items,8192,disabled&&!enabled)||changed;
}
static bool hook_ui_combo(const char*l,int*v,const char*items,int height){
    int before=v?*v:-1;
    bool enabled=rate_edit_enabled(l)||device_edit_enabled(l),disabled=widget_disabled();if(enabled)ui_PushItemFlag(4,false);
    bool changed=ui_Combo(l,v,items,height);if(enabled)ui_PopItemFlag();
    if(changed&&rate_widget(l)&&v&&!g_source_request.kind){defer_rate_change(*v);*v=before;changed=false;}
    if(changed&&g_supported&&g_enabled&&device_widget(l)&&v&&!g_source_request.kind){defer_device_change(l,*v);*v=before;changed=false;}
    if(v){observe_rate(l,*v,items);observe_device(l,*v,items);}
    changed=apply_agc_mode(l,v,disabled)||changed;
    if(v)observe_gain_mode(l,*v);
    return apply_device_widget(l,v,disabled&&!enabled)||apply_rate_widget(l,v,items,8192,disabled&&!enabled)||changed;
}
static void format_msps(int hz,char*out,usize size){
    char whole[24],fraction[8];u64str((u64)hz/1000000,whole,sizeof(whole));c_copy(out,size,whole);
    int remainder=hz%1000000;
    if(remainder){for(int i=5;i>=0;i--){fraction[i]=(char)('0'+remainder%10);remainder/=10;}fraction[6]=0;
        int last=5;while(last>=0&&fraction[last]=='0')fraction[last--]=0;c_append(out,size,".");c_append(out,size,fraction);}
    c_append(out,size,"MHz");
}
static void observe_numeric_rate(const char*l,int hz){
    if(hz<1000)return;g_rate={};g_rate.valid=g_rate.numeric=true;g_rate.numericHz=hz;
    c_copy(g_rate.source,sizeof(g_rate.source),string_chars(core_selected_source));c_copy(g_rate.key,sizeof(g_rate.key),l);
    static const int values[]={48000,96000,192000,250000,384000,500000,1000000,1024000,2000000,2048000,2400000,3200000,4000000,8000000,16000000,32000000,64000000};
    usize at=0;char value[40];format_msps(hz,value,sizeof(value));c_copy(g_rate.items,sizeof(g_rate.items),value);at=c_len(value)+1;g_rate.count=1;
    for(int rate:values)if(rate!=hz){format_msps(rate,value,sizeof(value));c_copy(g_rate.items+at,sizeof(g_rate.items)-at,value);at+=c_len(value)+1;g_rate.count++;}
    g_rate.items[at]=0;g_rate.selected=0;
}
static bool rate_integer_result(const char*l,int*v,bool changed){
    if(!g_supported||!g_enabled||!rate_widget(l)||!v)return server_observe_port(l,v,changed);
    observe_numeric_rate(l,*v);
    if(source_request_matches(2)&&!widget_disabled()){
        int index=g_source_request.direction?next_rate(g_rate.items,sizeof(g_rate.items),0,g_source_request.direction):(int)g_source_request.value;
        const char*p=g_rate.items;for(int i=0;i<index&&*p;i++)p+=c_len(p)+1;
        int hz=g_source_request.rateHz?g_source_request.rateHz:(int)(rate_hz(p,c_len(p))+0.5);g_source_request.done=true;
        if(index>=0&&index<g_rate.count&&hz>=1000&&hz!=*v){*v=hz;changed=true;}
    }
    if(changed){g_numeric_rate_apply=true;observe_numeric_rate(l,*v);}
    return changed;
}
static bool hook_server_port(const char*l,int*v,int step,int fast,int flags){
    int before=v?*v:0;bool rate=rate_widget(l);
    bool enable=rate_edit_enabled(l);if(enable)ui_PushItemFlag(4,false);
    bool changed=ui_InputInt(l,v,step,fast,rate?flags|32:flags);if(enable)ui_PopItemFlag();
    if(changed&&rate&&v&&!g_source_request.kind){defer_rate_change(0,*v<1000?1000:*v);*v=before;changed=false;}
    return rate_integer_result(l,v,changed);
}
static bool hook_sm_server_port(const char*l,int*v,int step,int fast,int flags){
    int before=v?*v:0;bool rate=rate_widget(l);
    bool enable=rate_edit_enabled(l);if(enable)ui_PushItemFlag(4,false);
    bool changed=sm_InputInt(l,v,step,fast,rate?flags|32:flags);if(enable)ui_PopItemFlag();
    if(changed&&rate&&v&&!g_source_request.kind){defer_rate_change(0,*v<1000?1000:*v);*v=before;changed=false;}
    return rate_integer_result(l,v,changed);
}
static bool hook_sm_gain(const char*l,int*v,int lo,int hi,int format,int flags){
    bool disabled=widget_disabled(),changed=sm_SliderInt(l,v,lo,hi,format,flags);
    bool applied=apply_gain_int(l,v,lo,hi,disabled);
    if(v&&(!starts_with(l,"##_rtlsdr_gain_")||hi>=lo))observe_gain(l,*v,hi,false,format==2,0,disabled);
    if(v)observe_gain_control(l,(float)*v,(float)lo,(float)hi,1,1);
    return applied||changed;
}
static bool hook_ui_gain(const char*l,int*v,int lo,int hi,const char*format,int flags){
    bool disabled=widget_disabled(),changed=ui_SliderInt(l,v,lo,hi,format,flags);
    bool applied=apply_gain_int(l,v,lo,hi,disabled);
    if(v&&(!starts_with(l,"##_rtlsdr_gain_")||hi>=lo))observe_gain(l,*v,hi,false,contains(format,"dB"),format,disabled);
    if(v)observe_gain_control(l,(float)*v,(float)lo,(float)hi,1,1);
    if((applied||changed)&&gain_display_widget(l))g_gain_refresh=true;
    return applied||changed;
}
static bool hook_sm_float(const char*l,float*v,float lo,float hi,int format,int flags){
    bool disabled=widget_disabled(),changed=sm_SliderFloat(l,v,lo,hi,format,flags);
    bool applied=apply_gain_float(l,v,lo,hi,1,disabled);
    if(v){observe_gain(l,*v,0,true,format>=8,0,disabled);observe_gain_control(l,*v,lo,hi,1,2);}
    return applied||changed;
}
static bool hook_sm_steps(const char*l,float*v,float lo,float hi,float step,int format){
    bool disabled=widget_disabled(),changed=sm_SliderFloatSteps(l,v,lo,hi,step,format);
    bool applied=apply_gain_float(l,v,lo,hi,step,disabled);
    if(v){observe_gain(l,*v,0,true,format>=8,0,disabled);observe_gain_control(l,*v,lo,hi,step,2);}
    return applied||changed;
}
static bool hook_sm_checkbox(const char*l,bool*v){bool disabled=widget_disabled(),changed=sm_Checkbox(l,v);changed=apply_agc_checkbox(l,v,disabled)||changed;if(v)observe_gain_agc(l,*v);return changed;}
static bool hook_ui_checkbox(const char*l,bool*v){bool disabled=widget_disabled(),changed=ui_Checkbox(l,v);changed=apply_agc_checkbox(l,v,disabled)||changed;if(v)observe_gain_agc(l,*v);return changed;}
struct SmElement { int type,step;bool sync,b;int i;float f;CoreString str; };
struct SmList { SmElement*first,*last,*capacity; };
static_assert(sizeof(SmElement)==56,"SmGui DrawListElem MSVC x64 ABI");
static void hook_remote_draw(void*list,void*diffId,void*diffValue,bool*sync){
    g_remote_rate_draw=true;sm_DrawListDraw(list,diffId,diffValue,sync);g_remote_rate_draw=false;
    SmList*dl=(SmList*)list;if(!dl||!dl->first||dl->last<dl->first||dl->last-dl->first>20000)return;
    begin_gain_sample(true);
    bool hasDiff=*(u64*)((u8*)diffId+16)!=0;
    if(hasDiff&&g_supported&&g_enabled&&!g_source_request.kind){
        const char*label=string_chars(diffId);
        if(device_widget(label)&&((SmElement*)diffValue)->type==2){
            defer_device_change(label,((SmElement*)diffValue)->i);
            CoreString empty={};*(u64*)(empty.bytes+24)=15;
            core_stringDestroy(diffId);core_stringCopy(diffId,&empty);
            *sync=false;hasDiff=false;
        }
    }
    bool allowEdit=g_source_request.kind&&!g_source_request.done&&!hasDiff;
    // A remote gain slider is disabled by the server when Tuner AGC is active.
    // Also check its checkbox so stale/malformed enabled-gain packets cannot override AGC.
    bool agc=false;
    for(SmElement*p=dl->first;dl->last-p>2;p++)if(p->type==0&&p->step==0x8B&&p[1].type==4&&p[2].type==1){
        const char*label=string_chars(&p[1].str);
        bool value=p[2].b;
        if(hasDiff&&c_equal(label,string_chars(diffId))&&((SmElement*)diffValue)->type==1)value=((SmElement*)diffValue)->b;
        observe_gain_agc(label,value);
        if(starts_with(label,"Tuner AGC")||contains(label,"tuner_agc")||starts_with(label,"IF AGC")||starts_with(label,"AGC"))agc=agc||value;
    }
    int disabled=0;
    for(SmElement*p=dl->first;p<dl->last;p++){
        if(p->type!=0)continue;
        if(p->step==2){disabled++;continue;}if(p->step==3){if(disabled)disabled--;continue;}
        bool changed=false;int type=2;
        if(p->step==0x80&&dl->last-p>4&&p[1].type==4&&p[2].type==2&&p[3].type==4){
            const char*label=string_chars(&p[1].str);int value=p[2].i;
            if(hasDiff&&c_equal(label,string_chars(diffId))&&((SmElement*)diffValue)->type==2)value=((SmElement*)diffValue)->i;
            observe_gain_mode(label,value);
            observe_rate(label,value,string_chars(&p[3].str),(usize)*(u64*)((u8*)&p[3].str+16));
            observe_device(label,value,string_chars(&p[3].str),(usize)*(u64*)((u8*)&p[3].str+16));
            changed=allowEdit&&(apply_agc_mode(label,&p[2].i,disabled!=0)||
                apply_device_widget(label,&p[2].i,disabled!=0&&!source_request_matches(6))||
                apply_rate_widget(label,&p[2].i,string_chars(&p[3].str),
                (usize)*(u64*)((u8*)&p[3].str+16),disabled!=0&&!source_request_matches(2)));
        }else if(p->step==0x8B&&dl->last-p>2&&p[1].type==4&&p[2].type==1){
            const char*label=string_chars(&p[1].str);type=1;
            changed=allowEdit&&apply_agc_checkbox(label,&p[2].b,disabled!=0);
            if(changed)observe_gain_agc(label,p[2].b);
        }else if(p->step==0x88&&dl->last-p>6&&p[1].type==4&&p[2].type==2&&p[3].type==2&&p[4].type==2){
            const char*label=string_chars(&p[1].str);
            changed=allowEdit&&apply_gain_int(label,&p[2].i,p[3].i,p[4].i,disabled||agc||g_gain.otherAuto);
            int value=p[2].i;
            if(hasDiff&&c_equal(label,string_chars(diffId))&&((SmElement*)diffValue)->type==2)value=((SmElement*)diffValue)->i;
            observe_gain(label,value,p[4].i,false,p[5].type==2&&p[5].i==2,0,disabled!=0);
            observe_gain_control(label,(float)value,(float)p[3].i,(float)p[4].i,1,1);
        }else if((p->step==0x89||p->step==0x8C)&&dl->last-p>6&&p[1].type==4&&p[2].type==3&&p[3].type==3&&p[4].type==3){
            type=3;float step=p->step==0x89&&p[5].type==3?p[5].f:1.0f;
            const char*label=string_chars(&p[1].str);
            changed=allowEdit&&apply_gain_float(label,&p[2].f,p[3].f,p[4].f,step,disabled||agc||g_gain.otherAuto);
            float value=p[2].f;
            if(hasDiff&&c_equal(label,string_chars(diffId))&&((SmElement*)diffValue)->type==3)value=((SmElement*)diffValue)->f;
            int formatIndex=p->step==0x89?6:5;
            observe_gain(label,value,0,true,p[formatIndex].type==2&&p[formatIndex].i>=8,0,disabled!=0);
            observe_gain_control(label,value,p[3].f,p[4].f,step,2);
        }
        if(changed){
            core_stringDestroy(diffId);core_stringCopy(diffId,&p[1].str);
            SmElement*value=(SmElement*)diffValue;value->type=type;value->b=p[2].b;value->i=p[2].i;value->f=p[2].f;
            *sync=p->sync;return;
        }
    }
}

struct SourceImport { void*module;void**slot;void*original;void*replacement; };
static SourceImport g_source_imports[512];static int g_source_import_count=0;
static bool replace_import(void**slot,void*value){
    u32 protection=0;if(!VirtualProtect(slot,sizeof(void*),4,&protection))return false;
    *slot=value;u32 ignored=0;VirtualProtect(slot,sizeof(void*),protection,&ignored);return true;
}
static bool hook_radio_snap(const char*,int*,int,int,int);
static void install_source_imports(){
    void*modules[256];u32 bytes=0;
    if(!K32EnumProcessModules(GetCurrentProcess(),modules,sizeof(modules),&bytes))return;
    int count=(int)(bytes/sizeof(void*));if(count>256)count=256;
    void*originals[]={(void*)sm_Combo,(void*)ui_Combo,(void*)sm_SliderInt,(void*)ui_SliderInt,
        (void*)sm_SliderFloat,(void*)sm_SliderFloatSteps,(void*)sm_DrawListDraw,(void*)sm_Checkbox,(void*)ui_Checkbox,
        (void*)ui_InputText,(void*)ui_InputInt,(void*)ui_Button,(void*)sm_InputText,(void*)sm_InputInt,(void*)sm_Button};
    void*hooks[]={(void*)hook_sm_combo,(void*)hook_ui_combo,(void*)hook_sm_gain,(void*)hook_ui_gain,
        (void*)hook_sm_float,(void*)hook_sm_steps,(void*)hook_remote_draw,(void*)hook_sm_checkbox,(void*)hook_ui_checkbox,
        (void*)hook_server_host,(void*)hook_server_port,(void*)hook_server_button,(void*)hook_sm_server_host,(void*)hook_sm_server_port,(void*)hook_sm_server_button};
    for(int m=0;m<count;m++){
        // Already replaced slots no longer equal the original import. Scanning
        // them is harmless and retries slots missed by a partial protection failure.
        u8*base=(u8*)modules[m];u32 nt=*(u32*)(base+0x3C);
        if(*(unsigned short*)base!=0x5A4D||nt>4096||*(u32*)(base+nt)!=0x4550||*(unsigned short*)(base+nt+24)!=0x20B)continue;
        u32 size=*(u32*)(base+nt+24+56),imports=*(u32*)(base+nt+24+120);
        auto info=(const ModuleManager::ModuleInfo_t*)GetProcAddress(modules[m],"_INFO_");
        usize address=(usize)info,start=(usize)base;
        if(!info||address<start||address-start>size||size-(address-start)<sizeof(*info))continue;
        usize name=(usize)info->name;
        if(name<start||name-start>=size)continue;
        usize length=0,available=size-(name-start);
        while(length<available&&length<255&&info->name[length])length++;
        if(length==available||length==255)continue;
        bool radio=c_equal(info->name,"radio");
        if(!radio&&!contains(info->name,"_source"))continue;
        int transportSlot=-1;
        if(!imports||imports+20>size)continue;
        for(u32 d=imports;d+20<=size&&*(u32*)(base+d+12);d+=20){
            u32 iat=*(u32*)(base+d+16);if(!iat||iat+8>size)continue;
            u32 lookup=*(u32*)(base+d),dllName=*(u32*)(base+d+12);
            bool coreImport=dllName<size&&size-dllName>=15&&c_equal((const char*)base+dllName,"sdrpp_core.dll");
            for(u32 off=iat;off+8<=size;off+=8){
                void**slot=(void**)(base+off);if(!*slot)break;
                const char*symbol=0;u64 lookupOffset=(u64)lookup+off-iat;
                if(coreImport&&lookup&&lookupOffset+8<=size){
                    u64 nameRva=*(u64*)(base+lookupOffset);
                    if(!(nameRva>>63)&&nameRva+2<size){
                        usize symbolLength=0;const char*symbolName=(const char*)base+nameRva+2;
                        while(nameRva+2+symbolLength<size&&symbolLength<512&&symbolName[symbolLength])symbolLength++;
                        if(nameRva+2+symbolLength<size&&symbolLength<512)symbol=symbolName;
                    }
                }
                if(radio){
                    if(*slot==(void*)ui_InputInt&&g_source_import_count<512){
                        SourceImport h={modules[m],slot,*slot,(void*)hook_radio_snap};
                        if(replace_import(slot,h.replacement))g_source_imports[g_source_import_count++]=h;
                    }continue;
                }
                if(*slot==(void*)ui_TextColored&&g_source_import_count<512){
                    SourceImport h={modules[m],slot,*slot,(void*)hook_server_text_colored};
                    if(replace_import(slot,h.replacement))g_source_imports[g_source_import_count++]=h;
                    continue;
                }
                bool patched=false;
                for(int k=0;k<15;k++)if(*slot==originals[k]&&g_source_import_count<512){
                    SourceImport h={modules[m],slot,*slot,hooks[k]};
                    if(replace_import(slot,h.replacement)){g_source_imports[g_source_import_count++]=h;patched=true;}
                    break;
                }
                if(!patched)for(int k=0;k<12;k++)if((*slot==TRANSPORT_ORIGINALS[k]||(symbol&&c_equal(symbol,TRANSPORT_SYMBOLS[k])))&&g_source_import_count<512){
                    if(transportSlot<0)transportSlot=transport_module_slot(modules[m],info->name);
                    if(transportSlot<0)break;
                    if(*slot==TRANSPORT_HOOKS[transportSlot][k])break;
                    SourceImport h={modules[m],slot,*slot,TRANSPORT_HOOKS[transportSlot][k]};
                    if(replace_import(slot,h.replacement)){g_transport[transportSlot].network=true;g_source_imports[g_source_import_count++]=h;g_transport_enabled=true;pin_transport_code();}
                    break;
                }
            }
        }
    }
}
static void restore_source_imports(){
    restore_source_lifecycle(true);__atomic_store_n(&g_transport_enabled,false,__ATOMIC_RELEASE);
    for(int i=g_source_import_count-1;i>=0;i--){
        SourceImport&h=g_source_imports[i];void*module=0;
        if(GetModuleHandleExA(6,(const char*)h.slot,&module)&&module==h.module&&*h.slot==h.replacement)
            replace_import(h.slot,h.original);
    }
    g_source_import_count=0;
    for(int i=0;i<TRANSPORT_SLOTS;i++){__atomic_store_n(&g_transport[i].alive,0,__ATOMIC_RELEASE);g_transport[i].discovery=0;}
    g_gain={};g_gain_next_probe=g_gain_next_hooks=0;g_gain_refresh=false;
    g_server={};g_endpoint_dialog={};g_server_select_requested=false;
    g_rate={};g_device={};g_deferred_rate={};g_deferred_device={};g_numeric_rate_apply=false;
}
static void finish_source_request(){
    bool resume=g_source_request.resume&&c_equal(g_source_request.source,string_chars(core_selected_source));
    bool primed=g_source_request.resumePrimed&&c_equal(g_source_request.source,string_chars(core_selected_source));
    bool hardwareChange=g_source_request.kind==2||g_source_request.kind==6;
    g_source_request={};
    if(resume&&!window_isPlaying(core_main_window)&&!core_main_window[1])window_setPlayState(core_main_window,true);
    else if(primed)source_start(core_source_manager);
    if(hardwareChange)zoom_finish_source_transition();
}
static void request_source_step(int kind,int direction){
    if(g_source_request.kind)return;
    u64 size=*(u64*)(core_selected_source+16);if(!size||size>255)return;
    install_source_imports();g_source_request.kind=kind;g_source_request.direction=direction;
    c_copy(g_source_request.source,sizeof(g_source_request.source),string_chars(core_selected_source));
    if(kind==2||kind==6){
        zoom_prepare_source_transition();
        if(window_isPlaying(core_main_window)){
            g_source_request.resume=true;window_setPlayState(core_main_window,false);
        }else if(g_source_lifecycle.started){g_source_request.resumePrimed=true;source_stop(core_source_manager);}
    }
}
static bool request_source_value(int kind,const char*key,float value,bool automatic=false){
    if(g_source_request.kind||!key||!*key||!(value==value))return false;
    u64 size=*(u64*)(core_selected_source+16);if(!size||size>255)return false;
    install_source_imports();g_source_request.kind=kind;g_source_request.value=value;g_source_request.automatic=automatic;
    c_copy(g_source_request.source,sizeof(g_source_request.source),string_chars(core_selected_source));
    c_copy(g_source_request.key,sizeof(g_source_request.key),key);return true;
}
static void visit_source_menu(bool selectServer=false){
    ui_SetNextWindowPos(ImVec2(-10000,-10000),1,ImVec2());ui_SetNextWindowSize(ImVec2(400,2000),1);
    ui_Begin("##Essential++ source shortcuts",0,1|2|4|16|32|128|256|512|4096|8192|262144|524288);
    bool previous=g_source_hidden_visit;g_source_hidden_visit=true;
    // Observation must not advance/reset a source's factory frame-time
    // counter a second time. The real visible panel owns that measurement.
    float*dt=(float*)((u8*)ui_GetIO()+0x10);float originalDelta=*dt;*dt=0;
    if(selectServer)source_menuDraw(0);else source_showMenu(core_source_manager);
    *dt=originalDelta;g_source_hidden_visit=previous;
    ui_End();
}
static void process_source_request(){
    if(!g_source_request.kind)return;
    if(g_supported&&g_enabled&&our_window_focused()&&
        c_equal(g_source_request.source,string_chars(core_selected_source))&&!g_source_request.done){
        // Call the selected source once even when its sidebar is closed. No mouse/keyboard
        // input enters this off-screen window; all callbacks still execute on the GUI thread.
        visit_source_menu();
    }
    g_gain_refresh=true;
    finish_source_request();
}
static void refresh_gain_reading(){
    if(!g_supported||!g_enabled)return;
    const char*source=string_chars(core_selected_source);double now=ui_GetTime();
    bool sourceChanged=!c_equal(g_gain.source,source);
    if(!c_equal(g_rate.source,source))g_rate={};
    if(!c_equal(g_device.source,source))g_device={};
    server_prepare_source();
    if(sourceChanged){g_gain={};c_copy(g_gain.source,sizeof(g_gain.source),source);g_gain.frame=-1;g_gain_next_probe=0;}
    if(!*source)return;
    if(sourceChanged||now>=g_gain_next_hooks){install_source_imports();g_gain_next_hooks=now+1;}
    if(!g_gain_refresh&&g_gain.frame==ui_GetFrameCount())return;
    if(!g_gain_refresh&&!sourceChanged&&now<g_gain_next_probe)return;
    if(*(bool*)core_main_window||ui_IsPopupOpen(0,384)||!our_window_focused())return;
    // Sampling the hidden source menu can be expensive and steal the active
    // ImGui widget. Never do it during ANY visible slider/VFO mouse gesture.
    if((GetAsyncKeyState(0x01)&0x8000)||ui_IsAnyItemActive()){
        g_gain_next_probe=now+0.15;return;
    }
    // Observe native widgets at most 5 times/second when the sidebar is hidden.
    // No source values are changed; hardware callbacks only run for real edits.
    begin_gain_sample(true);g_gain_refresh=false;
    visit_source_menu();g_gain_next_probe=now+0.2;
}

static bool server_bridge_ready(){
    if(!server_selected())return false;
    int mask=0;
    for(int i=0;i<g_source_import_count;i++){
        SourceImport&h=g_source_imports[i];void*module=0;
        if(!GetModuleHandleExA(6,(const char*)h.slot,&module)||module!=h.module||*h.slot!=h.replacement)continue;
        if(h.replacement==(void*)hook_server_host||h.replacement==(void*)hook_sm_server_host)mask|=1;
        if(h.replacement==(void*)hook_server_port||h.replacement==(void*)hook_sm_server_port)mask|=2;
        if(h.replacement==(void*)hook_server_button||h.replacement==(void*)hook_sm_server_button)mask|=4;
    }
    return (mask&3)==3||c_equal(string_chars(core_selected_source),"Hermes")||transport_selected_slot()>=0;
}
static void server_probe(){
    g_gain_refresh=true;refresh_gain_reading();server_prepare_source();
    server_observe_transport();server_sync_address();server_sample_download();
}
static void server_native_action(int direction,const char*host,int port){
    g_source_request={};g_source_request.kind=5;g_source_request.direction=direction;
    c_copy(g_source_request.source,sizeof(g_source_request.source),string_chars(core_selected_source));
    if(host){g_source_request.setEndpoint=true;c_copy(g_source_request.host,sizeof(g_source_request.host),host);g_source_request.port=port;}
    process_source_request();server_probe();
}
static bool request_server_connection(bool connect){
    if(!g_supported||!g_enabled||!server_selected()||*(bool*)core_main_window||!our_window_focused()||g_source_request.kind)return false;
    server_prepare_source();char host[1024]={};int port=0;bool edit=connect&&g_server.dirty;
    if(edit&&!parse_server_endpoint(g_server.address,host,sizeof(host),port)){
        c_copy(g_server.error,sizeof(g_server.error),l10n("Nieprawidlowy adres IP lub host[:port]."));g_server.status=SERVER_ERROR;return false;
    }
    install_source_imports();server_probe();
    if(!server_bridge_ready()){c_copy(g_server.error,sizeof(g_server.error),l10n("Wybrany klient nie udostepnia polaczenia."));g_server.status=SERVER_ERROR;return false;}
    bool buttons=g_server.hasButton;g_server.status=SERVER_CONNECTING;
    if(window_isPlaying(core_main_window))window_setPlayState(core_main_window,false);
    if(!connect||(edit&&g_server.connected)){
        if(buttons)server_native_action(-1,0,0);else source_stop(core_source_manager);
        g_server.connected=false;
    }
    if(connect&&!g_server.connected){
        if(buttons)server_native_action(1,edit?host:0,port);
        else{
            if(edit)server_native_action(0,host,port);
            if(!install_source_lifecycle()){g_server.status=SERVER_ERROR;c_copy(g_server.error,sizeof(g_server.error),l10n("Brak aktywnego klienta sieciowego."));return false;}
            source_start(core_source_manager);g_server.connected=transport_alive(transport_selected_slot());
        }
        g_server.dirty=false;server_sync_address();
    }
    if(connect&&!g_server.connected){
        if(!buttons&&g_source_lifecycle.started)source_stop(core_source_manager);
        g_server.status=SERVER_ERROR;c_copy(g_server.error,sizeof(g_server.error),l10n("Nie polaczono. Sprawdz adres, port i serwer."));return false;
    }
    g_server.error[0]=0;g_server.status=connect?SERVER_CONNECTED:SERVER_IDLE;
    g_server.transferReady=false;g_server.lastBytes=transport_bytes(transport_selected_slot());g_server.lastUpload=transport_uploaded(transport_selected_slot());g_server.lastSample=ui_GetTime();return true;
}
static void queue_server_connection(bool connect){
    g_server.pending=true;g_server.pendingConnect=connect;g_server.pendingFrame=ui_GetFrameCount()+2;
    g_server.status=connect?SERVER_CONNECTING:SERVER_IDLE;g_server.error[0]=0;
}
static void process_server_connection(){
    server_prepare_source();
    if(g_server.pending&&ui_GetFrameCount()>=g_server.pendingFrame){
        bool connect=g_server.pendingConnect,resume=g_server.resumeAfterConnect;
        g_server.pending=false;g_server.resumeAfterConnect=false;
        bool success=request_server_connection(connect);
        if(success&&connect&&resume&&!window_isPlaying(core_main_window)&&!core_main_window[1])
            window_setPlayState(core_main_window,true);
    }
}
static bool request_source_rate(int index){
    if(!g_rate.valid||index<0||index>=g_rate.count||g_source_request.kind)return false;
    int hz=0;if(g_rate.numeric){const char*p=g_rate.items;for(int i=0;i<index&&*p;i++)p+=c_len(p)+1;hz=(int)(rate_hz(p,c_len(p))+0.5);}
    request_source_step(2,0);if(g_source_request.kind!=2)return false;
    g_source_request.rateHz=hz;
    g_source_request.value=(float)index;c_copy(g_source_request.key,sizeof(g_source_request.key),g_rate.key);
    process_source_request();g_gain_refresh=true;refresh_gain_reading();return true;
}
static void process_deferred_rate(){
    if(!g_deferred_rate.ready)return;DeferredRate request=g_deferred_rate;g_deferred_rate={};
    if(!c_equal(request.source,string_chars(core_selected_source))||!g_enabled||!g_supported)return;
    if(request.hz){request_source_step(2,0);g_source_request.rateHz=request.hz;process_source_request();}
    else request_source_rate(request.index);
}

static void process_deferred_device(){
    if(!g_deferred_device.ready)return;
    DeferredDevice request=g_deferred_device;g_deferred_device={};
    if(!g_supported||!g_enabled||!c_equal(request.source,string_chars(core_selected_source))||g_source_request.kind)return;
    request_source_step(6,0);
    if(g_source_request.kind!=6)return;
    c_copy(g_source_request.key,sizeof(g_source_request.key),request.key);
    g_source_request.value=(float)request.index;
    process_source_request();g_gain_refresh=true;
}
