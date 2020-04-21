float InOutQuad(float t, float b, float c, float d){
  t /= d/2.0;
  if(t < 1.0) return c/2.0*t*t+b;
  t--;
	return -c/2.0 * (t*(t-2.0) - 1.0) + b;
}


namespace ease{
  float sharpen(float t, int n){
    return pow(sin(3.14159265*t/2.0), n);
  }

  float langmuir(float t, float k){
    return k*t/(1+k*t);
  }
};
