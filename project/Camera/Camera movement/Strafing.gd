extends State

func enter(_args):
	root_state.target_rotation = Quat(owner.transform.basis)
	root_state.target_position = owner.transform.origin

func run(delta):
	if(!Input.is_action_pressed("move_strafe")):
		return "Following"

	# focus to player position
	root_state.target_position = root_state.get_offset_pos(owner.player.get_translation())
	root_state.target_position += owner.transform.basis.z * owner.distance
