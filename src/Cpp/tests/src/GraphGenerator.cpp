#include "gtest/gtest.h"
#include <array>
#include <graph_generator.h>
#include <unique_queue.h>
#include <embree_raytracer.h>
#include <objloader.h>
#include <meshinfo.h>
#include <graph.h>
#include <edge.h>
#include <node.h>
#include <Constants.h>
#include <graph_generator.h>
#include <objloader.h>
#include <unique_queue.h>

using HF::SpatialStructures::Graph;
using HF::GraphGenerator::GraphGenerator;
using HF::RayTracer::EmbreeRayTracer;

using HF::SpatialStructures::Node;

EmbreeRayTracer CreateGGExmapleRT() {
	//! [EX_GraphGeneratorRayTracer]
	
	// Load an OBJ containing a simple plane
	auto mesh = HF::Geometry::LoadMeshObjects("plane.obj", HF::Geometry::ONLY_FILE, true);
	
	// Create a raytracer using this obj
	EmbreeRayTracer ray_tracer = HF::RayTracer::EmbreeRayTracer(mesh);
	
	//! [EX_GraphGeneratorRayTracer]
	return ray_tracer;
}	

std::ostringstream PrintGraph(const Graph & g) {

	//![EX_PrintGraph]
	
	// Get nodes from the graph
	auto nodes = g.Nodes();

	// Construct Output String
	std::ostringstream out_str;

	out_str << "[";
	for (int i = 0; i < nodes.size(); i++)
		out_str << nodes[i] << ((i != nodes.size() - 1) ? "," : "]");

	// Print to console.
	std::cout << out_str.str() << std::endl;
	
	//![EX_PrintGraph]
	
	return out_str;
}

TEST(_GraphGenerator, BuildNetwork) {
	EmbreeRayTracer ray_tracer = CreateGGExmapleRT();

	//! [EX_BuildNetwork]
	
	// Create a graphgenerator using the raytracer we just created
	HF::GraphGenerator::GraphGenerator GG = GraphGenerator::GraphGenerator(ray_tracer);

	// Setup Graph Parameters
	std::array<float, 3> start_point{ 0,0,0.25 };
	std::array<float, 3> spacing{ 1,1,1 };
	int max_nodes = 5;
	int up_step = 1; int down_step = 1;
	int up_slope = 45; int down_slope = 45;
	int max_step_connections = 1;

	// Generate the graph using our parameters
	HF::SpatialStructures::Graph g = GG.BuildNetwork(
		start_point,
		spacing,
		max_nodes,
		up_step, down_step,
		up_slope, down_slope,
		max_step_connections
	);

	//! [EX_BuildNetwork]
	
	auto out_str = PrintGraph(g);

	// Compare output against expected output
	std::string expected_output = "[(0, 0, -0),(-1, -1, -0),(-1, 0, -0),(-1, 1, -0),(0, -1, -0),(0, 1, -0),(1, -1, -0),(1, 0, -0),(1, 1, -0),(-2, -2, -0),(-2, -1, -0),(-2, 0, -0),(-1, -2, -0),(0, -2, -0),(-2, 1, -0),(-2, 2, 0),(-1, 2, 0),(0, 2, 0),(1, -2, -0)]";
	ASSERT_EQ(expected_output, out_str.str());
}

TEST(_GraphGenerator, CrawlGeom) {
	EmbreeRayTracer ray_tracer = CreateGGExmapleRT();

	//! [EX_CrawlGeom]

	// Create a graphgenerator using the raytracer we just created
	HF::GraphGenerator::GraphGenerator GG = GraphGenerator::GraphGenerator(ray_tracer);

	// Set parameters for graph generation
	std::array<float, 3> start_point{ 0,1,0 };
	std::array<float, 3> spacing{ 1,1,1 };
	int max_nodes = 5;
	int up_step = 1; int down_step = 1;
	int up_slope = 45; int down_slope = 45;
	int max_step_connections = 1;

	// Since we're not calling BuildNetwork, we will need to set some parameters
	// in the GraphGenerator in order to use this function standalone
	
	// These parameters need to be set on the graph generator
	GG.core_count = -1;
	GG.max_nodes = 5;
	GG.max_step_connection = 1;
	
	// Setup its params struct
	GG.params.up_step = up_step; GG.params.down_step = down_step;
	GG.params.up_slope = up_slope; GG.params.down_slope = down_slope;
	GG.params.precision.ground_offset = 0.01; 
	GG.params.precision.node_z = 0.001f;
	GG.params.precision.node_spacing = 0.001;

	// Assign it's spacing member, making sure to convert it to real3, the graph generator's
	// internal type for vectors of 3 coordinates.
	GG.spacing = HF::GraphGenerator::CastToReal3(spacing);

	// Construct a unique queue, and push back the start point
	HF::GraphGenerator::UniqueQueue queue;
	queue.push(start_point);
	
	//! [EX_CrawlGeom]
	
	// Define Expected Output
	std::string expected_output = "[(0, 1, 0),(-1, 0, -0),(-1, 1, -0),(-1, 2, 0),(0, 0, -0),(0, 2, 0),(1, 0, -0),(1, 1, -0),(1, 2, 0),(-2, -1, -0),(-2, 0, -0),(-2, 1, -0),(-1, -1, -0),(0, -1, -0),(-2, 2, 0),(-2, 3, 0),(-1, 3, 0),(0, 3, 0),(1, -1, -0)]";
	
	//! [EX_CrawlGeom_Serial]
	
	// Call Crawl Geom with the queue
	auto g = GG.CrawlGeom(queue);

	//! [EX_CrawlGeom_Serial]
	
	// Check Serial
	auto out_str = PrintGraph(g);
	ASSERT_EQ(expected_output, out_str.str());

	//! [EX_CrawlGeom_Parallel]
	
	// Call crawl geom parallel
	g = GG.CrawlGeomParallel(queue);
	
	//! [EX_CrawlGeom_Parallel]

	// CheckParallel
	std::string expected_parallel = "[(0, 2, 0),(-1, 1, -0),(-1, 2, 0),(-1, 3, 0),(0, 1, -0),(0, 3, 0),(1, 1, -0),(1, 2, 0),(1, 3, 0),(1, 0, -0),(0, -1, -0),(0, 0, -0),(1, -1, -0),(2, -1, -0),(2, 0, -0),(2, 1, -0),(2, 2, 0),(2, 3, 0),(-2, -1, -0),(-3, -2, -0),(-3, -1, -0),(-3, 0, -0),(-2, -2, -0),(-2, 0, -0),(-1, -2, -0),(-1, -1, -0),(-1, 0, -0)]";
	auto parallel_out_str = PrintGraph(g);
	ASSERT_EQ(expected_parallel, parallel_out_str.str());
}

TEST(_GraphGenerator, ValidateStartPoint) {
	EmbreeRayTracer ray_tracer = CreateGGExmapleRT();

	//! [EX_ValidateStartPoint]

	// Define tolerances
	HF::GraphGenerator::Precision precision;
	precision.node_z = 0.01f;
	precision.node_spacing = 0.001f;
	precision.ground_offset = 0.001f;

	// Create graphParameters to hold tolerances
	HF::GraphGenerator::GraphParams params;
	params.precision = precision;

	// Setup start_point
	HF::GraphGenerator::real3 start_point{ 0,0,10 };

	// Call ValidateStartPoint
	HF::GraphGenerator::optional_real3 result = HF::GraphGenerator::ValidateStartPoint(
		ray_tracer, start_point, params
	);

	// If the ray intersected, print the result
	if (result)
	{
		auto result_point = *result;
		printf("(%0.000f, %0.000f, %0.000f)\n", result_point[0], result_point[1], result_point[2]);
	}

	// If it didn't print a message
	else
		printf("No intersection found\n");

	//! [EX_ValidateStartPoint]

	// Assert that the ray hit and the start point was correctly updated
	// to the point of intersection
	ASSERT_TRUE(result);
	ASSERT_TRUE(result.pt[0] == 0, result.pt[1] == 0, result.pt[2] == 0);
}

TEST(_GraphGenerator, CheckRay) {
	EmbreeRayTracer ray_tracer = CreateGGExmapleRT();

	//! [EX_CheckRay]

	// Define z tolerance
	HF::GraphGenerator::real_t node_z = 0.01;

	// Create a start  point 10 units above the mesh, and
	// a direction vector facing straight down.
	HF::GraphGenerator::real3 start_point{ 1,1,1 };
	HF::GraphGenerator::real3 direction{0,0,-1};

	// Call CheckRay and capture the result
	HF::GraphGenerator::optional_real3 result = HF::GraphGenerator::CheckRay(
		ray_tracer, start_point, direction, node_z
	);

	// If the ray intersected, print it
	if (result)
	{
		auto result_point = *result;
		printf("(%0.000f, %0.000f, %0.000f)\n", result_point[0], result_point[1], result_point[2]);
	}

	// If it didn't print a message
	else
		printf("No intersection found\n");

	//! [EX_CheckRay]

	// Assert that the ray hit and the start point was correctly updated
	// to the point of intersection
	ASSERT_TRUE(result);
	ASSERT_TRUE(result.pt[0] == 1, result.pt[1] == 1, result.pt[2] == 0);
}

TEST(_GraphGenerator, CreateDirecs) {
	
	//! [EX_CreateDirecs]

	// Call create direcs with a max_step_conections of 2
	auto directions = HF::GraphGenerator::CreateDirecs(2);

	// Construct Output String
	std::ostringstream out_str;

	out_str << "[";
	for (int i = 0; i < directions.size(); i++)
		out_str << "(" << directions[i].first << ", " << directions[i].second << ")" << ((i != directions.size() - 1) ? "," : "]");

	// Print to console.
	std::cout << out_str.str() << std::endl;
	
	//! [EX_CreateDirecs]

	std::string expected_output = "[(-1, -1),(-1, 0),(-1, 1),(0, -1),(0, 1),(1, -1),(1, 0),(1, 1),(-2, -1),(-2, 1),(-1, -2),(-1, 2),(1, -2),(1, 2),(2, -1),(2, 1)]";
	ASSERT_EQ(expected_output, out_str.str());
}

TEST(_GraphGenerator, GeneratePotentialChildren) {
	//! [EX_CreatePotentialChildren]

	// Create a parent node and set the spacing for these offsets
	HF::GraphGenerator::real3 parent = { 0,0,1 };
	HF::GraphGenerator::real3 spacing = { 1,2,3 };

	// Create a vector of directions to offset it
	std::vector<HF::GraphGenerator::pair> directions = { {0,1}, {0,2}, {1,0}, {2,0}, {1,1}, {2,1} };

	// Construct a GraphParams with the spacing filled out
	HF::GraphGenerator::GraphParams gp;
	gp.precision.node_spacing = 0.001f;
	gp.precision.node_z = 0.001f;

	// Call CreateDirecs
	auto children = HF::GraphGenerator::GeneratePotentialChildren(parent, directions, spacing, gp);

	// Create Output
	std::ostringstream out_str;
	
	out_str << "[";
	for (int i = 0; i < directions.size(); i++)
		out_str << "(" << children[i][0] << ", " << children[i][1] << ", " 
		<< children[i][2] << ")" << ((i != directions.size() - 1) ? "," : "]");

	// Print to console
	std::cout << out_str.str() << std::endl;

	//![EX_CreatePotentialChildren]

	std::string expected_output = "[(0, 2, 4),(0, 4, 4),(1, 0, 4),(2, 0, 4),(1, 2, 4),(2, 2, 4)]";
	ASSERT_EQ(expected_output, out_str.str());
}

TEST(_GraphGenerator, GetChildren) {
	EmbreeRayTracer ray_tracer = CreateGGExmapleRT();
	
	//! [EX_CreateChildren]

	// Create a parent node
	HF::GraphGenerator::real3 parent{ 0,0,1 };
	
	// Create a vector of possible children
	std::vector<HF::GraphGenerator::real3> possible_children{
		HF::GraphGenerator::real3{0,2,1}, HF::GraphGenerator::real3{1,0,1},
		HF::GraphGenerator::real3{0,1,1}, HF::GraphGenerator::real3{2,0,1}
	};
	
	// Create graph parameters
	HF::GraphGenerator::GraphParams params;
	params.up_step = 2; params.down_step = 2;
	params.up_slope = 45; params.down_slope = 45;
	params.precision.node_z = 0.01f;
	params.precision.ground_offset = 0.01f;

	// Call GetChildren
	auto edges = HF::GraphGenerator::GetChildren(parent, possible_children, ray_tracer, params);

	// Print children
	std::ostringstream out_str;
	out_str << "[";
	for (int i = 0; i < edges.size(); i++)
		out_str << "(" << edges[i].child << ", " << edges[i].score << ", " << edges[i].step_type << ")" << ((i != edges.size() - 1) ? "," : "]");
	
	std::cout << out_str.str() << std::endl;
		
	//! [EX_CreateChildren]

	ASSERT_LT(0, edges.size());

	std::string expected_output = "[((0, 2, 0), 2.23607, 1),((2, 0, -0), 2.23607, 1)]";
	ASSERT_EQ(expected_output, out_str.str());
}

TEST(_GraphGenerator, CheckChildren) {
	EmbreeRayTracer ray_tracer = CreateGGExmapleRT();

	//! [EX_CheckChildren]

	// Create a parent node
	HF::GraphGenerator::real3 parent{ 0,0,1 };

	// Create a vector of possible children
	std::vector<HF::GraphGenerator::real3> possible_children{
		HF::GraphGenerator::real3{0,2,1}, HF::GraphGenerator::real3{1,0,1},
		HF::GraphGenerator::real3{0,1,1}, HF::GraphGenerator::real3{2,0,1}
	};

	// Create graph parameters
	HF::GraphGenerator::GraphParams params;
	params.up_step = 2; params.down_step = 2;
	params.up_slope = 45; params.down_slope = 45;
	params.precision.node_z = 0.01f;
	params.precision.ground_offset = 0.01f;

	// Call CheckChildren 
	auto valid_children = HF::GraphGenerator::CheckChildren(parent, possible_children, ray_tracer, params);

	// Print children
	std::ostringstream out_str;
	out_str << "[";
	for (int i = 0; i < valid_children.size(); i++)
		out_str << "(" << valid_children[i][0] << ", " << valid_children[i][1] << ", " << valid_children[i][2] << ")" << ((i != valid_children.size() - 1) ? "," : "]");

	std::cout << out_str.str() << std::endl;

	//! [EX_CheckChildren]

	std::string expected_output = "[(0, 2, 0),(1, 0, -0),(0, 1, -0),(2, 0, -0)]";
	ASSERT_EQ(expected_output, out_str.str());
}

TEST(_GraphGenerator, CheckConnection) {
	EmbreeRayTracer ray_tracer = CreateGGExmapleRT();

	//! [EX_CheckConnection]

	// Create a parent node
	HF::GraphGenerator::real3 parent{ 0,0,1 };

	// Create a vector of possible children
	std::vector<HF::GraphGenerator::real3> possible_children{
		HF::GraphGenerator::real3{0,2,0}, HF::GraphGenerator::real3{1,0,0},
		HF::GraphGenerator::real3{0,1,0}, HF::GraphGenerator::real3{2,0,0}
	};

	// Create graph parameters
	HF::GraphGenerator::GraphParams params;
	params.up_step = 2; params.down_step = 2;
	params.up_slope = 45; params.down_slope = 45;
	params.precision.node_z = 0.01f;
	params.precision.ground_offset = 0.01f;

	// Loop through potential children call each one with check connection
	std::vector<HF::SpatialStructures::STEP> connections;
	for (const auto& child : possible_children)
		connections.push_back(HF::GraphGenerator::CheckConnection(parent, child, ray_tracer, params));
		

	// Print children
	std::ostringstream out_str;
	out_str << "[";
	for (int i = 0; i < connections.size(); i++)
		out_str << connections[i] << ((i != connections.size() - 1) ? "," : "]");

	// In the output, 0s indicate no connection, while 1s indicate that nodes are on a flat plane
	// with no step between them. 
	std::cout << out_str.str() << std::endl;

	//! [EX_CheckConnection]

	std::string expected_output = "[1,0,0,1]";
	ASSERT_EQ(expected_output, out_str.str());
}


TEST(_GraphGenerator, CheckSlope) {
	
	//! [EX_CheckSlope]
	
	// Setup graph parameters so the slope limits are 30 degrees in both directions.
	HF::GraphGenerator::GraphParams gp;
	gp.up_slope = 30; gp.down_slope = 30;

	// Create a parent, a child that's traversble, and a child that's too steep
	// to pass the slope check
	HF::GraphGenerator::real3 parent { 0,0,0 };
	HF::GraphGenerator::real3 child_1 { 0,1,0.5 };
	HF::GraphGenerator::real3 child_2 { 0,1,1};

	// Perform slope checks
	bool slope_check_child_1 = HF::GraphGenerator::CheckSlope(parent, child_1, gp);
	bool slope_check_child_2 = HF::GraphGenerator::CheckSlope(parent, child_2, gp);
	
	std::cout << "Slope Check For Child 1 = " << (slope_check_child_1 ? "True" : "False") << std::endl;
	std::cout << "Slope Check For Child 2 = " << (slope_check_child_2 ? "True" : "False") << std::endl;

	//! [EX_CheckSlope]
	
	ASSERT_TRUE(slope_check_child_1);
	ASSERT_FALSE(slope_check_child_2);
}

TEST(_GraphGenerator, OcclusionCheck) {
	EmbreeRayTracer ray_tracer = CreateGGExmapleRT();

	//! [EX_CheckOcclusion]

	// Create a parent node, a child that has a clear line of sight, then
	// a child that is underneath the plane
	HF::GraphGenerator::real3 parent{ 0,0,1 };
	HF::GraphGenerator::real3 child_1{ 0,0,-3 };
	HF::GraphGenerator::real3 child_2{ 0,0,1 };

	// Perform slope checks
	bool occlusion_check_child_1 = HF::GraphGenerator::OcclusionCheck(parent, child_1, ray_tracer);
	bool occlusion_check_child_2= HF::GraphGenerator::OcclusionCheck(parent, child_2, ray_tracer);

	std::cout << "Occlusion Check For Child 1 = " << (occlusion_check_child_1 ? "True" : "False") << std::endl;
	std::cout << "Occlusion Check For Child 2 = " << (occlusion_check_child_2 ? "True" : "False") << std::endl;
	
	//! [EX_CheckOcclusion]

	ASSERT_TRUE(occlusion_check_child_1);
	ASSERT_FALSE(occlusion_check_child_2);
}



