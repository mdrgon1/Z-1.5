extends Node

var generation_queue : Array
var generated_chunks : Dictionary
var chunks : Dictionary

func _ready():
	for i in 16:
		var coordinates := Vector2(i % 4, int(i / 4))
		generation_queue.push_back(coordinates)

func _process(delta):
	
	# generate chunks, don't add them to tree just yet 
	if(generation_queue.size() != 0):# this is just to limit chunk generation to once a frame, TODO replace this with actual multithreading or something
		var chunk = Chunk.new()
		var coords = generation_queue.pop_front()
		
		chunk.heightmap = load("res://heightmap.png").get_data().get_rect(Rect2(coords * 15, Vector2(16, 16)))
		chunk.height = 5
		chunk.translation.x = coords.x * 15
		chunk.translation.z = coords.y * 15
		chunk._generate_mesh()
		
		generated_chunks[coords] = chunk
		add_chunk(coords)

# add a generated chunk to the tree
func add_chunk(coords : Vector2):
	
	add_child(generated_chunks[coords])
	chunks[coords] = generated_chunks[coords]
