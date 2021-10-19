# TinyEngine Python Binding Testing Script
import torch
from tinyengine import tiny
from tinyengine import torch as tinytorch

tinytorch.test(torch.eye(2,2));

'''

# Prepare Window, Camera, Shader

tensor = torch.Tensor([ [0.0, 0.0, 0.0],
                        [1.0, 0.0, 0.0],
                        [0.0, 1.0, 0.0],
                        [0.0, 0.0, 1.0],
                        [0.0, 1.0, 1.0],
                        [1.0, 0.0, 1.0],
                        [1.0, 1.0, 0.0],
                        [1.0, 1.0, 1.0]]);

tiny.view.pointsize(5.0);

tiny.window("Pointcloud Test", 1200, 800)

tiny.cam.near(-10.0)
tiny.cam.far(10.0)
tiny.cam.zoomrate(10.0);
tiny.cam.init(250)

particleshader = tiny.Shader(['shader/particle.vs', 'shader/particle.fs'], ['in_Position', 'in_Color']);

# Prepare Data, Buffers, Model

position = tiny.Buffer();
color = tiny.Buffer();

position.fill_tensor(tensor);
color.fill_tensor(1.0-tensor*0.5);

particles = tiny.Model(['position', 'color']);          #Create Model with Attribute
particles.bind_vec3('position', position);                #Bind Buffer to Attribute
particles.bind_vec3('color', color);                   #Bind Buffer to Attribute
particles.SIZE = 8;                                     #Set the Size

# Define Lambdas, Execute Loop

def handler():
    tiny.cam.handler()

def pipeline():
    tiny.view.target([0.0, 0.0, 0.0], True)
    particleshader.use();
    particleshader.uniform("vp", tiny.cam.vp());
    particles.render(tiny.GL.points);

def loop():
    global tensor
    tensor = 1.01*tensor;
    position.fill_tensor(tensor);
    return;

tiny.view.pipeline(pipeline);
tiny.event.handler(handler);
tiny.loop(loop);
tiny.quit()

'''
