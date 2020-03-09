extends RichTextLabel

# Declare member variables here. Examples:
# var a = 2
# var b = "text"

func _process(delta):
	if delta == 0:
		return
		
	var fps = 1 / delta
	set_text(str(int(fps)))
