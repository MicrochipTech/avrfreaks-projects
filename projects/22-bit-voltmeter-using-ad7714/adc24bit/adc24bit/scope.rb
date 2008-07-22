#!/usr/bin/env ruby

require 'gtk2'
# require 'serialport' doesn't work, so use Kernel::require
Kernel::require 'serialport'

class SingleStripChart < Gtk::DrawingArea
	def initialize(w, h, tv, bv, show_text=false)
		super()

		@topval = tv
		@botval = bv
		@dv = []
		@last_val = 0

		@back_color = Gdk::Color.new(0, 10000, 0)
		@trace_color = Gdk::Color.new(40000, 65535, 40000)

		set_size_request(w, h)

		if show_text
			layout = create_pango_layout
		else
			layout = nil
		end

		signal_connect('configure_event') do
			back_gc = Gdk::GC.new(window)
			back_gc.rgb_fg_color = @back_color

			@pixmap = Gdk::Pixmap.new(self.window,
					allocation.width,
					allocation.height, -1)
			@pixmap.draw_rectangle(back_gc, true,
				0, 0, allocation.width, allocation.height)
			@pm2 = Gdk::Pixmap.new(self.window,
					allocation.width,
					allocation.height, -1)

			true
		end

		signal_connect('expose_event') do |widget, event|
			window.draw_drawable(style.fg_gc(state),
				@pixmap,
				event.area.x, event.area.y,
				event.area.x, event.area.y,
				event.area.width, event.area.height)

			if !layout.nil?
				layout.set_markup("<span foreground='red' size='xx-large' face='monospace'>#{sprintf '%+.6f V', @last_val}</span>")
				window.draw_layout(style.fg_gc(state), 2, 2, layout)
			end

			true
		end
	end

	def updatePixmap
		return if @pixmap == nil # if not configured
		return if @dv.length <= 1

		back_gc = Gdk::GC.new(window)
		back_gc.rgb_fg_color = @back_color
		trace_gc = Gdk::GC.new(window)
		trace_gc.rgb_fg_color = @trace_color

		delta = @dv.length - 1

		@pm2.draw_drawable(style.fg_gc(state), @pixmap, 0, 0, 0, 0, -1, -1)
		@pixmap.draw_drawable(style.fg_gc(state), @pm2, delta, 0, 0, 0, -1, -1)
		@pixmap.draw_rectangle(back_gc, true,
			allocation.width-delta, 0, delta, allocation.height)

		x0 = allocation.width-1 - delta
		while(@dv.length > 1)
			sa = (@dv[0] - @botval) / (@topval - @botval)
			sb = (@dv[1] - @botval) / (@topval - @botval)
			fd = sb.floor - sa.floor
			va = (sa * allocation.height) % allocation.height
			vb = (sb * allocation.height) % allocation.height
			if(fd == 0)
				@pixmap.draw_line(trace_gc, x0, va, x0+1, vb)
			else
				@pixmap.draw_line(trace_gc, x0, va, x0+1, vb + fd*allocation.height)
				@pixmap.draw_line(trace_gc, x0, va - fd*allocation.height, x0+1, vb)
			end

			@dv.shift
			x0 = x0 + 1
		end

		update_rect = Gdk::Rectangle.new(
			0, 0, allocation.width, allocation.height)
		window.invalidate(update_rect, false)
	end

	def addVal(v)
		@dv.push(v)
		@last_val = v
	end
end

############################################################

class ScopeWindow < Gtk::Window
	def initialize
		super()

		self.title = "Scope"
		self.border_width = 0
		self.realize
		self.window.set_cursor(Gdk::Cursor.new(Gdk::Cursor::CROSSHAIR))

		@charts = []
		(0..5).each { |e|
			r = 3.0 * (10.0 ** (-e.to_f))
			show_text = e==0
			@charts.push(SingleStripChart.new(1000, 100, -r, r, show_text))
		}

		totalbox = Gtk::VBox.new
		self.add(totalbox)

		@charts.each { |c| 
			frame = Gtk::Frame.new
			frame.shadow_type = Gtk::SHADOW_IN
			frame.add(c)
			totalbox.pack_start(frame) 
		}

		self.signal_connect('delete_event') do
			Gtk.main_quit
			false
		end
		self.signal_connect('key_press_event') do |widget, event|
			if(event.state & Gdk::Window::CONTROL_MASK) != 0
				if(event.keyval == Gdk::Keyval::GDK_w ||
				   event.keyval == Gdk::Keyval::GDK_q)
					Gtk.main_quit
				end
			end
		end

		GLib::Timeout.add(50) do
			@charts.each { |c| c.updatePixmap }
			true
		end
	end

	def addVal(val)
		@charts.each { |c| c.addVal(val) }
	end
end

if ARGV.length != 1
	raise "Usage: $0 <port>"
end
portname = ARGV[0]

sp = SerialPort.new(portname, 115200, 8, 1, SerialPort::NONE)

charts = ScopeWindow.new

lines = []
Thread.new {
	while true do
		lines << sp.gets
	end
}

GLib::Timeout.add(10) do
	while !lines.empty?
		line = lines.shift
		line.chomp
		if line =~ /^\d+\s+([0-9.-]+)/
			val = $1.to_f
			charts.addVal(val)
		else
			puts "received: #{line}"
		end
	end
	true
end

charts.show_all
Gtk.main
