struct ToolbarFilterEditor { char target[256],text[32];bool focus,invalid; };
static ToolbarFilterEditor g_filter_editor={};
static ToolbarFilterEditor g_step_editor={};
struct ToolbarWheel { float remainder;int frame; };
static ToolbarWheel g_toolbar_wheel[3]={};
static void scroll_toolbar_receiver(int control){
    ToolbarWheel&state=g_toolbar_wheel[control];
    if(!ui_IsItemHovered(0)||!radio_interface_ready()||*(bool*)core_main_window){state={};return;}
    int frame=ui_GetFrameCount();if(frame!=state.frame+1)state.remainder=0;state.frame=frame;
    float wheel=get_mouse_wheel();if(wheel==0)return;
    // Consume this cell's wheel before the waterfall can act on the same event.
    clear_mouse_wheel();
    if((wheel>0&&state.remainder<0)||(wheel<0&&state.remainder>0))state.remainder=0;
    if(wheel>64)wheel=64;if(wheel< -64)wheel=-64;
    state.remainder+=wheel;int steps=(int)state.remainder;state.remainder-=steps;if(!steps)return;
    double frequency=selected_frequency();
    if(control==2){
        double value=vfo_snap(selected_vfo())+100.0*steps;
        if(value<1)value=1;if(value>2147483647)value=2147483647;
        set_radio_step((int)value);
    }else{
        if(control==0)change_filter(100.0*steps);
        else{int direction=steps>0?1:-1;for(int i=0;i<(steps>0?steps:-steps);i++)cycle_modulation(direction);}
        tune_frequency(frequency);
    }
}
static void open_toolbar_step(){
    void*v=selected_vfo();if(!v||!radio_interface_ready())return;
    c_copy(g_step_editor.target,sizeof(g_step_editor.target),string_chars(wf_selected_name()));
    u64str((u64)(vfo_snap(v)+0.5),g_step_editor.text,sizeof(g_step_editor.text));
    g_step_editor.focus=true;g_step_editor.invalid=false;ui_OpenPopup("##essential_step_hz_popup",0);
}
static bool apply_toolbar_step(){
    if(!c_equal(g_step_editor.target,string_chars(wf_selected_name())))return false;
    const char*p=g_step_editor.text;u64 value=0;
    return parse_uint(p,2147483647,value)&&!*p&&value>0&&set_radio_step((int)value);
}
static void open_toolbar_filter(){
    void*v=selected_vfo();if(!v||!radio_interface_ready())return;
    c_copy(g_filter_editor.target,sizeof(g_filter_editor.target),string_chars(wf_selected_name()));
    u64str((u64)(vfo_bandwidth(v)+0.5),g_filter_editor.text,sizeof(g_filter_editor.text));
    g_filter_editor.focus=true;g_filter_editor.invalid=false;ui_OpenPopup("##essential_filter_hz_popup",0);
}
static bool apply_toolbar_filter(){
    if(!c_equal(g_filter_editor.target,string_chars(wf_selected_name()))||!radio_interface_ready())return false;
    u64 hz=0;const char*s=g_filter_editor.text;if(!*s)return false;
    for(;*s;s++){if(*s<'0'||*s>'9')return false;hz=hz*10+(u64)(*s-'0');if(hz>1000000000)return false;}
    if(!hz)return false;double frequency=selected_frequency();float value=(float)hz;
    if(!radio_command(3,&value,0))return false;tune_frequency(frequency);return true;
}
static void toolbar_receiver_popups(){
    if(ui_BeginPopup("##essential_step_hz_popup",0)){
        if(!c_equal(g_step_editor.target,string_chars(wf_selected_name())))ui_CloseCurrentPopup();
        else{
            ui_TextUnformatted("Krok strojenia [Hz]",0);
            if(g_step_editor.focus){ui_SetKeyboardFocusHere(0);g_step_editor.focus=false;}
            ui_SetNextItemWidth(200*toolbar_scale());
            if(ui_InputText("##essential_step_hz",g_step_editor.text,sizeof(g_step_editor.text),1|32,0,0)){
                if(apply_toolbar_step())ui_CloseCurrentPopup();else g_step_editor.invalid=true;
            }
            if(g_step_editor.invalid)ui_TextUnformatted("Podaj dodatni krok w pelnych Hz.",0);
            if(ui_IsKeyPressed(K_ESCAPE,false))ui_CloseCurrentPopup();
        }ui_EndPopup();
    }
    if(ui_BeginPopup("##essential_modulation_popup",0)){
        g_tuning_input_suppressed_through=ui_GetFrameCount()+1;
        for(int i=0;i<8;i++){
            int mode=MODE_ORDER[i];
            if(ui_Selectable(modulation_name(mode),selected_modulation()==mode,0,ImVec2())){
                double frequency=selected_frequency();if(set_modulation(mode))tune_frequency(frequency);
                g_tuning_input_suppressed_through=ui_GetFrameCount()+3;
                ui_CloseCurrentPopup();
            }
        }ui_EndPopup();
    }
    if(ui_BeginPopup("##essential_filter_hz_popup",0)){
        if(!c_equal(g_filter_editor.target,string_chars(wf_selected_name())))ui_CloseCurrentPopup();
        else{
            ui_TextUnformatted("Filtr [Hz]",0);
            if(g_filter_editor.focus){ui_SetKeyboardFocusHere(0);g_filter_editor.focus=false;}
            ui_SetNextItemWidth(200*toolbar_scale());
            if(ui_InputText("##essential_filter_hz",g_filter_editor.text,sizeof(g_filter_editor.text),1|32,0,0)){
                if(apply_toolbar_filter())ui_CloseCurrentPopup();else g_filter_editor.invalid=true;
            }
            if(g_filter_editor.invalid)ui_TextUnformatted("Podaj dodatnia szerokosc w Hz.",0);
            if(ui_IsKeyPressed(K_ESCAPE,false))ui_CloseCurrentPopup();
        }ui_EndPopup();
    }
}
