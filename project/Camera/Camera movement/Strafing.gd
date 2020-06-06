extends State

func update(delta):
	if(!Input.is_action_pressed("move_strafe")):
		return "Following"

	#Lerp focus to player position
	root_state.target_translation = owner.get_translation() + root_state.focus_to_player()
