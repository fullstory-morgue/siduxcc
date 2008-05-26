#!/usr/bin/env ruby
# 
# Sidux-Control-Center (gtk2)
# Fabian Wuertz
# siduxcc-gtk2 based on the Dreamlinux Control Panel Copyright (c) 2007 Nelson Gomes da Silveira (nelsongs) <ngsilveira@gmail.com>
# License: GPL

require "gtk2"

Gtk.init

MAX_PER_ROW = 7 # number of maximum iconified application per row
# this class deals with the button constructor
class ImageButton < Gtk::Button
	def initialize(text = nil, image = nil, textalign = :vert, relief = true)
		super()
		set_relief(Gtk::RELIEF_NONE) if relief == true
		align = Gtk::Alignment.new(0.5, 0.5, 0, 0)
		add align
		textalign == :vert ? box = Gtk::VBox.new(false, 0) : box = Gtk::HBox.new(false, 0) 
		align.add box
		@widget_image = Gtk::Image.new(image) 
		box.pack_start(@widget_image, false, false, 2) 
		@widget_label = Gtk::Label.new(text)
		box.pack_start(@widget_label, false, false, 2)
	end
	def action(action)
		self.signal_connect("clicked") { system("#{action} &") }
	end
private
@widget_image
@widget_label
end	
# This class deals with a particular panel containing any number of ImageButtons
class Panel < Gtk::VBox
	def initialize(title)
		super()
		label = Gtk::Label.new.set_markup("<b>#{title}</b>").set_alignment(0,1)
		pack_start(label)
		@table = Gtk::Table.new(1, MAX_PER_ROW, true)
		pack_start(@table)
		@column = 1
		@row = 1
		@count = 0
	end
	def table_resize(rows, cols)
		@table.resize(rows, cols)
	end
	def add_button(label, image, action)
		button = ImageButton.new(label, image)
		button.action( action )
		if @count == MAX_PER_ROW
			@row += 1
			table_resize(@row, MAX_PER_ROW)
			@column = 1
			@count = 0
		end	
		@table.attach_defaults(button, @column - 1, @column, @row - 1, @row)
		@count += 1
		@column += 1
	end
end
# These two methods check the running window manager and if DL is running Live or installed. Setup things properly.

terminal = "x-terminal-emulator --noclose -e"
browser  = "x-www-browser"
icondir  = "/usr/share/siduxcc/gtk2/icons/"

# These variables hold the result of the runnig window manager and the running status.
cmd = "su-to-root -X -c"
# Set up window
win = Gtk::Window.new("sidux Control Center")
###win.set_icon("/usr/share/icons/Dream_Apps/scalable/apps/dream-start.png")
win.set_icon("/usr/share/icons/hicolor/32x32/apps/siduxcc.png")
win.set_window_position(Gtk::Window::POS_CENTER_ALWAYS)
win.set_border_width(10)

box = Gtk::VBox.new
win.add(box)

##Panel 1
panel1 = Panel.new("System Configuration")
box.pack_start(panel1)



if File.exists?("/usr/share/doc/xfce4-mcs-manager/copyright")
	panel1.add_button("   Xfce\nSettings", "#{icondir}settings.png", "xfce-setting-show")
end
if File.exists?("/usr/share/doc/gnome-control-center/copyright")
	panel1.add_button("  Gnome\nSettings", "#{icondir}gnome.png", "gnome-control-center")
end
panel1.add_button(" Display\nSettings",    "#{icondir}display.png", "#{cmd} \"siduxcc display ssftDisplay\"")
if File.exists?("/usr/share/doc/ceni/copyright")
	panel1.add_button("Network\nSettings", "#{icondir}network.png", "#{terminal} ceni")
end
if File.exists?("/usr/share/doc/install-meta/copyright")
	panel1.add_button("     Install\nMetapackages", "/usr/share/icons/hicolor/48x48/apps/metapackage-installer.png", "#{cmd} metapackage-installer")
end
if File.exists?("/usr/share/doc/gnome-system-tools/copyright")
	panel1.add_button(" Users\nGroups", "#{icondir}users.png", "#{cmd} users-admin")
	panel1.add_button(" Time/Date",     "#{icondir}clock.png", "#{cmd} time-admin")
end


## Panel 2
panel2 = Panel.new("Hardware Setup")
box.pack_start(panel2)
if File.exists?("/usr/share/doc/gnome-alsamixer/copyright")
	panel2.add_button(" Alsa\nMixer", "#{icondir}audio.png", "gnome-alsamixer")
end
if File.exists?("/usr/share/doc/gnome-cups-manager/copyright")
	panel2.add_button("  CUPS\nManager", "#{icondir}printer.png", "#{cmd} gnome-cups-manager")
end
if File.exists?("/usr/share/doc/hardinfo/copyright")
	panel2.add_button(" HardInfo", "#{icondir}info.png", "hardinfo")
end


##Panel 3
panel3 = Panel.new("Help")
box.pack_start(panel3)
panel3.add_button("Manual", "/usr/share/icons/hicolor/48x48/apps/sidux-manual.png", "#{browser} /usr/share/sidux-manual/index.html")
panel3.add_button("Forum", "#{icondir}forum.png", "#{browser} http://www.sidux.com/module-PNphpBB2.html")
if File.exists?("/usr/share/doc/sidux-irc/copyright")
	panel3.add_button(" IRC Chat", "/usr/share/icons/hicolor/48x48/apps/sidux-irc.png", "sidux-irc")
end
panel3.add_button("About", "#{icondir}about.png", "siduxcc dialog about")


win.show_all
win.signal_connect("destroy") { Gtk.main_quit }

Gtk.main