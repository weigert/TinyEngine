from tinyengine import tiny

# Main Functions

def loop():
    return;

def pipeline():
    tiny.view.target([1.0, 1.0, 1.0], True)

def interface():
    return;

def handler():
    return;

# Main Loop

tiny.window("Example Window", 600, 400)

tiny.view.pipeline(pipeline);
tiny.view.interface(interface);
tiny.event.handler(handler);

tiny.loop(loop);
tiny.quit()
