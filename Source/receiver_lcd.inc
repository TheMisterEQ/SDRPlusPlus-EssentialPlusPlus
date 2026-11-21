// Essential++ V32: five hardware-LCD colourways, drawn without textures or allocations.
// In ImGui packed colors are AABBGGRR (not RRGGBBAA).
struct LcdPalette { u32 ink,dim,glow,frame,rim,divider,glass,substrate,off,soft; };
static constexpr u32 lcd_rgb(u8 r,u8 g,u8 b){return 0xFF000000u|((u32)b<<16)|((u32)g<<8)|r;}
static const LcdPalette& lcd_palette(){
    static const LcdPalette palettes[5]={
        {lcd_rgb(194,231,185),lcd_rgb(173,199,165),lcd_rgb(167,245,191),lcd_rgb(75,91,66),lcd_rgb(108,128,102),lcd_rgb(56,71,48),lcd_rgb(9,13,6),lcd_rgb(20,27,16),lcd_rgb(29,38,25),lcd_rgb(154,194,161)},
        {lcd_rgb(169,216,255),lcd_rgb(135,179,215),lcd_rgb(111,191,255),lcd_rgb(57,83,109),lcd_rgb(96,137,170),lcd_rgb(40,68,98),lcd_rgb(7,14,24),lcd_rgb(13,26,43),lcd_rgb(28,48,69),lcd_rgb(127,171,207)},
        {lcd_rgb(255,190,187),lcd_rgb(216,147,148),lcd_rgb(255,119,119),lcd_rgb(110,62,63),lcd_rgb(159,93,99),lcd_rgb(84,43,46),lcd_rgb(21,7,9),lcd_rgb(36,15,18),lcd_rgb(58,27,29),lcd_rgb(217,143,144)},
        {lcd_rgb(255,208,159),lcd_rgb(219,171,121),lcd_rgb(255,177,85),lcd_rgb(112,77,50),lcd_rgb(155,111,73),lcd_rgb(91,62,36),lcd_rgb(22,12,5),lcd_rgb(39,24,11),lcd_rgb(64,42,23),lcd_rgb(225,168,102)},
        {lcd_rgb(249,189,253),lcd_rgb(211,157,222),lcd_rgb(233,121,255),lcd_rgb(97,57,115),lcd_rgb(145,93,169),lcd_rgb(81,42,104),lcd_rgb(17,8,24),lcd_rgb(37,17,45),lcd_rgb(59,30,70),lcd_rgb(215,148,229)}
    };
    return palettes[(unsigned)g_colour_map<5u?g_colour_map:0];
}
static const LcdPalette& lcd_original_palette(){
    // Legacy green palette retained for compatibility with older overlays.
    static const LcdPalette green={lcd_rgb(194,231,185),lcd_rgb(173,199,165),lcd_rgb(167,245,191),lcd_rgb(75,91,66),lcd_rgb(108,128,102),lcd_rgb(56,71,48),lcd_rgb(9,13,6),lcd_rgb(20,27,16),lcd_rgb(29,38,25),lcd_rgb(154,194,161)};
    return green;
}
static void draw_lcd_glass(ImDrawList*dl,const ImVec2&lo,const ImVec2&hi,float scale,bool toolbar=true){
    const LcdPalette&c=toolbar?lcd_palette():lcd_original_palette();
    draw_AddRectFilled(dl,lo,hi,c.frame,2*scale,0);
    draw_AddRectFilled(dl,ImVec2(lo.x+scale,lo.y+scale),ImVec2(hi.x-scale,hi.y-scale),c.glass,scale,0);
    draw_AddRectFilled(dl,ImVec2(lo.x+3*scale,lo.y+3*scale),ImVec2(hi.x-3*scale,hi.y-3*scale),c.substrate,0,0);
    draw_AddLine(dl,ImVec2(lo.x+3*scale,hi.y-2*scale),ImVec2(hi.x-3*scale,hi.y-2*scale),c.rim,scale);
    draw_AddLine(dl,ImVec2(hi.x-2*scale,lo.y+3*scale),ImVec2(hi.x-2*scale,hi.y-2*scale),c.rim,scale);
    // Exactly the same 3px LCD lattice as V29; no new rendering layers.
    for(float y=lo.y+5*scale;y<hi.y-4*scale;y+=3*scale)
        draw_AddLine(dl,ImVec2(lo.x+4*scale,y),ImVec2(hi.x-4*scale,y),0x08000B08u,scale*0.5f);
}
static const u8* lcd_glyph(char c){
    static const u8 letters[26][7]={
        {14,17,17,31,17,17,17},{30,17,17,30,17,17,30},{14,17,16,16,16,17,14},
        {30,17,17,17,17,17,30},{31,16,16,30,16,16,31},{31,16,16,30,16,16,16},
        {14,17,16,23,17,17,15},{17,17,17,31,17,17,17},{14,4,4,4,4,4,14},
        {7,2,2,2,18,18,12},{17,18,20,24,20,18,17},{16,16,16,16,16,16,31},
        {17,27,21,21,17,17,17},{17,25,25,21,19,19,17},{14,17,17,17,17,17,14},
        {30,17,17,30,16,16,16},{14,17,17,17,21,18,13},{30,17,17,30,20,18,17},
        {15,16,16,14,1,1,30},{31,4,4,4,4,4,4},{17,17,17,17,17,17,14},
        {17,17,17,17,17,10,4},{17,17,17,21,21,21,10},{17,17,10,4,10,17,17},
        {17,17,10,4,4,4,4},{31,1,2,4,8,16,31}};
    static const u8 numbers[10][7]={
        {14,17,19,21,25,17,14},{4,12,4,4,4,4,14},{14,17,1,2,4,8,31},
        {30,1,1,14,1,1,30},{2,6,10,18,31,2,2},{31,16,16,30,1,1,30},
        {14,16,16,30,17,17,14},{31,1,2,4,8,8,8},{14,17,17,14,17,17,14},
        {14,17,17,15,1,1,14}};
    static const u8 dot[]={0,0,0,0,0,6,6},dash[]={0,0,0,31,0,0,0},colon[]={0,4,4,0,4,4,0};
    static const u8 slash[]={1,2,2,4,8,8,16},plus[]={0,4,4,31,4,4,0};
    static const u8 left[]={2,4,8,8,8,4,2},right[]={8,4,2,2,2,4,8},empty[]={0,0,0,0,0,0,0};
    static const u8 lowerK[]={16,16,18,20,24,20,18},lowerZ[]={0,0,31,2,4,8,31},lowerD[]={1,1,15,17,17,17,15};
    if(c=='k')return lowerK;if(c=='z')return lowerZ;if(c=='d')return lowerD;
    if(c>='a'&&c<='z')c=(char)(c-'a'+'A');
    if(c>='A'&&c<='Z')return letters[c-'A'];if(c>='0'&&c<='9')return numbers[c-'0'];
    if(c=='.')return dot;if(c=='-')return dash;if(c==':')return colon;if(c=='/')return slash;if(c=='+')return plus;
    if(c=='('||c=='[')return left;if(c==')'||c==']')return right;return empty;
}
static float lcd_character_units(char c,bool segmented){
    if(c==' ')return 2.5f;if(c=='.'||c==':')return 2.0f;
    if(segmented&&((c>='0'&&c<='9')||c=='-'))return 4.2f;
    if(c=='-')return 5.5f;return 5.75f;
}
static float lcd_pixel(float value){return (float)(int)(value+0.5f);}
static const char* lcd_units_begin(const char*text,bool segmented){
    if(!segmented||!text||!((*text>='0'&&*text<='9')||*text=='-'))return 0;
    const char*p=text;
    while((*p>='0'&&*p<='9')||*p=='-'||*p=='.'||*p==' ')p++;
    return *p?p:0;
}
static float lcd_text_width(const char*text,float height,bool segmented=false){
    const char*units=lcd_units_begin(text,segmented);float width=0;
    for(const char*p=text;*p;p++)width+=lcd_character_units(*p,segmented)*height*(units&&p>=units?0.85f:1)/7;
    return width;
}
// Discrete electrodes with one-pixel isolation: the strokes never merge.
// Keep inactive electrodes just above the glass, rather than luminous ghosts.
static void draw_lcd_segment_digit(ImDrawList*dl,char digit,const ImVec2&pos,float height,float fit,u32 color){
    float x=lcd_pixel(pos.x),y=lcd_pixel(pos.y),h=lcd_pixel(height),w=lcd_pixel(height*0.5f*fit);
    float t=lcd_pixel(height*0.085f);if(t<1)t=1;if(t>w*0.25f)t=1;
    float middle=lcd_pixel(h*0.5f-t*0.5f),gap=1;
    static const u8 masks[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
    u8 mask=digit=='-'?0x40:masks[digit-'0'];
    ImVec2 lo[]={ImVec2(t+gap,0),ImVec2(w-t,t+gap),ImVec2(w-t,middle+t+gap),
        ImVec2(t+gap,h-t),ImVec2(0,middle+t+gap),ImVec2(0,t+gap),ImVec2(t+gap,middle)};
    ImVec2 hi[]={ImVec2(w-t-gap,t),ImVec2(w,middle-gap),ImVec2(w,h-t-gap),
        ImVec2(w-t-gap,h),ImVec2(t,h-t-gap),ImVec2(t,middle-gap),ImVec2(w-t-gap,middle+t)};
    for(int segment=0;segment<7;segment++){
        u32 ink=(mask&(1<<segment))?color:lcd_palette().off;
        if(hi[segment].x>lo[segment].x&&hi[segment].y>lo[segment].y)
            draw_AddRectFilled(dl,ImVec2(x+lo[segment].x,y+lo[segment].y),ImVec2(x+hi[segment].x,y+hi[segment].y),ink,0,0);
    }
}
static void draw_lcd_text(ImDrawList*dl,const char*text,const ImVec2&pos,float height,u32 color,float available,bool segmentNumbers=false){
    if(!text||!text[0]||!(height>0)||!(available>0))return;
    float natural=lcd_text_width(text,height,segmentNumbers),fit=natural>available?available/natural:1;
    const char*units=lcd_units_begin(text,segmentNumbers);float x=lcd_pixel(pos.x);
    for(const char*p=text;*p;p++){
        float glyphHeight=height*(units&&p>=units?0.85f:1),pixel=glyphHeight/7,pixelX=pixel*fit;
        float y=lcd_pixel(pos.y+height-glyphHeight),advance=lcd_character_units(*p,segmentNumbers)*pixelX;
        if(segmentNumbers&&((*p>='0'&&*p<='9')||*p=='-')){
            draw_lcd_segment_digit(dl,*p,ImVec2(x+pixelX*0.25f,y),glyphHeight,fit,color);
        }else if(*p=='.'||*p==':'){
            float dot=lcd_pixel(pixelX);if(dot<1)dot=1;
            if(*p=='.')draw_AddRectFilled(dl,ImVec2(lcd_pixel(x),lcd_pixel(y+glyphHeight)-dot),ImVec2(lcd_pixel(x)+dot,lcd_pixel(y+glyphHeight)),color,0,0);
            else for(int row=0;row<2;row++){
                float top=lcd_pixel(y+glyphHeight*(row?0.65f:0.2f));
                draw_AddRectFilled(dl,ImVec2(lcd_pixel(x),top),ImVec2(lcd_pixel(x)+dot,top+dot),color,0,0);
            }
        }else{
            const u8*rows=lcd_glyph(*p);
            // A real 5x7 LCD cell lattice, with discrete square electrodes.
            // Quantize the pitch as a whole so the dots have identical sizes
            // even at 80% scale; centre the shorter lattice on the baseline.
            float pitch=(float)(int)((glyphHeight+1)/7);if(pitch<1)pitch=1;
            float pitchX=pitch*fit,cell=pitch>1?pitch-0.5f:1;
            float top=lcd_pixel(y+glyphHeight-(6*pitch+cell));
            for(int row=0;row<7;row++)for(int c=0;c<5;c++){
                if(!(rows[row]&(1<<(4-c))))continue;
                ImVec2 a(lcd_pixel(x+c*pitchX),lcd_pixel(top+row*pitch));
                float cellWidth=cell*fit;if(cellWidth<1)cellWidth=1;
                cellWidth=(float)(int)(cellWidth*2+0.5f)*0.5f;
                ImVec2 b(a.x+cellWidth,a.y+cell);
                if(b.x>a.x&&b.y>a.y)draw_AddRectFilled(dl,a,b,color,0,0);
            }
        }
        x+=advance;
    }
}
