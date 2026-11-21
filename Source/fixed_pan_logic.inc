// Essential++ V32: deterministic SDR#-style pinned-tuning geometry.
// No SDR++, DSP, ImGui or Windows calls here: the same function is unit-tested
// independently of the GUI and the hardware-specific binary bridge.
struct FixedPanPlan {
    double displayCenterHz;
    double hardwareCenterHz;
    double viewOffsetHz;
    double markerHz;
    bool atEdge;
};
static double fixed_clamp(double n,double lo,double hi){return n<lo?lo:(n>hi?hi:n);}
static double fixed_abs(double v){return v<0?-v:v;}
static FixedPanPlan plan_fixed_pan(
    double startHardwareHz,double startViewOffsetHz,double pinnedHz,
    double wholeBandwidthHz,double viewBandwidthHz,double hzPerPixel,
    double pixelDragX,double currentHardwareHz,bool allowHardwareRetune)
{
    FixedPanPlan result={};
    // SDR++ drag direction: dragging the spectrum to the right reveals lower frequencies.
    const double desiredCenter=startHardwareHz+startViewOffsetHz-pixelDragX*hzPerPixel;
    const double availableOffset=(wholeBandwidthHz>viewBandwidthHz)?
        (wholeBandwidthHz-viewBandwidthHz)*0.5:0.0;
    // Consume the existing digital pan range before retuning the SDR hardware.
    const double offsetWithoutRetune=fixed_clamp(desiredCenter-currentHardwareHz,
                                                  -availableOffset,availableOffset);
    const double requiredHardwareCenter=desiredCenter-offsetWithoutRetune;
    result.hardwareCenterHz=allowHardwareRetune?requiredHardwareCenter:currentHardwareHz;
    result.viewOffsetHz=fixed_clamp(desiredCenter-result.hardwareCenterHz,
                                    -availableOffset,availableOffset);
    result.displayCenterHz=result.hardwareCenterHz+result.viewOffsetHz;
    // Clamp the TUNED frequency only when its red line reaches the *visible*
    // left/right edge. Before this point the absolute frequency is unchanged.
    const double halfView=viewBandwidthHz*0.5;
    const double margin=fixed_clamp(fixed_abs(hzPerPixel)*1.0,0.0,halfView*0.1);
    const double lower=result.displayCenterHz-halfView+margin;
    const double upper=result.displayCenterHz+halfView-margin;
    result.markerHz=fixed_clamp(pinnedHz,lower,upper);
    result.atEdge=result.markerHz!=pinnedHz;
    return result;
}
