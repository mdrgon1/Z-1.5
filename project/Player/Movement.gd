extends State

const UP = Vector3(0, 1, 0)
const ROT_LERP = 0.1

export var speed : float = 10
var vel_3D : Vector3
var vel_2D : Vector2
var forward : Vector3
var forward2D : Vector2

#onready var camera = get_viewport().get_camera()
onready var camera = get_node("../../Camera")

func run(delta):
	
	forward = owner.get_global_transform().basis.z
	forward2D.x = (forward.z)
	forward2D.y = (forward.x)
	
	#movement is always relative to the camera view
	vel_2D = (get_input_vector() * speed).rotated(3*PI/2 + camera.forward2D.angle())
	
	vel_3D.y = 0;
	vel_3D.x = vel_2D.y
	vel_3D.z = vel_2D.x
	
	owner.move_and_slide(vel_3D, UP)
	
	#only update rotation if the player is standing still
	if get_input_vector() != Vector2(0, 0):
		var rot = forward2D.angle_to(vel_2D) * ROT_LERP * delta * 60
		owner.global_rotate(UP, rot)


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
