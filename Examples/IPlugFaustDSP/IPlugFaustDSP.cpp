#include "IPlugFaustDSP.h"
#include "IPlug_include_in_plug_src.h"

IPlugFaustDSP::IPlugFaustDSP(IPlugInstanceInfo instanceInfo)
: IPLUG_CTOR(kNumParams, 1, instanceInfo)
{
  InitParamRange(0, kNumParams-1, 0, "Param %i", 0., 0., 1., 0.01, "", IParam::kFlagsNone);
  
#if IPLUG_DSP
  mFaustProcessor.SetMaxChannelCount(MaxNChannels(kInput), MaxNChannels(kOutput));
  mFaustProcessor.Init();
  mFaustProcessor.CompileCPP();
  mFaustProcessor.SetAutoRecompile(true);
#endif
  
#if IPLUG_EDITOR
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    IRECT b = pGraphics->GetBounds().GetPadded(-20);
    
    pGraphics->AttachCornerResizer(kUIResizerScale);
    pGraphics->LoadFont(ROBOTTO_FN);
    
    pGraphics->AttachPanelBackground(COLOR_BLACK);
    pGraphics->AttachControl(new IVScopeControl<>(b.GetReducedFromTop(50)), kControlTagScope);
  };
#endif
}

#if IPLUG_DSP

void IPlugFaustDSP::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  mFaustProcessor.ProcessBlock(inputs, outputs, nFrames);
  mScopeBallistics.ProcessBlock(outputs, nFrames);
}

void IPlugFaustDSP::OnReset()
{
  mFaustProcessor.SetSampleRate(GetSampleRate());
}

void IPlugFaustDSP::OnParamChange(int paramIdx)
{
  mFaustProcessor.SetParameterValueNormalised(paramIdx, GetParam(paramIdx)->Value());
}

void IPlugFaustDSP::OnIdle()
{
  mScopeBallistics.TransmitData(*this);
}
#endif
