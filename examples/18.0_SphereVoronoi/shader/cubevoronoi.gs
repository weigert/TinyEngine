#version 330

layout (triangles) in;

in vec3 g_Quad [];
in vec3 g_Centroid [];

flat in vec3 g_Color [];

layout (triangle_strip, max_vertices=18) out;

out vec4 FragPos; // FragPos from GS (output per emitvertex)

out vec3 ex_Quad;
out vec3 ex_Centroid;

flat out vec3 ex_Color;
flat out int ex_InstanceID;

uniform mat4 vp[6];

void main(){

    for(int face = 0; face < 6; ++face){
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; ++i){ // for each triangle vertex

            FragPos = gl_in[i].gl_Position;
            ex_Quad = g_Quad[i];
            ex_Centroid = g_Centroid[i];
            ex_Color = g_Color[i];

            gl_Position = vp[face]*FragPos;
            EmitVertex();

        }
        EndPrimitive();
    }

}
