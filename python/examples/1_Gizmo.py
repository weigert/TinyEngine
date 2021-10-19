# TinyEngine Python Binding Testing Script
from tinyengine import tiny

tiny.view.linewidth(2.0);
tiny.window("Python Test", 1200, 800)

gizmoshader = tiny.Shader(['shader/gizmo.vs', 'shader/gizmo.fs']);
gizmo = tiny.Gizmo();

def handler():
    tiny.cam.handler()

def pipeline():

    tiny.view.target([0.0, 0.0, 0.0], True)

    gizmoshader.use();
    gizmoshader.uniform("vp", tiny.cam.vp());
    gizmo.render(tiny.GL.lines);

def loop():
    return;

tiny.view.pipeline(pipeline);
tiny.event.handler(handler);

tiny.cam.zoomrate(5.0);
tiny.cam.moverate(0.1);
tiny.cam.near(-100.0);
tiny.cam.far(100.0);
tiny.cam.init(1.0, tiny.cam.type.ORTHO)

tiny.loop(loop);
tiny.quit()
