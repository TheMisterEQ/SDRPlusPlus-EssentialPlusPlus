// Radio's normal Snap Interval callback updates the VFO and persists the value
// for the selected demodulator. The public ModCom API has no snap command.
struct RadioStepRequest { bool pending,done;int value;char target[256]; };
static RadioStepRequest g_radio_step={};
static bool hook_radio_snap(const char*label,int*value,int step,int fast,int flags){
    bool changed=ui_InputInt(label,value,step,fast,flags);
    if(!g_radio_step.pending||!starts_with(label,"##_radio_snap_")||
       !c_equal(label+c_len("##_radio_snap_"),g_radio_step.target)||widget_disabled())return changed;
    *value=g_radio_step.value;g_radio_step.done=true;return true;
}
static bool set_radio_step(int value){
    if(value<1||!radio_interface_ready()||*(bool*)core_main_window||!our_window_focused())return false;
    install_source_imports();
    // Menu::items map at +0xB8 is verified in native registerEntry. Its MSVC
    // nodes hold the string at +32, callback at +64 and context at +72.
    u8*head=*(u8**)(core_menu+0xB8);if(!head)return false;
    u8*node=*(u8**)(head+8);const char*name=string_chars(wf_selected_name());
    for(int limit=0;node&&node!=head&&!node[25]&&limit<128;limit++){
        const char*a=name,*b=string_chars(node+32);while(*a&&*a==*b){a++;b++;}
        int order=(int)(u8)*a-(int)(u8)*b;
        if(order){node=*(u8**)(node+(order<0?0:16));continue;}
        auto draw=*(void(**)(void*))(node+64);if(!draw)return false;
        g_radio_step={true,false,value,{}};c_copy(g_radio_step.target,sizeof(g_radio_step.target),name);
        ui_SetNextWindowPos(ImVec2(-10000,-10000),1,ImVec2());ui_SetNextWindowSize(ImVec2(400,2000),1);
        ui_Begin("##Essential radio step",0,1|2|4|16|32|128|256|512|4096|8192|262144|524288);
        draw(*(void**)(node+72));ui_End();bool done=g_radio_step.done;g_radio_step={};return done;
    }
    return false;
}
