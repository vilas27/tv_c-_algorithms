#include <iostream>    
#include <stdio.h>
#include <vector>

#include "audio_grabber.h"

std::vector<std::vector<int>> SplitChannels(int* input, audio::AudioGrabber* grabber)
{
	std::vector<std::vector<int>> output;
	for(int i = 0 ; i < grabber->GetNumChannels() * grabber->GetNumSamplesBlock() ; )
	{
		for(int j = 0 ; j < grabber->GetNumChannels() ; j++)
		{
			if(i < grabber->GetNumChannels())
			{
				output.push_back(std::vector<int>());
			}
			output.at(j).push_back(input[i++]);
		}
	}
	return output;
}

int main ()
{
  audio::AudioGrabber* grabber = new audio::AudioGrabber();
  std::vector<FILE*> output_files;
  std::cout << "Starting" << std::endl;
  for(int i = 0 ; i < 60 ; i++)
  {
    int* result =  grabber->Grab();
    std::vector<std::vector<int>> output = SplitChannels(result, grabber);
    std::cout << i << std::endl;
    for(int j = 0 ; j < grabber->GetNumChannels() ; j++)
    {
    	if(i == 0)
    	{
    		output_files.push_back(fopen((std::to_string(j)).c_str(), "wb"));
    	}
    	for(int out : output.at(j))
        {
        	fwrite(&out, sizeof(int), 1, output_files.at(j));
        }
    }
    output.clear();
    free(result);
  }
  std::cout << "Finish!!" << std::endl;

  for(int i = 0 ; i < grabber->GetNumChannels() ; i++)
  {
	  fclose(output_files.at(i));
  }
  return 0;
}
