import tinyengine as tiny

tiny.view.linewidth = 2.0
tiny.window("Python Test", 400, 400)

gizmoshader = tiny.Shader(['shader/gizmo.vs', 'shader/gizmo.fs'], ["in_Pos", "in_Tex"]);
gizmo = tiny.Gizmo();

ortho = tiny.cam_orthogonal([400.0, 400.0], [-100.0, 100.0], 100.0)
orbit = tiny.cam_orbit([1, 0, 0], [0, 0, 0])

camera = tiny.camera(ortho, orbit)
camera.hook()

def pipeline():
    tiny.view.target([0.0, 0.0, 0.0], True, True)
    gizmoshader.use();
    gizmoshader.uniform("vp", camera.vp());
    gizmo.render(tiny.GL.lines);

def loop():
    return;

tiny.view.pipeline(pipeline);
tiny.loop(loop);
tiny.quit()