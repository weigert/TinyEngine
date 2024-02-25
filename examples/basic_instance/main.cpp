#include <TinyEngine/TinyEngine>
#include <TinyEngine/image>
#include <TinyEngine/camera>

int main( int argc, char* args[] ) {

	Tiny::window("Particle System", 800, 800);

	// Create Camera

	Tiny::cam::orthogonal ortho({Tiny::view.WIDTH, Tiny::view.HEIGHT}, {-10.0f, 10.0f}, 5.0f);
	Tiny::cam::orbit orbit(glm::vec3(1, 0, 0), glm::vec3(0, 0, 0));
	Tiny::camera cam(ortho, orbit);
	cam.hook();

	Tiny::Square3D model;									//Model we want to instance render!

	std::vector<glm::mat4> models;
	for(int i = 0; i < 1000; i++){	//Generate random model matrices
		glm::mat4 model = glm::translate(glm::mat4(1.0),  glm::vec3(rand()%101-50, rand()%101-50, rand()%101-50));
		models.push_back(glm::rotate(model, 2.0f*3.14159265f*(float)(rand()%360)/360.0f, glm::vec3(0.0, 1.0, 0.0)));
	}

	Tiny::Buffer modelbuf(models);
	Tiny::Instance particle(model);			//Particle system based on this model
	particle.bind<glm::mat4>(modelbuf);			//Update particle system

	Tiny::png image("dot.png");
	Tiny::Texture tex(image.width(), image.height(), Tiny::Texture::RGBA8U, image.data());
	Tiny::Shader particleShader({"shader/particle.vs", "shader/particle.fs"}, {"in_Quad", "in_Tex", "in_Model"});

	Tiny::view.pipeline = [&](){

		Tiny::view.target(glm::vec3(0));

		particleShader.use();
		particleShader.texture("spriteTexture", tex);
		particleShader.uniform("vp", cam.vp());
		particle.render(GL_TRIANGLE_STRIP, models.size());

	};

	Tiny::loop([&](){ //Autorotate Camera
		//cam.control.pan(0.1f);
		cam.update();
	});

	Tiny::quit();

	return 0;
}
