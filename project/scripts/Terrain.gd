#tool
extends MeshInstance

var test = load("res://bin/terrainGen.gdns").new()
export var heightmap_tex : Texture
var heightmap_img : Image
var heightmap_arr : Array
var height = 1

export var load_heightmap = false setget set_load_heightmap

func _ready():
	#load_heightmap();
	#update_mesh()
	pass

func set_load_heightmap(value):
	load_heightmap = false
	load_heightmap()

func load_heightmap():
	test.set_height(height)
	test.set_heightmap(heightmap_tex.get_data())
	test.gen_densitymap()
	

func change_height(new_height):
	height = new_height
	load_heightmap()
	
func update_mesh():
	
	#var time = OS.get_ticks_msec()	
	#test.generate_mesh();

#	var vertex_array
#
#	var arrays = []
#	arrays.resize(Mesh.ARRAY_MAX)
#
#	var normal_array = PoolVector3Array()
#	normal_array.resize(vertex_array.size())
#	var uv_array = PoolVector3Array()
#	uv_array.resize(vertex_array.size())
#	var index_array = PoolIntArray()
#	index_array.resize(vertex_array.size())
#
#	for i in range(vertex_array.size()):
#		normal_array[i] = Vector3(0, 0, 0)
#		uv_array[i] = Vector3(0, 0, 0)
#		index_array[i] = i;
#
#	arrays[Mesh.ARRAY_VERTEX] = vertex_array
#	arrays[Mesh.ARRAY_NORMAL] = normal_array
#	arrays[Mesh.ARRAY_TEX_UV] = uv_array
#	arrays[Mesh.ARRAY_INDEX] = index_array
#
#	var m = ArrayMesh.new()
#
#	m.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, arrays)
#
#	m.regen_normalmaps()
	test.generate_mesh()
	
	var m = test.get_mesh()
	
	var arrays = m.surface_get_arrays(0)
	
	set_mesh(m)
	

func _process(delta):
	var time = OS.get_ticks_msec()
	if Input.is_key_pressed(KEY_T):
		#change_height(height + delta * 3)
		change_height(height + delta * 3)
		update_mesh();
		
	if Input.is_key_pressed(KEY_Y):
		change_height(height - delta * 3)
		update_mesh()
	print(OS.get_ticks_msec() - time)
		
	if Input.is_key_pressed(KEY_ESCAPE):
		get_tree().quit()

func _exit_tree():
	test.free()
