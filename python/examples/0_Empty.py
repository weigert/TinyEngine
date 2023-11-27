import tinyengine as tiny

def pipeline():
    tiny.view.target([0.0, 1.0, 1.0], True, True)
    return

def loop():
    return;

tiny.window("Example Window", 600, 400)
tiny.view.pipeline = pipeline;
tiny.loop(loop);
tiny.quit()