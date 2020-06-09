extends State

func update(delta):
	if(Input.is_action_pressed("move_strafe")):
		return ["Pinpoint", owner.player]
		#return "Strafing"
	
	#angle from the forward vector to the player's target velocity
	var forward_to_velocity = owner.forward2D.angle_to(root_state.player.movement.vel_2D)
	
	#rotate the camera to align with player motion
	if(root_state.player.movement.get_input_vector() != Vector2(0, 0)):
		var rot = forward_to_velocity * root_state.ROT_LERP * delta * 60
		root_state.target_rotation = owner.get_rotation() + Vector3(0, rot, 0)

	#Lerp focus to player position
	root_state.target_translation = owner.get_translation() + root_state.focus_to_player()
