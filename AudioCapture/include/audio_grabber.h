#ifndef SRC_AUDIO_CAPTURE_H_
#define SRC_AUDIO_CAPTURE_H_

#include <string>
#include <iostream>

#include <alsa/asoundlib.h>

namespace audio
{

class AudioGrabberConfigs
{
 public:
  std::string Device;
  int NumSamplesBlock;
  unsigned int Channels;
  unsigned int Freq;
  snd_pcm_format_t Format;
  int ChannelBufferSize;
};

class AudioGrabber {
 public:
  AudioGrabber(std::string configs_file_ = "./configs.xml")
  {
    configs_file = configs_file_;
    Setup();
  }
  virtual ~AudioGrabber()
  {
    snd_pcm_hw_params_free (grabber_params);
    snd_pcm_close (grabber);
    std::cout << "Audio interface closed" << std::endl;
  }
  
  int* Grab();
  
  inline int GetChannelBufferSize()
  {
	  return configs.ChannelBufferSize;
  }

  inline int GetTotalBufferSize()
  {
	  return configs.ChannelBufferSize * configs.Channels;
  }

  inline int GetNumSamplesBlock()
  {
	  return configs.NumSamplesBlock;
  }

  inline int GetNumChannels()
  {
	  return configs.Channels;\
  }

 private:
  std::string configs_file;
  AudioGrabberConfigs configs;
  snd_pcm_t *grabber;
  snd_pcm_hw_params_t *grabber_params;
  
  void Setup();
  void LoadConfigs();
  int SetConfigs();
  int ApplyConfigs();
};

}

#endif /* SRC_AUDIO_CAPTURE_H_ */
