// History belongs to the selected receiver, and stores complete, valid Radio states.
// A complete state enters history after at least one second without tuning changes.
struct ReceiverState { double frequency,bandwidth;int modulation; };
struct ReceiverHistory { ReceiverState states[11],candidate;int count,cursor;char target[256];bool pending;double since; };
static ReceiverHistory g_history={};
static void history_reset(){g_history={};}
static bool receiver_state(ReceiverState&state){
    void*v=selected_vfo();if(!v)return false;
    int mode=selected_modulation();if(mode_index(mode)<0)return false;
    state={selected_frequency(),vfo_bandwidth(v),mode};
    return state.frequency>=0&&state.frequency<=999999999999.0&&state.bandwidth>0&&state.bandwidth<=1e12;
}
static bool receiver_state_equal(const ReceiverState&a,const ReceiverState&b){
    return a.modulation==b.modulation&&dabs(a.frequency-b.frequency)<0.5&&dabs(a.bandwidth-b.bandwidth)<0.5;
}
static void history_poll(){
    ReceiverState state;const char*target=string_chars(wf_selected_name());
    if(!receiver_state(state)){history_reset();return;}
    double now=ui_GetTime();
    if(!c_equal(g_history.target,target)){
        history_reset();c_copy(g_history.target,sizeof(g_history.target),target);
    }
    if(g_history.count&&receiver_state_equal(state,g_history.states[g_history.cursor])){
        g_history.pending=false;return;
    }
    if(!g_history.pending||!receiver_state_equal(state,g_history.candidate)){
        g_history.candidate=state;g_history.since=now;g_history.pending=true;return;
    }
    if(now<g_history.since){g_history.since=now;return;}
    if(now-g_history.since<1.0)return;
    // A new edit after going back discards the abandoned forward branch.
    if(g_history.count)g_history.count=g_history.cursor+1;
    if(g_history.count==11){
        for(int i=0;i<10;i++)g_history.states[i]=g_history.states[i+1];
        g_history.count=10;g_history.cursor=9;
    }
    g_history.states[g_history.count++]=state;g_history.cursor=g_history.count-1;g_history.pending=false;
}
static bool restore_receiver_state(const ReceiverState&state){
    if(!set_modulation(state.modulation))return false;
    float width=(float)state.bandwidth;
    if(!radio_command(3,&width,0)||dabs(vfo_bandwidth(selected_vfo())-state.bandwidth)>=0.5)return false;
    tune_frequency(state.frequency);
    ReceiverState actual;return receiver_state(actual)&&receiver_state_equal(actual,state);
}
static bool history_back_available(){
    if(!g_history.count)return false;
    if(g_history.cursor>0)return true;
    ReceiverState state;return receiver_state(state)&&!receiver_state_equal(state,g_history.states[g_history.cursor]);
}
static bool history_move(int direction){
    history_poll();
    if(!g_history.count)return false;
    ReceiverState before;if(!receiver_state(before))return false;
    // An unrecorded, transient tune returns to the last stable state, without
    // adding the transient frequency or creating a forward entry for it.
    int next=direction<0&&!receiver_state_equal(before,g_history.states[g_history.cursor])?
        g_history.cursor:g_history.cursor+(direction<0?-1:1);
    if(next<0||next>=g_history.count)return false;
    if(!restore_receiver_state(g_history.states[next])){
        restore_receiver_state(before);return false;
    }
    g_history.cursor=next;g_history.pending=false;return true;
}
