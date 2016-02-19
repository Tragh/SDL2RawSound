#include <iostream>
#include <cassert>
#include <atomic>

#include <SDL2/SDL.h>

//error message macro
#define assertm(condition, message) do { \
if(!(condition)){std::cerr<<"Assert message: "<< message <<std::endl; \
assert((condition));}}while(false)


constexpr int SAMPLE_RATE = 44100;


class Sounder
{
	std::atomic<uint64_t> sample_time; //how many samples have elapsed since the start of the thingy.
	uint32_t sample_rate;
	
public:
	Sounder(int sample_rate_);
	~Sounder();
	void Start();
	
private:
	void SetBuffer(int32_t* stream32, int len);
	static void AudioCallback(void *userdata, Uint8 *stream, int len);
};


Sounder::Sounder(int sample_rate_):
	sample_time(0),
	sample_rate(sample_rate_)
{
	//here we initialise the audio
	assertm(SDL_Init(SDL_INIT_AUDIO) >= 0, "SDL2 failed to init audio");
	
	SDL_AudioSpec wav_spec;
	SDL_zero(wav_spec); //creat a blank AudioSpec
	
	wav_spec.freq = sample_rate;
	wav_spec.format = AUDIO_S32SYS;
	wav_spec.channels = 2;
	wav_spec.samples = 1024;
	wav_spec.callback = AudioCallback; //our callback. Must be a static member function / regular function.
	wav_spec.userdata = this; //send a pointer to this class.
	assertm(SDL_OpenAudio(&wav_spec, NULL) >= 0, "SDL2 failed to open audio");
}

Sounder::~Sounder(){
	SDL_CloseAudio();
}

void Sounder::Start(){
	SDL_PauseAudio(0); //starts playing
}

void Sounder::SetBuffer(int32_t* stream32, int len)
{
	for(int i=0;i<len/2;++i){ //two channels so we need len/2 samples
		sample_time++;
		const int32_t &tmp=(2<<27)*sin(float(sample_time)/sample_rate *M_PI*2*400); //generage a sample, a 400hz sin wave
		stream32[2*i]=tmp; //write to sound buffer RIGHT STREAM
		stream32[2*i+1]=tmp; //write to sound buffer LEFT STREAM
	}
}


//The audio callback used by SDL
void Sounder::AudioCallback(void *userdata, Uint8 *stream, int len)
{
	int32_t* const &stream32 = reinterpret_cast<int32_t*>(stream); //using S32 audio.
	Sounder &sounder = *reinterpret_cast<Sounder*>(userdata); //cast sounder from userdata. This way we can access our class methods.
	sounder.SetBuffer(stream32,len/4); //pass these to our class function
}


int main(int argc, char* argv[])
{

	Sounder sounder(44100);


	SDL_Window *window;
	SDL_Renderer *renderer;
	assertm(SDL_Init( SDL_INIT_VIDEO ) >= 0, "SDL2 failed to init video");
	{
		SDL_DisplayMode current;
		SDL_GetCurrentDisplayMode(0, &current);
		window=SDL_CreateWindow("Window Name",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,current.w, current.h, SDL_WINDOW_MAXIMIZED | SDL_WINDOW_FULLSCREEN);
	}

	sounder.Start(); // start playing
	
	int done = 0;
	SDL_Event event;
	while(!done) {
		SDL_Delay(20);


		while(SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_QUIT:
				done = 1;
				break;
			case SDL_KEYDOWN:


				break;
			case SDL_KEYUP:


				if(event.key.keysym.sym == SDLK_ESCAPE) {
					done = 1;
				}
				break;



			default:
				break;
			}
		}
	}


	SDL_Quit();
	return 0;
}





