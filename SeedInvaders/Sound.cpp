#include "Sound.h"


int endWithError(char* msg, int error=0)
{
    //Display error message in console
    cout << msg << "\n";
    return error;
}

Sound::~Sound()                 // destructor, just an example
{
    //Clean-up
    fclose(fp);
    delete[] buf;
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
    
}
Sound::Sound(char*  filePath){
    
    path  = filePath;
    //Loading of the WAVE file
    fp=fopen(filePath,"rb");                                            //Open the WAVE file
    if (!fp)  endWithError(errors[0]);                        //Could not open file
    
    //Check that the WAVE file is OK
    fread(&riff_header, sizeof(RIFF_Header), 1, fp);
    
    if(riff_header.chunkID[0]!='R' || riff_header.chunkID[1]!='I' || riff_header.chunkID[2]!='F' || riff_header.chunkID[3]!='F')            //Should be "RIFF"
        endWithError (errors[1]);                                            //Not RIFF
    
    
    if (riff_header.format[0]!='W' || riff_header.format[1]!='A' || riff_header.format[2]!='V' || riff_header.format[3]!='E')           //This part should be "WAVE"
        endWithError(errors[2]);                                            //Not WAVE
    
    fread(&wave_format, sizeof(WAVE_Format), 1, fp);
    
    if (wave_format.subChunkID[0]!='f' || wave_format.subChunkID[1]!='m' || wave_format.subChunkID[2]!='t' || wave_format.subChunkID[3]!=' ')           //This part should be "fmt "
        endWithError(errors[3]);                                            //Not fmt
    
    
    fread(type,sizeof(char),4,fp);
    if (type[0]!='d' || type[1]!='a' || type[2]!='t' || type[3]!='a')           //This part should be "data"
        endWithError(errors[4]);                                        //not data
    
    fread(&dataSize,sizeof(int),1,fp);                                        //The size of the sound data is read
    
    buf= new unsigned char[dataSize];                            //Allocate memory for the sound data
    cout << fread(buf,1,dataSize,fp) << " bytes loaded\n";           //Read the sound data and display the
    //number of bytes loaded.
    //Should be the same as the Data Size if OK
    
    
    device = alcOpenDevice(NULL);                                               //Open the device
    if(!device) endWithError(errors[5]);                         //Error during device oening
    context = alcCreateContext(device, NULL);                                   //Give the device a context
    alcMakeContextCurrent(context);                                             //Make the context the current
    if(!context) endWithError(errors[6]);                       //Error during context handeling
    
    //Stores the sound data
    frequency=wave_format.sampleRate;;                                               //The Sample Rate of the WAVE file
    
    alGenBuffers(1, &buffer);                                                    //Generate one OpenAL Buffer and link to "buffer"
    alGenSources(1, &source);                                                   //Generate one OpenAL Source and link to "source"
    if(alGetError() != AL_NO_ERROR) endWithError("Error GenSource");     //Error during buffer/source generation
    
    //Figure out the format of the WAVE file
    std::cout<<wave_format.numChannels;
    if(wave_format.bitsPerSample  == 8)
    {
        if(wave_format.numChannels  == 1)
            format = AL_FORMAT_MONO8;
        else if(wave_format.numChannels  == 2)
            format = AL_FORMAT_STEREO8;
    }
    else if(wave_format.bitsPerSample == 16)
    {
        if(wave_format.numChannels  == 1)
            format = AL_FORMAT_MONO16;
        else if(wave_format.numChannels  == 2)
            format = AL_FORMAT_STEREO16;
    }
    if(!format) endWithError(errors[7]);                      //Not valid format
    
    alBufferData(buffer, format, buf, dataSize, frequency);                    //Store the sound data in the OpenAL Buffer
    if(alGetError() != AL_NO_ERROR)
        endWithError(errors[8]);                              //Error during buffer loading
    
    //Sound setting variables
    ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };
    ALfloat SourceVel[] = { 0.0, 0.0, 0.0 };
    ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };
    ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };
    ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };
    //First direction vector, then vector pointing up)
    //Listener
    alListenerfv(AL_POSITION,    ListenerPos);
    alListenerfv(AL_VELOCITY,    ListenerVel);
    alListenerfv(AL_ORIENTATION, ListenerOri);
    
    //Source
    alSourcei (source, AL_BUFFER,   buffer);
    alSourcef (source, AL_PITCH,    1.0f     );
    alSourcef (source, AL_GAIN,     1.0f     );
    alSourcefv(source, AL_POSITION, SourcePos);
    alSourcefv(source, AL_VELOCITY, SourceVel);
    alSourcei (source, AL_LOOPING,  AL_FALSE );
}

void Sound::PlaySound(){
    
    //Play the sound buffer linked to the source
    alSourcePlay(source);
    if(alGetError() != AL_NO_ERROR) endWithError(errors[9]); //Error when playing sound
    
}

void Sound::PauseSound(){
    
    //Pause the sound buffer linked to the source
    alSourceStop(source);
    if(alGetError() != AL_NO_ERROR) endWithError(errors[9]); //Error when pausing sound
    
}
