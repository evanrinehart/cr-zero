require 'app'

def setup
    @app = App.new
    puts "setup called"
end

def quit
    puts "quit called"
end

def update
    @app.update
end

def redraw
    @app.redraw
end

def input_key key, down
    @app.input_key key, down
end

def input_mouse x, y
    puts "input_mouse #{key} #{down}"
end

def input_button i
    puts "input_button #{key} #{down}"
end

def input_char c
    puts "input_char #{c}"
end
