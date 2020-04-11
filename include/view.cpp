//Load Dependencies
#include "utility/shader.h"
#include "utility/model.h"
#include "utility/billboard.h"

class View{
  public:
    bool init(std::string windowName, int width, int height);
    void cleanup();

    //General Properties
    unsigned int WIDTH;
    unsigned int HEIGHT;

    //Window Stuff
    SDL_Window* gWindow;    //Window Pointer
    SDL_GLContext gContext; //Render Context

    //Interface Stuff
    ImGuiIO io;
    Handle interface;           //User defined Interface
    bool showInterface = false;
    void drawInterface();

    //Rendering Calls
    Handle pipeline;            //User defined Pipeline
    void render();

    //Other Flags
    bool fullscreen = false;
    bool vsync = true;
};

/*
================================================================================
                              Setup / Cleanup
================================================================================
*/

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
  glLineWidth(2.5f);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return true;
}

void View::cleanup(){
  //Shutdown IMGUI
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  //Destroy Context and Window
	SDL_GL_DeleteContext( gContext );
	SDL_DestroyWindow( gWindow );
}

/*
================================================================================
                              Rendering Calls
================================================================================
*/

void View::render(){
  //Clear the Screen
  glViewport(0, 0, WIDTH, HEIGHT);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //Black Screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //Do any necessary drawing logic...
  (pipeline)();

  //Draw the Interface
  if(showInterface)
    drawInterface();

  //Update the Window
  SDL_GL_SwapWindow(gWindow);
}

void View::drawInterface(){
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame(gWindow);
  ImGui::NewFrame();

  //Draw the Interface
  (interface)();

  //ImGui::ShowDemoWindow();
  ImGui::Render();
  glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/*
================================================================================
                                Rendering
================================================================================
*/

/*
void View::render(World &world){

  glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
  glBindFramebuffer(GL_FRAMEBUFFER, shadow.fbo);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  depthShader.useProgram();
  depthShader.setMat4("dmvp", depthProjection * depthCamera * model.model);
  model.render();             //Render Scene

  glBindVertexArray(0);

  //Regular Drawing

  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  glBindFramebuffer(GL_FRAMEBUFFER, image.fbo);
  glClearColor(skyCol.x, skyCol.y, skyCol.z, 1.0f); //Blue
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //Use the Shader
  defaultShader.useProgram();

  //Texture Stuff
  glActiveTexture(GL_TEXTURE0+0);
  glBindTexture(GL_TEXTURE_2D, shadow.depthTexture);
  defaultShader.setInt("shadowMap", 0);
  defaultShader.setVec3("lightCol", lightCol);
  defaultShader.setVec3("lightPos", lightPos);
  defaultShader.setVec3("lookDir", lookPos-cameraPos);
  defaultShader.setFloat("lightStrength", lightStrength);
  defaultShader.setMat4("projectionCamera", projection * camera);
  defaultShader.setMat4("dbmvp", biasMatrix * depthProjection * depthCamera * glm::mat4(1.0f));
  defaultShader.setMat4("model", model.model);

  //Add uniforms for flat and steep color!
  defaultShader.setVec3("flatColor", flatColor);
  defaultShader.setVec3("steepColor", steepColor);
  defaultShader.setFloat("steepness", steepness);

  model.render();             //Render Scene

  //Render to screen using the effect shader!
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //Render the Stuff
  effectShader.useProgram();
  glActiveTexture(GL_TEXTURE0+0);
  glBindTexture(GL_TEXTURE_2D, image.texture);
  effectShader.setInt("imageTexture", 0);
  glActiveTexture(GL_TEXTURE0+1);
  glBindTexture(GL_TEXTURE_2D, image.depthTexture);
  effectShader.setInt("depthTexture", 1);
  glBindVertexArray(image.vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  //Render the Depthmap Billboard
  billboardShader.useProgram();
  glActiveTexture(GL_TEXTURE0+0);
  glBindTexture(GL_TEXTURE_2D, image.depthTexture);
  billboardShader.setInt("imageTexture", 0);

  //Position the Quad Appropriately
  image.relativeSize(glm::vec2(0.0, 0.8), glm::vec2(0.2));
  billboardShader.setMat4("model", image.model);

  //Draw the Model
  glBindVertexArray(image.vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

*/
