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

  int size = pow(2, 24);

  std::vector<float> buffer;								          //Create the Empty Buffer
  for(int i = 0; i < size; i++)
    buffer.push_back((float)(rand()%100)/100.0f);     //Random Elements

  std::vector<double> dbuffer;
  for(auto& b: buffer)
    dbuffer.push_back((double)b);

  double dsum = 0.0;

  //Serial Reduce Analogon
  std::cout<<"Serial Double ";
  timer::benchmark<std::chrono::microseconds>([&](){

    for(int j = 0; j < size; j++)
      dsum += dbuffer.at(j);

  });

  std::cout<<"DOUBLE DIRECT: "<<std::setprecision(16)<<dsum<<std::endl;

  //Parallel

  Compute compute("shader/accumulate.cs", {"buff"});		//Create the Compute Shader
  compute.buffer("buff", buffer);							          //Upload the Buffer
  compute.use();																        //Use the Shader

  std::cout<<"Parallel ";
  timer::benchmark<std::chrono::microseconds>([&](){

  int K = 4;                                            //K-Ary Merge
  int rest = size%K;
  compute.uniform("K", K);
  compute.uniform("rest", rest);

  for(int stride = size / K; stride >= 1; stride /= K){

    compute.uniform("stride", stride);              //Set the Stride, Which is Now the Remaining Size
    compute.dispatch(1+stride/1024);

    rest = stride%K;
    compute.uniform("rest", rest);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  }

  compute.retrieve("buff", buffer);

  //Check if Remainder is non-zero
  if(rest > 0)
  for(int i = 1; i < rest; i++)
    buffer[0] += buffer[i];

  });

  std::cout<<"SINGLE PARALLEL: "<<std::setprecision(16)<<buffer[0]<<std::endl;

}

//Outer Product Test: Basically an N-M Body Interaction of some sort!
