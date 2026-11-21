// Essential V32 - panels, native slider extensions, throttled live waterfall contrast.
// All modifications are in memory and verified against the 2026-11-18 SDR++ 1.3.2 binary.
// The core PE image on disk is never modified.
static bool g_right_hover=false,g_left_hover=false;
static bool mouse_button_held(){return (GetAsyncKeyState(0x01)&0x8000)!=0;}
// Only the CUSTOM Contrast control uses a throttled historical recolour.
// Original SDR++ Zoom/Max/Min always follow SDR++'s own update policy.
// Reconstructing the backbuffer is O(visible_width * history_height), so
// never force that operation after native-slider mouse release.
static bool g_lightweight_drag=false;
static bool g_lightweight_drag_dirty=false;
static bool g_contrast_live_dirty=false;
static double g_contrast_next_repaint=0.0;
static double g_contrast_frame_average=1.0/60.0;
static double g_contrast_last_frame=-1.0;
static int g_contrast_live_repaint_count=0;
static bool g_saved_full_waterfall_update=false;
static int g_history_rebuild_count=0;
static void begin_lightweight_slider_drag(){
    if(!g_supported || !g_enabled || !g_panel_hooked || g_lightweight_drag)return;
    g_saved_full_waterfall_update=core_fullWaterfallUpdate;
    g_lightweight_drag=true;
    g_lightweight_drag_dirty=false;
    g_contrast_live_dirty=false;
    g_contrast_next_repaint=0.0;
    g_contrast_frame_average=1.0/60.0;
    g_contrast_last_frame=-1.0;
    if(g_saved_full_waterfall_update)wf_setFullWaterfallUpdate(core_waterfall,false);
}
static void finish_lightweight_slider_drag(bool force){
    if(!g_lightweight_drag || (!force && mouse_button_held()))return;
    g_lightweight_drag=false;
    if(g_saved_full_waterfall_update){
        wf_setFullWaterfallUpdate(core_waterfall,true);
        if(g_lightweight_drag_dirty){
            // Exactly one backbuffer update with the FINAL value, never on click-only.
            wf_updateWaterfallFb(core_waterfall);
            g_history_rebuild_count++;
        }
    }
    g_lightweight_drag_dirty=false;
    g_contrast_live_dirty=false;
}
// These are SDR++ native FFT framerate controls; never adjust waterfall geometry.
static void apply_speed(){
    if(!g_supported || !g_enabled || !g_panel_hooked || !g_speed_custom)return;
    if(g_speed<1)g_speed=1;if(g_speed>60)g_speed=60;
    if(core_display_fft_rate!=g_speed){
        core_display_fft_rate=g_speed;
        frontend_setFFTRate(core_iq_frontend,(double)g_speed);
        display_updateFFTSpeeds();
    }
    g_speed_pending=false;
    g_speed_last_applied=ui_GetTime();
}
static void on_speed_slider(){
    int next=(int)(g_speed_slider+0.5f);if(next<1)next=1;if(next>60)next=60;
    if(g_speed==next && g_speed_custom)return;
    g_speed=next;g_speed_slider=(float)next;g_speed_pending=true;g_speed_custom=true;
    g_speed_apply_after=ui_GetTime();
    panel_changed();
}
// SDR++ maps raw signal amplitude (dB) to waterfall color using its native
// waterfallMin/waterfallMax values. Changing the dynamic range around a
// fixed anchor creates real signal-to-noise contrast, not a colored overlay.
// This leaves native Max/Min GUI values untouched and works with any colormap.
static void contrast_values(float&outMin,float&outMax){
    const float nativeMin=*(float*)(core_main_window+0x94);
    float nativeMax=*(float*)(core_main_window+0x98);
    if(nativeMax<nativeMin+10.0f)nativeMax=nativeMin+10.0f;
    const float range=nativeMax-nativeMin;
    const float anchor=nativeMin+range*0.35f;
    // 0 => 2x native dynamic range; 100 => unmodified; 200 => 0.4x.
    const float factor=g_contrast>100 ? 1.0f-0.006f*(g_contrast-100)
                                         : 1.0f+0.01f*(100-g_contrast);
    outMin=anchor-(anchor-nativeMin)*factor;
    outMax=anchor+(nativeMax-anchor)*factor;
}
static void apply_waterfall_contrast(void*wf){
    if(!g_supported||!g_enabled||!g_panel_hooked)return;
    float mn=0,mx=0;contrast_values(mn,mx);
    const bool changeMin=dabs((double)(wf_getWaterfallMin(wf)-mn))>0.001;
    const bool changeMax=dabs((double)(wf_getWaterfallMax(wf)-mx))>0.001;
    if(changeMin||changeMax){
        // WaterFall::setWaterfallMin/Max each rebuild the full history when
        // 'Full Waterfall Update' is on. Batch both into ONE rebuild, and
        // completely skip the work when the color levels did not change.
        const bool wasFull=core_fullWaterfallUpdate;
        if(wasFull)wf_setFullWaterfallUpdate(wf,false);
        if(changeMin)wf_setWaterfallMin(wf,mn);
        if(changeMax)wf_setWaterfallMax(wf,mx);
        if(wasFull){wf_setFullWaterfallUpdate(wf,true);wf_updateWaterfallFb(wf);}
    }
    g_contrast_needs_apply=false;
}
static void restore_native_waterfall_levels(){
    if(!g_supported||!g_panel_hooked||g_contrast==100)return;
    const bool wasFull=core_fullWaterfallUpdate;
    if(wasFull)wf_setFullWaterfallUpdate(core_waterfall,false);
    wf_setWaterfallMin(core_waterfall,*(float*)(core_main_window+0x94));
    wf_setWaterfallMax(core_waterfall,*(float*)(core_main_window+0x98));
    if(wasFull){wf_setFullWaterfallUpdate(core_waterfall,true);wf_updateWaterfallFb(core_waterfall);}
}
static void on_contrast_slider(){
    int next=(int)(g_contrast_slider+0.5f);if(next<0)next=0;if(next>200)next=200;
    if(next==g_contrast)return;
    g_contrast=next;g_contrast_slider=(float)next;
    if(mouse_button_held())begin_lightweight_slider_drag();
    if(g_lightweight_drag){
        g_lightweight_drag_dirty=true;
        g_contrast_live_dirty=true;
    }
    apply_waterfall_contrast(core_waterfall);
    panel_changed();
}
// Use actual child-window space rather than a hardcoded height. Reserve room for
// five captions, margins and the native EndChild; no vertical scrollbar is required.
static float g_right_slider_h=56.0f;
static void apply_deferred_panel_sliders(){
    if(!g_supported||!g_enabled||!g_panel_hooked)return;
    // Contrast must affect not just incoming rows but the visible history
    // *while* the mouse moves. Redrawing every UI frame at 2.4MHz stalls SDR++.
    // Instead budget the CPU-heavy historical recolour separately from slider
    // input. Faster PCs update more often; wide displays self-throttle.
    if(g_lightweight_drag && mouse_button_held()){
        const double now=ui_GetTime();
        // ImGui::GetTime changes once per frame: using it to time a synchronous
        // repaint would always report zero. Adapt instead to the *observed* GUI
        // frame intervals. This avoids an extra Windows timing/API dependency.
        if(g_contrast_last_frame>=0.0){
            const double dt=now-g_contrast_last_frame;
            if(dt>0.0001 && dt<0.5)
                g_contrast_frame_average=0.75*g_contrast_frame_average+0.25*dt;
        }
        g_contrast_last_frame=now;
        if(g_contrast_live_dirty && now>=g_contrast_next_repaint){
            wf_updateWaterfallFb(core_waterfall);
            double delay=0.10; // no more than 10 costly historical recolours/s
            if(g_contrast_frame_average>0.025)
                delay+=(g_contrast_frame_average-0.025)*3.0;
            if(delay>0.250)delay=0.250;
            g_contrast_next_repaint=now+delay;
            g_contrast_live_dirty=false;
            g_contrast_live_repaint_count++;
        }
    }
    finish_lightweight_slider_drag(false);
    if(!g_speed_custom)g_speed_pending=false;
    if(g_contrast_needs_apply){
        // Initial load and re-enable must restore the saved contrast.
        if(g_contrast!=100){
            // The waterfall history needs one update, not two.
            const bool wasFull=core_fullWaterfallUpdate;
            if(wasFull)wf_setFullWaterfallUpdate(core_waterfall,false);
            apply_waterfall_contrast(core_waterfall);
            if(wasFull){wf_setFullWaterfallUpdate(core_waterfall,true);wf_updateWaterfallFb(core_waterfall);}
        }
        g_contrast_needs_apply=false;
    }
    // Native FFT rate changes can reconfigure the DSP pipeline. Throttle them
    // during dragging but always apply the final position as soon as LMB is up.
    const double now=ui_GetTime();
    // FFT Framerate reconfigures the CPU DSP chain. ImGui's native InputInt
    // commits a value, rather than executing that costly work dozens of times
    // during one continuous drag. Apply one final value on mouse release.
    if(g_speed_pending && !mouse_button_held() && now>=g_speed_apply_after)apply_speed();
    if(g_speed_custom){
        // V36 bug: overwriting g_speed from native FFT rate after an SDR++
        // reconfiguration also overwrote the saved position in the UI. Once
        // the user sets Speed, that saved value remains authoritative.
        // Retry after host updates (e.g. source/sample-rate changes), but
        // throttle DSP reconfiguration to at most once every 1.5 seconds.
        if(!g_speed_pending && core_display_fft_rate!=g_speed &&
           now-g_speed_last_applied>=1.5){
            g_speed_pending=true;
            g_speed_apply_after=now+0.2;
        }
    }else if(!g_speed_pending && core_display_fft_rate>=1 && core_display_fft_rate<=60 &&
             core_display_fft_rate!=g_speed){
        // Follow the native FFT rate only until a custom Speed is selected.
        g_speed=core_display_fft_rate;g_speed_slider=(float)g_speed;
    }
}
static float right_slider_height(){
    float scale=core_ui_scale>0?core_ui_scale:1.0f;
    ImVec2 win=ui_GetWindowSize(),cursor=ui_GetCursorPos();
    // Native labels, ImGui item spacing, two additional captions and panel padding.
    // The five sliders divide ALL remaining height; no hard upper cap.
    float h=(win.y-cursor.y-178.0f*scale)/5.0f;
    return h>24.0f*scale?h:24.0f*scale;
}
static bool patched_native_slider(const char*label,const ImVec2&sz,float*value,float high,float low,const char*format,int flags){
    if(!g_supported||!g_enabled||!g_panel_hooked)return ui_VSliderFloat(label,sz,value,high,low,format,flags);
    // All three original SDR++ widgets retain their original callbacks in both layouts.
    const bool zoom=value==(float*)(core_main_window+0x9C);
    if(zoom)g_right_slider_h=right_slider_height();
    // Preserve native SDR++ 20px slider width and horizontal alignment.
    ImVec2 size=sz;size.y=g_right_slider_h;
    bool changed=ui_VSliderFloat(label,size,value,high,low,format,flags);
    // Zoom/Max/Min are NATIVE SDR++ controls: preserve the host callbacks,
    // its Full Waterfall Update preference, and its existing waterfall texture.
    // Only the widget height is changed to accommodate the two extra sliders.
    // In V30 a second plugin drag handler forced updateWaterfallFb() when the
    // mouse was released. That unnecessary full-history conversion is removed.
    if(changed && zoom)g_zoom_transition={};
    return changed;
}
static void extra_slider_pair(float height){
    const float scale=core_ui_scale>0?core_ui_scale:1.0f;
    // Use exactly the centering logic of SDR++'s original Zoom, Max and Min.
    // Neither mode draws its own copy of native controls.
    ImVec2 size(20.0f*scale,height);
    const float width=ui_GetWindowSize().x;
    const char*labels[]={"Speed","Contrast"};
    for(int i=0;i<2;i++){
        ui_NewLine();
        const float tw=ui_CalcTextSize(labels[i],0,false,-1).x;
        ui_SetCursorPosX((width-tw)*0.5f);
        ui_TextUnformatted(labels[i],0);
        ui_SetCursorPosX(width*0.5f-10.0f*scale);
        if(i==0){
            // ImGui VSliderFloat: min at the bottom, max at the top.
            // Speed 1..60: up = faster, down = slower.
            if(ui_VSliderFloat("##essential_speed",size,&g_speed_slider,1.0f,60.0f,"%.0f",0))on_speed_slider();
        } else {
            if(ui_VSliderFloat("##essential_contrast",size,&g_contrast_slider,0.0f,200.0f,"%.0f",0))on_contrast_slider();
        }
    }
}
static bool patched_native_min_slider(const char*label,const ImVec2&sz,float*value,float high,float low,const char*format,int flags){
    bool changed=patched_native_slider(label,sz,value,high,low,format,flags);
    // Hover uses the same native right-panel window redirected by Begin/End.
    // In the previous build only mode 0 (Show) drew the two extra controls.
    if(g_supported&&g_enabled&&g_panel_hooked&&
       (g_right_panel==0 || (g_right_panel==1 && g_right_hover)))
        extra_slider_pair(g_right_slider_h);
    return changed;
}
static void patched_column_width(int index,float width){
    if(g_supported&&g_enabled&&g_panel_hooked&&g_right_panel!=0){
        float removed=52.0f*core_ui_scale;
        if(index==1)width+=removed;
        if(index==2)width-=removed;
    }
    ui_SetColumnWidth(index,width);
}
// Native onResize allocates FFT/waterfall buffers to fit the expanded dataWidth.
// We give it back the left 50px when the scale is hidden, then correct all plot rects
// and recompute VFO geometry, avoiding visual blank strips or buffer overflows.
static bool g_panel_resize_active=false;
static void patched_waterfall_resize(void*wf){
    if(!g_supported||!g_enabled||!g_panel_hooked||g_left_panel==0||g_panel_resize_active){wf_onResize(wf);return;}
    float*widgetX=(float*)((u8*)wf+0x3d09d4);
    float original=*widgetX;
    if(original<100.0f){wf_onResize(wf);return;}
    float delta=50.0f*core_ui_scale;
    g_panel_resize_active=true;
    *widgetX=original+delta;
    wf_onResize(wf);
    *widgetX=original;
    for(int offset=0x90;offset<=0xb8;offset+=8)*(float*)((u8*)wf+offset)-=delta;
    wf_updateAllVFOs(wf,false);
    g_panel_resize_active=false;
    // Do not call setViewBandwidth() or setViewBandwidthSlider() here:
    // their native resize callbacks feed back into the zoom widget and cause stutter.
}
static void patched_waterfall_min(void*wf,float min){
    if(g_contrast==100 || !g_supported || !g_enabled){wf_setWaterfallMin(wf,min);return;}
    // Native main_window.cpp always invokes patched_waterfall_max immediately
    // afterwards. Apply the custom contrast ONCE there with both final limits.
}
static void draw_overlay_scale(const ImVec2&plotMin,const ImVec2&plotMax){
    float scale=core_ui_scale;
    ImVec2 start(plotMin.x,plotMin.y);
    ImVec2 end(plotMin.x+46*scale,plotMax.y);
    ImDrawList*dl=ui_GetWindowDrawList();
    draw_AddRectFilled(dl,start,end,0xE0202525u,0.0f,0);
    float min=wf_getFFTMin(core_waterfall),max=wf_getFFTMax(core_waterfall);
    float h=plotMax.y-plotMin.y;
    if(max-min<1||h<50)return;
    for(int n=-160;n<=40;n+=20){
        if((float)n<min||(float)n>max)continue;
        float y=plotMax.y-((float)n-min)/(max-min)*h;
        char label[16];int a=n<0?-n:n;char digits[12];u64str(a,digits,sizeof(digits));
        label[0]=0;if(n<0)c_append(label,sizeof(label),"-");c_append(label,sizeof(label),digits);
        draw_AddText(dl,ImVec2(start.x+3*scale,y-6*scale),0xFFD4E1DDu,label,0);
        draw_AddLine(dl,ImVec2(start.x+35*scale,y),ImVec2(end.x,y),0xFF85958Eu,1.0f);
    }
}
// Native controls are redirected at BeginChild/EndChild; there is no
// reimplementation of Zoom/Max/Min for popup mode.
static bool mouse_over_right_controls(const ImVec2&mouse,const InputHandlerArgs&args){
    if(!g_supported||!g_enabled||!g_panel_hooked||g_right_panel==2)return false;
    const float scale=core_ui_scale>0?core_ui_scale:1.0f;
    const float right=args.fftRectMax.x;
    if(mouse.y<args.fftRectMin.y||mouse.y>args.waterfallRectMax.y)return false;
    if(g_right_panel==0)return mouse.x>=right-2.0f*scale && mouse.x<=right+64.0f*scale;
    const bool onEdge=mouse.x>=right-17.0f*scale && mouse.x<=right+8.0f*scale;
    const bool onOverlay=g_right_hover && mouse.x>=right-82.0f*scale && mouse.x<=right+8.0f*scale;
    return onEdge||onOverlay;
}
static void draw_panel_hover_overlays(){
    if(!g_supported||!g_enabled||!g_panel_hooked)return;
    ImVec2 mouse=ui_GetMousePos();
    const ImVec2 fftMin(*(float*)(core_waterfall+0x90),*(float*)(core_waterfall+0x94));
    const ImVec2 fftMax(*(float*)(core_waterfall+0x98),*(float*)(core_waterfall+0x9c));
    const float bottom=*(float*)(core_waterfall+0xbc);
    float scale=core_ui_scale;
    if(g_left_panel==1){
        bool near=mouse.x>=fftMin.x && mouse.x<=fftMin.x+15*scale && mouse.y>=fftMin.y&&mouse.y<=bottom;
        if(g_left_hover){near=mouse.x>=fftMin.x&&mouse.x<=fftMin.x+48*scale&&mouse.y>=fftMin.y&&mouse.y<=bottom;}
        g_left_hover=near;
        if(near){
            ImVec2 pos(fftMin.x,fftMin.y);
            ui_SetNextWindowPos(pos,0,ImVec2(0,0));
            ui_SetNextWindowSize(ImVec2(48*scale,fftMax.y-fftMin.y),0);
            if(ui_Begin("##EssentialLeftScaleOverlay",0,1|2|4|8|256|4096))draw_overlay_scale(pos,ImVec2(pos.x+48*scale,fftMax.y));
            ui_BringWindowToDisplayFront(ui_GetCurrentWindow());ui_End();
        }
    }else g_left_hover=false;
    // Right hover panel now renders using the actual original SDR++ child widgets.
}
static void patched_waterfall_max(void*wf,float max){
    if(g_contrast==100 || !g_supported || !g_enabled)wf_setWaterfallMax(wf,max);
    else apply_waterfall_contrast(wf);
    if(g_supported&&g_enabled)draw_panel_hover_overlays();
}

// A single native call path: the ORIGINAL SDR++ BeginChild/VSliderFloat/EndChild
// calls render into their original child (Show) or a floating ImGui window
// (Hover).  Hidden mode uses an off-screen, noninteractive window so the host
// callbacks remain the only implementation, with no duplicated Zoom formula.
static bool g_native_controls_popup=false;
static void* g_right_native_overlay_window=nullptr;
static bool patched_native_controls_begin(const char*name,const ImVec2&size,bool border,int flags){
    g_native_controls_popup=false;
    if(!g_supported||!g_enabled||!g_panel_hooked){
        g_right_hover=false;g_right_native_overlay_window=nullptr;
        return ui_BeginChild(name,size,border,flags);
    }
    const float scale=core_ui_scale>0.0f?core_ui_scale:1.0f;
    ImVec2 mouse=ui_GetMousePos();
    ImVec2 fftMin(*(float*)(core_waterfall+0x90),*(float*)(core_waterfall+0x94));
    ImVec2 fftMax(*(float*)(core_waterfall+0x98),*(float*)(core_waterfall+0x9c));
    const float bottom=*(float*)(core_waterfall+0xbc);
    bool hovered=false;
    if(g_right_panel==1){
        hovered=mouse.x>=fftMax.x-17.0f*scale&&mouse.x<=fftMax.x+12.0f*scale&&
            mouse.y>=fftMin.y&&mouse.y<=bottom;
        if(g_right_hover){
            hovered=hovered||(mouse.x>=fftMax.x-83.0f*scale&&mouse.x<=fftMax.x+12.0f*scale&&
                mouse.y>=fftMin.y&&mouse.y<=bottom);
        }
        if(g_right_hover&&mouse_button_held())hovered=true;
    }
    g_right_hover=hovered;
    // Show: use a real top-level, mouse-active window anchored to the reserved
    // 60px native column. Hover: the same native sliders float over the waterfall.
    // Both can be raised above plugin windows such as Priyom.
    if(g_right_panel==0){hovered=true;}
    g_right_hover=hovered;
    const float width=(g_right_panel==0?60.0f:82.0f)*scale;
    const float posX=g_right_panel==0?fftMax.x+2.0f*scale:
        (hovered?fftMax.x-width+2*scale:-20000.0f);
    const float posY=(hovered?fftMin.y+4*scale:-20000.0f);
    ui_SetNextWindowPos(ImVec2(posX,posY),0,ImVec2(0,0));
    ui_SetNextWindowSize(ImVec2(width,hovered?((bottom-posY-3.0f*scale)>250.0f*scale?(bottom-posY-3.0f*scale):250.0f*scale):640.0f*scale),0);
    g_native_controls_popup=true;
    const int popupFlags=1|2|4|8|256|4096|(hovered?0:(128|512));
    bool active=ui_Begin("##EssentialNativeControls",0,popupFlags);
    g_right_native_overlay_window=hovered?ui_GetCurrentWindow():nullptr;
    return active;
}
static void patched_native_controls_end(){
    if(g_native_controls_popup){ ui_End();g_native_controls_popup=false; }
    else ui_EndChild();
}
static void raise_native_right_panel(){
    if(!g_supported||!g_enabled||!g_panel_hooked||g_right_panel==2)return;
    if(g_right_native_overlay_window && (g_right_panel==0||g_right_hover))
        ui_BringWindowToDisplayFront(g_right_native_overlay_window);
}
struct EssentialPanelCall {u32 rva;u32 originalDestination;u8 original[5];u8 patched[5];void*hook;};
static EssentialPanelCall g_panel_calls[]={
    {0x58041,0xa1230,{}, {},(void*)patched_native_controls_begin},
    {0x58459,0xa88a0,{}, {},(void*)patched_native_controls_end},
    {0x5745a,0xff8e0,{}, {},(void*)patched_column_width},
    {0x57470,0xff8e0,{}, {},(void*)patched_column_width},
    {0x57ba2,0xff8e0,{}, {},(void*)patched_column_width},
    {0x57bbc,0xff8e0,{}, {},(void*)patched_column_width},
    {0x5813a,0x1256b0,{}, {},(void*)patched_native_slider},
    {0x58260,0x1256b0,{}, {},(void*)patched_native_slider},
    {0x583bc,0x1256b0,{}, {},(void*)patched_native_min_slider},
    {0x58489,0x99ec0,{}, {},(void*)patched_waterfall_min},
    {0x58499,0x99e30,{}, {},(void*)patched_waterfall_max},
    {0x94d9c,0x97000,{}, {},(void*)patched_waterfall_resize},
    {0x96fa4,0x97000,{}, {},(void*)patched_waterfall_resize},
    {0x97a83,0x97000,{}, {},(void*)patched_waterfall_resize},
    {0x995a9,0x97000,{}, {},(void*)patched_waterfall_resize},
    {0x9a034,0x97000,{}, {},(void*)patched_waterfall_resize}
};
static u8*g_panel_thunks=0;
static u8*g_panel_core=0;
static bool restore_panel_hooks(){
    finish_lightweight_slider_drag(true);
    bool good=true;
    for(int i=(int)(sizeof(g_panel_calls)/sizeof(g_panel_calls[0]))-1;i>=0;i--){
        auto&c=g_panel_calls[i];if(!c.original[0])continue;
        u8*addr=g_panel_core+c.rva;
        if(bytes_equal(addr,c.patched,5)&&!write_code(addr,c.original,5))good=false;
        if(good)c.original[0]=0;
    }
    if(good&&g_panel_thunks){if(VirtualFree(g_panel_thunks,0,0x8000))g_panel_thunks=0;else good=false;}
    g_panel_hooked=!good;
    return good;
}
static bool install_panel_hooks(){
    if(g_panel_hooked)return true;
    if(g_panel_thunks&&!restore_panel_hooks())return false;
    g_panel_core=(u8*)GetModuleHandleA("sdrpp_core.dll");if(!g_panel_core)return false;
    // Exact expected rel32 destinations from the user's November 2026 Windows x64 build.
    for(auto&c:g_panel_calls){
        const u8*raw=g_panel_core+c.rva;
        if(raw[0]!=0xE8)return false;
        int rel=*(const int*)(raw+1);
        if((u32)((int)c.rva+5+rel)!=c.originalDestination)return false;
    }
    usize base=(usize)g_panel_core&~(usize)0xFFFF;
    for(usize dist=0xB00000;dist<0x70000000&&!g_panel_thunks;dist+=0x10000){
        g_panel_thunks=(u8*)VirtualAlloc((void*)(base+dist),4096,0x3000,0x40);
        if(!g_panel_thunks&&base>dist)g_panel_thunks=(u8*)VirtualAlloc((void*)(base-dist),4096,0x3000,0x40);
    }
    if(!g_panel_thunks)return false;
    for(int i=0;i<(int)(sizeof(g_panel_calls)/sizeof(g_panel_calls[0]));i++){
        auto&c=g_panel_calls[i];u8*where=g_panel_core+c.rva;u8*thunk=g_panel_thunks+16*i;
        for(int k=0;k<5;k++)c.original[k]=where[k];
        thunk[0]=0xFF;thunk[1]=0x25;
        for(int k=2;k<6;k++)thunk[k]=0;
        *(u64*)(thunk+6)=(u64)(usize)c.hook;
        thunk[14]=0x90;thunk[15]=0x90;
        long long diff=(long long)(usize)thunk-(long long)(usize)(where+5);
        if(diff<(-2147483647ll-1)||diff>2147483647ll){restore_panel_hooks();return false;}
        c.patched[0]=0xE8;*(int*)(c.patched+1)=(int)diff;
        if(!write_code(where,c.patched,5)){restore_panel_hooks();return false;}
    }
    u32 oldProtect=0;
    if(!VirtualProtect(g_panel_thunks,4096,0x20,&oldProtect)){restore_panel_hooks();return false;}
    FlushInstructionCache(GetCurrentProcess(),g_panel_thunks,4096);
    g_panel_hooked=true;
    request_panel_resize();
    return true;
}
