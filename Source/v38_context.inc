// Essential++ V38 context menu / quick presets / native Frequency Manager bridge.
// This file deliberately does not retune the SDR when opening or saving any form.
// Native Frequency Manager's bookmarkEditDialog() performs the actual save.

struct V34Context {
    int stage=0; // 0 closed, 1 context, 2 quick form, 3 quick confirm, 4 bookmark confirm, 5 native bookmark editor, 6 unavailable
    ImVec2 origin={};
    u64 frequency=0;
    int mode=2;
    float filter_khz=10.0f;
    int number=0;
    char frequency_text[32]={};
    char duplicate_name[256]={};
    char error[192]={};
    void* bookmark_instance=nullptr;
    bool native_edit_open=false;
    bool native_category_move=false;
    char edit_origin_category[256]={};
    char edit_target_category[256]={};
    ImVec2 window_pos={};
    ImVec2 window_size={};
    bool window_valid=false;
    ImVec2 slot_popup_pos={};
    ImVec2 slot_popup_size={};
    bool slot_popup_valid=false;
    int raised_stage=-1; // Raise the form only on transition, never over its popup.
    int selected_slot=-1;
    bool on_bookmark=false;
    char bookmark_name[256]={};
    char bookmark_list[256]={};
    // Categories come from the native Frequency Manager list vector.
    // Double-NUL-terminated ImGui Combo buffer; no hard-coded category names.
    int category_count=0;
    int category_selection=0;
    bool bookmark_popup_placed=false;  // per opening; avoid reanchoring on every click
    bool form_placed=false; // reset on new context, not on every frame or stage change
};
static V34Context g_v34_context={};
// Positions persist across right-clicks because v34_open_context() clears its context.
static ImVec2 g_v39_bookmark_popup_last_pos={};
static bool g_v39_bookmark_popup_last_pos_valid=false;
static ImVec2 g_v39_quick_form_last_pos={};
static bool g_v39_quick_form_last_pos_valid=false;
static char g_v39_category_options[8192]={};
static char g_v39_frequency_manager_name[128]="Frequency Manager";
static bool v34_context_open(){return g_v34_context.stage!=0;}
static bool v34_mouse_over_context(const ImVec2&mouse){
    const V34Context& c=g_v34_context;
    if(!c.stage)return false;
    const bool overForm=c.window_valid&&in_rect(mouse,c.window_pos,
        ImVec2(c.window_pos.x+c.window_size.x,c.window_pos.y+c.window_size.y));
    const bool overSlots=c.slot_popup_valid&&in_rect(mouse,c.slot_popup_pos,
        ImVec2(c.slot_popup_pos.x+c.slot_popup_size.x,c.slot_popup_pos.y+c.slot_popup_size.y));
    return overForm||overSlots;
}
static bool g_v34_right_was_down=false;

static bool v34_text_equal_folded(const char* a,const char* b){
    while(*a||*b){
        while(*a==' '||*a=='_'||*a=='-')++a;
        while(*b==' '||*b=='_'||*b=='-')++b;
        int x=(u8)*a,y=(u8)*b;
        if(x>='A'&&x<='Z')x+=32;if(y>='A'&&y<='Z')y+=32;
        if(x!=y)return false;
        if(*a)++a;if(*b)++b;
    }
    return true;
}
static int v34_compare_strings(const char* a,const char* b){
    while(*a&&*a==*b){++a;++b;}
    return (int)(u8)*a-(int)(u8)*b;
}
static bool v34_native_frequency_manager_image(){
#ifndef ESSENTIAL_TEST
    u8* dll=(u8*)GetModuleHandleA("frequency_manager.dll");
    if(!dll||*(unsigned short*)dll!=0x5a4d)return false;
    const u32 pe=*(u32*)(dll+0x3c);
    if(pe>4096 || *(u32*)(dll+pe)!=0x4550 || *(u32*)(dll+pe+8)!=0x6afe1709 ||
        *(u32*)(dll+pe+24+56)!=0x4f000)return false;
    const u8 expected[]={0x48,0x8b,0xc4,0x55,0x53,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57};
    for(int i=0;i<15;i++)if(dll[0x1cd00+i]!=expected[i])return false;
#endif
    return true;
}
static void* v34_frequency_manager_context(){
    if(!v34_native_frequency_manager_image())return nullptr;
    // Menu::order uses 40-byte entries, with std::string at offset zero.
    u8* first=*(u8**)core_menu,*last=*(u8**)(core_menu+8);
    if(!first||!last||last<first||(usize)(last-first)%40)return nullptr;
    if((usize)(last-first)/40>256)return nullptr;
    const char* exact=nullptr;
    for(u8*p=first;p<last;p+=40){
        const char*name=string_chars(p);
        if(name&&c_len(name)<128&&v34_text_equal_folded(name,"Frequency Manager")){
            exact=name;
            c_copy(g_v39_frequency_manager_name,sizeof(g_v39_frequency_manager_name),name);
            break;
        }
    }
    if(!exact)return nullptr;
    // Same native Menu::items tree already used by Radio snap integration.
    u8* head=*(u8**)(core_menu+0xb8);
    if(!head)return nullptr;
    u8* node=*(u8**)(head+8);
    for(int limit=0;node&&node!=head&&!node[25]&&limit<256;limit++){
        int cmp=v34_compare_strings(exact,string_chars(node+32));
        if(cmp==0)return *(void**)(node+72);
        node=*(u8**)(node+(cmp<0?0:16));
    }
    return nullptr;
}

static void* v34_bookmark_head(void* instance){return instance?*(void**)((u8*)instance+0x80):nullptr;}
static void* v34_bookmark_find_frequency(void* instance,u64 target){
    u8*head=(u8*)v34_bookmark_head(instance);if(!head)return nullptr;
    if(head[0x19]!=1)return nullptr;
    u8*node=*(u8**)head; // leftmost
    for(int steps=0;node&&node!=head&&!node[0x19]&&steps<4096;steps++){
        double freq=*(double*)(node+0x40);
        if(freq>=0&&freq<=999999999999.0&&((u64)(freq+0.5))==target)return node;
        u8*right=*(u8**)(node+16);
        if(right&&!right[0x19]){
            node=right;
            for(int k=0;k<4096;k++){
                u8*left=*(u8**)node;if(!left||left[0x19])break;node=left;
            }
        }else{
            u8*parent=*(u8**)(node+8);
            for(int k=0;parent&&parent!=head&&node==*(u8**)(parent+16)&&k<4096;k++){
                node=parent;parent=*(u8**)(parent+8);
            }
            node=parent;
        }
    }
    return nullptr;
}
static bool v34_bookmark_name_exists(void* instance,const char* name){
    u8*head=(u8*)v34_bookmark_head(instance);if(!head)return false;
    u8*node=*(u8**)(head+8);
    for(int limit=0;node&&node!=head&&!node[0x19]&&limit<4096;limit++){
        int cmp=v34_compare_strings(name,string_chars(node+0x20));
        if(!cmp)return true;
        node=*(u8**)(node+(cmp<0?0:16));
    }
    return false;
}
static void v34_assign_native_string(void* at,const char* str){
    core_stringDestroy(at);
    core_stringFromText(at,str);
}
static bool v34_open_native_bookmark(void* instance,bool existing){
    if(!instance||!v34_native_frequency_manager_image())return false;
    // Layout verified against the matching FrequencyManagerModule constructor:
    // createOpen +0x59, editOpen +0x5a, bookmarks +0x80,
    // editedBookmarkName +0x90, firstEditedBookmarkName +0xb0,
    // FrequencyBookmark +0xd0 (frequency, bandwidth, mode, selected).
    u8*base=(u8*)instance;
    if(base[0x59]||base[0x5a])return false; // do not overwrite native dialogs
    char name[256]="New Bookmark";
    if(existing)c_copy(name,sizeof(name),g_v34_context.duplicate_name);
    else if(v34_bookmark_name_exists(instance,name)){
        for(int i=1;i<1000;i++){
            char digits[20],text[256];u64str((u64)i,digits,sizeof(digits));
            c_copy(text,sizeof(text),"New Bookmark (");c_append(text,sizeof(text),digits);c_append(text,sizeof(text),")");
            if(!v34_bookmark_name_exists(instance,text)){c_copy(name,sizeof(name),text);break;}
        }
    }
    v34_assign_native_string(base+0x90,name);
    if(existing)v34_assign_native_string(base+0xb0,g_v34_context.duplicate_name);
    // Only set editOpen for the duration of the native dialog call.
    // Leaving it true between frames would make Frequency Manager draw its own
    // duplicate editor when its sidebar happens to be expanded.
    g_v34_context.native_edit_open=existing;
    // Keep the originally selected list for Cancel and for true category moves.
    c_copy(g_v34_context.edit_origin_category,sizeof(g_v34_context.edit_origin_category),string_chars(base+0x120));
    g_v34_context.edit_target_category[0]=0;
    g_v34_context.native_category_move=false;
    *(double*)(base+0xd0)=(double)g_v34_context.frequency;
    *(double*)(base+0xd8)=(double)g_v34_context.filter_khz*1000.0;
    *(int*)(base+0xe0)=g_v34_context.mode;
    base[0xe4]=0;
    g_v34_context.bookmark_instance=instance;
    g_v34_context.bookmark_popup_placed=false;
    g_v34_context.window_valid=false;
    g_v34_context.stage=5;
    return true;
}
// V39: list selection is a native Frequency Manager operation. We must switch
// its live bookmarks map as well as the selected list name; changing only the
// string would silently save the WRONG bookmarks into a different category.
// Runtime signature is checked before touching private module functions.
static bool v39_build_category_combo(void* instance){
    if(!instance||!v34_native_frequency_manager_image())return false;
    V34Context& c=g_v34_context;
    c.category_count=0;c.category_selection=0;g_v39_category_options[0]=0;
    u8*base=(u8*)instance;
    u8*first=*(u8**)(base+0xe8),*last=*(u8**)(base+0xf0);
    if(!first||!last||last<first||(usize)(last-first)%32)return false;
    const usize count=(usize)(last-first)/32;
    if(count==0||count>128)return false;
    const char*selected=string_chars(base+0x120);
    if(!selected||c_len(selected)>255)return false;
    usize used=0;
    for(usize i=0;i<count;i++){
        const char*category=string_chars(first+i*32);
        if(!category)return false;
        const usize len=c_len(category);
        if(len==0||len>255||used+len+2>sizeof(g_v39_category_options))return false;
        c_copy(g_v39_category_options+used,sizeof(g_v39_category_options)-used,category);
        if(c_equal(category,selected))c.category_selection=(int)i;
        used+=len+1;
        c.category_count++;
    }
    g_v39_category_options[used]=0;
    return true;
}
static const char* v39_selected_category(const V34Context& c){
    if(c.category_selection<0||c.category_selection>=c.category_count)return nullptr;
    const char*current=g_v39_category_options;
    const char*end=g_v39_category_options+sizeof(g_v39_category_options);
    for(int i=0;i<c.category_selection;i++){
        usize n=c_len(current);
        if(n==0||current+n+1>=end)return nullptr;
        current+=n+1;
    }
    return (current<end&&*current)?current:nullptr;
}
static void* v39_native_load_list_function(){
#ifndef ESSENTIAL_TEST
    if(!v34_native_frequency_manager_image())return nullptr;
    u8*module=(u8*)GetModuleHandleA("frequency_manager.dll");
    if(!module)return nullptr;
    // Byte signature for FrequencyManagerModule::loadByName(std::string).
    // Verified to occur exactly once in two shipped x64 FM builds; on any
    // other build, fail closed instead of guessing a function RVA.
    static const u8 signature[]={
        0x48,0x8b,0xc4,0x48,0x89,0x58,0x18,0x55,0x56,0x57,
        0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,
        0x48,0x8d,0xa8,0xa8,0xfe,0xff,0xff
    };
    const u32 pe=*(u32*)(module+0x3c);
    const unsigned sections=*(unsigned short*)(module+pe+6);
    const unsigned optSize=*(unsigned short*)(module+pe+20);
    if(sections<1||sections>16||optSize>512)return nullptr;
    u8*header=module+pe+24+optSize;
    u8*found=nullptr;
    for(unsigned k=0;k<sections;k++){
        u8*section=header+k*40;
        if(section[0]!='.'||section[1]!='t'||section[2]!='e'||
           section[3]!='x'||section[4]!='t'||section[5]!=0)continue;
        u32 offset=*(u32*)(section+12),size=*(u32*)(section+8);
        if(offset<0x1000||offset>0x400000||size>0x800000||
           offset+size>*(u32*)(module+pe+24+56)||
           size<sizeof(signature))return nullptr;
        for(usize i=0;i+sizeof(signature)<=size;i++){
            bool same=true;
            for(usize j=0;j<sizeof(signature);j++){
                if(module[offset+i+j]!=signature[j]){same=false;break;}
            }
            if(same){
                if(found)return nullptr; // multiple matches are ambiguous
                found=module+offset+i;
            }
        }
    }
    return found;
#else
    return nullptr;
#endif
}
static bool v39_switch_category(void* instance,const char*category){
    if(!instance||!category||!category[0])return false;
    if(c_equal(string_chars((u8*)instance+0x120),category))return true;
    void*fn=v39_native_load_list_function();
    if(!fn)return false;
    // MSVC x64 std::string passed by value. Callee owns/destroys this temp.
    CoreString selected;
    core_stringFromText(&selected,category);
    using NativeLoadByName=void(*)(void*,void*);
    ((NativeLoadByName)fn)(instance,&selected);
    return c_equal(string_chars((u8*)instance+0x120),category);
}

// V39 final: render the native bookmark editor INSIDE a regular Essential++
// ImGui::Begin window, just like the quick-access editor.  Frequency Manager's
// bookmarkEditDialog has the correct save/map/refresh logic; only its OpenPopup,
// BeginPopup and EndPopup calls need redirecting.  Its UI is otherwise untouched.
// In particular, NO per-frame SetWindowPos and NO synthetic drag button.
static bool v39_name_has_prefix(const char* name,const char* prefix){
    if(!name||!prefix)return false;
    while(*prefix){if(*name++!=*prefix++)return false;}
    return true;
}
static bool v39_rva_in_image(u32 rva,usize size,u32 image_size){
    return rva && rva<image_size && size<=image_size-rva;
}
static bool v39_find_fm_imgui_import(void***out,const char*target){
    *out=nullptr;
    u8*module=(u8*)GetModuleHandleA("frequency_manager.dll");
    if(!module||!v34_native_frequency_manager_image()||!target)return false;
    const u32 nt=*(u32*)(module+0x3c);
    if(nt>4096||*(u32*)(module+nt)!=0x4550)return false;
    u8*opt=module+nt+24;
    if(*(unsigned short*)opt!=0x20b)return false;
    const u32 image_size=*(u32*)(opt+56);
    const u32 imports=*(u32*)(opt+120);
    if(image_size<0x10000||image_size>0x1000000||
       !v39_rva_in_image(imports,20,image_size))return false;
    const usize name_len=c_len(target)+1;
    if(name_len<2||name_len>240)return false;
    for(u32 j=0;j<256;j++){
        const u32 desc=imports+j*20;
        if(!v39_rva_in_image(desc,20,image_size))return false;
        const u32 lookup=*(u32*)(module+desc);
        const u32 dll_name=*(u32*)(module+desc+12);
        const u32 iat=*(u32*)(module+desc+16);
        if(!lookup&&!dll_name&&!iat)break;
        if(!lookup||!iat||!v39_rva_in_image(dll_name,20,image_size))continue;
        if(!v34_text_equal_folded((const char*)module+dll_name,"sdrpp_core.dll"))continue;
        for(u32 k=0;k<4096;k++){
            if(!v39_rva_in_image(lookup+k*8,8,image_size)||
               !v39_rva_in_image(iat+k*8,8,image_size))return false;
            const u64 thunk=*(u64*)(module+lookup+k*8);
            if(!thunk)break;
            if(thunk>>63||thunk>image_size)continue;
            const u32 name_rva=(u32)thunk;
            if(!v39_rva_in_image(name_rva,2+name_len,image_size))continue;
            if(c_equal((const char*)module+name_rva+2,target)){
                *out=(void**)(module+iat+k*8);return true;
            }
        }
    }
    return false;
}
static bool v39_set_import(void**slot,void*value){
    if(!slot)return false;
    u32 old=0;
    if(!VirtualProtect(slot,sizeof(void*),4,&old))return false;
    *slot=value;
    u32 unused=0;VirtualProtect(slot,sizeof(void*),old,&unused);
    return true;
}
using V39OpenPopup=void(*)(const char*,int);
using V39BeginPopup=bool(*)(const char*,int);
using V39EndPopup=void(*)();
static V39OpenPopup g_v39_original_open_popup=nullptr;
static V39BeginPopup g_v39_original_begin_popup=nullptr;
static V39EndPopup g_v39_original_end_popup=nullptr;
static void** g_v39_native_editor_slots[3]={};
static bool g_v39_embedded_editor_call=false;
static bool g_v39_embedded_popup_started=false;
static bool g_v39_embedded_popup_seen=false;
static bool v39_is_embedded_bookmark_popup(const char*id){
    return g_v39_embedded_editor_call&&
        v39_name_has_prefix(id,"Edit##freq_manager_edit_popup_");
}
static void v39_embedded_open_popup(const char*id,int flags){
    if(v39_is_embedded_bookmark_popup(id))return;
    if(g_v39_original_open_popup)g_v39_original_open_popup(id,flags);
}
static bool v39_embedded_begin_popup(const char*id,int flags){
    if(v39_is_embedded_bookmark_popup(id)){
        g_v39_embedded_popup_started=true;
        g_v39_embedded_popup_seen=true;
        return true;
    }
    return g_v39_original_begin_popup?g_v39_original_begin_popup(id,flags):false;
}
static void v39_embedded_end_popup(){
    if(g_v39_embedded_editor_call&&g_v39_embedded_popup_started){
        // EndPopup normally pops a child ImGui window. We never started that
        // window; the native fields already belong to our ordinary Begin.
        g_v39_embedded_popup_started=false;
        return;
    }
    if(g_v39_original_end_popup)g_v39_original_end_popup();
}
static bool v39_install_embedded_editor_hooks(){
    if(g_v39_native_editor_slots[0])return true;
    static const char* names[3]={
        "?OpenPopup@ImGui@@YAXPEBDH@Z",
        "?BeginPopup@ImGui@@YA_NPEBDH@Z",
        "?EndPopup@ImGui@@YAXXZ"
    };
    void*targets[3]={(void*)v39_embedded_open_popup,
        (void*)v39_embedded_begin_popup,(void*)v39_embedded_end_popup};
    void**slots[3]={};void*orig[3]={};
    void*core=GetModuleHandleA("sdrpp_core.dll");
    if(!core)return false;
    // Resolve ALL hooks and validate the imports BEFORE changing anything.
    for(int i=0;i<3;i++){
        if(!v39_find_fm_imgui_import(&slots[i],names[i])||!slots[i])return false;
        orig[i]=(void*)GetProcAddress(core,names[i]);
        if(!orig[i]||*slots[i]!=orig[i])return false;
    }
    for(int i=0;i<3;i++){
        if(!v39_set_import(slots[i],targets[i])){
            for(int k=i-1;k>=0;k--)v39_set_import(slots[k],orig[k]);
            return false;
        }
    }
    g_v39_original_open_popup=(V39OpenPopup)orig[0];
    g_v39_original_begin_popup=(V39BeginPopup)orig[1];
    g_v39_original_end_popup=(V39EndPopup)orig[2];
    for(int i=0;i<3;i++)g_v39_native_editor_slots[i]=slots[i];
    return true;
}
static void v39_restore_embedded_editor_hooks(){
    if(!g_v39_native_editor_slots[0])return;
    void* callbacks[3]={(void*)v39_embedded_open_popup,
        (void*)v39_embedded_begin_popup,(void*)v39_embedded_end_popup};
    void* original[3]={(void*)g_v39_original_open_popup,
        (void*)g_v39_original_begin_popup,(void*)g_v39_original_end_popup};
    for(int i=0;i<3;i++){
        void**slot=g_v39_native_editor_slots[i];
        if(slot&&*slot==callbacks[i]&&!v39_set_import(slot,original[i])){
            // Keep callback addresses alive if the host refuses restoration.
            void*pin=nullptr;
            GetModuleHandleExA(5,(const char*)v39_embedded_open_popup,&pin);
            return;
        }
        g_v39_native_editor_slots[i]=nullptr;
    }
    g_v39_original_open_popup=nullptr;
    g_v39_original_begin_popup=nullptr;
    g_v39_original_end_popup=nullptr;
}
static void v39_draw_bookmark_category_before_name(){
    V34Context&c=g_v34_context;
    if(c.category_count<=0||!c.bookmark_instance)return;
    const float scale=core_ui_scale>0?core_ui_scale:1.0f;
    // This runs inside the native editor; it never opens a duplicate popup.
    // No visible title or artificial drag button is inserted.
    // Category is before native Name/Frequency/Mode/Apply fields.
    if(ui_BeginTable("##v39_category_before_name",2,0,ImVec2(),0)){
        ui_TableNextRow(0,0);
        ui_TableNextColumn();ui_TextUnformatted("Kategoria",nullptr);
        ui_TableNextColumn();
        ui_SetNextItemWidth(200.0f*scale);
        const int old_choice=c.category_selection;
        if(ui_Combo("##v39_category_inline",&c.category_selection,g_v39_category_options,-1)){
            const char*choice=v39_selected_category(c);
            bool allowed=choice!=nullptr;
            if(allowed&&c.native_edit_open&&
               !c_equal(choice,string_chars((u8*)c.bookmark_instance+0x120))){
                char previous_category[256];
                c_copy(previous_category,sizeof(previous_category),
                       string_chars((u8*)c.bookmark_instance+0x120));
                if(v39_switch_category(c.bookmark_instance,choice)){
                    // Never overwrite the original bookmark's name in the new list.
                    if(!c_equal(choice,c.edit_origin_category) &&
                       (v34_bookmark_name_exists(c.bookmark_instance,c.bookmark_name) ||
                        v34_bookmark_name_exists(c.bookmark_instance,
                            string_chars((u8*)c.bookmark_instance+0x90)))){
                        if(!v39_switch_category(c.bookmark_instance,previous_category)){
                            c_copy(c.error,sizeof(c.error),l10n("Nie mozna przywrocic kategorii."));
                        }else c_copy(c.error,sizeof(c.error),
                            l10n("W tej kategorii istnieje juz taka zakladka."));
                        allowed=false;
                    }
                }else allowed=false;
            }else if(allowed){
                allowed=v39_switch_category(c.bookmark_instance,choice);
            }
            if(!allowed){
                c.category_selection=old_choice;
                if(!c.error[0])c_copy(c.error,sizeof(c.error),
                    l10n("Zmiana kategorii jest niedostepna dla tego builda."));
            }else{
                c.error[0]=0;
                c.native_category_move=c.native_edit_open&&
                    !c_equal(c.edit_origin_category,choice);
                c_copy(c.edit_target_category,sizeof(c.edit_target_category),choice);
            }
        }
        ui_EndTable();
    }
    if(c.error[0])ui_TextUnformatted(c.error,nullptr);
}
static void v34_prepare_bookmark();
static void v39_start_category_selection(){
    V34Context& c=g_v34_context;
    void*instance=v34_frequency_manager_context();
    if(!v39_build_category_combo(instance)){
        c_copy(c.error,sizeof(c.error),
           l10n("Nie mozna odczytac list z Frequency Manager."));
        c.stage=6;return;
    }
    c.bookmark_instance=instance;
    // The category selector now lives INSIDE the native bookmark editor popup.
    // No wizard stage, no 'Dalej' and no second window.
    v34_prepare_bookmark();
}
static void v34_prepare_bookmark(){
    void*instance=v34_frequency_manager_context();
    if(!instance){
        c_copy(g_v34_context.error,sizeof(g_v34_context.error),
            l10n("Frequency Manager nie jest aktywny lub jego build jest niezgodny."));
        g_v34_context.stage=6;return;
    }
    g_v34_context.bookmark_instance=instance;
    void*existing=v34_bookmark_find_frequency(instance,g_v34_context.frequency);
    if(existing){
        const char*existingName=string_chars((u8*)existing+0x20);
        c_copy(g_v34_context.duplicate_name,sizeof(g_v34_context.duplicate_name),existingName);
        c_copy(g_v34_context.bookmark_name,sizeof(g_v34_context.bookmark_name),existingName);
        c_copy(g_v34_context.bookmark_list,sizeof(g_v34_context.bookmark_list),
            string_chars((u8*)instance+0x120));
        g_v34_context.stage=4;
    }else if(!v34_open_native_bookmark(instance,false)){
        c_copy(g_v34_context.error,sizeof(g_v34_context.error),l10n("Nie mozna otworzyc edytora zakladek."));
        g_v34_context.stage=6;
    }
}
// V38: hit testing matches the native Frequency Manager label rendering:
// centerX=round((frequency-lowHz)*pxPerHz)+fftMin.x, text centered +/- 5px,
// and either the top or bottom row of the FFT. Only the visible/clipped
// label rectangle is interactive, never the empty area beside it.
struct V38BookmarkHit {
    bool valid=false;
    bool on_label=false;
    u64 frequency=0;
    double bandwidth=0.0;
    int mode=7;
    char name[256]={};
    char list[256]={};
};
static bool v38_bookmark_hit(const InputHandlerArgs& a,const ImVec2& mouse,
                              bool label_only,V38BookmarkHit& out){
    out={};
    if(!(a.highFreq>a.lowFreq&&a.freqToPixelRatio>0.0)||
       !in_rect(mouse,a.fftRectMin,a.fftRectMax))return false;
    void*instance=v34_frequency_manager_context();
    if(!instance)return false;
    u8*base=(u8*)instance;
    const int mode=*(int*)(base+0x1a0);
    if(mode!=1 && mode!=2)return false;
    // Avoid calculating text widths on the entire waterfall or FFT body.
    float text_h=ui_GetFontSize();
    if(!(text_h>0.0f&&text_h<128.0f))return false;
    if(label_only && mode==1 && mouse.y>a.fftRectMin.y+text_h+2.0f)return false;
    if(label_only && mode==2 && mouse.y<a.fftRectMax.y-text_h-2.0f)return false;
    u8*first=*(u8**)(base+0x188);
    u8*last=*(u8**)(base+0x190);
    if(!first||!last||last<first || (usize)(last-first)%0x58 ||
       (usize)(last-first)/0x58>8192)return false;
    // Native uses back-to-front hit testing, so overlapping labels select the
    // most recently rendered one instead of arbitrarily the nearest carrier.
    for(u8*bm=last;bm>first;){
        bm-=0x58;
        const double freq=*(double*)(bm+0x40);
        if(!(freq>0.0&&freq<999999999999.0))continue;
        const double center=(double)a.fftRectMin.x +
            round_nearest((freq-a.lowFreq)*a.freqToPixelRatio);
        if(center<(double)a.fftRectMin.x-2500.0||
           center>(double)a.fftRectMax.x+2500.0)continue;
        if(*(u64*)(bm+0x30)>255 || *(u64*)(bm+0x10)>255)continue;
        const char*name=string_chars(bm+0x20);
        if(!name || !name[0])continue;
        const ImVec2 size=ui_CalcTextSize(name,nullptr,false,-1.0f);
        if(!(size.x>=0&&size.x<5000&&size.y>0&&size.y<128))continue;
        const float left=(float)(center-size.x*0.5-5.0);
        const float right=(float)(center+size.x*0.5+5.0);
        const float y1=mode==1?a.fftRectMin.y:a.fftRectMax.y-size.y;
        const float y2=y1+size.y;
        const float x1=left>a.fftRectMin.x?left:a.fftRectMin.x;
        const float x2=right<a.fftRectMax.x?right:a.fftRectMax.x;
        const bool label=x2>x1&&in_rect(mouse,ImVec2(x1,y1),ImVec2(x2,y2));
        const bool marker=!label_only && !label &&
            freq>=a.lowFreq && freq<=a.highFreq && dabs(mouse.x-center)<=3.0;
        if(!label && !marker)continue;
        out.valid=true;out.on_label=label;
        out.frequency=(u64)(freq+0.5);out.bandwidth=*(double*)(bm+0x48);
        out.mode=*(int*)(bm+0x50);
        c_copy(out.name,sizeof(out.name),name);
        c_copy(out.list,sizeof(out.list),string_chars(bm));
        return true;
    }
    return false;
}
// PPM on a bookmarked *selected receiver passband* must target the bookmark,
// not the arbitrary frequency below the mouse pointer. The native Frequency
// Manager hit test covers only the label and a thin carrier line; the grey
// passband drawn by SDR++ is the selected VFO's full bandwidth rectangle.
// Restrict this fallback to the FFT grey area so clicking elsewhere on the
// waterfall or another bookmarked station never selects the wrong entry.
static bool v39_bookmark_selected_passband_hit(const InputHandlerArgs& a,
                                              const ImVec2& mouse,
                                              V38BookmarkHit& out){
    out={};
    // FFT and waterfall share the same selected-VFO frequency/passband,
    // but expose separate Y rectangles. Only handle the actual receive band.
    const bool inSpectrum=in_rect(mouse,a.fftRectMin,a.fftRectMax);
    const bool inWaterfall=in_rect(mouse,a.waterfallRectMin,a.waterfallRectMax);
    if(!(inSpectrum||inWaterfall)||
       !selected_vfo_passband_hit(a,mouse))return false;
    const double tuned=selected_frequency();
    if(!(tuned>0.0&&tuned<999999999999.0))return false;
    void*instance=v34_frequency_manager_context();
    if(!instance)return false;
    u8*base=(u8*)instance;
    const int displayMode=*(int*)(base+0x1a0);
    if(displayMode!=1 && displayMode!=2)return false;
    u8*first=*(u8**)(base+0x188);
    u8*last=*(u8**)(base+0x190);
    if(!first||!last||last<first || (usize)(last-first)%0x58 ||
       (usize)(last-first)/0x58>8192)return false;
    // Last visible bookmark wins for coincident carriers, just like the label.
    // Requiring the exact selected frequency is important when different
    // bookmarks have overlapping saved bandwidths.
    for(u8*bm=last;bm>first;){
        bm-=0x58;
        const double freq=*(double*)(bm+0x40);
        if(!(freq>0.0&&freq<999999999999.0)||dabs(freq-tuned)>0.5)continue;
        if(*(u64*)(bm+0x30)>255 || *(u64*)(bm+0x10)>255)continue;
        const char*name=string_chars(bm+0x20);
        if(!name||!name[0])continue;
        out.valid=true;
        out.on_label=false;
        out.frequency=(u64)(freq+0.5);
        out.bandwidth=*(double*)(bm+0x48);
        out.mode=*(int*)(bm+0x50);
        c_copy(out.name,sizeof(out.name),name);
        c_copy(out.list,sizeof(out.list),string_chars(bm));
        return true;
    }
    return false;
}
static void* v38_bookmark_find_named(void* instance,const char*name){
    u8*head=(u8*)v34_bookmark_head(instance);if(!head||head[0x19]!=1)return nullptr;
    u8*node=*(u8**)(head+8);
    for(int limit=0;node&&node!=head&&!node[0x19]&&limit<4096;limit++){
        int cmp=v34_compare_strings(name,string_chars(node+0x20));
        if(!cmp)return node;
        node=*(u8**)(node+(cmp<0?0:16));
    }
    return nullptr;
}
static void v38_bookmark_edit(){
    V34Context&c=g_v34_context;
    void*instance=v34_frequency_manager_context();
    if(!c.on_bookmark||!instance){
        c_copy(c.error,sizeof(c.error),l10n("Brak wybranej zakladki."));c.stage=6;return;
    }
    // The visible bookmark may come from a different displayed list.
    // Never edit a same-name bookmark from the currently selected list.
    if(!c_equal(c.bookmark_list,string_chars((u8*)instance+0x120))){
        c_copy(c.error,sizeof(c.error),l10n("Wybierz liste zakladki w Frequency Manager."));c.stage=6;return;
    }
    void*node=v38_bookmark_find_named(instance,c.bookmark_name);
    if(!node||((u64)(*(double*)((u8*)node+0x40)+0.5))!=c.frequency){
        c_copy(c.error,sizeof(c.error),l10n("Otworz liste tej zakladki w Frequency Manager."));c.stage=6;return;
    }
    if(!v39_build_category_combo(instance)){
        c_copy(c.error,sizeof(c.error),l10n("Nie mozna odczytac kategorii Frequency Manager."));
        c.stage=6;return;
    }
    c_copy(c.duplicate_name,sizeof(c.duplicate_name),c.bookmark_name);
    c.mode=*(int*)((u8*)node+0x50);
    c.filter_khz=(float)(*(double*)((u8*)node+0x48)/1000.0);
    if(!v34_open_native_bookmark(instance,true)){
        c_copy(c.error,sizeof(c.error),l10n("Nie mozna otworzyc edytora zakladki."));c.stage=6;
    }
}
// Native Frequency Manager implements its own map erase, waterfall refresh and
// persistence. We never rewrite frequency_manager_config.json ourselves.
static void (*v38_native_fm_menu_callback(void*instance))(void*);
static bool v38_begin_native_bookmark_delete(){
    V34Context&c=g_v34_context;
    void*instance=v34_frequency_manager_context();
    if(!instance||!v38_native_fm_menu_callback(instance))return false;
    if(!c_equal(c.bookmark_list,string_chars((u8*)instance+0x120)))return false;
    u8*head=(u8*)v34_bookmark_head(instance);
    if(!head||head[0x19]!=1)return false;
    u8*target=(u8*)v38_bookmark_find_named(instance,c.bookmark_name);
    if(!target || ((u64)(*(double*)(target+0x40)+0.5))!=c.frequency)return false;
    // Unselect all before asking native Frequency Manager to remove exactly
    // this one entry; never allow another selected row to be removed too.
    u8*node=*(u8**)head;
    for(int steps=0;node&&node!=head&&!node[0x19]&&steps<4096;steps++){
        node[0x54]=(node==target)?1:0;
        u8*right=*(u8**)(node+16);
        if(right&&!right[0x19]){
            node=right;
            for(int j=0;j<4096;j++){
                u8*left=*(u8**)node;if(!left||left[0x19])break;node=left;
            }
        }else{
            u8*parent=*(u8**)(node+8);
            for(int j=0;parent&&parent!=head&&node==*(u8**)(parent+16)&&j<4096;j++){
                node=parent;parent=*(u8**)(parent+8);
            }
            node=parent;
        }
    }
    // Flags verified for the pinned matching Frequency Manager binary:
    // createOpen=0x59, editOpen=0x5a, deleteBookmarksOpen=0x5f.
    ((u8*)instance)[0x5f]=1;
    c.bookmark_instance=instance;c.stage=10;
    return true;
}
static void (*v38_native_fm_menu_callback(void*instance))(void*){
    u8*head=*(u8**)(core_menu+0xb8);
    if(!head)return nullptr;
    u8*node=*(u8**)(head+8);
    for(int steps=0;node&&node!=head&&!node[25]&&steps<256;steps++){
        if(*(void**)(node+72)==instance &&
            v34_text_equal_folded(string_chars(node+32),"Frequency Manager"))
            return (void(*)(void*))(*(void**)(node+64));
        int cmp=v34_compare_strings("Frequency Manager",string_chars(node+32));
        node=*(u8**)(node+(cmp<0?0:16));
    }
    return nullptr;
}
static void v34_open_context(const InputHandlerArgs& a,const ImVec2& mouse,const V38BookmarkHit* hit){
    // A second PPM replaces the existing menu instead of being discarded.
    if(!selected_vfo())return;
    const bool red=selected_vfo_passband_hit(a,mouse)||selected_vfo_edge_hover(mouse);
    CursorPreview pv=make_preview(a,mouse);
    const double hz=red?selected_frequency():pv.freq;
    if(!(hz>0.0&&hz<=999999999999.0))return;
    g_v34_context={};
    g_v34_context.stage=1;g_v34_context.origin=mouse;
    g_v34_context.frequency=(u64)(hit && hit->valid?hit->frequency:(u64)(hz+0.5));
    if(hit && hit->valid){
        g_v34_context.on_bookmark=true;
        c_copy(g_v34_context.bookmark_name,sizeof(g_v34_context.bookmark_name),hit->name);
        c_copy(g_v34_context.bookmark_list,sizeof(g_v34_context.bookmark_list),hit->list);
    }
    g_v34_context.mode=selected_modulation();
    if(mode_index(g_v34_context.mode)<0)g_v34_context.mode=7;
    g_v34_context.filter_khz=(float)(vfo_bandwidth(selected_vfo())/1000.0);
    u64str(g_v34_context.frequency,g_v34_context.frequency_text,sizeof(g_v34_context.frequency_text));
    for(int i=0;i<10;i++)if(!g_presets[i].frequency){g_v34_context.number=i;break;}
    for(int i=0;i<10;i++)if(g_presets[i].frequency==g_v34_context.frequency){
        g_v34_context.selected_slot=i;g_v34_context.number=i;break;
    }
    // A quick preset has no label of its own: accept a small visual target
    // around its saved carrier, instead of requiring pixel-perfect Hz equality.
    // Only the nearest preset in the current view can become a removal target.
    if(g_v34_context.selected_slot<0 && !hit && a.freqToPixelRatio>0.0){
        double nearest=4.0;
        for(int i=0;i<10;i++){
            const double f=(double)g_presets[i].frequency;
            if(!(f>=a.lowFreq&&f<=a.highFreq))continue;
            const double x=a.fftRectMin.x+round_nearest((f-a.lowFreq)*a.freqToPixelRatio);
            const double dx=dabs(mouse.x-x);
            if(dx<=nearest){nearest=dx;g_v34_context.selected_slot=i;g_v34_context.number=i;}
        }
        if(g_v34_context.selected_slot>=0){
            g_v34_context.frequency=g_presets[g_v34_context.selected_slot].frequency;
            u64str(g_v34_context.frequency,g_v34_context.frequency_text,sizeof(g_v34_context.frequency_text));
        }
    }
}
static void v34_draw_context(){
    V34Context& c=g_v34_context;if(!c.stage||!g_supported||!g_enabled)return;
    if(c.stage!=10 && ui_IsKeyPressed(K_ESCAPE,false)){
        v39_restore_embedded_editor_hooks();c.stage=0;return;
    }
    if(c.stage!=5 && g_v39_native_editor_slots[0])v39_restore_embedded_editor_hooks();
    const float scale=core_ui_scale>0?core_ui_scale:1.0f;
    const int stage=c.stage;
    c.slot_popup_valid=false;
    if(stage==10){
        if(!c.bookmark_instance||!v34_native_frequency_manager_image()){
            c.stage=0;return;
        }
        auto handler=v38_native_fm_menu_callback(c.bookmark_instance);
        if(!handler){c.stage=0;return;}
        ui_SetNextWindowPos(c.origin,1,ImVec2());
        ui_SetNextWindowSize(ImVec2(2,2),1);
        if(ui_Begin("##EssentialNativeDeleteHost",nullptr,1|2|4|8|32|128|256|4096)){
            handler(c.bookmark_instance); // native YES/NO and native saveByName
        }
        ui_End();
        if(!((u8*)c.bookmark_instance)[0x5f]){
            if(c.edit_target_category[0])v39_switch_category(c.bookmark_instance,c.edit_target_category);
            c.stage=0;
        }
        return;
    }
    if(stage==5){
        void*module=(void*)GetModuleHandleA("frequency_manager.dll");
        if(!module||!v34_native_frequency_manager_image()||!c.bookmark_instance){
            v39_restore_embedded_editor_hooks();c.stage=0;return;
        }
        if(!v39_install_embedded_editor_hooks()){
            c_copy(c.error,sizeof(c.error),
                l10n("Brak zgodnego interfejsu Frequency Manager."));
            c.stage=6;return;
        }
        // Ordinary ImGui window, NOT a popup. The initial position is applied
        // once per opening. From then on ImGui owns movement like quick access.
        const float width=285.0f*scale;
        u8*io=(u8*)ui_GetIO();
        const float screenW=*(float*)(io+8),screenH=*(float*)(io+12);
        if(!c.bookmark_popup_placed){
            ImVec2 target=g_v39_bookmark_popup_last_pos_valid
                ?g_v39_bookmark_popup_last_pos
                :ImVec2((screenW-width)*0.5f,(screenH-230.0f*scale)*0.35f);
            if(target.x<4)target.x=4;if(target.y<4)target.y=4;
            if(screenW>width+8&&target.x+width>screenW-4)target.x=screenW-width-4;
            if(screenH>240*scale&&target.y+240*scale>screenH-4)
                target.y=screenH-240*scale-4;
            ui_SetNextWindowPos(target,1,ImVec2());
            c.bookmark_popup_placed=true;
        }
        ui_SetNextWindowSize(ImVec2(width,0.0f),1);
        if(ui_Begin("##EssentialNativeBookmarkForm",nullptr,1|2|8|32|64|256)){
            // Same native border drag behavior as the quick-access window.
            move_headerless_window_from_top_border();
            c.window_pos=ui_GetWindowPos();c.window_size=ui_GetWindowSize();
            c.window_valid=true;
            g_v39_bookmark_popup_last_pos=c.window_pos;
            g_v39_bookmark_popup_last_pos_valid=true;
            if(c.raised_stage!=5){
                ui_BringWindowToDisplayFront(ui_GetCurrentWindow());
                c.raised_stage=5;
            }
            v39_draw_bookmark_category_before_name();
            // Only three native popup operations are redirected. The native
            // dialog still owns all its input widgets, validation, map updates
            // and saveByName(), so Frequency Manager sees the result at once.
            using EditDialog=bool(*)(void*);
            EditDialog edit=(EditDialog)((u8*)module+0x1cd00);
            ((u8*)c.bookmark_instance)[0x5a]=c.native_edit_open?1:0;
            g_v39_embedded_popup_seen=false;
            g_v39_embedded_popup_started=false;
            g_v39_embedded_editor_call=true;
            const bool stillOpen=edit(c.bookmark_instance);
            g_v39_embedded_editor_call=false;
            ((u8*)c.bookmark_instance)[0x5a]=0;
            if(!g_v39_embedded_popup_seen||g_v39_embedded_popup_started){
                c_copy(c.error,sizeof(c.error),
                    l10n("Nie mozna wyswietlic edytora Frequency Manager."));
                c.stage=6;
            }else if(!stillOpen){
                if(c.native_edit_open && c.native_category_move){
                    const char*finalName=string_chars((u8*)c.bookmark_instance+0x90);
                    void*entry=v38_bookmark_find_named(c.bookmark_instance,finalName);
                    const double savedHz=entry?*(double*)((u8*)entry+0x40):-1.0;
                    const double editedHz=*(double*)((u8*)c.bookmark_instance+0xd0);
                    const bool saved=entry && savedHz==editedHz;
                    if(saved && v39_switch_category(c.bookmark_instance,c.edit_origin_category)){
                        if(!v38_begin_native_bookmark_delete())c.stage=0;
                    }else{
                        v39_switch_category(c.bookmark_instance,c.edit_origin_category);
                        c.stage=0;
                    }
                }else{
                    if(c.edit_origin_category[0]){
                        bool changed=!c_equal(c.edit_origin_category,
                            string_chars((u8*)c.bookmark_instance+0x120));
                        if(changed){
                            const char*name=string_chars((u8*)c.bookmark_instance+0x90);
                            void*savedEntry=v38_bookmark_find_named(c.bookmark_instance,name);
                            if(c.native_edit_open || !savedEntry)
                                v39_switch_category(c.bookmark_instance,c.edit_origin_category);
                        }
                    }
                    c.stage=0;
                }
            }
        }
        ui_End();
        if(c.stage!=5)v39_restore_embedded_editor_hooks();
        return;
    }
    // Two-item context menu must not reserve the height of the editing form.
    // Keep the two-item PPM menu compact; quick save/overwrite has a narrower
    // form while bookmark confirmations and errors retain their old width.
    const float width=(stage==1?(c.on_bookmark?236.0f:226.0f):
        ((stage==2 || stage==3)?280.0f:320.0f))*scale;
    // V39: no empty black strip below controls; retain enough room for
    // errors and the active confirmation row at normal UI scaling.
    // Approximate height is only used to keep the first placement on-screen.
    // Actual height is 0 (ImGui auto-fit), eliminating blank space below
    // buttons at every DPI scale and in both overwrite/normal forms.
    const float height=(stage==1?(c.on_bookmark?90.0f:62.0f):
        (stage==3?267.0f:(stage==2?236.0f:(stage==4?110.0f:
        (stage==9||stage==8?116.0f:100.0f)))))*scale;
    ImVec2 origin=c.origin;
    // Clamp menus to the ImGui viewport rather than opening off-screen.
    u8*io=(u8*)ui_GetIO();
    float screenW=*(float*)(io+8),screenH=*(float*)(io+12);
    if(screenW>width+8&&origin.x>screenW-width-8)origin.x=screenW-width-8;
    if(screenH>height+8&&origin.y>screenH-height-8)origin.y=screenH-height-8;
    if(origin.x<4)origin.x=4;if(origin.y<4)origin.y=4;
    // PPM menu opens at the click, but a FORM must never inherit that mouse
    // location. ImGuiCond_Appearing previously re-anchored ##EssentialV39Form
    // to c.origin after EVERY new right-click, which made it "jump".
    // Remember its last user-moved position independently of the context.
    // Only set the form position once on entry; otherwise mouse dragging
    // would fight a per-frame SetNextWindowPos override.
    if(stage==1){
        ui_SetNextWindowPos(origin,1,ImVec2());
    }else if(!c.form_placed){
        ImVec2 target=g_v39_quick_form_last_pos_valid
            ? g_v39_quick_form_last_pos
            : ImVec2((screenW-width)*0.5f,(screenH-height)*0.35f);
        if(screenW>width+8.0f){
            if(target.x<4.0f)target.x=4.0f;
            if(target.x+width>screenW-4.0f)target.x=screenW-width-4.0f;
        }else target.x=4.0f;
        if(screenH>height+8.0f){
            if(target.y<4.0f)target.y=4.0f;
            if(target.y+height>screenH-4.0f)target.y=screenH-height-4.0f;
        }else target.y=4.0f;
        ui_SetNextWindowPos(target,1,ImVec2());
        c.form_placed=true;
    }
    ui_SetNextWindowSize(ImVec2(width,0.0f),1); // auto-fit vertical height
    const int flags=1|2|8|32|256|(stage==1?4:0);
    if(ui_Begin(stage==1?"##EssentialContextMenu":"##EssentialV39Form",nullptr,flags)){
        if(stage!=1)move_headerless_window_from_top_border();
        c.window_pos=ui_GetWindowPos();c.window_size=ui_GetWindowSize();c.window_valid=true;
        if(stage!=1){
            g_v39_quick_form_last_pos=c.window_pos;
            g_v39_quick_form_last_pos_valid=true;
        }
        // Raising the parent every frame covered its own ##v34_slots popup.
        // The dropdown should retain higher z-order until it is closed.
        if(c.raised_stage!=stage){ui_BringWindowToDisplayFront(ui_GetCurrentWindow());c.raised_stage=stage;}
        if(stage==1){
            if(ui_Selectable(c.selected_slot>=0?"Usun z szybkiego wyboru":"Dodaj do szybkiego wyboru",false,0,ImVec2())){
                c.stage=c.selected_slot>=0?8:2;
            }
            if(ui_Selectable(c.on_bookmark?"Usun zakladke":"Dodaj do zakladek",false,0,ImVec2())){
                if(c.on_bookmark)c.stage=9;
                else v39_start_category_selection();
            }
            if(c.on_bookmark && ui_Selectable("Edytuj zakladke",false,0,ImVec2()))v38_bookmark_edit();
        }else if(stage==2 || stage==3){
            ui_TextUnformatted("Dodaj do szybkiego wyboru",nullptr);
            ui_TextUnformatted("Czestotliwosc [Hz]",nullptr);
            ui_SetNextItemWidth(-1);
            ui_InputText("##v34_freq",c.frequency_text,sizeof(c.frequency_text),0,nullptr,nullptr);
            ui_TextUnformatted("Modulacja",nullptr);
            ui_SetNextItemWidth(-1);
            int index=mode_index(c.mode);
            if(ui_Combo("##v34_mode",&index,"NFM\0WFM\0AM\0LSB\0USB\0DSB\0CW\0RAW\0\0",-1)&&index>=0&&index<8)c.mode=MODE_ORDER[index];
            ui_TextUnformatted("Szerokosc pasma [kHz]",nullptr);
            ui_SetNextItemWidth(-1);
            ui_InputFloat("##v34_bw",&c.filter_khz,0,0,"%.3f",0);
            char chosen[64],num[8];u64str((u64)c.number,num,sizeof(num));
            c_copy(chosen,sizeof(chosen),l10n("Numer: "));c_append(chosen,sizeof(chosen),num);
            c_append(chosen,sizeof(chosen),g_presets[c.number].frequency?l10n(" (zajety)##slot"):l10n(" (wolny)##slot"));
            if(ui_Button(chosen,ImVec2(-1,0)))ui_OpenPopup("##v34_slots",0);
            // Place the 0-9 popup inside the viewport, ABOVE the parent in
            // z-order. If little space remains beneath the button, flip up.
            const ImVec2 anchor=ui_GetItemRectMax();
            float popupHeight=10.0f*(ui_GetFontSize()+7.0f*scale)+16.0f*scale;
            float popupWidth=175.0f*scale;
            float popupX=anchor.x-popupWidth;
            float popupY=anchor.y+3.0f*scale;
            if(screenW>popupWidth+8.0f && popupX>screenW-popupWidth-4.0f)popupX=screenW-popupWidth-4.0f;
            if(popupX<4.0f)popupX=4.0f;
            if(screenH>popupHeight+8.0f && popupY+popupHeight>screenH-4.0f)popupY=anchor.y-popupHeight-26.0f*scale;
            if(popupY<4.0f)popupY=4.0f;
            ui_SetNextWindowPos(ImVec2(popupX,popupY),1,ImVec2());
            if(ui_BeginPopup("##v34_slots",0)){
                c.slot_popup_pos=ui_GetWindowPos();c.slot_popup_size=ui_GetWindowSize();
                c.slot_popup_valid=true;
                for(int i=0;i<10;i++){
                    const bool used=g_presets[i].frequency!=0;
                    ui_PushTextColor(0,used?0xFF7777FFu:0xFF77DF77u);
                    char label[40],number[8];u64str((u64)i,number,sizeof(number));
                    c_copy(label,sizeof(label),number);
                    c_append(label,sizeof(label),used?l10n(" - zajety"):l10n(" - wolny"));
                    if(ui_Selectable(label,c.number==i,0,ImVec2())){c.number=i;ui_CloseCurrentPopup();}
                    ui_PopStyleColor(1);
                }
                ui_EndPopup();
            }
            if(stage==3){
                ui_TextUnformatted("Skrot jest zajety. Nadpisac?",nullptr);
                if(ui_Button("Tak##overwrite_slot",ImVec2(75*scale,0)))c.stage=7;
                ui_SameLine(0,10*scale);
                if(ui_Button("Nie##overwrite_slot",ImVec2(75*scale,0)))c.stage=2;
            }else{
                if(ui_Button("Zapisz##v34",ImVec2(90*scale,0))){
                    u64 hz=0;
                    const double bw=(double)c.filter_khz*1000.0;
                    if(parse_frequency(c.frequency_text,hz)&&hz>0 && bw>=0.0&&bw<=(double)MAX_FILTER&&mode_index(c.mode)>=0){
                        c.frequency=hz;
                        if(g_presets[c.number].frequency)c.stage=3;
                        else c.stage=7;
                    }else c_copy(c.error,sizeof(c.error),"Nieprawidlowa czestotliwosc lub szerokosc filtra.");
                }
                ui_SameLine(0,10*scale);
                if(ui_Button("Anuluj##v34",ImVec2(80*scale,0)))c.stage=0;
            }
            if(c.error[0])ui_TextUnformatted(c.error,nullptr);
        }else if(stage==4){
            ui_TextUnformatted("Ta czestotliwosc jest juz zapisana.",nullptr);
            ui_TextUnformatted("Czy nadpisac istniejaca zakladke?",nullptr);
            if(ui_Button("Tak##bookmark",ImVec2(90*scale,0))){
                if(!v34_open_native_bookmark(c.bookmark_instance,true)){
                    c_copy(c.error,sizeof(c.error),l10n("Nie mozna otworzyc edytora zakladek."));c.stage=6;
                }
            }
            ui_SameLine(0,10*scale);
            if(ui_Button("Nie##bookmark",ImVec2(90*scale,0)))c.stage=0;
        }else if(stage==8){
            char number[8],frequency[32],message[96];
            u64str((u64)c.selected_slot,number,sizeof(number));
            u64str(c.frequency,frequency,sizeof(frequency));
            c_copy(message,sizeof(message),l10n("Usun szybki wybor nr "));
            c_append(message,sizeof(message),number);
            c_append(message,sizeof(message),"?");
            ui_TextUnformatted(message,nullptr);
            ui_TextUnformatted(frequency,nullptr);
            if(ui_Button("Tak##delete_quick",ImVec2(90*scale,0))){
                if(c.selected_slot>=0&&c.selected_slot<10 &&
                   g_presets[c.selected_slot].frequency==c.frequency)
                    delete_preset(c.selected_slot);
                c.stage=0;
            }
            ui_SameLine(0,10*scale);
            if(ui_Button("Nie##delete_quick",ImVec2(90*scale,0)))c.stage=0;
        }else if(stage==9){
            ui_TextUnformatted("Usun wybrana zakladke?",nullptr);
            ui_TextUnformatted(c.bookmark_name,nullptr);
            char frequency[40];u64str(c.frequency,frequency,sizeof(frequency));
            ui_TextUnformatted(frequency,nullptr);
            if(ui_Button("Tak##delete_bookmark",ImVec2(90*scale,0))){
                if(!v38_begin_native_bookmark_delete()){
                    c_copy(c.error,sizeof(c.error),l10n("Otworz liste zakladki w Frequency Manager."));c.stage=6;
                }
            }
            ui_SameLine(0,10*scale);
            if(ui_Button("Nie##delete_bookmark",ImVec2(90*scale,0)))c.stage=0;
        }else if(stage==6){
            ui_TextUnformatted(c.error,nullptr);
            if(ui_Button("Zamknij##error",ImVec2(100*scale,0)))c.stage=0;
        }
    }
    ui_End();
    // Stage 7 applies the preset AFTER the ImGui UI has ended. Do not retune.
    if(c.stage==7){
        g_presets[c.number]={(u64)c.frequency,c.mode,(u32)((double)c.filter_khz*1000.0+0.5)};
        refresh_preset_editors();preset_changed();c.stage=0;
    }
}
