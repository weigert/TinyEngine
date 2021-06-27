/*
================================================================================
                Compute Shader Example Programs / Tests
================================================================================
*/

#include <iomanip>      // std::setprecision


//Increment Test: Take a vector of a certain size, increment all elements!

void increment(){

  	std::vector<float> buffer(SIZE);								//Create the Empty Buffer
  	std::fill(buffer.begin(), buffer.end(), 0.0f);

  	//Increment Every Element of Buffer, ITER times

  	std::cout<<"Serial ";
  	timer::benchmark<std::chrono::milliseconds>([&](){

  	for(int i = 0; i < ITER; i++)
  	for(int j = 0; j < SIZE; j++)
  		buffer[j]++;

  	std::cout<<buffer[0]<<std::endl;

  	});


  	std::fill(buffer.begin(), buffer.end(), 0.0f);

  	Compute compute("shader/increment.cs", {"buff"});		//Create the Compute Shader
  	compute.buffer("buff", buffer);									//Upload the Buffer

  	compute.use();																	//Use the Shader

  	std::cout<<"Parallel ";
  	timer::benchmark<std::chrono::milliseconds>([&](){

  	for(int i = 0; i < ITER; i++)
  		compute.dispatch(SIZE/1024);			//Execute the Compute Shader

  	compute.retrieve("buff", buffer);							//Retrieve Data from Compute Shader
  	std::cout<<buffer[0]<<std::endl;

  	});

}

//Reduce Test: Perform a reduction over all elements!

void reduce(){

  const int k = 24;
  int size = (1 << k);
  std::cout<<"Size: "<<size<<std::endl;
  if(size > 65535) std::cout<<"Exceeded Max Work-Group Size"<<std::endl;

  std::vector<int> buffer;								  //Create the Empty Buffer
  for(int i = 0; i < size; i++)
    buffer.push_back(rand()%100);                      //Random Elements

  //Serial

  int sum = 0;

  std::cout<<"Serial ";
  timer::benchmark<std::chrono::microseconds>([&](){

  for(int j = 0; j < size; j++)
    sum += buffer[j];

  });

  std::cout<<"RESULT: "<<std::setprecision(16)<<sum<<std::endl;

  //Parallel

  std::vector<int> result(1, 0);

  Compute compute("shader/sum.cs", {"buffA", "buffB"});		 //Create the Compute Shader
  compute.use();																	         //Use the Shader

  compute.buffer("buffA", buffer);									       //Upload the Buffer
  compute.buffer("buffB", buffer);									       //Upload the Buffer

  int val = 0;

  std::cout<<"Parallel ";
  timer::benchmark<std::chrono::microseconds>([&](){

  for(int i = 0; i < k; i++){
    compute.uniform("flip", i%2);
    compute.uniform("stride", size/(2 << i));
    int N = size/1024/(2 << i);
    if(N < 1) N = 1;
    compute.dispatch(N);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
  }

  compute.retrieve("buffA", val, 0);

  });

  std::cout<<"RESULT: "<<std::setprecision(16)<<val<<std::endl;

}

//Outer Product Test: Basically an N-M Body Interaction of some sort!
