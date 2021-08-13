# TinyEngine Python Binding Testing Script
from tinyengine import tiny

'''
import torch

pointcloud = torch.Tensor([ [0.0, 0.0, 0.0],
                            [1.0, 0.0, 0.0],
                            [0.0, 1.0, 0.0],
                            [0.0, 0.0, 1.0],
                            [1.0, 1.0, 0.0],
                            [1.0, 0.0, 1.0],
                            [0.0, 1.0, 1.0],
                            [1.0, 1.0, 1.0] ]).float();

print(hex(pointcloud.data_ptr()))
'''

#
# Initialize TinyEngine, Session Properties, Start an OpenGL Context w. Window
#

#tiny.benchmark(True);
#tiny.view.vsync(False);
tiny.view.linewidth(2.0);

tiny.window("Python Test", 1200, 800)

#
# Define Model and Utility Variables
#

a = 0.0;

# Shader for Drawing Flat Surface, Flat Surface Geometry Primitive

flatshader = tiny.Shader(['shader/flat.vs', 'shader/flat.fs']);
flat = tiny.Square2D();

# Shader for Gizmo, Gizmo Geometry Primitive

gizmoshader = tiny.Shader(['shader/gizmo.vs', 'shader/gizmo.fs']);
gizmo = tiny.Gizmo();

#
# Define Main Control Flow
#

# Event Handler

def handler():
    tiny.cam.handler()

# Rendering Pipeline

def pipeline():

    tiny.view.target([0.0, 0.0, 0.0], True)

    #flatshader.use()
    #flatshader.uniform("color", [1.0-a, a, 1.0])
    #flat.render();

    gizmoshader.use();
    gizmoshader.uniform("vp", tiny.cam.vp());
    gizmo.render(tiny.GL.lines);

# Execution Loop (Repeat per Frame)

def loop():
    global a
    a += 0.01
    if a > 1.0:
        a = 0.0

tiny.view.pipeline(pipeline);
tiny.event.handler(handler);

#
# Camera Helper
#

tiny.cam.zoomrate(5.0);
tiny.cam.near(-100.0);
tiny.cam.far(100.0);
tiny.cam.init(1.0, tiny.cam.type.ORTHO)

#
# Execute Main Loop (Ends upon Signal Kill / Quit)
#

tiny.loop(loop);

tiny.quit()
