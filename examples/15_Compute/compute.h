/*
================================================================================
                Compute Shader Example Programs / Tests
================================================================================

Examples:

Implemented:
1. Parallel Operation on large vector
2. Accumulation over a large vector
3. Gauss Transform with Normalization
4. Matrix-Matrix Multiply

*/

#include <Eigen/Dense>

#include <iomanip> // std::setprecision

#define SIZE 65536
#define BIGSIZE (pow(2,24))
#define ITER 10000

class CacheFlusher{
public:
    using Size = long long int;
    CacheFlusher(Size size = 1<<26 /* 64 MB */) :
        size(size),
        buf(new volatile unsigned char[size])
    {}

    ~CacheFlusher(){
        delete[] buf;
    }

    void flush(){
      for(Size i = 0; i < size; ++i)
        buf[i] += i;
    }

private:
    const Size size {0};
    volatile unsigned char *buf;
};


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

	std::fill(buffer.begin(), buffer.end(), 0.0f);         //Reset Buffer

  Buffer buf(buffer);                                   //Raw Buffer Object

	Compute compute("shader/increment.cs", {"buff"});		  //Create the Compute Shader
	compute.bind<float>("buff", &buf);								    //Upload the Buffer
	compute.use();																	      //Use the Shader

	std::cout<<"Parallel ";
	timer::benchmark<std::chrono::milliseconds>([&](){

	for(int i = 0; i < ITER; i++)
		compute.dispatch(SIZE/1024);			                  //Execute the Compute Shader

	buf.retrieve(buffer);			                            //Retrieve Data from Compute Shader

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
  Buffer buf(buffer);
  compute.bind<float>("buff", &buf);							    //Upload the Buffer
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

  buf.retrieve(buffer);

  });


  std::cout<<"Result: "<<std::setprecision(16)<<buffer[0]<<std::endl;

}



//IMPORTANT NOTE: WATCH THE DIMENSIONS!

void gausstransform(){

  std::cout<<"Launching Gauss Tranform Test..."<<std::endl;

  int N = 1024*4;
  int M = 1024*4;

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
  timer::benchmark<std::chrono::milliseconds>([&](){

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
  std::cout<<"Result: "<<P[1]<<std::endl;
  std::cout<<"Result: "<<PX[1]<<std::endl;
  std::cout<<"Result: "<<PY[1]<<std::endl;

  //Parallel

  for(int n = 0; n < N; n++)
    PX[n] = 0.0f;

  for(int m = 0; m < M; m++)
    PY[m] = 0.0f;

  for(int m = 0; m < M; m++)
  for(int n = 0; n < N; n++)
    P[m*N+n] = 0.0f;

  //Define Shaders and their Interfaces
  Compute compute("shader/gausstransform.cs", {"pointsetA", "pointsetB", "probability"});
  Compute accumulate("shader/2Daccumulate.cs", {"vector", "probability"});

  Buffer pA(pointsetA);
  Buffer pB(pointsetB);
  Buffer prob(N*M, (float*)NULL);
  Buffer vec((M>N)?M:N, (float*)NULL);

  compute.bind<glm::vec4>("pointsetA", &pA);
  compute.bind<glm::vec4>("pointsetB", &pB);
  compute.bind<float>("probability", &prob);
  compute.bind<float>("vector", &vec);

  std::cout<<"Parallel ";
  timer::benchmark<std::chrono::milliseconds>([&](){

  //Execute N-N Interaction

  compute.use();

  compute.uniform("N", N);
  compute.uniform("M", M);
  compute.uniform("var", var);
  compute.uniform("T", T);

  compute.dispatch(N/32, M/32);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  accumulate.use();

  accumulate.uniform("DX", N);
  accumulate.uniform("DY", M);

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

  prob.retrieve(N*M, P);

  accumulate.uniform("set", 0.0);

  accumulate.uniform("operation", 0);
  accumulate.uniform("minordim", true);
  accumulate.dispatch(M/1024);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  vec.retrieve(M, PY);

  accumulate.uniform("operation", 0);
  accumulate.uniform("minordim", false);
  accumulate.dispatch(N/1024);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  vec.retrieve(N, PX);

  });

  std::cout<<"Result: "<<P[0]<<std::endl;
  std::cout<<"Result: "<<PX[0]<<std::endl;
  std::cout<<"Result: "<<PY[0]<<std::endl;
  std::cout<<"Result: "<<P[M]<<std::endl; //Note: P is transposed
  std::cout<<"Result: "<<PX[1]<<std::endl;
  std::cout<<"Result: "<<PY[1]<<std::endl;

  delete[] P, PX, PY;
  std::cout<<std::endl;

}



void matrixmatrix(){

  CacheFlusher cacheFlusher;

  std::cout<<"Launching Matrix-Matrix Multiply Test..."<<std::endl;

  int N = 1024*4;
  int K = 1024*4;
  int M = 1024*4;

  //Create the Buffers
  float* A = new float[N*K];  //Matrix A
  float* B = new float[K*M];  //Matrix B
  float* R = new float[N*M];  //Result Matrix

  //Fill the Buffers
  for(int i = 0; i < N*K; i++)
    A[i] = (float)(rand()%1000)/1000.0f;
  for(int i = 0; i < K*M; i++)
    B[i] = (float)(rand()%1000)/1000.0f;
  for(int i = 0; i < K*M; i++)
    R[i] = 0.0f;

  //Parallel

  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  //Define Shaders and their Interfaces
  Compute compute("shader/matmult.cs", {"matrixA", "matrixB", "result"});

  Buffer mA(N*K, A);
  Buffer mB(K*M, B);
  Buffer res(N*M, (float*)R);

  compute.bind<float>("matrixA", &mA);
  compute.bind<float>("matrixB", &mB);
  compute.bind<float>("result", &res);

  //const int BS = 2;

  glFinish();

  std::cout<<"Parallel ";
  timer::benchmark<std::chrono::microseconds>([&](){

  compute.use();
  compute.uniform("N", N);
  compute.uniform("K", K);
  compute.uniform("M", M);

  compute.dispatch(N/16, M/1024);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  res.retrieve(N*M, R);

  });

  std::cout<<"Result: "<<R[0]<<std::endl;
  std::cout<<"Result: "<<R[1]<<std::endl;
  std::cout<<"Result: "<<R[M]<<std::endl;

  //Serial Eigen

  cacheFlusher.flush();

  Eigen::Map<Eigen::Matrix<float,-1,-1,Eigen::RowMajor>> AM(A,N,K);
  Eigen::Map<Eigen::Matrix<float,-1,-1,Eigen::RowMajor>> BM(B,K,M);
  Eigen::Map<Eigen::Matrix<float,-1,-1,Eigen::RowMajor>> RM(R,N,M);

  std::cout<<"Serial Eigen ";
  timer::benchmark<std::chrono::microseconds>([&](){

    RM = AM*BM;

  });

  std::cout<<"Result: "<<RM(0,0)<<std::endl;
  std::cout<<"Result: "<<RM(0,1)<<std::endl;
  std::cout<<"Result: "<<RM(1,0)<<std::endl;


  std::cout<<std::endl;
  delete[] A, B, R;

}
