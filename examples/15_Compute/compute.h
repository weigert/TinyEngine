/*
================================================================================
                Compute Shader Example Programs / Tests
================================================================================

Examples:

Implemented:
1. Parallel Operation on large vector
2. Accumulation over a large vector

To-Do:

3. 2D Accumulation, along some dimension?




*/

#include <iomanip> // std::setprecision

#define SIZE 65536
#define BIGSIZE (pow(2,24))
#define ITER 10000

void increment(){

  std::cout<<"Launching Vector Increment Test..."<<std::endl;

	std::vector<float> buffer(SIZE, 0.0f);					     //Create the Empty Buffer

  //Serial

	std::cout<<"Serial ";
	timer::benchmark<std::chrono::milliseconds>([&](){

	for(int i = 0; i < ITER; i++)
	for(int j = 0; j < SIZE; j++)
		buffer[j]++;

	});

  std::cout<<"Result: "<<std::setprecision(16)<<buffer[0]<<std::endl;

  //Parallel

	std::fill(buffer.begin(), buffer.end(), 0.0f);       //Reset Buffer

	Compute compute("shader/increment.cs", {"buff"});		 //Create the Compute Shader
	compute.buffer("buff", buffer);									     //Upload the Buffer
	compute.use();																	     //Use the Shader

	std::cout<<"Parallel ";
	timer::benchmark<std::chrono::milliseconds>([&](){

	for(int i = 0; i < ITER; i++)
		compute.dispatch(SIZE/1024);			                 //Execute the Compute Shader

	compute.retrieve("buff", buffer);			               //Retrieve Data from Compute Shader

	});

  std::cout<<"Result: "<<std::setprecision(16)<<buffer[0]<<std::endl;

  std::cout<<std::endl;

}


void accumulate(){

  std::cout<<"Launching Vector Accumulation Test..."<<std::endl;

  int size = BIGSIZE;

  std::vector<float> buffer;								          //Create the Empty Buffer
  for(int i = 0; i < size; i++)
    buffer.push_back((float)(rand()%100)/100.0f);     //Random Elements

  std::vector<double> dbuffer;                        //Vector of Doubles for Precision
  for(auto& b: buffer)
    dbuffer.push_back((double)b);

  double dsum = 0.0;

  //Serial

  std::cout<<"Serial ";
  timer::benchmark<std::chrono::milliseconds>([&](){

    for(int j = 0; j < size; j++)
      dsum += dbuffer.at(j);

  });

  std::cout<<"Result: "<<std::setprecision(16)<<dsum<<std::endl;

  //Parallel

  Compute compute("shader/accumulate.cs", {"buff"});		//Create the Compute Shader
  compute.buffer("buff", buffer);							          //Upload the Buffer
  compute.use();																        //Use the Shader

  int K = 256;                                          //K-Ary Merge
  compute.uniform("K", K);                              //Set Uniform

  std::cout<<"Parallel ";
  timer::benchmark<std::chrono::milliseconds>([&](){

  for(int rest = size%K, stride = size/K;
      stride >= 1 || rest > 0;
      rest = stride%K, stride /= K){

    compute.uniform("rest", rest);                      //Set Uniforms
    compute.uniform("stride", stride);                  //
    compute.dispatch(1+stride/1024);                    //Round-Up Division

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  }

  compute.retrieve("buff", buffer);

  });

  std::cout<<"Result: "<<std::setprecision(16)<<buffer[0]<<std::endl;

  std::cout<<std::endl;

}

//IMPORTANT NOTE: WATCH THE DIMENSIONS!

void gausstransform(){

  std::cout<<"Launching Gauss Tranform Test..."<<std::endl;

  int N = 4*1024;
  int M = 4*1024;

  std::vector<glm::vec4> pointsetA;   //Pointset A
  std::vector<glm::vec4> pointsetB;   //Pointset B
  float* P = new float[N*M];          //Interaction Matrix
  float* PX = new float[N];           //Probability along N
  float* PY = new float[M];           //Probability along M

  for(int n = 0; n < N; n++){
    pointsetA.emplace_back((float)(rand()%100)/100.0f, (float)(rand()%100)/100.0f, (float)(rand()%100)/100.0f, 1.0f);
  }
  for(int m = 0; m < M; m++){
    pointsetB.emplace_back((float)(rand()%100)/100.0f, (float)(rand()%100)/100.0f, (float)(rand()%100)/100.0f, 1.0f);
  }

  for(int n = 0; n < N; n++)
    PX[n] = 0.0f;

  for(int m = 0; m < M; m++)
    PY[m] = 0.0f;

  for(int m = 0; m < M; m++)
  for(int n = 0; n < N; n++)
    P[m*N+n] = 0.0f;

  const float var = 1.0f;
  const float bias = 1.0f;
  const glm::mat4 T = glm::mat4(1.0f);

  //Serial

  std::cout<<"Serial ";
  timer::benchmark<std::chrono::microseconds>([&](){

  for(int m = 0; m < M; m++){

    float Z = bias;

    for(size_t n = 0; n < N; n++){
      glm::vec4 d = pointsetA[n]-T*pointsetB[m];          //Distance Between 2 Points
      P[m*N+n] = exp(-0.5/var*glm::dot(d, d));            //Probability of Association (One-Way)
      Z += P[m*N+n];                                      //Accumulate Partition Function
    }

    for(size_t n = 0; n < N; n++){
      P[m*N+n] /= Z;                                      //Normalize Density
      PX[n] += P[m*N+n];                                  //Accumulate Probability along N
      PY[m] += P[m*N+n];                                  //Accumulate Probability along M
    }

  }

  });

  std::cout<<"Result: "<<P[0]<<std::endl;
  std::cout<<"Result: "<<PX[0]<<std::endl;
  std::cout<<"Result: "<<PY[0]<<std::endl;

  //Parallel

  for(int n = 0; n < N; n++)
    PX[n] = 0.0f;

  for(int m = 0; m < M; m++)
    PY[m] = 0.0f;

  for(int m = 0; m < M; m++)
  for(int n = 0; n < N; n++)
    P[m*N+n] = 0.0f;

  Compute compute("shader/gausstransform.cs", {"pointsetA", "pointsetB", "probability"});
  compute.buffer("pointsetA", pointsetA);
  compute.buffer("pointsetB", pointsetB);
  compute.buffer("probability", (float*)NULL, N*M);

  Compute accumulate("shader/2Daccumulate.cs", {"vector"});
  accumulate.buffer("vector", (float*)NULL, (M>N)?M:N);   //Empty Vector!

  //add the probability buffer to accumulate too

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ShaderBase::ssbo["probability"]);
  glShaderStorageBlockBinding(accumulate.program, glGetProgramResourceIndex(compute.program, GL_SHADER_STORAGE_BLOCK, "probability"), ShaderBase::sbpi["probability"]);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ShaderBase::sbpi["probability"], ShaderBase::ssbo["probability"]);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


  std::cout<<"Parallel ";
  timer::benchmark<std::chrono::microseconds>([&](){

  //Execute N-N Interaction

  compute.use();

  compute.uniform("N", N);
  compute.uniform("M", M);
  compute.uniform("var", var);
  compute.uniform("T", T);

  compute.dispatch(N/32, M/32);

  accumulate.use();

  accumulate.uniform("DY", N);
  accumulate.uniform("DX", M);

  //Sum Z over Minor Dimension
  accumulate.uniform("set", bias);
  accumulate.uniform("operation", 0);
  accumulate.uniform("minordim", true);
  accumulate.dispatch(M/1024);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  //Divide P by Z
  accumulate.uniform("operation", 1);
  accumulate.dispatch(M/1024);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  accumulate.uniform("set", 0.0);
  accumulate.uniform("operation", 0);
  accumulate.uniform("minordim", true);
  accumulate.dispatch(M/1024);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  accumulate.retrieve("vector", PY, M);

  accumulate.uniform("operation", 0);
  accumulate.uniform("minordim", false);
  accumulate.dispatch(N/1024);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  accumulate.retrieve("vector", PX, N);
  accumulate.retrieve("probability", P, N*M);

  });

  std::cout<<"Result: "<<P[0]<<std::endl;
  std::cout<<"Result: "<<PX[0]<<std::endl;
  std::cout<<"Result: "<<PY[0]<<std::endl;

  delete[] P, PX, PY;
  std::cout<<std::endl;

}
