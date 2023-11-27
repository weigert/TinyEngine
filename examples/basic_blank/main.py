import tinyengine as tiny

def pipeline():
    tiny.view.target([1.0, 1.0, 1.0], True, True)
    return

def loop(): 
    return;

# Initialize a Window
tiny.window("Example Window", 600, 400)

# Add Event Handler, Interface, Rendering Pipeline
tiny.event.handler = loop;
tiny.view.interface = loop;
tiny.view.pipeline = pipeline;

# Execute the Render Loop
tiny.loop(loop);

tiny.quit()