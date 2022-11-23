#include "DaisyDuino.h"

#define MAXCH 10

DaisyHardware hw;
Oscillator osc[MAXCH];
AdEnv env[MAXCH];

Svf filt;
ReverbSc verb;
Compressor comp;

const static float scale[7] = {0, 2, 4, 5, 7, 9, 11};
uint8_t keylist[] = {'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/'};

static float freq;
float sig, rawsig, filtsig, sendsig, wetvl, wetvr;
float comp_sig;
float out_sigl,out_sigr;
float vamt, dec;

uint8_t ch_cnt = 0;

static void audio(float **in, float **out, size_t size) {

  // Parameters
  dec = (1.0f) * 5 + .2;
  vamt = 1.0f;

  // Audio Loop
  for (size_t i = 0; i < size; i++) {
    // Process
    for (int c = 0; c < MAXCH; c++) {
      osc[c].SetAmp(env[c].Process() / (MAXCH/4));
      rawsig += osc[c].Process() / (size * MAXCH);
    }
    comp_sig = comp.Process(rawsig);
    filt.Process(comp_sig);
    filtsig = filt.Low();
    sendsig = filtsig * vamt;
    verb.Process(sendsig, sendsig, &wetvl, &wetvr);

    out[0][i] = (filtsig + (wetvl)) * 0.707f;
    out[1][i] = (filtsig + (wetvr)) * 0.707f;
  }
}

void InitSynth(float samplerate) {
  dec = 0.62;
  // Init Osc and Nse
  for (int c = 0; c < MAXCH; c++) {
    osc[c].Init(samplerate);
    osc[c].SetWaveform(Oscillator::WAVE_SIN);
    osc[c].SetFreq(100.0f);
    osc[c].SetAmp(.5f);
    env[c].Init(samplerate);
    env[c].SetCurve(-15.0f);
    env[c].SetTime(ADENV_SEG_ATTACK, 0.01f);
    env[c].SetTime(ADENV_SEG_DECAY, 0.1f);
    env[c].SetTime(ADSR_SEG_RELEASE, .5);
    env[c].SetMax(1.f);
    env[c].SetMin(0.f);
  }

  filt.Init(samplerate);
  filt.SetRes(0.5f);
  filt.SetDrive(0.8f);
  filt.SetFreq(2400.0f);

  verb.Init(samplerate);
  verb.SetFeedback(0.91f);
  verb.SetLpFreq(12000.0f);

  comp.SetThreshold(-1.5f);
  comp.SetRatio(2.0f);
  comp.SetAttack(0.005f);
  comp.SetRelease(0.1250);
}

void setup() {
  float samplerate, callback_rate;
  hw = DAISY.init(DAISY_POD, AUDIO_SR_48K);

  samplerate = DAISY.get_samplerate();
  InitSynth(samplerate);

  Serial.begin(115200);
  while (!Serial) {
  };
  DAISY.begin(audio);
}

void loop() {
  if (Serial.available()) {
    uint8_t rcvdata = Serial.read();
    for (int c = 0; c < MAXCH; c++) {
      if (rcvdata == keylist[c]) {
        freq = mtof(72.0f + scale[c % 7] + (int)(c / 7) * 12);
        osc[ch_cnt%MAXCH].SetFreq(freq);
        env[ch_cnt%MAXCH].SetTime(ADENV_SEG_DECAY, dec);
        env[ch_cnt%MAXCH].Trigger();
        ch_cnt++;
      }
    }
  }
}
