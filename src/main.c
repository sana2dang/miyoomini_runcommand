#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <fcntl.h>
#include <stdbool.h>  
#include <sys/stat.h>  
#include "sys/ioctl.h"
#include <dirent.h>

#include <pthread.h>
#include <libgen.h>

// Max number of records in the DB
#define NUMBER_OF_LAYERS 200
#define MAX_LAYER_NAME_SIZE 256
#define MAY_LAYER_DISPLAY 35


char layers[NUMBER_OF_LAYERS][MAX_LAYER_NAME_SIZE];
int bInstall[2][NUMBER_OF_LAYERS];
int nb_Layers;
int nSelection = 0; 
int nListPostion = 0;
int nTab = 0;
int allActivated = 0;

SDL_Surface* video = SDL_SetVideoMode(640,480, 32, SDL_HWSURFACE);
SDL_Surface* screen = SDL_CreateRGBSurface(SDL_HWSURFACE, 640,480, 32, 0,0,0,0);

int loadFlag = 1;
int stopFlag = 0;
int move = 0;
pthread_t thread_id;


#define	BUTTON_A	KEY_SPACE
#define	BUTTON_B	KEY_LEFTCTRL  

#define	BUTTON_X	KEY_LEFTSHIFT
#define	BUTTON_Y	KEY_LEFTALT  

#define	BUTTON_START	KEY_ENTER
#define	BUTTON_SELECT	KEY_RIGHTCTRL

#define	BUTTON_MENU	KEY_ESC
#define	BUTTON_POWER	KEY_POWER

#define	BUTTON_L2	KEY_TAB
#define	BUTTON_R2	KEY_BACKSPACE

#define BUTTON_L1	KEY_E
#define BUTTON_R1	KEY_T

void refreshScreen(); 	
 	

void logMessage(char* Message) {
	FILE *file = fopen("log_OI_Message.txt", "a");

    char valLog[200];
    sprintf(valLog, "%s%s", Message, "\n");
    fputs(valLog, file);
	fclose(file); 
}


bool file_exists (char *filename) {
  struct stat   buffer;   
  return (stat (filename, &buffer) == 0);
}

void setLayersInstall (int bInstallValue){
	for(int n = 0 ; n < 2 ; n++){
		for(int i = 0 ; i < NUMBER_OF_LAYERS ; i++){
			bInstall[n][i] = bInstallValue;
		}	
	}
}

void loadRessources(char* ressourcesPath, char* ff){

	struct dirent **namelist;
	int count;
	int idx;


	DIR *dp;
	struct dirent *ep;  

	char cSingleRessource[250]; 
	nb_Layers=0;

	char *ext;
	char filename[1024];
	char fileExt[1024];


	char temp[1024];
	char cut[2][256];
	char cutStr[256];
	char defaultCore[1024];
	
	char baseName[500];
	char dirName[500];
	char confPath[1024] = "/mnt/SDCARD/App/runcommand/conf/";


	if( loadFlag == 1 )
	{
		strcpy(baseName, basename(ff));
		//logMessage(baseName);
		strcpy(dirName, basename( dirname(ff) ) );
		//logMessage(dirName);
		
		strcat(confPath, dirName );
		strcat(confPath, "/");
		strcat(confPath, baseName);
		//logMessage(confPath);

		FILE *pFile = NULL;
		pFile = fopen( confPath, "r");
		if( pFile == NULL )
		{
			strcpy( cutStr, "--");
			pthread_cancel(thread_id);
			stopFlag = 1;
		}
		else
		{
			fscanf( pFile, "%s", temp);
			
			char *ptr = strtok( temp, ",");
			int i=0;
			while( ptr != NULL )
			{
				strcpy(cut[i],ptr );
				ptr = strtok(NULL, ",");
				i++;
			}

			//logMessage( cut[0] );
			//logMessage( cut[1] );
			strcpy( cutStr, cut[1] );
			fclose(pFile);
		}
	}
	




	//////////////////
	if( (count = scandir( ressourcesPath, &namelist, NULL, alphasort) ) == -1 ) {

		SDL_Quit();
	        return; 
	}

	for(idx = 0; idx<count; idx++ )
	{
		if( strcmp(namelist[idx]->d_name,".") == 0 || strcmp(namelist[idx]->d_name, "..") == 0)
		{
			continue;
		}
		sprintf(filename, "%s", namelist[idx]->d_name);
		ext = strrchr(filename, '.');
		sprintf( fileExt, "%s", ext );
		//logMessage(fileExt);
		if( strcmp(fileExt, ".so") != 0 )
		{
			continue;
		}
		else
                {
                        char cShort[MAX_LAYER_NAME_SIZE];
                        strcpy(cShort, namelist[idx]->d_name);     
                        cShort[MAY_LAYER_DISPLAY] = '\0';

                        //logMessage(cShort);
                        //logMessage(cutStr);

                        if( loadFlag == 1 )
                        {
                                cutStr[MAY_LAYER_DISPLAY] = '\0';
                                if( strcmp( cShort, cutStr) == 0 )
                                {
                                        //logMessage(cutStr);
                                        bInstall[0][nb_Layers] = 1;
                                        loadFlag = 0;
                                }
                        }


                        size_t len = strlen(cShort);
                        if ((len > 2)||(cShort[0]!='.')){
                                //logMessage(cShort);
                                strcpy(layers[nb_Layers],cShort);
                                nb_Layers ++;
	                }
		}

	}
	for( idx = 0; idx<count;idx++)
	{
		free(namelist[idx]);
	}
	free(namelist);
	//////////////////



	if( stopFlag == 1 )
	{
		stopFlag = 0;
		move = 1;
		refreshScreen();

	}


/*

	dp = opendir (ressourcesPath);

  		if (dp != NULL)
  		{
    		while ((ep = readdir (dp)) && (nb_Layers<NUMBER_OF_LAYERS)){
				
				if( strcmp(ep->d_name,".") == 0 || strcmp(ep->d_name, "..") == 0)
				{
					continue;
				}
				sprintf(filename, "%s", ep->d_name);
				ext = strrchr(filename, '.');
				sprintf( fileExt, "%s", ext );
				//logMessage(fileExt);
				if( strcmp(fileExt, ".so") != 0 )
				{
					continue;
				}
				else
				{
	
					char cShort[MAX_LAYER_NAME_SIZE];
					strcpy(cShort, ep->d_name);	
					cShort[MAY_LAYER_DISPLAY] = '\0';

					//logMessage(cShort);
					//logMessage(cutStr);

					if( loadFlag == 1 )
					{
						cutStr[MAY_LAYER_DISPLAY] = '\0';
						if( strcmp( cShort, cutStr) == 0 )
						{
							logMessage(cutStr);
							bInstall[0][nb_Layers] = 1;
							loadFlag = 0;
						}
					}
						

					size_t len = strlen(cShort);
					if ((len > 2)||(cShort[0]!='.')){
						//logMessage(cShort);
						strcpy(layers[nb_Layers],cShort);
						nb_Layers ++;   		
				}
    			}
    		   			
    		}    
    		

    		(void) closedir (dp);
  		}
  		
  		else{
  			perror ("Couldn't open the directory");
  		}	
*/


  		
}

void displayLayersNames(){
SDL_Rect rectRessName;	
	SDL_Surface* surfaceRessName;
	TTF_Font* font25 = TTF_OpenFont("/customer/app/Exo-2-Bold-Italic.ttf", 25);
	SDL_Color color_white={255,255,255,0};
	
	for (int i = 0 ; i < 7 ; i++){
		if ((i + nListPostion) < nb_Layers){
			surfaceRessName = TTF_RenderUTF8_Blended(font25, layers[i + nListPostion], color_white);
			rectRessName = { 35, 92 + (i*47) , 80, 20};
			SDL_BlitSurface(surfaceRessName, NULL, screen, &rectRessName);
		}
	}
	SDL_FreeSurface(surfaceRessName);
}

void displayLayersInstall(){
	SDL_Rect rectInstall;
	
	SDL_Surface* surfaceCheck = IMG_Load("./ressources/checked.png");
	SDL_Surface* surfaceCross = IMG_Load("./ressources/cross.png");
	
	for (int i = 0 ; i < 7 ; i++){
		if ((i + nListPostion) < nb_Layers){
		
			rectInstall = { 567, 96 + (i*47) , 27, 27};
			if (bInstall[nTab][i + nListPostion] == 1){
				SDL_BlitSurface(surfaceCheck, NULL, screen, &rectInstall);			
			} 
			else {
				SDL_BlitSurface(surfaceCross, NULL, screen, &rectInstall);	
			}
			
		}
	}
	SDL_FreeSurface(surfaceCheck);
	SDL_FreeSurface(surfaceCross);
}


void showScroller(){
	SDL_Surface* surfaceScroller = IMG_Load("./ressources/scroller.png");

	int shiftY = (int)((nListPostion * 311) / (nb_Layers-7));
	
	SDL_Rect rectSroller = { 608, 86+(shiftY), 16, 16};
	
	SDL_BlitSurface(surfaceScroller, NULL, screen, &rectSroller);
	SDL_FreeSurface(surfaceScroller);
}

void refreshScreen(){
	
	TTF_Font* font35 = TTF_OpenFont("/customer/app/Exo-2-Bold-Italic.ttf", 35);
	TTF_Font* font18 = TTF_OpenFont("/customer/app/Exo-2-Bold-Italic.ttf", 18);
	
	SDL_Color color_white={255,255,255,0};
	SDL_Color color_pink={136,97,252,0};

	SDL_Rect rectSelection = { 15, 84+(nSelection*47), 593, 49};	
	SDL_Rect rectTitle = { 457, 9, 200, 50};	
	SDL_Rect rectTabSelection = { 15 + (199 * nTab), 59, 199, 26};	
	
	SDL_Surface* surfaceBackground = IMG_Load("./ressources/bgApp.png");
	SDL_Surface* surfaceSelection = IMG_Load("./ressources/selection.png");
	SDL_Surface* surfaceTableau = IMG_Load("./ressources/tableau.png");
	SDL_Surface* surfacesTabSelection = IMG_Load("./ressources/selectionTitle.png");

	SDL_BlitSurface(surfaceBackground, NULL, screen, NULL);
	SDL_BlitSurface(surfacesTabSelection, NULL, screen, &rectTabSelection);
	SDL_BlitSurface(surfaceTableau, NULL, screen, NULL);
	
	SDL_BlitSurface(surfaceSelection, NULL, screen, &rectSelection);
	
	if (nb_Layers > 0){
		displayLayersNames();
		showScroller();
		displayLayersInstall();
	}



	SDL_BlitSurface(screen, NULL, video, NULL); 
	SDL_Flip(video);
	
	SDL_FreeSurface(surfaceBackground);
	SDL_FreeSurface(surfaceTableau);
	SDL_FreeSurface(surfaceSelection);
	SDL_FreeSurface(surfacesTabSelection);
}


void *pthread_ex(void *arg)
{



	int i = 0;
	while(1)
	{
		i++;
		if( i == 10 )
		{
	                SDL_Surface* surfaceBlack = IMG_Load("./ressources/blackBG.png");
			SDL_BlitSurface(surfaceBlack, NULL, screen, NULL);
			SDL_BlitSurface(screen, NULL, video, NULL); 
			SDL_Flip(video);
	                SDL_FreeSurface(surfaceBlack);
			SDL_Quit();
	                execl("/mnt/SDCARD/App/runcommand/run.sh", "run", arg, NULL );
			break;
	                return EXIT_SUCCESS; 
		}
		usleep(100000);
	}
}



int main(int argc, char *argv[]) {
	
	uint32_t		val;
	uint32_t		l2_pressed = 0;
	uint32_t		r2_pressed = 0;
	uint32_t		menu_pressed = 0;

	uint32_t		a_pressed = 0;
	uint32_t		b_pressed = 0;
	uint32_t		x_pressed = 0;
	uint32_t		y_pressed = 0;
	uint32_t		start_pressed = 0;
	uint32_t		select_pressed = 0;	

	uint32_t		left_pressed = 0;
	uint32_t		right_pressed = 0;		
	uint32_t		up_pressed = 0;	
	uint32_t		down_pressed = 0;	


	int sts;
	void *t_return;
	if( (sts=pthread_create( &thread_id, NULL, pthread_ex, argv[2] ) ) != 0 )
	{
		exit(1);
	}


	SDL_Init(SDL_INIT_VIDEO);
	SDL_ShowCursor(SDL_DISABLE);

/*
	// runGame
	if( runGame == 1 )
	{
                SDL_Surface* surfaceBlack = IMG_Load("./ressources/blackBG.png");
                SDL_FreeSurface(surfaceBlack);		
		
		SDL_Quit();
                execl("/mnt/SDCARD/App/runcommand/run.sh", "run", argv[2], NULL );
                return EXIT_SUCCESS; 
	}
*/
	
	TTF_Init();

	
	char ressourcesPath[200] ;
	char filePathTemp[200];
	sprintf(ressourcesPath,"%s", argv[1]);
	sprintf(filePathTemp,"%s", argv[2]);

	SDL_Surface* surfaceLoading = IMG_Load("./ressources/loading.png");
	SDL_BlitSurface(surfaceLoading, NULL, screen, NULL);
	SDL_BlitSurface(screen, NULL, video, NULL); 
	SDL_Flip(video);
	SDL_FreeSurface(surfaceLoading);

	setLayersInstall(0);	
	loadRessources(ressourcesPath, filePathTemp);
//	refreshScreen();		// first screen hide

	// Prepare for Poll button input
	int			input_fd;
	input_fd = open("/dev/input/event0", O_RDONLY);	
	struct input_event	ev;

	FILE *fp;
	long lSize;	


	
	while( read(input_fd, &ev, sizeof(ev)) == sizeof(ev) ) {

		val = ev.value	; 
		 
		if (( ev.type != EV_KEY ) || ( val > 1 )) continue;
		if ( ev.code == BUTTON_L1 ) {
				l2_pressed = val;
		}
		else
			if ( ev.code == BUTTON_R1 ) {
					r2_pressed = val;
			}
			else
				if ( ev.code == BUTTON_START ) {
						start_pressed = val;
				}
				else
					if ( ev.code == BUTTON_SELECT ) {
							select_pressed = val;
					}
					else	
						if ( ev.code == BUTTON_MENU ) {
								menu_pressed = val;
						}
						else
							if ( ev.code == BUTTON_A ) {
									a_pressed = val;
							}
							else
								if ( ev.code == BUTTON_B ) {
										b_pressed = val;
								}
								else
									if ( ev.code == KEY_LEFT ) {
											left_pressed = val;
									}
									else
										if ( ev.code == KEY_RIGHT ) {
												right_pressed = val;
										}	
										else
										if ( ev.code == KEY_UP ) {
												up_pressed = val;
										}
											else
												if ( ev.code == KEY_DOWN ) {
														down_pressed = val;
												}
												else
													if ( ev.code == BUTTON_X){
														x_pressed = val;
													}
													else
														if ( ev.code == BUTTON_Y){
															y_pressed = val;
														}											
		if (y_pressed && ( move == 1 )) {


			char coreName[100];
	                char cCommand[500];

	                //SDL_Surface* surfaceBackground = IMG_Load("./ressources/blackBG.png");

	                sprintf(ressourcesPath,"%s", argv[1]);
			sprintf(filePathTemp,"%s", argv[2]);
			//loadRessources(ressourcesPath, filePathTemp);

	                for(int nT = 0; nT < 1; nT ++ )
	                {
	                        for(int nLayer = 0; nLayer < nb_Layers; nLayer++ )
	                        {
	                                if( bInstall[nT][nLayer] == 1 )
	                                {
	                                        strcpy(coreName, layers[nLayer]);
						//logMessage( "Y : " );
						//logMessage( coreName );
	                                        //sprintf(cCommand, "cd /mnt/SDCARD/App/runcommand ; ./test.sh \"%s\"", layers[nLayer]);
	                                        //system(cCommand);
	                                }
	                        }
	                }




	                //sprintf(cCommand, "cd /mnt/SDCARD/App/runcommand ; ./test.sh \"%s\"", argv[2] );
	                //system(cCommand);

	                //sprintf(cCommand, "cd /mnt/SDCARD/App/runcommand ; ./test2.sh \"%s\" \"%s\" \"%s\"", "retroarch", coreName, argv[2] );
	                //system(cCommand);


	                //SDL_FreeSurface(surfaceBackground);
	                //execl("/mnt/SDCARD/App/The_Onion_Installer/retroarch.sh", "retroarch", coreName, argv[1], NULL );

	                sprintf(cCommand, "cd /mnt/SDCARD/App/runcommand ; ./coreSave.sh \"%s\" \"%s\" \"%s\"", "ra32.ss", coreName, argv[2] );
	                system(cCommand);

	                SDL_Surface* surfaceBlack = IMG_Load("./ressources/blackBG.png");
			SDL_BlitSurface(surfaceBlack, NULL, screen, NULL);
			SDL_BlitSurface(screen, NULL, video, NULL); 
			SDL_Flip(video);
	                SDL_FreeSurface(surfaceBlack);
			SDL_Quit();

	                execl("/mnt/SDCARD/App/runcommand/run.sh", "run", argv[2], NULL );

	                break;


			/*
			if (allActivated == 0){
				allActivated = 1; 
				setLayersInstall(1);
			}
			else {
				allActivated = 0; 
				setLayersInstall(0);
			}
			

			refreshScreen();
			*/
		}	 

										
		if (right_pressed) {			
		/*
			if (nTab < 2){
				nTab ++;
				switch(nTab){
					case 0 :
						sprintf(ressourcesPath,"%s","./cores");
						break;
					case 1 :
						sprintf(ressourcesPath,"%s","./run");
						break;
					case 2 :
						sprintf(ressourcesPath,"%s","./etc");
						break;
				}
				loadRessources(ressourcesPath);
				nSelection = 0; 
				nListPostion = 0;
				refreshScreen();
			}
		*/
		}	 
		if (left_pressed) {			
		/*
			if (nTab > 0){
				nTab --;
				switch(nTab){
					case 0 :
						sprintf(ressourcesPath,"%s","./cores");
						break;
					case 1 :
						sprintf(ressourcesPath,"%s","./run");
						break;
					case 2 :
						sprintf(ressourcesPath,"%s","./etc");
						break;
				}
				loadRessources(ressourcesPath);
				nSelection = 0; 
				nListPostion = 0;
				refreshScreen();
			}
		*/
		}
		if (x_pressed && (move == 1 )) {

                        char coreName[100];
                        char cCommand[500];

                        //SDL_Surface* surfaceBackground = IMG_Load("./ressources/blackBG.png");

                        sprintf(ressourcesPath,"%s", argv[1]);
			sprintf(filePathTemp,"%s", argv[2]);
			//loadRessources(ressourcesPath, filePathTemp);

                        for(int nT = 0; nT < 1; nT ++ )
                        {
                                for(int nLayer = 0; nLayer < nb_Layers; nLayer++ )
                                {
                                        if( bInstall[nT][nLayer] == 1 )
                                        {
                                                strcpy(coreName, layers[nLayer]);
						//logMessage("X : ");
						//logMessage( coreName );
                                                //sprintf(cCommand, "cd /mnt/SDCARD/App/runcommand ; ./test.sh \"%s\"", layers[nLayer]);
                                                //system(cCommand);
                                        }
                                }
                        }




                        //sprintf(cCommand, "cd /mnt/SDCARD/App/runcommand ; ./test.sh \"%s\"", argv[2] );
                        //system(cCommand);

                        //sprintf(cCommand, "cd /mnt/SDCARD/App/runcommand ; ./test2.sh \"%s\" \"%s\" \"%s\"", "retroarch", coreName, argv[2] );
                        //system(cCommand);


                        //SDL_FreeSurface(surfaceBackground);
                        //execl("/mnt/SDCARD/App/The_Onion_Installer/retroarch.sh", "retroarch", coreName, argv[1], NULL );

                        sprintf(cCommand, "cd /mnt/SDCARD/App/runcommand ; ./coreSave.sh \"%s\" \"%s\" \"%s\"", "retroarch", coreName, argv[2] );
                        system(cCommand);

	                SDL_Surface* surfaceBlack = IMG_Load("./ressources/blackBG.png");
			SDL_BlitSurface(surfaceBlack, NULL, screen, NULL);
			SDL_BlitSurface(screen, NULL, video, NULL); 
			SDL_Flip(video);
	                SDL_FreeSurface(surfaceBlack);
			SDL_Quit();

                        execl("/mnt/SDCARD/App/runcommand/run.sh", "run", argv[2], NULL );

                        break;


                        /*
                        if (allActivated == 0){
                                allActivated = 1; 
                                setLayersInstall(1);
                        }
                        else {
                                allActivated = 0; 
                                setLayersInstall(0);
                        }


                        refreshScreen();
                        */
                }	

/*
		if ((r2_pressed)&&(nb_Layers > 0)&&(move==1)) {			
			if ((nListPostion + 14) <nb_Layers){
				nListPostion += 7;
			}
			else if ((nListPostion + 7) <nb_Layers){
				nListPostion = nb_Layers - 7;
				nSelection = 6 ;
			}
			refreshScreen();
		}	 
		
		if ((l2_pressed)&&(nb_Layers > 0)&&(move==1)) {			
			if ((nListPostion - 7) > 0) {
				nListPostion -= 7;
			}
			else {
				nListPostion = 0;
				nSelection = 0;
			
			}
			refreshScreen();
		}	
*/		
		if ((down_pressed)&&(nb_Layers > 0)&&(move==1)) {			
			if (nSelection < 6){
				nSelection ++;
			}
			else if ((nSelection+nListPostion) < nb_Layers-1){
				nListPostion++;				
			}
			refreshScreen();
		}
		if ((up_pressed)&&(nb_Layers > 0)&&(move==1)) {			
			if (nSelection > 0){
				nSelection --;
			}
			else if (nListPostion > 0){
				nListPostion--;				
			}
			refreshScreen();
		}
	
		if (b_pressed&&(move==1)) {			
			break;
		}
		if ((a_pressed)&&(nb_Layers > 0)&&(move==1)) {		

			allActivated = 0; 
			setLayersInstall(0);

			if (nListPostion+nSelection<nb_Layers){
				if (bInstall[nTab][nListPostion+nSelection] == 1){
					bInstall[nTab][nListPostion+nSelection] = 0;
				}
				else{
					bInstall[nTab][nListPostion+nSelection] = 1;
		 		}
				/*
				if (nSelection < 6){
					nSelection ++;
				}
				else if ((nSelection+nListPostion) < nb_Layers-1){
					nListPostion++;				
				}
				*/
				//logMessage( layers[nListPostion+nSelection] );
				refreshScreen();
			}	
		}

		if (start_pressed && (move == 0) ) {
			pthread_cancel(thread_id);
			move = 1;
			refreshScreen();

                }
		if (select_pressed && (move == 0)) {
			pthread_cancel(thread_id);
			move = 1;
			refreshScreen();

		}
	
	}


    return EXIT_SUCCESS;
}


