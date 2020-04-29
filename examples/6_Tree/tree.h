struct Branch{

  int ID = 0;         //For Leaf Hashing
  bool leaf = true;
  Branch* A;
  Branch* B;

  //Parameters
  float ratio, spread, splitsize;

  Branch(float r, float s, float ss){
    ratio = r;
    spread = s;
    splitsize = ss;
  };

  ~Branch(){
    if(leaf) return;
    delete(A);
    delete(B);
  }

  //Position Data
  glm::vec3 dir = glm::vec3(0.0, 1.0, 0.0);
  double length = 0.0;
  double girth = 0.0;
  double size = 0.0;

  void grow(double _size);
  void split();
};

class Tree{
public:
  Tree(){ root = new Branch(0.6, 0.5, 0.5); }
  ~Tree(){ delete root; }

  float rate = 1.0;

  Branch* root;
  void grow(){
    root->grow(rate);
  }
};

void Branch::grow(double feed){

  double pass;
  if(leaf) pass = 0.0;
  else if(conservearea) pass = 1.0 - (A->girth+B->girth)/(A->girth+B->girth+girth);
  else if(conservediameter) pass = 1.0 - (A->length+B->length)/(A->length+B->length+length);
  else pass = passratio;

  size += (1.0-pass)*feed;

  //Compute Size Parameters
  length = cbrt(size);
  girth = sqrt(size);

  if(leaf && size > splitsize)
    split();  //Split Behavior

  if(!leaf){  //Grow Children
    A->grow(ratio*pass*feed);
    B->grow((1-ratio)*pass*feed);
  }
}

void Branch::split(){
  leaf = false;

  A = new Branch(ratio, spread, splitsize);
  B = new Branch(ratio, spread, splitsize);
  A->ID = rand()%1000;
  B->ID = rand()%1000;

  //Random Vector in Space
  glm::vec3 O = glm::normalize(glm::vec3((double)(rand()%100)/100.0-0.5, (double)(rand()%100)/100.0, (double)(rand()%100)/100.0-0.5));

  //Randomly Oriented Normal Vector to Previous Branch
  glm::vec3 N = glm::cross(glm::normalize(dir), O);

  //Reflect around vertical axis
  glm::vec3 M = glm::vec3(-1.0, 1.0, -1.0)*N;

  //Set Directions
  A->dir = glm::normalize( glm::mix(spread*N, dir, ratio) );
  B->dir = glm::normalize( glm::mix(spread*M, dir, 1.0-ratio) );
}

Tree tree;

#define PI 3.14159265f

// Model Constructing Function for Tree
std::function<void(Model*)> _construct = [&](Model* h){

  //Basically Add Lines for the Tree!
  std::function<void(Branch*, glm::vec3)> addBranch = [&](Branch* b, glm::vec3 p){

    //No Leaves
    if(b->leaf) return;

    glm::vec3 start = p;
    glm::vec3 end   = p + glm::vec3(b->length*treescale[0])*b->dir;

    //Get Some Normal Vector
    glm::vec3 x = glm::normalize(b->dir + glm::vec3(1.0, 1.0, 1.0));
    glm::vec4 n = glm::vec4(glm::normalize(glm::cross(b->dir, x)), 1.0);

    //Add the Correct Number of Indices
    glm::mat4 r = glm::rotate(glm::mat4(1.0), PI/ringsize, b->dir);

    //Index Buffer
    int _b = h->positions.size()/3;

    //GL TRIANGLES
    for(int i = 0; i < ringsize; i++){
      //Bottom Triangle
      h->indices.push_back(_b+i*2+0);
      h->indices.push_back(_b+i*2+1);
      h->indices.push_back(_b+(i*2+2)%(2*ringsize));
      //Upper Triangle
      h->indices.push_back(_b+(i*2+2)%(2*ringsize));
      h->indices.push_back(_b+i*2+1);
      h->indices.push_back(_b+(i*2+3)%(2*ringsize));
    }

    for(int i = 0; i < ringsize; i++){

      h->positions.push_back(start.x + b->girth*treescale[1]*n.x);
      h->positions.push_back(start.y + b->girth*treescale[1]*n.y);
      h->positions.push_back(start.z + b->girth*treescale[1]*n.z);
      h->normals.push_back(n.x);
      h->normals.push_back(n.y);
      h->normals.push_back(n.z);
      n = r*n;

      h->positions.push_back(end.x + taper*b->girth*treescale[1]*n.x);
      h->positions.push_back(end.y + taper*b->girth*treescale[1]*n.y);
      h->positions.push_back(end.z + taper*b->girth*treescale[1]*n.z);
      h->normals.push_back(n.x);
      h->normals.push_back(n.y);
      h->normals.push_back(n.z);
      n = r*n;

    }

    addBranch(b->A, end);
    addBranch(b->B, end);
  };

  //Recursive add Branches
  addBranch(tree.root, glm::vec3(0.0));
};

//Hash Function for Leaf Displacement!
std::hash<std::string> position_hash;
double hashrand(int i){
  return (double)(position_hash(std::to_string(i))%1000)/1000.0;
}

//Construct Leaf Particle System from Tree Data
std::function<void(Particle*)> addLeaves = [&](Particle* p){
  p->models.clear();

  //Explore the Tree and Add Leaves!
  std::function<void(Branch*, glm::vec3)> addLeaf = [&](Branch* b, glm::vec3 pos){

    if(b->leaf){

      for(int i = 0; i < leafcount; i++){
        //Hashed Random Displace
        glm::vec3 d = glm::vec3(hashrand(b->ID+i), hashrand(b->ID+i+leafcount), hashrand(b->ID+i+2*leafcount))-glm::vec3(0.5);
        d = d * glm::vec3(leafspread[0], leafspread[1], leafspread[2]);

        //Rotate Towards Camera and Scale
        glm::mat4 model = glm::translate(glm::mat4(1.0), pos+d);
        model = glm::rotate(model, -glm::radians(rotation - 45.0f), glm::vec3(0.0, 1.0, 0.0));
        p->models.push_back(glm::scale(model, glm::vec3(leafsize)));

      }

      return;
    }

    //Otherwise get other leaves!
    glm::vec3 end   = pos + glm::vec3(b->length*treescale[0])*b->dir;
    addLeaf(b->A, end);
    addLeaf(b->B, end);
  };

  addLeaf(tree.root, glm::vec3(0.0));
};
