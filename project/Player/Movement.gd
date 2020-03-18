extends Node

const UP = Vector3(0, 1, 0)
const ROT_LERP = 0.04

export var speed : float = 10
var vel_3d : Vector3


func _physics_process(delta):
	vel_3d.y = owner.translation.y;
	var vel_2d = (get_input_vector() * speed).rotated(-owner.rotation.y)
	vel_3d.x = vel_2d.x
	vel_3d.z = vel_2d.y
	
	#angle from the forward vector to the target velocity
	var forward = owner.get_global_transform().basis.z
	var forward_to_velocity = Vector2(forward.x, forward.z).angle_to(vel_2d)
	owner.global_rotate(UP, forward_to_velocity * ROT_LERP * delta * 60)
	
	owner.move_and_slide(vel_3d, UP)
	
func get_input_vector():
	var input_vec = Vector2(0, 0)
	if Input.is_action_pressed("move_forward"):
		input_vec.y += 1
	if Input.is_action_pressed("move_backward"):
		input_vec.y -= 1
	if Input.is_action_pressed("move_right"):
		input_vec.x += 1
	if Input.is_action_pressed("move_left"):
		input_vec.x -= 1
	
	return input_vec
