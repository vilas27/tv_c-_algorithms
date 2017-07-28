#include "audio_grabber.h"

#include <fstream>
#include <sstream>

#include "rapidxml.hpp"

namespace audio
{
/*
 * Public methods
 */
int* AudioGrabber::Grab()
{
  int error = 0;
  int* buffer = (int*) malloc(configs.Channels * configs.ChannelBufferSize);
  if ((error = snd_pcm_readi(grabber, buffer, configs.NumSamplesBlock)) != configs.NumSamplesBlock)
  {
    std::cerr << "Read from audio interface failed (" << snd_strerror (error) << ")" << std::endl;
  }
  return buffer;
}

/*
 * Private methods
 */
  void AudioGrabber::Setup()
  {
    LoadConfigs();
    SetConfigs();
    ApplyConfigs();
  }
  
  void AudioGrabber::LoadConfigs()
  {
    std::ifstream file;
    file.open(configs_file.c_str());

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    std::string content(buffer.str());
    rapidxml::xml_document<> doc;
    doc.parse<0>(&content[0]);
    
    /*
     * Get Audio 
     */
    rapidxml::xml_node<> *root = doc.first_node();
    std::cout << root->name() << std::endl;
    
    /*
     * Get Audio format 
     */
    rapidxml::xml_attribute<> *attribute = root->first_attribute("format");
    configs.Format = static_cast<snd_pcm_format_t>(atoi(attribute->value()));
    std::cout << "\t" << attribute->name() << "\t" << attribute->value() << std::endl;
    /*
     * Get Audio freq
     */
    attribute = root->first_attribute("freq");
    configs.Freq = atoi(attribute->value());
    std::cout << "\t" << attribute->name() << "\t" << attribute->value() << std::endl;
    /*
     * Get Audio number channel
     */
    attribute = root->first_attribute("channels");
    configs.Channels = atoi(attribute->value());
    std::cout << "\t" << attribute->name() << "\t" << attribute->value() << std::endl;
    
    /*
     * Get Device ex:(hw:0,1)
     */
    rapidxml::xml_node<> *node = root->first_node("DEVICE");
    configs.Device = "hw:";
    std::cout << node->name() << std::endl;
    /*
     * Get Device id 
     */
    attribute = node->first_attribute("id");
    configs.Device.append(attribute->value());
    configs.Device.append(",");
    std::cout << "\t" << attribute->name() << "\t" << attribute->value() << std::endl;
    /*
     * Get Device channel
     */
    attribute = node->first_attribute("channel");
    configs.Device.append(attribute->value());
    std::cout << "\t" << attribute->name() << "\t" << attribute->value() << std::endl;
    
    /*
     * Get Block 
     */
    node = root->first_node("BLOCK");
    std::cout << node->name() << std::endl;
    /*
     * Get Block size 
     */
    attribute = node->first_attribute("size");
    configs.NumSamplesBlock = atoi(attribute->value());
    std::cout << "\t" << attribute->name() << "\t" << attribute->value() << std::endl;    

    /*
     * Calculate Channel Buffer Size
     */
    configs.ChannelBufferSize = configs.NumSamplesBlock * snd_pcm_format_width(configs.Format) / 8;
  }
  
  int AudioGrabber::SetConfigs()
  {
    int error = 0;
    if ((error = snd_pcm_open(&grabber, configs.Device.c_str(), SND_PCM_STREAM_CAPTURE, 0)) < 0)
    {
      std::cerr << "Cannot open audio device " << configs.Device << " (" << snd_strerror (error) << ")" << std::endl;
    }         
    else if ((error = snd_pcm_hw_params_malloc(&grabber_params)) < 0)
    {
      std::cerr << "Cannot allocate hardware parameter structure (" << snd_strerror (error) << ")" << std::endl;
    }      
    else if ((error = snd_pcm_hw_params_any(grabber, grabber_params)) < 0)
    {
      std::cerr << "Cannot initialize hardware parameter structure (" << snd_strerror (error) << ")" << std::endl;
    }    
    else if ((error = snd_pcm_hw_params_set_access(grabber, grabber_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
    {
      std::cerr << "Cannot set access type (" << snd_strerror (error) << ")" << std::endl;
    }    
    else if ((error = snd_pcm_hw_params_set_format(grabber, grabber_params, configs.Format)) < 0)
	{
		std::cerr << "Cannot set sample format (" << snd_strerror (error) << ") " << std::endl;
	}
    else if ((error = snd_pcm_hw_params_set_rate(grabber, grabber_params, configs.Freq, 0)) < 0)
    {
      std::cerr << "Cannot set sample rate (" << snd_strerror (error) << ")" << std::endl;
    }  
    else if ((error = snd_pcm_hw_params_set_channels(grabber, grabber_params, configs.Channels)) < 0)
    {
      std::cerr << "Cannot set channel count (" << snd_strerror (error) << ")" << std::endl;
    }
    return error;
  }
  
  int AudioGrabber::ApplyConfigs()
  {
    int error = 0;
    if ((error = snd_pcm_hw_params (grabber, grabber_params)) < 0)
    {
      std::cerr << "Cannot set parameters (" << snd_strerror (error) << ")" << std::endl;      
    }
    if ((error = snd_pcm_prepare(grabber)) < 0)
    {
      std::cerr << "Cannot prepare audio interface for use (" << snd_strerror (error) << ")" << std::endl;
    }
    return error;
  }
}
