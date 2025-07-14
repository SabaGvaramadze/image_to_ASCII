
#define SDL_MAIN_HANDLED


#include <iostream>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
using namespace std;

struct rgb8{
	Uint8 r,g,b;
};

#define rgb rgb8

struct rgb32{
	Uint32 r,g,b;
};

const char *shades = "$@B%8#*ahbpwjft\()1}[?-+<>il;:,\"^`' ";
const int slen = strlen(shades)-1;


int main(int argc,char **argv){
	if(argc <2){
		cout << "err: not enough arguments" << endl;
		return -1;
	}
	char *image_path = argv[1];
	if(argc == 3)image_path = argv[2];
	if(argc == 4)image_path = argv[3];
	
	SDL_Surface *image = NULL;
	
	TTF_Init();



	if(SDL_Init(SDL_INIT_VIDEO)<0){
		cout << "SDL COULD NOT INITIALIZE" << endl;
		return -1;
	}
	size_t hchunk_count=0, vchunk_count=0;

	image = IMG_Load(image_path);
	if(image == NULL){
		cout << "IMAGE COULD NOT BE LOADED" << endl;
		return -1;
	}
	
	int cwidth=1,cheight=1;
	if(argc == 3){
		try{
			cout << "WARNING CHUNK COUNT NEEDS TO BE FIXED" << endl;
			hchunk_count = stoi(argv[1]);
			vchunk_count = hchunk_count*image->h/image->w;
			cwidth = (float)image->w/hchunk_count;
			cheight = image->h /vchunk_count;
		}
		catch(invalid_argument){
			cout << "one of the chunk parameters are wrong" << endl;
			return -1;
		}
		catch(out_of_range){
			cout << "chunk parameters out of range of int" << endl;
			return -1;
		}
	}
	if(argc == 4){
		try{
			hchunk_count = stoi(argv[1]);
			vchunk_count = stoi(argv[2]);
			cwidth = image->w/stoi(argv[1]);
			cheight = image->h/stoi(argv[2]);
		}
		catch(invalid_argument){
			cout << "one of the chunk parameters are wrong" << endl;
			return -1;
		}
		catch(out_of_range){
			cout << "chunk parameters out of range of int" << endl;
			return -1;
		}
	}
	const size_t chunk_width = cwidth,chunk_height = cheight;
	
	SDL_Window *window = SDL_CreateWindow("asciimg",100,100,900,900,0);
	SDL_Renderer *renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_SOFTWARE);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer,image);
	
	TTF_Font *default_font = TTF_OpenFont("./def2.ttf",16);
	if(default_font == NULL){
		cout << "win_default.ttf font could not be loaded" << endl;
		cout << SDL_GetError() << endl;
		return -1;
	}
	

	
	SDL_Rect hi;
	hi.x=0;
	hi.y=0;
	hi.w=300;
	hi.h=300;
	
	const size_t BPP = (int)image->format->BytesPerPixel;
	
	
	SDL_SetRenderDrawColor(renderer,0,0,0,255);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer,texture,NULL,&hi);
	
	
	SDL_RenderPresent(renderer);
	
	//SDL_Delay(2000);
	
	
	rgb8 pixel = {0,0,0};
	
	SDL_GetRGB(*(Uint32*)image->pixels,image->format,&pixel.r,&pixel.g,&pixel.b);
	int avg = 0;
	rgb32 avg_color = {0,0,0};
	
	rgb32 sum = {0,0,0};
	char* text_buffer = (char*)malloc(hchunk_count * vchunk_count );
	rgb8* text_color_buffer = (rgb8*)malloc(sizeof(rgb8)*(hchunk_count * vchunk_count));
	memset(text_buffer,0,hchunk_count*vchunk_count);
	memset(text_color_buffer,0,sizeof(rgb8)*(hchunk_count*vchunk_count));
	
	size_t last_offset =0;
	size_t smth=0,highi=0,highj=0,highavg=0;
	for(int i=0;i<vchunk_count;i++){
		for(int j=0;j<hchunk_count;j++){
			sum.r=0;
			sum.g=0;
			sum.b=0;
			int a=0;
			for(int k=0;k<chunk_height;k++){
				for(int l=0;l<chunk_width;l++){
					SDL_GetRGB(((Uint8*)image->pixels)[i*chunk_height*image->pitch + k*image->pitch +j*chunk_width*BPP + l*BPP],image->format,&pixel.r,&pixel.g,&pixel.b);
					rgb8 tmp = *((rgb8*)(image->pixels+ i*chunk_height*image->pitch + k*image->pitch +j*chunk_width*BPP + l*BPP)) ;
					sum.r+=tmp.r;
					sum.g+=tmp.g;
					sum.b+=tmp.b;
				}
			}
			text_color_buffer[i*hchunk_count + j] = {sum.r/chunk_width/chunk_height,sum.g/chunk_width/chunk_height,sum.b/chunk_width/chunk_height};
			smth = max(smth, i*hchunk_count + j);
			highi = max(highi,(size_t)i);
			highj = max(highj,(size_t)j);
			avg = slen - ((((sum.r + sum.g + sum.b)/chunk_width)/chunk_height/3)/256.0)*slen;
			highavg = max(highavg,(size_t)avg);
			text_buffer[i*hchunk_count + j] = shades[avg];
		}
	}
	cout << "HIGHEST INDEX: " << smth  << " I: " << highi << " J: " << highj << " AVG: " << highavg<< endl;
	cout << "SLEN: " << slen << endl;
	cout << "BPP: " << (int)image->format->BytesPerPixel<< endl;
	cout << "PITCH: " << image->pitch << endl;
	cout << "CWIDTH: " << chunk_width << " " << image->w / hchunk_count << endl;
	cout << "CHEIGHT: " << chunk_height<< " " << image->h /vchunk_count << endl;
	cout << "WIDTH: " << image->w<< " " << chunk_width * hchunk_count << endl;
	cout << "HEIGHT: " << image->h<< " " << chunk_height * vchunk_count <<  endl;
	cout << "CHUNK COUNT HORIZONTAL: " << hchunk_count << endl;
	cout << "CHUNK COUNT VERTICAL: " << vchunk_count << endl;
	cout << "BUFFER SIZE: " << hchunk_count*vchunk_count + vchunk_count << endl;

	SDL_RenderClear(renderer);
	for(int i=0;i<vchunk_count-1;i++){
		for(int j=1;j<hchunk_count;j++){
			rgb8 current_color = text_color_buffer[i*hchunk_count+j];
			SDL_Surface *text_surface = TTF_RenderGlyph_Solid(default_font,(Uint16)(*(text_buffer + i*hchunk_count +j)),{current_color.r,current_color.g,current_color.b,255});
			if(text_surface == 0){
				cout << SDL_GetError() << endl;
				continue;
			}

			SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer,text_surface);
			SDL_Rect text_rect;
			text_rect.x=j*text_surface->w;
			text_rect.y=i*text_surface->h;
			text_rect.w = text_surface->w;
			text_rect.h = text_surface->h;
			SDL_RenderCopy(renderer,text_texture,NULL,&text_rect);
			SDL_DestroyTexture(text_texture);
			SDL_FreeSurface(text_surface);
		}
}
	SDL_RenderPresent(renderer);
	bool quit = false;
	while(!quit){
		SDL_Event e;
		while(SDL_PollEvent(&e)){
			switch(e.type){
				case SDL_QUIT:
					quit=true;
					break;
			}
		}
	}
	
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();

	cout << "EXECTUTION FINISHED" << endl;

}
