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

ortho = tiny.cam_orthogonal([400.0, 400.0], [-10.0, 10.0], 10.0)
orbit = tiny.cam_orbit([1, 0, 0], [0, 0, 0])

camera = tiny.camera(ortho, orbit)
camera.hook()

#tiny.event.tick()

particleshader = tiny.Shader(['shader/particle.vs', 'shader/particle.fs'], ['in_Position', 'in_Color']);

# Prepare Data, Buffers, Model

position = tiny.Buffer([10*i - 5 for i in cube]);
color = tiny.Buffer([1.0 - i*0.5 for i in cube]);

particles = tiny.Model(['position', 'color']);          #Create Model with Attribute
particles.bind('position', position);                #Bind Buffer to Attribute
particles.bind('color', color);                   #Bind Buffer to Attribute
#particles.SIZE = 8;                                     #Set the Size

# Define Lambdas, Execute Loop

def _pipeline():
    tiny.view.target([0.0, 0.0, 0.0], True, True)
    particleshader.use();
    particleshader.uniform("vp", camera.vp());
    particles.render(tiny.GL.points);

def loop():
    return

tiny.view.pipeline(_pipeline);
tiny.loop(loop);
tiny.quit()