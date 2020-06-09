extends State

var target : Spatial;

func enter(args : Array):
	if(args.size() != 1):
		print_debug("pinpoint state only takes one argument")
	
	target = args[0]

func update(delta):

	root_state.target_translation = root_state.focus_to(target)
	
	#calculate target rotation so that the camera is facing the target
	var rot = target.get_rotation().rotated(target.get_transform().basis.y, PI)
	root_state.target_rotation = rot
	
	if(!Input.is_action_pressed("move_strafe")):
		return "Following"
