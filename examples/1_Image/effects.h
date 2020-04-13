/*
    Effects File. Contains all Parameters for effects.
*/

int ind = 0;    //Effect Index
int res = 100;  //Pixelate Effect
int bits = 4;   //Bitreduce Effect

Handle interfaceFunc = [&](){
  /* Do Something */
  ImGui::Text("Shader Effects");
  ImGui::DragInt("Effect", &ind, 1, 0, 2);
  ImGui::DragInt("Resolution", &res, 1, 1, 400);
  ImGui::DragInt("Bits", &bits, 1, 1, 16);
};
