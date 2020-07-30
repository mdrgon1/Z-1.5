extends Spatial
class_name Chunk

var mesh := MeshInstance.new()
var terrain_gen = preload("res://bin/terrainGen.gdns").new()

export var heightmap : Texture
export var height : float

func _ready():
	terrain_gen.set_heightmap(heightmap.get_data())
	terrain_gen.set_height(height)
	terrain_gen.gen_densitymap()
	terrain_gen.generate_mesh()
	mesh.set_mesh(terrain_gen.get_mesh())
	
	add_child(mesh)

func _exit_tree():
	mesh.free()
