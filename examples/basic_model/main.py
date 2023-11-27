import tinyengine as tiny

cube = [0.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, 1.0,
        0.0, 1.0, 1.0,
        1.0, 0.0, 1.0,
        1.0, 1.0, 0.0,
        1.0, 1.0, 1.0];

tiny.view.pointsize = 5.0;
tiny.window("Pointcloud Test", 400, 400)

tiny.cam.near(-10.0)
tiny.cam.far(10.0)
tiny.cam.zoomrate(10.0);
tiny.cam.init(50)

particleshader = tiny.Shader(['shader/particle.vs', 'shader/particle.fs'], ['in_Position', 'in_Color']);

# Prepare Data, Buffers, Model

position = tiny.Buffer([10*i - 5 for i in cube]);
color = tiny.Buffer([1.0 - i*0.5 for i in cube]);

particles = tiny.Model(['position', 'color']);          #Create Model with Attribute
particles.bind('position', position);                #Bind Buffer to Attribute
particles.bind('color', color);                   #Bind Buffer to Attribute
#particles.SIZE = 8;                                     #Set the Size

# Define Lambdas, Execute Loop

def handler():
    tiny.cam.handler()

def pipeline():
    tiny.view.target([0.0, 0.0, 0.0], True, True)
    particleshader.use();
    particleshader.uniform("vp", tiny.cam.vp());
    particles.render(tiny.GL.points);

def loop():
    return;

tiny.view.pipeline = pipeline;
tiny.event.handler = handler;
tiny.loop(loop);
tiny.quit()
