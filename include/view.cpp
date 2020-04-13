class View{
  public:
    bool init(std::string windowName, int width, int height);
    void cleanup();

    unsigned int WIDTH, HEIGHT;

    SDL_Window* gWindow;        //Window Pointer
    SDL_GLContext gContext;     //Render Context

    ImGuiIO io;
    Handle interface;           //User defined Interface
    bool showInterface = false;
    void drawInterface();

    Handle pipeline;            //User defined Pipeline
    void render();
    void target(glm::vec3 clearcolor);  //Target main window for drawing

    //Flags
    bool fullscreen = false;
    bool vsync = true;
};

bool View::init(std::string _name, int _width, int _height){
  WIDTH = _width;
  HEIGHT = _height;

  //Initialize the Window and Context
  gWindow = SDL_CreateWindow(_name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
  if( gWindow == NULL ){
    printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
    return false;
  }
  SDL_SetWindowResizable(gWindow, SDL_TRUE);
  gContext = SDL_GL_CreateContext(gWindow);

  //Initialize OPENGL Stuff
	SDL_GL_SetSwapInterval(vsync);
	glewExperimental = GL_TRUE;
	glewInit();

  //Setup the Guy
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  io = ImGui::GetIO(); (void)io;

  ImGui_ImplSDL2_InitForOpenGL(gWindow, gContext);
  ImGui_ImplOpenGL3_Init("#version 130");

  ImGui::StyleColorsCustom();

  //Configure Global OpenGL State
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_BLEND) ;
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CW);
  glLineWidth(1.0f);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return true;
}

void View::cleanup(){
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

	SDL_GL_DeleteContext( gContext );
	SDL_DestroyWindow( gWindow );
}

void View::render(){

  //User-defined rendering pipeline
  (pipeline)();

  if(showInterface)
    drawInterface();

  SDL_GL_SwapWindow(gWindow); //Update Window
}

void View::drawInterface(){
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame(gWindow);
  ImGui::NewFrame();

  (interface)();  //Draw user-defined interface
  //ImGui::ShowDemoWindow();

  ImGui::Render();
  glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void View::target(glm::vec3 clearcolor){
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, WIDTH, HEIGHT);
  glClearColor(clearcolor.x, clearcolor.y, clearcolor.z, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
