// Count bytes at the selected source module's native transport boundary, not
// from sample-rate estimates. Each module owns a separate atomic counter.
DLLIMPORT int net_socket_recv(void*,u8*,usize,bool,int,void*) CORE_ALIAS("?recv@Socket@net@@QEAAHPEAE_K_NHPEAVAddress@2@@Z");
DLLIMPORT int net_socket_send(void*,const u8*,usize,const void*) CORE_ALIAS("?send@Socket@net@@QEAAHPEBE_KPEBVAddress@2@@Z");
DLLIMPORT bool net_legacy_write(void*,int,u8*) CORE_ALIAS("?write@ConnClass@net@@QEAA_NHPEAE@Z");
DLLIMPORT bool net_socket_open(void*) CORE_ALIAS("?isOpen@Socket@net@@QEAA_NXZ");
DLLIMPORT void net_socket_close(void*) CORE_ALIAS("?close@Socket@net@@QEAAXXZ");
DLLIMPORT void* net_connect(void*,void*,int) CORE_ALIAS("?connect@net@@YA?AV?$shared_ptr@VSocket@net@@@std@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@3@H@Z");
DLLIMPORT void* net_connect_legacy(void*,void*,unsigned short) CORE_ALIAS("?connect@net@@YA?AV?$unique_ptr@VConnClass@net@@U?$default_delete@VConnClass@net@@@std@@@std@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@3@G@Z");
DLLIMPORT void* net_udp_hosts(void*,void*,int,void*,int,bool) CORE_ALIAS("?openudp@net@@YA?AV?$shared_ptr@VSocket@net@@@std@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@3@H0H_N@Z");
DLLIMPORT void* net_udp_address(void*,const void*,void*,int,bool) CORE_ALIAS("?openudp@net@@YA?AV?$shared_ptr@VSocket@net@@@std@@AEBVAddress@1@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@3@H_N@Z");
DLLIMPORT void* net_udp_addresses(void*,const void*,const void*,bool) CORE_ALIAS("?openudp@net@@YA?AV?$shared_ptr@VSocket@net@@@std@@AEBVAddress@1@0_N@Z");
DLLIMPORT int net_legacy_read(void*,int,u8*,bool) CORE_ALIAS("?read@ConnClass@net@@QEAAHHPEAE_N@Z");
DLLIMPORT void net_legacy_async(void*,int,u8*,void(*)(int,u8*,void*),void*,bool) CORE_ALIAS("?readAsync@ConnClass@net@@QEAAXHPEAEP6AXH0PEAX@Z1_N@Z");
DLLIMPORT void net_legacy_close(void*) CORE_ALIAS("?close@ConnClass@net@@QEAAXXZ");

static const int TRANSPORT_SLOTS=16;
struct TransportReading { void*module;char name[256],endpoint[80];int port;void*discovery;u64 bytes,uploaded;int alive;bool network; };
static TransportReading g_transport[TRANSPORT_SLOTS]={};
static bool g_transport_enabled=true,g_transport_pinned=false;
static u64 transport_bytes(int slot){return slot>=0?__atomic_load_n(&g_transport[slot].bytes,__ATOMIC_RELAXED):0;}
static u64 transport_uploaded(int slot){return slot>=0?__atomic_load_n(&g_transport[slot].uploaded,__ATOMIC_RELAXED):0;}
static bool transport_alive(int slot){return slot>=0&&__atomic_load_n(&g_transport[slot].alive,__ATOMIC_ACQUIRE)!=0;}
static void transport_received(int slot,int bytes){
    if(!__atomic_load_n(&g_transport_enabled,__ATOMIC_RELAXED))return;
    if(bytes>0){__atomic_fetch_add(&g_transport[slot].bytes,(u64)bytes,__ATOMIC_RELAXED);__atomic_store_n(&g_transport[slot].alive,1,__ATOMIC_RELEASE);}
}
static void transport_connected(int slot,void*out){
    if(out&&*(void**)out)__atomic_store_n(&g_transport[slot].alive,1,__ATOMIC_RELEASE);
}
template<int Slot> static int transport_recv(void*socket,u8*data,usize count,bool force,int timeout,void*address){
    bool discovery=socket==g_transport[Slot].discovery;
    int got=net_socket_recv(socket,data,count,force,timeout,address);if(discovery)return got;
    transport_received(Slot,got);
    if(got<=0&&!net_socket_open(socket))__atomic_store_n(&g_transport[Slot].alive,0,__ATOMIC_RELEASE);
    return got;
}
template<int Slot> static int transport_send(void*socket,const u8*data,usize count,const void*address){
    bool discovery=socket==g_transport[Slot].discovery;
    int sent=net_socket_send(socket,data,count,address);
    if(sent>0&&!discovery&&__atomic_load_n(&g_transport_enabled,__ATOMIC_RELAXED))
        __atomic_fetch_add(&g_transport[Slot].uploaded,(u64)sent,__ATOMIC_RELAXED);
    return sent;
}
template<int Slot> static bool transport_write(void*connection,int count,u8*data){
    bool sent=net_legacy_write(connection,count,data);
    if(sent&&count>0&&__atomic_load_n(&g_transport_enabled,__ATOMIC_RELAXED))
        __atomic_fetch_add(&g_transport[Slot].uploaded,(u64)count,__ATOMIC_RELAXED);
    return sent;
}
template<int Slot> static void transport_close(void*socket){
    bool discovery=socket==g_transport[Slot].discovery;net_socket_close(socket);
    if(discovery){g_transport[Slot].discovery=0;return;}
    __atomic_store_n(&g_transport[Slot].alive,0,__ATOMIC_RELEASE);
}
template<int Slot> static void* transport_connect(void*out,void*host,int port){
    c_copy(g_transport[Slot].endpoint,sizeof(g_transport[Slot].endpoint),string_chars(host));g_transport[Slot].port=port;
    void*r=net_connect(out,host,port);transport_connected(Slot,out);return r;
}
template<int Slot> static void* transport_connect_old(void*out,void*host,unsigned short port){
    c_copy(g_transport[Slot].endpoint,sizeof(g_transport[Slot].endpoint),string_chars(host));g_transport[Slot].port=port;
    void*r=net_connect_legacy(out,host,port);transport_connected(Slot,out);return r;
}
template<int Slot> static void* transport_udp_hosts(void*out,void*host,int port,void*local,int localPort,bool broadcast){
    bool discovery=c_equal(string_chars(host),"255.255.255.255");
    void*r=net_udp_hosts(out,host,port,local,localPort,broadcast);
    if(discovery&&out)g_transport[Slot].discovery=*(void**)out;else transport_connected(Slot,out);return r;
}
static void transport_endpoint(int slot,const void*address){
    if(!address)return;const u8*a=(const u8*)address;char part[16];g_transport[slot].endpoint[0]=0;
    for(int i=0;i<4;i++){if(i)c_append(g_transport[slot].endpoint,sizeof(g_transport[slot].endpoint),".");
        u64str(a[4+i],part,sizeof(part));c_append(g_transport[slot].endpoint,sizeof(g_transport[slot].endpoint),part);}
    g_transport[slot].port=(int)a[2]*256+a[3];
}
template<int Slot> static void* transport_udp_address(void*out,const void*address,void*local,int port,bool broadcast){
    transport_endpoint(Slot,address);void*r=net_udp_address(out,address,local,port,broadcast);transport_connected(Slot,out);return r;
}
template<int Slot> static void* transport_udp_addresses(void*out,const void*address,const void*local,bool broadcast){
    transport_endpoint(Slot,address);void*r=net_udp_addresses(out,address,local,broadcast);transport_connected(Slot,out);return r;
}
template<int Slot> static int transport_read(void*connection,int count,u8*data,bool force){
    int got=net_legacy_read(connection,count,data,force);transport_received(Slot,got);
    if(got<=0)__atomic_store_n(&g_transport[Slot].alive,0,__ATOMIC_RELEASE);return got;
}
struct AsyncReadTap { int slot;void(*callback)(int,u8*,void*);void*context; };
static void transport_async_result(int count,u8*data,void*context){
    AsyncReadTap*entry=(AsyncReadTap*)context;int slot=entry->slot;
    auto callback=entry->callback;void*original=entry->context;free(entry);
    transport_received(slot,count);if(count<=0)__atomic_store_n(&g_transport[slot].alive,0,__ATOMIC_RELEASE);
    callback(count,data,original);
}
template<int Slot> static void transport_async(void*connection,int count,u8*data,void(*callback)(int,u8*,void*),void*context,bool force){
    AsyncReadTap*entry=(AsyncReadTap*)malloc(sizeof(AsyncReadTap));
    if(!entry||!callback){if(entry)free(entry);net_legacy_async(connection,count,data,callback,context,force);return;}
    *entry={Slot,callback,context};net_legacy_async(connection,count,data,transport_async_result,entry,force);
}
template<int Slot> static void transport_close_old(void*connection){net_legacy_close(connection);__atomic_store_n(&g_transport[Slot].alive,0,__ATOMIC_RELEASE);}
#define TRANSPORT_ROW(N) {(void*)transport_recv<N>,(void*)transport_close<N>,(void*)transport_connect<N>,(void*)transport_connect_old<N>,(void*)transport_udp_hosts<N>,(void*)transport_udp_address<N>,(void*)transport_udp_addresses<N>,(void*)transport_read<N>,(void*)transport_async<N>,(void*)transport_close_old<N>,(void*)transport_send<N>,(void*)transport_write<N>}
static void*const TRANSPORT_HOOKS[TRANSPORT_SLOTS][12]={
    TRANSPORT_ROW(0),TRANSPORT_ROW(1),TRANSPORT_ROW(2),TRANSPORT_ROW(3),TRANSPORT_ROW(4),TRANSPORT_ROW(5),TRANSPORT_ROW(6),TRANSPORT_ROW(7),
    TRANSPORT_ROW(8),TRANSPORT_ROW(9),TRANSPORT_ROW(10),TRANSPORT_ROW(11),TRANSPORT_ROW(12),TRANSPORT_ROW(13),TRANSPORT_ROW(14),TRANSPORT_ROW(15)};
#undef TRANSPORT_ROW
static void*const TRANSPORT_ORIGINALS[]={(void*)net_socket_recv,(void*)net_socket_close,(void*)net_connect,(void*)net_connect_legacy,
    (void*)net_udp_hosts,(void*)net_udp_address,(void*)net_udp_addresses,(void*)net_legacy_read,(void*)net_legacy_async,(void*)net_legacy_close,(void*)net_socket_send,(void*)net_legacy_write};
static const char*const TRANSPORT_SYMBOLS[]={
    "?recv@Socket@net@@QEAAHPEAE_K_NHPEAVAddress@2@@Z",
    "?close@Socket@net@@QEAAXXZ",
    "?connect@net@@YA?AV?$shared_ptr@VSocket@net@@@std@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@3@H@Z",
    "?connect@net@@YA?AV?$unique_ptr@VConnClass@net@@U?$default_delete@VConnClass@net@@@std@@@std@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@3@G@Z",
    "?openudp@net@@YA?AV?$shared_ptr@VSocket@net@@@std@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@3@H0H_N@Z",
    "?openudp@net@@YA?AV?$shared_ptr@VSocket@net@@@std@@AEBVAddress@1@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@3@H_N@Z",
    "?openudp@net@@YA?AV?$shared_ptr@VSocket@net@@@std@@AEBVAddress@1@0_N@Z",
    "?read@ConnClass@net@@QEAAHHPEAE_N@Z",
    "?readAsync@ConnClass@net@@QEAAXHPEAEP6AXH0PEAX@Z1_N@Z",
    "?close@ConnClass@net@@QEAAXXZ",
    "?send@Socket@net@@QEAAHPEBE_KPEBVAddress@2@@Z",
    "?write@ConnClass@net@@QEAA_NHPEAE@Z"};
static int transport_module_slot(void*module,const char*name){
    for(int i=0;i<TRANSPORT_SLOTS;i++)if(g_transport[i].module==module)return i;
    for(int i=0;i<TRANSPORT_SLOTS;i++)if(!g_transport[i].module){g_transport[i].module=module;c_copy(g_transport[i].name,sizeof(g_transport[i].name),name);return i;}
    return -1;
}
struct NativeSourceHandler { void*stream;void(*menu)(void*);void(*select)(void*);void(*deselect)(void*);void(*start)(void*);void(*stop)(void*);void(*tune)(double,void*);void*context; };
static NativeSourceHandler* selected_source_handler(){return *(NativeSourceHandler**)(core_source_manager+0x90);}
static void (*transport_native_start(NativeSourceHandler*h))(void*);
static int transport_selected_slot(){
    NativeSourceHandler*h=selected_source_handler();void*owner=0;
    auto start=transport_native_start(h);if(start)GetModuleHandleExA(6,(const char*)start,&owner);
    for(int i=0;i<TRANSPORT_SLOTS;i++)if(owner&&g_transport[i].module==owner&&g_transport[i].network)return i;
    owner=0;if(h&&h->menu)GetModuleHandleExA(6,(const char*)h->menu,&owner);
    for(int i=0;i<TRANSPORT_SLOTS;i++)if(owner&&g_transport[i].module==owner&&g_transport[i].network)return i;
    // Some clients expose an imported/shared start callback. Their own menu
    // callback or built-in module identity still identifies the transport.
    {
        owner=0;
        const char*names[]={"SDR++ Server","RTL-TCP","SpyServer","RFspace","Network","Hermes"};
        const char*modules[]={"sdrpp_server_source.dll","rtl_tcp_source.dll","spyserver_source.dll","rfspace_source.dll","network_source.dll","hermes_source.dll"};
        for(int i=0;i<6;i++)if(c_equal(string_chars(core_selected_source),names[i])){owner=GetModuleHandleA(modules[i]);break;}
    }
    for(int i=0;i<TRANSPORT_SLOTS;i++)if(owner&&g_transport[i].module==owner&&g_transport[i].network)return i;
    return -1;
}
static void pin_transport_code(){
    if(g_transport_pinned)return;void*module=0;
    // Outstanding asynchronous native reads may finish after the import is
    // restored. Keep their callback code resident until process shutdown.
    g_transport_pinned=GetModuleHandleExA(5,(const char*)transport_async_result,&module)!=0;
}
