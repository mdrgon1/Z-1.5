#include "cubifier.hpp"

using namespace godot;

void Cubifier::_register_methods() {
	//register_property<Cubifier, Ref<Image>>("heightmap", &Cubifier::set_heightmap, &Cubifier::get_heightmap, Ref<Image>(NULL));

	register_method("set_heightmap", &Cubifier::SetHeightmap);
	register_method("get_heightmap", &Cubifier::GetHeightmap);
	register_method("generate_mesh", &Cubifier::GenerateMesh);
	register_method("get_mesh_arrays", &Cubifier::GetMeshArrays);
	register_method("density_func", &Cubifier::DensityFunc);
}

void Cubifier::_init() {
}

void Cubifier::SetHeightmap(Array new_heightmap) {
	if (new_heightmap.size() != CHUNK_SIZE) {
		return;
	}
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		if (new_heightmap[i].get_type() != Variant::ARRAY) {
			return;
		}
		Array row = new_heightmap[i];
		if (row.size() != CHUNK_SIZE) {
			return;
		}
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			_heightmap[i][j] = row[j];
		}
	}
}

Array Cubifier::GetHeightmap(int i, int j) {
	Array heightmapGDArr = Array();
	heightmapGDArr.resize(CHUNK_SIZE);

	/*Array heightmapGDArr = Array();
	heightmapGDArr.resize(CHUNK_SIZE);*/

	for (i = 0; i < CHUNK_SIZE; ++i) {
		Array row = Array();
		row.resize(CHUNK_SIZE);
		for (j = 0; j < CHUNK_SIZE; ++j) {
			row[j] = _heightmap[i][j];
		}
		heightmapGDArr[i] = row;
	}
	
	return heightmapGDArr;
}

void Cubifier::GenerateMesh(float terrain_height) {

	height = terrain_height;

	float corner_densities[8];
	int coordinates[3] = {0};
	int cubeId;
	int edge_index;
	float cube_offset[3];
	Vertex vertex;
	//for (int i = 0; i < NUM_CUBES; i++) {
	//	vertices[i] = Vector3(0, 2, 0);
	//}

	int index = 0;
	for (int i = 0; i < CHUNK_SIZE; i++) {
		for (int j = 0; j < CHUNK_SIZE; j++) {
			for (int k = 0; k < CHUNK_SIZE; k++) {
				for (int l = 0; l < 8; l++) {
					coordinates[0] = i + VERTEX_TABLE[l][0];
					coordinates[1] = j + VERTEX_TABLE[l][1];
					coordinates[2] = k + VERTEX_TABLE[l][2];

					//corner_densities[l] = DensityFunc(coordinates);
					corner_densities[l] = DensityFunc(coordinates[0], coordinates[1], coordinates[2]);
				}

				cubeId = GetCubeId(corner_densities);
				cube_offset[0] = i;
				cube_offset[1] = j;
				cube_offset[2] = k;

				for (int vertex_index = 0; vertex_index < MAX_NUM_VERTICES_PER_CUBE; vertex_index++) {
					edge_index = TRIANGUALATION_TABLE[cubeId][vertex_index];
					if (edge_index != -1) {
						vertex = EdgeVertexPos(edge_index, corner_densities);
						vertex.x += i;
						vertex.y += j;
						vertex.z += k;
						vertices[index] = vertex;
						index++;
					}
					else {
						vertex_index = MAX_NUM_VERTICES_PER_CUBE;
					}
				}



				//chunk.cube_indices[index] = get_cube_index(corner_densities);
				//chunk.cube_offsets[index][0] = i;
				//chunk.cube_offsets[index][1] = j;
				//chunk.cube_offsets[index][2] = k;

				//vertices[index] = Vector3(i + 1, j, k);
				//vertices[index++] = Vector3(i, j + 1, k);
				//vertices[index++] = Vector3(i, j, k + 1);
				//index++;
			}
		}
	}

	numVertices = index + 1;
	//int num_vertices = 0;
	//for (int i : cube_indices) {
	//	num_vertices += VERTICES_PER_CUBE[i];
	//}
	//
	//Vector3 vertices[num_vertices];

	//vertex_arr = vertices;
}

Array Cubifier::GetMeshArrays(int i) {
	//Vertex vertex = vertices[i];
	//return Vector3(vertex.x, vertex.y, vertex.z);

	//return num_vertices;

	Array vertex_gdarr = Array();
	vertex_gdarr.resize(numVertices);

	Array mesh_arr = Array();
	mesh_arr.resize(1);

	Vertex vertex;
	for (int i = 0; i < numVertices; i++) {
		vertex = vertices[i];
		vertex_gdarr[i] = Vector3(vertex.x, vertex.z, vertex.y);
		//vertex_gdarr[i] = Vector3(density_func([10, 3, 6]), 0, 0);
	}

	mesh_arr[0] = vertex_gdarr;
	//Array debugArr = Array();
	//debugArr.append(numVertices);
	//return debugArr;
	return mesh_arr;
}

int Cubifier::GetCubeId(float values[8]) {
	int cube_index = 0;
	for (int i = 0; i < 8; i++) {
		if (values[i] > 0) {
			cube_index |= 1 << i;
		}
	}

	return cube_index;
}

//float Cubifier::DensityFunc(int coordinates[3]) {
float Cubifier::DensityFunc(int x, int y, int z) {
	/*int x = coordinates[0];
	int y = coordinates[1];
	int z = coordinates[2];*/

	float density = z - _heightmap[x][y] * height;
	return density;
}

Vertex Cubifier::EdgeVertexPos(int edge_index, float corner_densities[8]) {
	Vertex pos;
	
	int edge[2];
	edge[0] = EDGE_TABLE[edge_index][0];
	edge[1] = EDGE_TABLE[edge_index][1];

	float v1 = corner_densities[edge[0]];
	float v2 = corner_densities[edge[1]];
	float density_range = abs(v1 - v2);
	float lerp_value = abs(v1) / density_range;

	float pos_arr[3];
	for (int i = 0; i < 3; i++) {
		//lerp between VERTEX_TABLE[edge[0]][i] and VERTEX_TABLE[edge[1]][i], apparently not already a function
		pos_arr[i] = VERTEX_TABLE[edge[0]][i] + lerp_value * (VERTEX_TABLE[edge[1]][i] - VERTEX_TABLE[edge[0]][i]);
	}

	pos.x = pos_arr[0];
	pos.y = pos_arr[1];
	pos.z = pos_arr[2];

	return pos;
}