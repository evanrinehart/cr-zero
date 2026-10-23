# def c_draw_rectangle(x, y, w, h, color) -> nil

Color = Data.define :r, :g, :b, :a

class App

    def initialize
        @angle = 0.0
        @life = 4
    end

    def update
        @angle += 0.01
    end

    def redraw
        x = 200 + 100*Math.cos(@angle)
        y = 200 + 100*Math.sin(@angle)
        green = Color.new 0, 255, 0, 255
        yellow = Color.new 255, 255, 0, 255
        c_draw_rectangle(x, y, 100.0, 100, yellow)
    end

    def input_key key, down
        puts "input_key called key=#{key} down=#{down}"
        @life -= 1
        return @life <= 0
    end

end
