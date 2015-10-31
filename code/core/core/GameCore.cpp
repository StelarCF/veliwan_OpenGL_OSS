#include "GameCore.h"

void GameCore::init() {
	log(LOG_DEBUG, "Entered init of Game Core");
	log(LOG_INFO, "Initializing SDL2...");
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY); //Set compatibility with opengl context
	//initialize defaults, video and audio
	if ((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) == -1) { //Initialize SDL and check for errors
		log(LOG_EMERG, "ERROR: Could not initialize SDL: %s. \n", SDL_GetError());
		exit(-1);
	}
	log(LOG_INFO, "SDL successfully initialized.\nCreating window...");
	window = SDL_CreateWindow("Veliwan Indev", 50, 50, args.resx, args.resy, args.type /*| SDL_WINDOW_FULLSCREEN*/); //Create a windows of 600x508 pixels at (50,50)
	context = SDL_GL_CreateContext(window); //Create OpenGL context
	glewInit(); //initialize OpenGL with GLEW
	log(LOG_INFO, "Window created.\nChecking OGL Context:\n");
	log(LOG_INFO, "Vendor: %s\nRenderer: %s\nVersion: %s\nShading Language Version: %s\nExtensions:\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_SHADING_LANGUAGE_VERSION), glGetString(GL_EXTENSIONS));
	SDL_GL_MakeCurrent(window, context); // Use the OpenGL context on the current window

	rc.gc = this;
	rc.resx = args.resx;
	rc.resy = args.resy;
	rc.cam = camera = new RTSCamera();
	rc.cam->gc = this;
	camera->init();
	rc.cam->rc = &rc; // link the camera to the core
	rc.init();
	ic.addListener(&rc);
	ic.addListener(this);
}

void GameCore::argument_interpreter(int argv, char **argc) {
	int i;
	log(LOG_DEBUG, "Entered argument interpreter of Game Core");
	// defaults
	args.debug = false;
	args.resx = 1024;
	args.resy = 768;
	args.type = SDL_WINDOW_OPENGL;
	for(i = 1; i < argv; i++)
		if(strcmp(argc[i], "--debug") == 0) {
			args.debug = true;
			logger.current_log_level = 9;
		} else if(strcmp(argc[i], "--ssaa") == 0)
			args.ssaa = true;
		else if(strcmp(argc[i], "--log") == 0) {
			freopen(argc[i + 1], "w", stdout);
			i++;
		} else if(strcmp(argc[i], "--res") == 0) {
			sscanf(argc[i + 1], "%fx%f", &args.resx, &args.resy);
			i++;
		} else if(strcmp(argc[i], "--fullscreen") == 0) {
			args.type = args.type | SDL_WINDOW_FULLSCREEN;
		} else if(strcmp(argc[i], "--debuglevel") == 0) {
			sscanf(argc[i + 1], "%d", &logger.current_log_level);
			i++;
		}
	Frame_Event = registerEvent("Frame");
}

void GameCore::run() {
	log(LOG_DEBUG, "Entered run of Game Core");
	GameEvent frame_ev;
	frame_ev.data = NULL;
	frame_ev.eventtype = Frame_Event;
	while (true) {
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) { //poll for SDL events
			switch (ev.type) { // check the event type
			case SDL_QUIT:
				die();
				break;
			default:
				ic.processEvent(ev);
			}
		}
		rc.doFrame();
		throwEvent(frame_ev);
		triggerEvents();
		//GL rendering here
		rc.render();
		SDL_GL_SwapWindow(window);
	}
}

void GameCore::die() {
	printf("Quiting SDL.\n");
	SDL_Quit();
	printf("Quiting....");
	exit(0);
}

int GameCore::registerEvent(string s) {
	if(registeredEvents.count(s) > 0)
		return registeredEvents[s];
	else return registeredEvents[s] = recount++;
}

void GameCore::throwEvent(GameEvent ev) {
	// Add all necessary systems here
	camera->addToEventQueue(ev);
}

void GameCore::triggerEvents() {
	// Same as with throwEvent
	camera->processEvents();
}