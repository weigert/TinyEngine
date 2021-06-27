/*
================================================================================
                Compute Shader Example Programs / Tests
================================================================================
*/

#include <iomanip>      // std::setprecision
#include <numeric>

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

  std::vector<float> buffer;								          //Create the Empty Buffer
  for(int i = 0; i < size; i++)
    buffer.push_back((float)(rand()%100)/100.0f);     //Random Elements

  std::vector<double> dbuffer;
  for(auto& b: buffer)
    dbuffer.push_back((double)b);

  //Serial

  float sum = 0.0f;

  std::cout<<"Serial Single ";
  timer::benchmark<std::chrono::microseconds>([&](){

  for(int j = 0; j < size; j++)
    sum += buffer.at(j);

  });

  std::cout<<"SINGLE DIRECT: "<<std::setprecision(16)<<sum<<std::endl;

  double dsum = 0.0;

  //Serial Reduce Analogon
  std::cout<<"Serial Double ";
  timer::benchmark<std::chrono::microseconds>([&](){

    for(int j = 0; j < size; j++)
      dsum += dbuffer.at(j);

  });

  std::cout<<"DOUBLE DIRECT: "<<std::setprecision(16)<<dsum<<std::endl;

  //Parallel

  Compute compute("shader/accumulate_binary.cs", {"buff"});		//Create the Compute Shader
  compute.buffer("buff", buffer);							  //Upload the Buffer
  compute.use();																//Use the Shader

  std::cout<<"Parallel ";
  timer::benchmark<std::chrono::microseconds>([&](){

  for(int i = 0; i < k; i++){
    compute.uniform("stride", size/(2 << i));
    int N = size/1024/(2 << i);
    if(N < 1) N = 1;
    compute.dispatch(N);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
  }

  compute.retrieve("buff", buffer);

  });

//  for(int i = 0; i < 50; i++)
  std::cout<<"SINGLE PARALLEL: "<<std::setprecision(16)<<buffer[0]<<std::endl;

}

//Outer Product Test: Basically an N-M Body Interaction of some sort!
