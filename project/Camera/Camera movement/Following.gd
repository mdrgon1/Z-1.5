extends State

func run(delta):
	if(Input.is_action_pressed("move_strafe")):
		#return ["Pinpoint", owner.player]
		return ["Pinpoint", owner.player]
	
	var target_rot := Quat(owner.rotation)
	
	# point camera at the player (plus offset)
	var focus : Vector3 = root_state.get_offset_pos(owner.player.get_translation())
	target_rot = Quat(owner.transform.looking_at(focus, owner.UP).basis)
	
	# move camera the correct distance from player
	var target_pos : Vector3 = owner.player.get_translation() + root_state.to_player().normalized() * owner.distance
	
	root_state.target_position = target_pos
	root_state.target_rotation = target_rot
