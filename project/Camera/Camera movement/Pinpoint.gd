extends State

var target : Spatial;

func enter(_args : Array):
	if(_args == []):
		return
	
	target = _args[0]

func update(delta):

	root_state.target_translation = root_state.focus_to(target)
	
	#calculate target rotation so that the camera is facing the target
	var rot = target.get_rotation().rotated(target.get_transform().basis.y, PI)
	rot.z = 0
	root_state.target_rotation = rot
	
	if(!Input.is_action_pressed("move_strafe")):
		return "Following"
