extends State

var target : Spatial;

func enter(_args : Array):
	if(_args.size() != 1):
		print_debug("pinpoint state only takes one argument")
		return
	
	target = _args[0]
	
	.enter(_args)

func run(delta):

	root_state.target_position = target.get_translation()
	root_state.target_position += target.transform.basis.z * owner.distance
	
	# calculate target rotation so that the camera is facing the target
	var rot = Quat(owner.transform.looking_at(target.get_translation(), owner.UP).basis)
	root_state.target_rotation = rot
	
	if(!Input.is_action_pressed("move_strafe")):
		return "Following"
