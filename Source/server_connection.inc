// Native endpoint widgets and callbacks own connection/configuration state.
enum ServerStatus { SERVER_IDLE,SERVER_CONNECTING,SERVER_CONNECTED,SERVER_ERROR };
struct ServerReading {
    char source[256],host[1024],address[1040],error[192],hostKey[256],portKey[256];
    int port,status,slot;bool observed,connected,dirty,editing,hostEditable,hasButton,pending,pendingConnect;
    int pendingFrame;u64 lastBytes,lastUpload;double lastSample,download,upload,nativeDownload,nativeSample;bool transferReady,nativeReady;
};
static ServerReading g_server={};
static bool g_server_select_requested=false;
// The address editor is intentionally separate from the live native source.
// Cancel/ESC must never modify either the active connection or its settings.
struct EndpointDialog {
    char source[256],host[1024],error[160];
    int port;bool open;
};
static EndpointDialog g_endpoint_dialog={};
struct SourceLifecycle { NativeSourceHandler*handler;void(*start)(void*);void(*stop)(void*);void(*deselect)(void*);bool started; };
static SourceLifecycle g_source_lifecycle={};
static void (*transport_native_start(NativeSourceHandler*h))(void*){
    return h==g_source_lifecycle.handler?g_source_lifecycle.start:(h?h->start:0);
}
static bool server_selected(){
    const char*s=string_chars(core_selected_source);
    return c_equal(s,"SDR++ Server")||c_equal(s,"RTL-TCP")||c_equal(s,"SpyServer")||c_equal(s,"RFspace")||
        c_equal(s,"Network")||c_equal(s,"Hermes")||transport_selected_slot()>=0;
}
static void server_prepare_source(){
    const char*s=string_chars(core_selected_source);
    if(!c_equal(g_server.source,s)){
        g_server={};g_endpoint_dialog={};g_server.slot=transport_selected_slot();
        c_copy(g_server.source,sizeof(g_server.source),s);
        g_server.lastBytes=transport_bytes(g_server.slot);g_server.lastSample=ui_GetTime();
    }
}
static bool server_host_widget(const char*label){
    return label&&(contains(label,"_srv_host_")||contains(label,"network_source_host_")||
        (c_equal(string_chars(core_selected_source),"RTL-TCP")&&starts_with(label,"##_ip_select_"))||
        contains(label,"server_host")||contains(label,"server_addr"));
}
static bool server_port_widget(const char*label){
    return label&&(contains(label,"_srv_port_")||contains(label,"network_source_port_")||
        (c_equal(string_chars(core_selected_source),"RTL-TCP")&&starts_with(label,"##_port_select_"))||contains(label,"server_port"));
}
static bool parse_server_endpoint(const char*text,char*host,usize capacity,int&port){
    port=0;if(!text||!*text)return false;usize length=c_len(text),start=0,end=length;const char*digits=0;
    if(length>=1040)return false;
    if(text[0]=='['){
        start=1;end=start;while(end<length&&text[end]!=']')end++;
        if(end==length||end==start)return false;
        if(end+1<length){if(text[end+1]!=':')return false;digits=text+end+2;}
    }else{
        int colons=0;usize colon=0;for(usize i=0;i<length;i++)if(text[i]==':'){colons++;colon=i;}
        if(colons==1){end=colon;digits=text+colon+1;}
    }
    if(end<=start||end-start>=capacity||end-start>=1023)return false;
    for(usize i=start;i<end;i++){
        unsigned char c=(unsigned char)text[i];
        if(c<=32||c>=127||c=='/'||c=='\\'||c=='['||c==']'||c=='?'||c=='#'||c=='@')return false;
        host[i-start]=(char)c;
    }host[end-start]=0;
    if(digits){
        if(!*digits)return false;for(;*digits;digits++){
            if(*digits<'0'||*digits>'9')return false;port=port*10+(*digits-'0');if(port>65535)return false;
        }if(port<1)return false;
    }return true;
}
// Build a bounded endpoint string. Hosts and IPv6 addresses are accepted;
// the port is always explicit in the dialog and must be in 1..65535.
static bool compose_server_endpoint(const char*host,int port,char*out,usize cap){
    if(!out||!cap||port<1||port>65535)return false;
    char parsed[1024]={};int parsedPort=0;
    if(!parse_server_endpoint(host,parsed,sizeof(parsed),parsedPort)||parsedPort||!c_equal(parsed,host))return false;
    bool ipv6=contains(host,":");char digits[24];u64str((u64)port,digits,sizeof(digits));
    usize needed=c_len(host)+c_len(digits)+1+(ipv6?2:0);
    if(needed>=cap)return false;
    out[0]=0;
    if(ipv6)c_append(out,cap,"[");c_append(out,cap,host);
    if(ipv6)c_append(out,cap,"]");c_append(out,cap,":");c_append(out,cap,digits);
    return true;
}
static void server_sync_address(){
    if(g_server.dirty||g_server.editing||!g_server.observed)return;
    char value[1040]={};
    if(compose_server_endpoint(g_server.host,g_server.port,value,sizeof(value)))
        c_copy(g_server.address,sizeof(g_server.address),value);
    else c_copy(g_server.address,sizeof(g_server.address),g_server.host);
}
static void endpoint_dialog_open(){
    g_endpoint_dialog={};
    c_copy(g_endpoint_dialog.source,sizeof(g_endpoint_dialog.source),string_chars(core_selected_source));
    int port=0;char host[1024]={};
    // A pending edit takes precedence over the last observed native address.
    bool parsed=g_server.dirty&&parse_server_endpoint(g_server.address,host,sizeof(host),port);
    if(!parsed){c_copy(host,sizeof(host),g_server.host);port=g_server.port;}
    c_copy(g_endpoint_dialog.host,sizeof(g_endpoint_dialog.host),host);
    g_endpoint_dialog.port=(port>0&&port<=65535)?port:5259;
    g_endpoint_dialog.open=true;g_server.editing=true;
}
static bool endpoint_dialog_save(){
    if(!g_endpoint_dialog.open||!c_equal(g_endpoint_dialog.source,string_chars(core_selected_source)))return false;
    char value[1040]={};
    if(!compose_server_endpoint(g_endpoint_dialog.host,g_endpoint_dialog.port,value,sizeof(value))){
        c_copy(g_endpoint_dialog.error,sizeof(g_endpoint_dialog.error),l10n("Podaj poprawny adres i port (1-65535)."));return false;
    }
    bool changed=!c_equal(value,g_server.address);
    if(changed){
        c_copy(g_server.address,sizeof(g_server.address),value);
        g_server.dirty=true;g_server.error[0]=0;
        if(!g_server.connected)g_server.status=SERVER_IDLE;
    }
    g_server.editing=false;g_endpoint_dialog.open=false;
    return true;
}
static void endpoint_dialog_cancel(){g_endpoint_dialog={};g_server.editing=false;}
static bool hook_native_source_combo(const char*l,int*v,const char*items,int height){return ui_Combo(l,v,items,height);}
static bool server_observe_host(const char*l,char*text,usize capacity,bool changed){
    if(!g_supported||!g_enabled||!server_selected()||!server_host_widget(l)||!text||!capacity)return changed;
    server_prepare_source();g_server.observed=true;g_server.hostEditable=true;c_copy(g_server.hostKey,sizeof(g_server.hostKey),l);
    if(source_request_matches(5)&&g_source_request.setEndpoint&&!widget_disabled()){
        if(!c_equal(text,g_source_request.host)){c_copy(text,capacity,g_source_request.host);changed=true;}
    }c_copy(g_server.host,sizeof(g_server.host),text);return changed;
}
static bool server_observe_port(const char*l,int*v,bool changed){
    if(!g_supported||!g_enabled||!server_selected()||!server_port_widget(l)||!v)return changed;
    server_prepare_source();c_copy(g_server.portKey,sizeof(g_server.portKey),l);
    if(source_request_matches(5)&&g_source_request.setEndpoint&&g_source_request.port>0&&!widget_disabled()){
        if(*v!=g_source_request.port){*v=g_source_request.port;changed=true;}
    }g_server.port=*v;return changed;
}
static bool hook_server_host(const char*l,char*text,usize capacity,int flags,int(*callback)(void*),void*data){return server_observe_host(l,text,capacity,ui_InputText(l,text,capacity,flags,callback,data));}
static bool hook_sm_server_host(const char*l,char*text,usize capacity,int flags){return server_observe_host(l,text,capacity,sm_InputText(l,text,capacity,flags));}
static bool server_observe_button(const char*l,bool changed){
    if(!g_supported||!g_enabled||!server_selected()||!l)return changed;
    bool connect=starts_with(l,"Connect"),disconnect=starts_with(l,"Disconnect");if(!connect&&!disconnect)return changed;
    server_prepare_source();g_server.observed=true;g_server.hasButton=true;bool previously=g_server.connected;g_server.connected=disconnect;
    if(!g_server.pending&&g_server.status!=SERVER_CONNECTING){
        if(disconnect){g_server.status=SERVER_CONNECTED;g_server.error[0]=0;}
        else if(previously){g_server.status=SERVER_ERROR;c_copy(g_server.error,sizeof(g_server.error),l10n("Polaczenie zostalo przerwane."));}
    }server_sync_address();
    if(source_request_matches(5)&&!widget_disabled()){
        g_source_request.done=true;
        if((g_source_request.direction>0&&connect)||(g_source_request.direction<0&&disconnect))return true;
    }
    if(changed){g_server.status=connect?SERVER_CONNECTING:SERVER_IDLE;}
    return changed;
}
static bool hook_server_button(const char*l,const ImVec2&size){return server_observe_button(l,ui_Button(l,size));}
static void hook_server_text_colored(const ImVec4&color,const char*format,...){
    NativeVaList args;__builtin_va_start(args,format);
    if(g_supported&&g_enabled&&!g_source_hidden_visit&&c_equal(format,"Connected (%.3f Mbit/s)")){
        NativeVaList copy;__builtin_va_copy(copy,args);double rate=__builtin_va_arg(copy,double);__builtin_va_end(copy);
        if(rate>=0&&rate<1000000){server_prepare_source();g_server.nativeDownload=rate*1048576.0/8.0;g_server.nativeSample=ui_GetTime();g_server.nativeReady=true;}
    }
    ui_TextColoredV(color,format,args);__builtin_va_end(args);
}
static bool hook_sm_server_button(const char*l,ImVec2 size){
    bool changed=sm_Button(l,size);
    if(g_numeric_rate_apply&&starts_with(l,"Apply##network_source_apply_")&&!widget_disabled()){
        g_numeric_rate_apply=false;return true;
    }return server_observe_button(l,changed);
}
static void lifecycle_start(void*context){
    if(g_source_lifecycle.started)return;g_source_lifecycle.start(context);g_source_lifecycle.started=true;
}
static void lifecycle_stop(void*context){
    if(g_source_lifecycle.started)g_source_lifecycle.stop(context);g_source_lifecycle.started=false;
    g_server.connected=false;g_server.status=SERVER_IDLE;
}
static void restore_source_lifecycle(bool closePrimed){
    NativeSourceHandler*h=g_source_lifecycle.handler;if(!h)return;
    if(h==selected_source_handler()){
        if(closePrimed&&g_source_lifecycle.started&&!window_isPlaying(core_main_window))g_source_lifecycle.stop(h->context);
        if(h->start==lifecycle_start)h->start=g_source_lifecycle.start;if(h->stop==lifecycle_stop)h->stop=g_source_lifecycle.stop;
        h->deselect=g_source_lifecycle.deselect;
    }g_source_lifecycle={};
}
static void lifecycle_deselect(void*context){auto original=g_source_lifecycle.deselect;restore_source_lifecycle(true);if(original)original(context);}
static bool install_source_lifecycle(){
    NativeSourceHandler*h=selected_source_handler();if(!h||!h->start||!h->stop)return false;
    if(h==g_source_lifecycle.handler)return true;restore_source_lifecycle(true);
    g_source_lifecycle={h,h->start,h->stop,h->deselect,window_isPlaying(core_main_window)};
    h->start=lifecycle_start;h->stop=lifecycle_stop;h->deselect=lifecycle_deselect;return true;
}
static void server_sample_download(){
    int slot=transport_selected_slot();double now=ui_GetTime();u64 bytes=transport_bytes(slot),uploaded=transport_uploaded(slot);
    bool native=g_server.nativeReady&&now-g_server.nativeSample<1.0;
    if(slot<0){g_server.transferReady=g_server.connected&&native;g_server.download=native?g_server.nativeDownload:0;g_server.upload=0;return;}
    if(slot!=g_server.slot||!g_server.connected){g_server.slot=slot;g_server.lastBytes=bytes;g_server.lastUpload=uploaded;g_server.lastSample=now;g_server.download=g_server.upload=0;g_server.transferReady=false;return;}
    double elapsed=now-g_server.lastSample;if(elapsed>=0.5){
        g_server.download=native?g_server.nativeDownload:(double)(bytes-g_server.lastBytes)/elapsed;
        g_server.upload=(double)(uploaded-g_server.lastUpload)/elapsed;g_server.transferReady=true;
        g_server.lastBytes=bytes;g_server.lastUpload=uploaded;g_server.lastSample=now;
    }
}
static void server_observe_transport(){
    if(!server_selected()||g_server.hasButton)return;
    int slot=transport_selected_slot();if(slot<0)return;
    g_server.observed=true;
    if(!g_server.hostEditable&&g_transport[slot].endpoint[0]){
        c_copy(g_server.host,sizeof(g_server.host),g_transport[slot].endpoint);g_server.port=g_transport[slot].port;
    }
    bool before=g_server.connected;g_server.connected=transport_alive(slot);
    if(g_server.connected){if(!g_server.pending)g_server.status=SERVER_CONNECTED;g_server.error[0]=0;}
    else if(before&&g_server.status!=SERVER_CONNECTING){g_server.status=SERVER_ERROR;c_copy(g_server.error,sizeof(g_server.error),l10n("Polaczenie zostalo przerwane."));}
}
